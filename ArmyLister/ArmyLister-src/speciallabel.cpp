/*#include "speciallabel.h"
#include "modelitem.h"

#include <QPainter>
#include <QPushButton>

SpecialLabel::SpecialLabel(const QString &text, ModelItem *parent,
                           int position)
    : QLabel(text, parent)
    , _modelParent(parent)
    , _baseText(text)
    , _basePoints(0)
    , _position(position)
    , _selection(nullptr)
    , _selectionDenied(false)
    , _mouseIn(false)
{
}

SpecialLabel::SpecialLabel(SpecialLabel *copy, ModelItem *parent)
    : SpecialLabel(copy->text(), parent, copy->_position)
{
    setFixedHeight(copy->height());
    setFont(copy->font());
    move(copy->pos());
    _basePoints = copy->_basePoints;
    if (copy->_selection)// && !copy->_selectionDenied)
    {
        _selection = new Selection(copy->_selection);
        connect(_selection, &Selection::selected,
                this, &SpecialLabel::on_selection);
    }
    show();
}

SpecialLabel::~SpecialLabel()
{
    if (_selection)
        delete _selection;
}

void SpecialLabel::reset()
{
    _modelParent->changeSelection(_position, _baseText, _basePoints);
}

void SpecialLabel::addSelection(QList<std::pair<QString, int> > list)
{
    if (!_selection)
    {
        _selection = new Selection(font());
        connect(_selection, &Selection::selected,
                this, &SpecialLabel::on_selection);
    }
    bool found = false;
    for (int i = 0; i < list.count(); ++i)
        if (list.at(i).first == text())
            found = true;

    if (!found)
        list.prepend(std::make_pair(text(),_basePoints));

    _selection->addSelection(list);
}

void SpecialLabel::removeSelection(const QString &text)
{
    if (hasSelections())
        _selection->removeSelection(text);
}

void SpecialLabel::denySelection(bool denied)
{
    _selectionDenied = denied;
}

bool SpecialLabel::hasSelections() const
{
    if (_selection)
        return _selection->hasSelections();
    return false;
}

void SpecialLabel::select(const QString &s)
{
    if (_selection)
        _selection->select(s);
}

void SpecialLabel::enterEvent(QEvent *)
{
    if (hasSelections())
        _mouseIn = true;
}

void SpecialLabel::leaveEvent(QEvent *)
{
    _mouseIn = false;
}

void SpecialLabel::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::RightButton && hasSelections()
        && !_selectionDenied && hasSelections())
    {
        QPoint posi = mapToGlobal(pos());
        posi.ry() += height();

        _selection->move(posi);
        _selection->show();
        _selection->grabMouse();

        e->accept();

    }
    else
        e->ignore();
}

void SpecialLabel::paintEvent(QPaintEvent *e)
{
    QLabel::paintEvent(e);
    QPainter p(this);

    if (hasSelections() && !_selectionDenied)
    {
        p.drawRect(0,height()-3,width()-1,2);
        if (_mouseIn)
        {
            p.drawRect(0,0,width()-1,height()-1);
        }
    }
}

void SpecialLabel::setText(const QString &s)
{
    QLabel::setText(s);
}

void SpecialLabel::setPoint(int p)
{
    _basePoints = p;
}

void SpecialLabel::on_selection(const QString &text)
{
    _selection->releaseMouse();
    _selection->hide();
    _modelParent->changeSelection(_position, text.split(", ").at(0),
                                  text.split(", ").at(1).toInt());
}

Selection::Selection(const QFont &f)
    : QWidget()
    , _f(f)
    , _labels(QList<QPushButton*>())
{
    setWindowFlags(Qt::Popup);
    setFixedSize(0,0);
}

Selection::Selection(Selection *copy)
    : Selection(copy->_f)
{
    QStringList texts;

    for (int i = 0; i < copy->_labels.count(); ++i)
        texts << copy->_labels.at(i)->text();

    createSelection(texts);
}

void Selection::addSelection(const QList<std::pair<QString, int> > &list)
{
    QStringList texts;
    std::pair<QString, int> p;

    for (int i = 0; i < list.count(); ++i)
    {
        p = list.at(i);
        texts << p.first + ", " + QString::number(p.second);

    }

    createSelection(texts);
}

void Selection::removeSelection(const QString &text)
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
}
bool Selection::hasSelections() const
{
    if(_labels.count())
        return true;
    return false;
}

void Selection::mousePressEvent(QMouseEvent *e)
{
    if (!childAt(e->pos()))
    {
        releaseMouse();
        hide();
        e->accept();
    }
}

void Selection::select(const QString &s)
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
}

void Selection::createSelection(const QStringList &list)
{
    QPushButton *label;
    QString text;
    int vpos = this->height();
    int hpos = this->width();

    for (int i = 0; i < list.count(); ++i)
    {
        text = list.at(i);
        label = new QPushButton(text, this);
        label->setFont(_f);
        label->setFlat(true);
        label->setFixedHeight(itemHeight);
        label->move(0, vpos);
        label->show();
        vpos += itemHeight;
        if (label->width() > hpos)
            hpos = label->width();

        connect(label, &QPushButton::clicked,
                this, [=](){emit selected(text);});
        _labels << label;
    }
    setFixedSize(hpos, vpos);
}

MultiSelectionHandler::MultiSelectionHandler(ModelItem *parent)
    : QObject(parent)
    , _ats(QList<int>())
    , _texts(QStringList())
{
    connect(parent, &ModelItem::selectionChanged,
            this, &MultiSelectionHandler::selectionChanged);
}

MultiSelectionHandler::MultiSelectionHandler(MultiSelectionHandler *copy,
                                             ModelItem *parent)
    : QObject(parent)
    , _ats(QList<int>(copy->_ats))
    , _texts(QStringList(copy->_texts))
{
    connect(parent, &ModelItem::selectionChanged,
            this, &MultiSelectionHandler::selectionChanged);
}

void MultiSelectionHandler::addText(const QList<std::pair<QString, int> >
                                    &list)
{
    for (int i = 0; i < list.count(); ++i)
        _texts << list.at(i).first;
}

void MultiSelectionHandler::addAt(int at)
{
    _ats << at;
}

void MultiSelectionHandler::selectionChanged(int at, const QString &text,
                                             ModelItem *copy)
{
    if (_ats.contains(at))
    {
        if (_texts.contains(text))
            for (int i = 0; i < _ats.count(); ++i)
            {
                if (i != at)
                    copy->removeText(i);
            }
        else
            for (int i = 0; i < _ats.count(); ++i)
            {
                if (i != at)
                    foreach (QString s, _texts)
                        copy->removeSelection(at,s);
            }
    }
}
*/
