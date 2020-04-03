#include "slotselection.h"
#include "structs.h"
#include "slotbutton.h"

#include <QMouseEvent>
//#include <QPushButton>
//#include <QButtonGroup>
#include <QGridLayout>

SlotSelection::SlotSelection(const QFont &f)
    : QWidget()
    , _f(f)
    , _lay(new QHBoxLayout(this))
    , _laycols(QList<QVBoxLayout*>())
    , _labels(QList<SlotButtonGroup*>())
    , _limiter(nullptr)
    , _models(-1)
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

void SlotSelection::setDefault(Gear &def, int at, int models)
{
    QString text;
    text = def.name + ", " + QString::number(def.cost);

    SlotButtonGroup *grp = createGroup(at);
    SlotButton * but = createButton(text);
    but->setAmount(models);
    if (models >= 0)
    {
        _models = models;
        connect(this, &SlotSelection::modelsChanged,
                but, &SlotButton::changeModels);
    }
    else
        grp->setExclusive();
    but->toggleCheck(true);
    grp->addButton(but);
    _laycols.at(at)->addWidget(but);
}

void SlotSelection::addSelection(const QList<Gear> &list, int at
                                 , int perModels)
{
    SlotButtonGroup *grp = createGroup(at);
    SlotButton * but;

    if (grp)
    {
        but = createBlanc();
        grp->addButton(but);
    }
    else
        grp = _labels.at(at);

    QVBoxLayout *col = _laycols.at(at);

    QRegExp xp("!<(.+)>"), xd("(.)(\\d+)!");
    int group = -1;
    QString text;

    if (perModels > 0)
    {
        if (!_limiter)
            _limiter = new SelectionLimiter(this);
        group = _limiter->addLimit(_models/perModels, perModels);
    }

    for (int i = 0; i < list.count(); ++i)
    {
        text = list.at(i).name + ", " + QString::number(list.at(i).cost);

        if (xp.indexIn(text) >= 0)
        {
            group = xp.cap(1).toInt();
            text.remove(xp);
        }

        if (xd.indexIn(text) >= 0)
        {
            if (!_limiter)
                _limiter = new SelectionLimiter(this);
            _limiter->addCrossLimit(group, xd.cap(2).toInt(), perModels);
        }
        else
        {
            but = createButton(text);
            grp->addButton(but);
            col->addWidget(but);
            if (group >= 0 || perModels > 0)
            {
                if (!_limiter)
                    _limiter = new SelectionLimiter(this);

                _limiter->addButton(but, group, at);
            }
        }
        if (perModels <= 0)
            group = -1;
    }
}

bool SlotSelection::hasSelections() const
{
    if(_lay->count())
        return true;
    return false;
}

void SlotSelection::modelsChange(int change)
{
    if (_models >= 0)
    {
        emit modelsChanged(change);
        _models += change;
        if (_limiter)
            _limiter->modelsChanged(_models);
    }
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

void SlotSelection::on_selection()
{
    QStringList list, texts;
    QList<int> cost;
    for (int i = 0; i < _labels.count(); ++i)
    {
        texts = _labels.at(i)->checkedTexts();
        foreach (QString buttontext, texts)
        {
            cost << buttontext.section(",",-1).toInt();
            list << buttontext.section(",",0,-2);
        }
    }
    emit selected(list,cost);
}

SlotButtonGroup *SlotSelection::createGroup(int at)
{
    while (_labels.count() <= at)
    {
        _labels << nullptr;
        _laycols << nullptr;
    }
    if (_labels.at(at))
        return nullptr;

    SlotButtonGroup *grp = new SlotButtonGroup(this);
    grp->setExclusive();
    _labels.replace(at, grp);
    QVBoxLayout *col = new QVBoxLayout(this);
    col->setAlignment(Qt::AlignLeft);
    _lay->addLayout(col);
    _laycols.replace(at, col);
    connect(grp, &SlotButtonGroup::buttonClicked,
            this, &SlotSelection::on_selection);

    return grp;
}

SlotButton *SlotSelection::createBlanc()
{
    return createButton("-");
}

SlotButton *SlotSelection::createButton(QString text)
{
    SlotButton *label = new SlotButton(text, this);
    label->setFont(_f);

    return label;
}

SelectionLimiter::SelectionLimiter(QObject *parent)
    : QObject (parent)
    , _limits(QMap<int, Limit>())
    , _indexCounter(0)
{
}

void SelectionLimiter::addButton(SlotButton *label, int group, int col)
{
    connect(label, &SlotButton::toggled,
            [=](bool check){buttonPress(check, group, col, _indexCounter);});
    connect(this, &SelectionLimiter::modelChange,
            [=](int change, int grp)
    {
        if (grp == group)
        {
            label->changeLimit(change);
        }
    });
    connect(this, &SelectionLimiter::disableButtons,
            [=](int chck, int grp, int cl, int index)
    {
        if (grp == group)
        {
            if ((group >= 0 && col != cl)
                    || (group < 0 && col == cl && index != _indexCounter))
                label->changeLimit(-chck);
        }
    });
    ++_indexCounter;
}

void SelectionLimiter::addCrossLimit(int group, int limit, int perModel)
{
    _limits.insert(group, Limit{0,limit, perModel});
}

int SelectionLimiter::addLimit(int limit, int perModel)
{
    int c = -(_limits.count()+1);
    _limits.insert(c, Limit{0, limit, perModel});
    return c;
}

void SelectionLimiter::modelsChanged(int models)
{
    int lim, max;
    foreach (int ind, _limits.keys())
    {
        if (ind < 0)
        {
            max = _limits.value(ind).max;
            lim = models/_limits.value(ind).per;
            _limits[ind].max = lim;
            emit modelChange(lim-max, ind);
        }
    }
}

void SelectionLimiter::buttonPress(bool check, int group, int col, int index)
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
        emit disableButtons(dsable, group, col, index);
}
