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

ItemSatelliteSelectionLimiter::ItemSatelliteSelectionLimiter(int max,
                                                             int type,
                                                             QObject *parent)
    : ItemSatellite(parent)
    , _limit(max)
    , _current(0)
    , _type(type)
{

}

void ItemSatelliteSelectionLimiter::on_itemSelected(int b, int)
{
    if (b < 0 && _current >= _limit)
    {
        emit limitReach(-_type);
    }

    _current += b;

    if (b > 0 && _current >= _limit)
    {
        emit limitReach(_type);
    }
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

ItemSatelliteModelMirror::ItemSatelliteModelMirror(QObject *parent)
    : ItemSatellite(parent)
{}

void ItemSatelliteModelMirror::on_modelsChanged(int i, bool check,
                                                bool toggle)
{
    if (check || toggle)
        emit currentLimit(i, false);
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

void ItemSatelliteSelectionMirror::on_itemSelected(int b, int)
{
    if (b > 0)
        _selected = true;
    else
        _selected = false;
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
        _clone->on_itemSelected(1,-1);
    ItemSatellite::releaseClone();
}

ItemSatelliteSelectionLimiterModels::ItemSatelliteSelectionLimiterModels
    (int one, int per, int type, int crit, QObject *parent)
    : ItemSatelliteSelectionLimiter(0, type, parent)
    , _currentModels(0)
    , _criticalType(crit)
    , _limitFor(one)
    , _perModel(per)
{}

void ItemSatelliteSelectionLimiterModels::on_itemSelected(int i, int)
{
    emit currentLimit(i, false);
    ItemSatelliteSelectionLimiter::on_itemSelected(i,-1);
}

void ItemSatelliteSelectionLimiterModels::on_modelsChanged(int i, bool,
                                                           bool toggle)
{
    if (!toggle)
    {
        int limitChange = _limit;
        _currentModels += i;
        _limit = _limitFor*(floor(_currentModels/_perModel));
        limitChange = _limit - limitChange;
        emit currentLimit(-limitChange, true);

        if (limitChange < 0 && _current > _limit)
        {
            emit limitReach(_criticalType);
        }

        if (limitChange > 0 && _current <= _limit)
        {
            emit limitReach(-_criticalType);
        }
    }
}

void ItemSatelliteSelectionLimiterModels::createClone()
{
    _clone = new ItemSatelliteSelectionLimiterModels(_limitFor, _perModel,
                                                     _type, _criticalType,
                                                     this);
    _firstCall = true;
}
