#include "slotslabel.h"
#include "slotselection.h"
#include "settings.h"
#include "structs.h"
//#include "modelitem.h"

#include <QPainter>
#include <QMouseEvent>

BaseLabel::BaseLabel(QWidget *parent, Settings *set, const QString& text)
    : QLabel (text, parent)
{
    setFixedHeight(set->itemHeight);
    QFont f(QString(set->font),
            set->textFontSize);
    f.setBold(true);
    setFont(f);

    move(set->expandButtonSize+10,0);
}

SlotLabel::SlotLabel(QWidget *parent, Settings *set)
    : BaseLabel(parent, set)
//    , _modelParent(parent)
    , _defaultTexts(QStringList())
    , _defaultPoints(QList<int>())
    , _selection(nullptr)
//    , _selectionDenied(false)
    , _mouseIn(false)
{
}

/*SlotLabel::SlotLabel(SlotLabel *copy, ModelItem *parent)
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
                this, &SlotLabel::on_selection);
    }
    show();
}*/

SlotLabel::~SlotLabel()
{
    if (_selection)
        delete _selection;
}

/*void SlotLabel::reset()
{
    _modelParent->changeSelection(_position, _baseText, _basePoints);
}*/

void SlotLabel::setDefault(Gear& def, int at)
{
    while (_defaultTexts.count() <= at)
    {
        _defaultTexts << "";
        _defaultPoints << 0;
    }

    _defaultTexts.replace(at, def.name);
    _defaultPoints.replace(at, def.cost);

    setTexts(_defaultTexts);
}

void SlotLabel::addSelection(QList<Gear > list, int at)
{
    if (!_selection)
    {
        _selection = new SlotSelection(font());
        connect(_selection, &SlotSelection::selected,
                this, &SlotLabel::on_selection);
    }

    // check if the default exists in the list, and add if not
    if (_defaultTexts.count() > at)
    {
        bool found = false;
        for (int i = 0; i < list.count(); ++i)
            if (list.at(i).name == text())
                found = true;

        if (!found)
            list.prepend(Gear{_defaultTexts.at(at),_defaultPoints.at(at)});
    }
    else
        list.prepend(Gear{"-",0}); // Move to when showing for first time so setdefault may be called after adding


    _selection->addSelection(list, at);
}

/*void SlotLabel::removeSelection(const QString &text)
{
    if (hasSelections())
        _selection->removeSelection(text);
}*/

/*void SlotLabel::denySelection(bool denied)
{
    _selectionDenied = denied;
}*/

bool SlotLabel::hasSelections() const
{
    if (_selection)
        return _selection->hasSelections();
    return false;
}

/*void SlotLabel::select(const QString &s)
{
    if (_selection)
        _selection->select(s);
}*/

void SlotLabel::enterEvent(QEvent *)
{
    if (hasSelections())
        _mouseIn = true;
}

void SlotLabel::leaveEvent(QEvent *)
{
    _mouseIn = false;
}

void SlotLabel::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::RightButton && hasSelections())
//        && !_selectionDenied)
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

void SlotLabel::paintEvent(QPaintEvent *e)
{
    QLabel::paintEvent(e);
    QPainter p(this);

    if (hasSelections())// && !_selectionDenied)
    {
        p.drawRect(0,height()-3,width()-1,2);
        if (_mouseIn)
        {
            p.drawRect(0,0,width()-1,height()-1);
        }
    }
}

void SlotLabel::setText(const QString &)
{
//    QLabel::setText(s);
}

void SlotLabel::setTexts(const QStringList& s)
{
    QString text = s.join(", ");
    QLabel::setText(text);
}

/*void SlotLabel::setPoint(int p)
{
    _basePoints = p;
}*/

void SlotLabel::on_selection(const QStringList &text, int cost)
{

    setTexts(text);
    emit selectedCost(cost);
}



/*MultiSelectionHandler::MultiSelectionHandler(ModelItem *parent)
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
