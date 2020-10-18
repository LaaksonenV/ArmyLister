#include "itemsatellite.h"

ItemSatellite::ItemSatellite(QObject *parent)
    : QObject(parent)
    , _firstCall(false)
    , _clone(nullptr)
{

}

void ItemSatellite::ping(bool)
{
    if (_clone)
    {
        emit sendConnection(_clone, _firstCall);
        if (_firstCall)
            _firstCall = false;
    }
    else
        emit sendConnection(this, false);
}

void ItemSatellite::releaseClone()
{
    _clone = nullptr;
}

ItemSatelliteSelectionLimiter::ItemSatelliteSelectionLimiter(int max, int type,
                                             QObject *parent)
    : ItemSatellite(parent)
    , _limit(max)
    , _current(0)
    , _type(type)
{

}

void ItemSatelliteSelectionLimiter::on_itemChecked(bool b)
{
    if (b)
        editCurrent(1);
    else
        editCurrent(-1);
}

void ItemSatelliteSelectionLimiter::ping(bool status)
{
    if (status)
    {
        if(_current >= _limit)
            emit limitReach(_type);
    }
    else ItemSatellite::ping(status);
}

void ItemSatelliteSelectionLimiter::createClone()
{
    _clone = new ItemSatelliteSelectionLimiter(_limit, _type, this);
    _firstCall = true;
}

void ItemSatelliteSelectionLimiter::editCurrent(int i)
{
    if (i < 0 && _current >= _limit)
    {
        emit limitReach(-_type);
    }

    _current += i;

    if (i > 0 && _current >= _limit)
    {
        emit limitReach(_type);
    }
}

ItemSatelliteModelMirror::ItemSatelliteModelMirror(QObject *parent)
    : ItemSatellite(parent)
{}

void ItemSatelliteModelMirror::on_modelsChanged(int i)
{
    emit currentLimit(i);
}

void ItemSatelliteModelMirror::createClone()
{
    _clone = new ItemSatelliteModelMirror(this);
    _firstCall = true;
}

ItemSatelliteSelectionMirror::ItemSatelliteSelectionMirror(QObject *parent)
    : ItemSatellite(parent)
    , _selected(false)
{}

void ItemSatelliteSelectionMirror::on_itemChecked(bool b)
{
    _selected = b;
    emit check(b);
}

void ItemSatelliteSelectionMirror::createClone()
{
    _clone = new ItemSatelliteSelectionMirror(this);
    _firstCall = true;
}

void ItemSatelliteSelectionMirror::releaseClone()
{
    if (_selected)
        _clone->on_itemChecked(true);
    ItemSatellite::releaseClone();
}
