#include "slotselection.h"
#include "structs.h"

#include <QMouseEvent>
#include <QPushButton>
#include <QButtonGroup>
#include <QGridLayout>

SlotSelection::SlotSelection(const QFont &f)
    : QWidget()
    , _f(f)
    , _lay(new QHBoxLayout(this))
    , _laycols(QList<QVBoxLayout*>())
    , _labels(QList<QButtonGroup*>())
    , _limiter(nullptr)
{
    setWindowFlags(Qt::Popup);
//    setFixedSize(0,0);
    _lay->setSpacing(0);
    _lay->setContentsMargins(0,0,0,0);
}

/*Selection::Selection(Selection *copy)
    : Selection(copy->_f)
{
    QStringList texts;

    for (int i = 0; i < copy->_labels.count(); ++i)
        texts << copy->_labels.at(i)->text();

    createSelection(texts);
}*/

void SlotSelection::addSelection(const QList<Gear> &list, int at)
{
    QStringList texts;

    for (int i = 0; i < list.count(); ++i)
    {

        texts << list.at(i).name + ", " + QString::number(list.at(i).cost);
    }

    addToSelection(texts, at);
}

/*void Selection::removeSelection(const QString &text)
{
    int hpos = 0;
    for (int i = 0; i < _labels.count(); ++i)
    {
        if (_labels.at(i)->text().startsWith(text + ","))
        {
            for (int j = i+1; j < _labels.count(); ++j)
                _labels.at(j)->move(0,i*itemHeight);
            delete _labels.takeAt(i);
            --i;
        }
        if (_labels.at(i)->width() > hpos)
            hpos = _labels.at(i)->width();
    }
    setFixedSize(hpos, _labels.count()*itemHeight);
}*/

bool SlotSelection::hasSelections() const
{
    if(_lay->count())
        return true;
    return false;
}

void SlotSelection::mousePressEvent(QMouseEvent *e)
{
    if (!childAt(e->pos()) || e->button() == Qt::RightButton)
    {
        releaseMouse();
        hide();
        e->accept();
    }
}

/*void Selection::select(const QString &s)
{
    QRegExp xp("^" + s + ", \\d+$");
    foreach (QPushButton *b, _labels)
    {
        if (xp.indexIn(b->text()) == 0)
        {
            emit selected(b->text());
            return;
        }
    }
}*/

void SlotSelection::on_selection(int)
{
    QStringList list;
    QString buttontext;
    QList<int> cost;
    for (int i = 0; i < _labels.count(); ++i)
    {
        buttontext = _labels.at(i)->checkedButton()->text();
        cost << buttontext.section(",",-1).toInt();
        list << buttontext.section(",",0,-2);
    }
    emit selected(list,cost);
}

void SlotSelection::addToSelection(const QStringList &list, int at)
{
    QPushButton *label = nullptr;
    QString text;
//    int vpos = this->height();
//    int hpos = this->width();
    QButtonGroup *grp = nullptr;
    QVBoxLayout *col = nullptr;

    if (_labels.count() > at)
    {
        grp = _labels.at(at);
        col = _laycols.at(at);
    }
    else
    {
        while (_labels.count() <= at)
        {
            grp = new QButtonGroup(this);
            _labels << grp;
            col = new QVBoxLayout(this);
            col->setAlignment(Qt::AlignLeft);
            _lay->addLayout(col);
            _laycols << col;
            connect(grp, QOverload<int>::of(&QButtonGroup::buttonClicked),
                    this, &SlotSelection::on_selection);
        }
    }

    QRegExp xp("!<(.+)>"), xd("(.)(\\d+)!");
    int group;

    for (int i = 0; i < list.count(); ++i)
    {
        group = -1;
        text = list.at(i);
        if (xp.indexIn(text) >= 0)
        {
            group = xp.cap(1).toInt();
            text.remove(xp);
        }

        if (xd.indexIn(text) >= 0)
        {
            if (!_limiter)
                _limiter = new SelectionLimiter(this);
            _limiter->addLimit(group, xd.cap(2).toInt());
        }
        else
        {

            label = new QPushButton(text, this);
            label->setCheckable(true);
            label->setFont(_f);
            label->setFlat(true);
            label->setFixedHeight(itemHeight);
            //        label->move(0, vpos);
            //        label->show();
            //        vpos += itemHeight;
            //        if (label->width() > hpos)
            //          hpos = label->width();

            if (group >= 0)
            {
                if (!_limiter)
                    _limiter = new SelectionLimiter(this);

                _limiter->addButton(label, group, at);
            }

            col->addWidget(label);
            if (col->count() == 1)
                label->toggle();
            grp->addButton(label);
        }
    }
//    setFixedSize(hpos, vpos);
}

SelectionLimiter::SelectionLimiter(QObject *parent)
    : QObject (parent)
    , _limits(QMap<int, Limit>())
{
}

void SelectionLimiter::addLimit(int group, int limit)
{
    _limits.insert(group, Limit{0,limit});
}

void SelectionLimiter::addButton(QPushButton *label, int group, int col)
{
    connect(label, &QPushButton::toggled,
            [=](bool check){buttonPress(check, group, col);});
    connect(this, &SelectionLimiter::disableButtons,
            [=](bool chck, int grp, int cl)
    {
        if (grp == group)
            if (col != cl)
                label->setDisabled(chck);
    });
}

void SelectionLimiter::buttonPress(bool check, int group, int col)
{
    bool dsable = false;
    bool need = false;
    if (check)
    {
        _limits[group].current++;
        if (_limits[group].current == _limits[group].max)
        {
            need = true;
            dsable = true;
        }
    }
    else
    {
        if (_limits[group].current == _limits[group].max)
            need = true;
        _limits[group].current--;

    }
    if (need)
        emit disableButtons(dsable, group, col);
}
