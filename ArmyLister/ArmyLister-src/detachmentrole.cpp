#include "detachmentrole.h"

#include <QIcon>
#include <QPainter>
#include <QBrush>

DetachmentRoleType::DetachmentRoleType(QPixmap icon, QWidget *parent)
    : QWidget(parent)
    , icon_(icon)
    , min_(0)
    , max_(-1)
    , current_(0)
    , effectiveMin_(0)
    , effectiveMax_(0)
{
    setFixedSize(50,50);
}

void DetachmentRoleType::setLimits(int min, int max)
{
    setMax(max);
    setMin(min);
}

void DetachmentRoleType::setMax(int max)
{
    max_ = max;
    effectiveMax_ = max_;
    emit betweenLimits(isBetweenLimits());
    update();
}

void DetachmentRoleType::setMin(int min)
{
    min_ = min;
    effectiveMin_ = min_;
    emit betweenLimits(isBetweenLimits());
    update();
}

int DetachmentRoleType::getMin()
{
    return min_;
}

int DetachmentRoleType::getMax()
{
    return max_;
}

bool DetachmentRoleType::isBetweenLimits()
{
    if (current_ < effectiveMin_
            || (max_ >= 0 && current_ > effectiveMax_))
        return false;
    return true;
}

void DetachmentRoleType::selected(int amount)
{
    current_ += amount;
    update();
}

void DetachmentRoleType::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QRect r(QPoint(), size());
    if (effectiveMax_ >= 0 && current_ > effectiveMax_)
        p.fillRect(r, "#ff8888");
    else if (isBetweenLimits())
        p.fillRect(r, "#88ff55");

    else
    {
        p.fillRect(r,Qt::white);
        if (effectiveMin_ && current_ < effectiveMin_)
        {
            r.setBottom(r.height()-(r.height() * current_ / effectiveMin_));
            p.fillRect(r,Qt::lightGray);
            
        }
    }
    p.drawPixmap(QRect(QPoint(), size()), icon_);
    
}

void DetachmentRoleType::on_Selection(int incrMin, int incrMax)
{  
    effectiveMin_ += incrMin;
    effectiveMax_ += incrMax;
    emit betweenLimits(isBetweenLimits());
    update();
}
