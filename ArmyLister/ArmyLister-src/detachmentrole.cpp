#include "detachmentrole.h"

#include <QIcon>
#include <QPainter>
#include <QBrush>

RoleSlot::RoleSlot(QPixmap icon, QWidget *parent)
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

void RoleSlot::setLimits(int min, int max)
{
    setMax(max);
    setMin(min);
}

void RoleSlot::setMax(int max)
{
    v_max = max;
    _effectiveMax = v_max;
    emit betweenLimits(isBetweenLimits());
    update();
}

void RoleSlot::setMin(int min)
{
    v_min = min;
    _effectiveMin = v_min;
    emit betweenLimits(isBetweenLimits());
    update();
}

int RoleSlot::getMin()
{
    return v_min;
}

int RoleSlot::getMax()
{
    return v_max;
}

bool RoleSlot::isBetweenLimits()
{
    if (v_current < _effectiveMin
            || (v_max >= 0 && v_current > _effectiveMax))
        return false;
    return true;
}

void RoleSlot::selected(int amount)
{
    v_current += amount;
    update();
}

void RoleSlot::paintEvent(QPaintEvent *)
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

void RoleSlot::on_Selection(int incrMin, int incrMax)
{  
    _effectiveMin += incrMin;
    _effectiveMax += incrMax;
    emit betweenLimits(isBetweenLimits());
    update();
}
