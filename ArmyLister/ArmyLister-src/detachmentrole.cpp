#include "detachmentrole.h"

#include <QIcon>
#include <QPainter>
#include <QBrush>

DetachmentRoleType::DetachmentRoleType(QPixmap icon, QWidget *parent)
    : QWidget(parent)
    , vq_pixm(icon)
    , v_min(0)
    , v_max(-1)
    , v_current(0)
    , _effectiveMin(0)
    , _effectiveMax(0)
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
    v_max = max;
    _effectiveMax = v_max;
    emit betweenLimits(isBetweenLimits());
    update();
}

void DetachmentRoleType::setMin(int min)
{
    v_min = min;
    _effectiveMin = v_min;
    emit betweenLimits(isBetweenLimits());
    update();
}

int DetachmentRoleType::getMin()
{
    return v_min;
}

int DetachmentRoleType::getMax()
{
    return v_max;
}

bool DetachmentRoleType::isBetweenLimits()
{
    if (v_current < _effectiveMin
            || (v_max >= 0 && v_current > _effectiveMax))
        return false;
    return true;
}

void DetachmentRoleType::selected(int amount)
{
    v_current += amount;
    update();
}

void DetachmentRoleType::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QRect r(QPoint(), size());
    if (_effectiveMax >= 0 && v_current > _effectiveMax)
        p.fillRect(r, "#ff8888");
    else if (isBetweenLimits())
        p.fillRect(r, "#88ff55");

    else
    {
        p.fillRect(r,Qt::white);
        if (_effectiveMin && v_current < _effectiveMin)
        {
            r.setBottom(r.height()-(r.height() * v_current / _effectiveMin));
            p.fillRect(r,Qt::lightGray);
            
        }
    }
    p.drawPixmap(QRect(QPoint(), size()), vq_pixm);
    
}

void DetachmentRoleType::on_Selection(int incrMin, int incrMax)
{  
    _effectiveMin += incrMin;
    _effectiveMax += incrMax;
    emit betweenLimits(isBetweenLimits());
    update();
}
