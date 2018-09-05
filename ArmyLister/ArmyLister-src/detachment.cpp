#include "detachment.h"

#include <QStringList>
#include <QList>
#include <QCheckBox>
#include <QLabel>
#include <QPainter>
#include <QBrush>
#include <QRect>

#include "detachmentrole.h"
#include "limithandler.h"

Detachment::Detachment(const QStringList &args, QWidget *parent)
    : QWidget(parent)
    , vq_args(args)
    , vq_slotList()
    , vq_availableList()
    , vq_handlerList()
    , vq_name("")
    , v_points(0)
    , v_isAvailable(false)
    , v_isSelected(false)
    , v_hasMouse(false)
{
    setFixedWidth(c_iconSideLength * BattlefieldRole::OverRoles
                  + 2*c_frameWidth);

    if (!args.count())
    {
        printFaultLabel();
        return;
        
    } 
    
    QStringList line(args.at(0).split(','));
    if (line.count() < 2)
    {
        printFaultLabel();
        return;
        
    } 
    vq_name = line.at(0);
    v_points = line.at(1).toInt();

    if (args.count() > 1)
    {
        RoleSlot *slot;
        for (int i = 0; i < BattlefieldRole::OverRoles; ++i)
        {
            vq_slotList << nullptr;
            vq_handlerList << nullptr;
        }
        int role;
        v_isAvailable = true;
        for (int i = 1; i < args.count(); ++i)
        {
            line = args.at(i).split(',');
            if (line.count() <= SlotIndex::Role)
                continue;
            role = line.at(SlotIndex::Role).toInt();
            slot = new RoleSlot(getIcon(role), this);
            vq_slotList[line.at(SlotIndex::Role).toInt()] = slot;
            slot->setFixedSize(c_iconSideLength,c_iconSideLength);
            slot->move((i-1)*c_iconSideLength + c_frameWidth
                       , c_labelHeight + c_frameWidth);

            if (line.count() > SlotIndex::Min)
                slot->setMin(line.at(SlotIndex::Min).toInt());

            if (line.count() > SlotIndex::Max)
                slot->setMax(line.at(SlotIndex::Max).toInt());
            
            if (line.count() > SlotIndex::Special)
            {
                if (line.at(SlotIndex::Special).toInt()
                        == SpecialSlot::LimitByMax)
                    vq_handlerList[line.at(SlotIndex::Role).toInt()]
                            = new MaxHandler(slot, this);
                else if (line.at(SlotIndex::Special).toInt()
                         == SpecialSlot::LimitByMin)
                    vq_handlerList[line.at(SlotIndex::Role).toInt()]
                            = new MinHandler(slot, this);
            }

            vq_availableList << slot->isBetweenLimits();
            if (!slot->isBetweenLimits())
                v_isAvailable = false;
            connect(slot, &RoleSlot::betweenLimits,
                    this, [=](bool b){on_betweenLimits(i-1,b);});
        }
        setFixedHeight(c_iconSideLength + c_labelHeight + 2*c_frameWidth);


    }
    else
        setFixedHeight(c_labelHeight + 2*c_frameWidth);

    setSizePolicy(QSizePolicy());
}

QSize Detachment::sizeHint() const
{
    return size();
}

void Detachment::roleSelected(int role, int amount)
{
    if (vq_slotList.at(role) == nullptr)
        return;

    vq_slotList.at(role)->selected(amount);

    if (vq_handlerList.at(role) == nullptr)
        return;

    LimitHandler *p;
    for(int i = 0; i < vq_handlerList.count(); ++i)
    {
        if(i == role)
            continue;
        p = vq_handlerList.at(i);
        if (p)
            p->selection(amount);
    }
}

void Detachment::selectionChange(const QList<int> &mins,
                                 const QList<int> &maxs)
{
    if (mins.count() != vq_slotList.count()
            || maxs.count() != vq_slotList.count())
        return;
    for (int i = 0; i < BattlefieldRole::OverRoles; ++i)
        if (vq_slotList.at(i) != nullptr)
            vq_slotList.at(i)->on_Selection(mins.at(i), maxs.at(i));

}

