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
#include "settings.h"

Detachment::Detachment(const QStringList &args, QWidget *parent)
    : OrganisationRole(args, parent)
    , argsList_(args)
    , slotList_()
    , availableList_()
    , handlerList_()
    , points_(0)
    , bAvailable_(false)
    , bSelected_(false)
    , bHasMouse_(false)
{
    setFixedWidth(s_iconSideLength_ * BattlefieldRole::eBattlefieldRole_OverRoles
                  + 2*s_frameWidth_);

    
    QStringList line(args.at(0).split(','));
    if (line.count() < 2)
    {
        printFaultLabel();
        return;
        
    } 
    name_ = line.at(0);
    points_ = line.at(1).toInt();

    if (args.count() > 1)
    {
        DetachmentRoleType *slot;
        for (int i = 0; i < BattlefieldRole::eBattlefieldRole_OverRoles; ++i)
        {
            slotList_ << nullptr;
            handlerList_ << nullptr;
        }
        int role;
        bAvailable_ = true;
        for (int i = 1; i < args.count(); ++i)
        {
            line = args.at(i).split(',');
            if (line.count() <= SlotIndex::eRole)
                continue;
            role = line.at(SlotIndex::eRole).toInt();
            slot = new DetachmentRoleType(getIcon(role), this);
            slotList_[line.at(SlotIndex::eRole).toInt()] = slot;
            slot->setFixedSize(s_iconSideLength_,s_iconSideLength_);
            slot->move((i-1)*s_iconSideLength_ + s_frameWidth_
                       , s_labelHeight_ + s_frameWidth_);

            if (line.count() > SlotIndex::eMin)
                slot->setMin(line.at(SlotIndex::eMin).toInt());

            if (line.count() > SlotIndex::eMax)
                slot->setMax(line.at(SlotIndex::eMax).toInt());
            
            if (line.count() > SlotIndex::eSpecial)
            {
                if (line.at(SlotIndex::eSpecial).toInt()
                        == SpecialSlot::eSpecialSlot_LimitByMax)
                    handlerList_[line.at(SlotIndex::eRole).toInt()]
                            = new MaxHandler(slot, this);
                else if (line.at(SlotIndex::eSpecial).toInt()
                         == SpecialSlot::eSpecialSlot_LimitByMin)
                    handlerList_[line.at(SlotIndex::eRole).toInt()]
                            = new MinHandler(slot, this);
            }

            availableList_ << slot->isBetweenLimits();
            if (!slot->isBetweenLimits())
                bAvailable_ = false;
            connect(slot, &DetachmentRoleType::betweenLimits,
                    this, [=](bool b){on_betweenLimits(i-1,b);});
        }

    }
}

Detachment::~Detachment()
{

}

void Detachment::roleSelected(int role, int amount)
{
    if (slotList_.at(role) == nullptr)
        return;

    slotList_.at(role)->selected(amount);

    if (handlerList_.at(role) == nullptr)
        return;

    LimitHandler *p;
    for(int i = 0; i < handlerList_.count(); ++i)
    {
        if(i == role)
            continue;
        p = handlerList_.at(i);
        if (p)
            p->selection(amount);
    }
}

void Detachment::selectionChange(const QList<int> &mins,
                                 const QList<int> &maxs)
{
    if (mins.count() != slotList_.count()
            || maxs.count() != slotList_.count())
        return;
    for (int i = 0; i < BattlefieldRole::eBattlefieldRole_OverRoles; ++i)
        if (slotList_.at(i) != nullptr)
            slotList_.at(i)->on_Selection(mins.at(i), maxs.at(i));

}

QList<int> Detachment::getMinimums()
{
    QList<int> ret;
    for (int i = 0; i < BattlefieldRole::eBattlefieldRole_OverRoles; ++i)
    {
        if (slotList_.at(i) != nullptr)
            ret << slotList_.at(i)->getMin();
        else
            ret << 0;
    }
    return ret;
}

QList<int> Detachment::getMaximums()
{
    QList<int> ret;
    for (int i = 0; i < BattlefieldRole::eBattlefieldRole_OverRoles; ++i)
    {
        if (slotList_.at(i) != nullptr)
            ret << slotList_.at(i)->getMax();
        else
            ret << 0;
    }
    return ret;
}

int Detachment::getPoints()
{
    return points_;
}

void Detachment::enterEvent(QEvent *)
{
    bHasMouse_ = true;
    update();
}

void Detachment::leaveEvent(QEvent *)
{
    bHasMouse_ = false;
    update();
}

void Detachment::mouseReleaseEvent(QMouseEvent *)
{
    bSelected_ = !bSelected_;
    emit toggled(this, bSelected_);
    update();
}

void Detachment::mouseDoubleClickEvent(QMouseEvent *)
{
    emit clone(argsList_);
}

void Detachment::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QPen pen;
    QRect r(QPoint(), size());
    r.adjust(2,2,-2,-2);
    if (bSelected_)
        pen.setColor(Qt::blue);

    else if (bHasMouse_)
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
    if (bAvailable_)
        p.fillRect(r,Qt::white);
    pen.setColor(Qt::black);
    p.setPen(pen);
    r.setTopLeft(QPoint(s_frameWidth_,s_frameWidth_));
    r.setBottomRight(QPoint(width()-s_frameWidth_-s_iconSideLength_,
                            s_labelHeight_+s_frameWidth_));
    p.setFont(Settings::Font(Settings::eFont_OrgFont));
    p.drawText(r,Qt::AlignVCenter, name_);

    r.setRight(width()-s_frameWidth_);
    r.setLeft(width()-s_frameWidth_-s_iconSideLength_);
    p.drawText(r, Qt::AlignCenter, QString::number(points_));
}

void Detachment::on_betweenLimits(int from, bool check)
{
    if (availableList_.count() > from)
        availableList_[from] = check;
    if (!check)
    {
        bAvailable_ = false;
        update();
        return;
    }
    foreach (bool b, availableList_)
        if (!b)
        {
            bAvailable_ = false;
            update();
            return;
        }
    bAvailable_ = true;
    update();
}


QPixmap Detachment::getIcon(int ofRole)
{
    switch (ofRole)
    {
    case BattlefieldRole::eBattlefieldRole_HQ:
        return QPixmap(":/icons/HQ");
    case BattlefieldRole::eBattlefieldRole_Troop:
        return QPixmap(":/icons/T");
    case BattlefieldRole::eBattlefieldRole_Elite:
        return QPixmap(":/icons/E");
    case BattlefieldRole::eBattlefieldRole_Fast:
        return QPixmap(":/icons/FA");
    case BattlefieldRole::eBattlefieldRole_Heavy:
        return QPixmap(":/icons/HS");
    case BattlefieldRole::eBattlefieldRole_Transport:
        return QPixmap(":/icons/TP");
    case BattlefieldRole::eBattlefieldRole_Flyer:
        return QPixmap(":/icons/F");
    case BattlefieldRole::eBattlefieldRole_Fort:
        return QPixmap(":/icons/Fort");
    case BattlefieldRole::eBattlefieldRole_Lord:
        return QPixmap(":/icons/Lord");
    default:
        return QPixmap();
    }
}