QList<int> Detachment::getMinimums()
{
    QList<int> ret;
    for (int i = 0; i < BattlefieldRole::OverRoles; ++i)
    {
        if (vq_slotList.at(i) != nullptr)
            ret << vq_slotList.at(i)->getMin();
        else
            ret << 0;
    }
    return ret;
}

QList<int> Detachment::getMaximums()
{
    QList<int> ret;
    for (int i = 0; i < BattlefieldRole::OverRoles; ++i)
    {
        if (vq_slotList.at(i) != nullptr)
            ret << vq_slotList.at(i)->getMax();
        else
            ret << 0;
    }
    return ret;
}

int Detachment::getPoints()
{
    return v_points;
}

void Detachment::enterEvent(QEvent *)
{
    v_hasMouse = true;
    update();
}

void Detachment::leaveEvent(QEvent *)
{
    v_hasMouse = false;
    update();
}

void Detachment::mouseReleaseEvent(QMouseEvent *)
{
    v_isSelected = !v_isSelected;
    emit toggled(this, v_isSelected);
    update();
}

void Detachment::mouseDoubleClickEvent(QMouseEvent *)
{
    emit clone(this, vq_args);
}

void Detachment::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QPen pen;
    QRect r(QPoint(), size());
    r.adjust(2,2,-2,-2);
    if (v_isSelected)
        pen.setColor(Qt::blue);

    else if (v_hasMouse)
        pen.setColor(Qt::black);

    else
        pen.setColor(Qt::white);

    p.setPen(pen);
    r.adjust(1,1,-1,-1);
    p.drawRect(r);
    r.adjust(1,1,-1,-1);
    p.drawRect(r);
    pen.setColor(Qt::gray);
    p.setPen(pen);
    r.adjust(1,1,-1,-1);
    p.drawRect(r);
    if (v_isAvailable)
        p.fillRect(r,Qt::white);
    pen.setColor(Qt::black);
    p.setPen(pen);
    r.setTopLeft(QPoint(c_frameWidth,c_frameWidth));
    r.setBottomRight(QPoint(width()-c_frameWidth-c_iconSideLength,
                            c_labelHeight+c_frameWidth));
    QFont f = p.font();
    f.setPixelSize(c_fontsize);
    p.setFont(f);
    p.drawText(r,Qt::AlignVCenter, vq_name);

    r.setRight(width()-c_frameWidth);
    r.setLeft(width()-c_frameWidth-c_iconSideLength);
    p.drawText(r, Qt::AlignCenter, QString::number(v_points));
}

void Detachment::on_betweenLimits(int from, bool check)
{
    if (vq_availableList.count() > from)
        vq_availableList[from] = check;
    if (!check)
    {
        v_isAvailable = false;
        update();
        return;
    }
    foreach (bool b, vq_availableList)
        if (!b)
        {
            v_isAvailable = false;
            update();
            return;
        }
    v_isAvailable = true;
    update();
}

void Detachment::printFaultLabel()
{
    vq_name = "Faulty detachment line";
    setFixedHeight(30);
}

QPixmap Detachment::getIcon(int ofRole)
{
    switch (ofRole)
    {
    case BattlefieldRole::HQ:
        return QPixmap(":/icons/HQ");
    case BattlefieldRole::Troop:
        return QPixmap(":/icons/T");
    case BattlefieldRole::Elite:
        return QPixmap(":/icons/E");
    case BattlefieldRole::Fast:
        return QPixmap(":/icons/FA");
    case BattlefieldRole::Heavy:
        return QPixmap(":/icons/HS");
    case BattlefieldRole::Transport:
        return QPixmap(":/icons/TP");
    case BattlefieldRole::Flyer:
        return QPixmap(":/icons/F");
    case BattlefieldRole::Fort:
        return QPixmap(":/icons/Fort");
    case BattlefieldRole::Lord:
        return QPixmap(":/icons/Lord");
    default:
        return QPixmap();
    }
}
