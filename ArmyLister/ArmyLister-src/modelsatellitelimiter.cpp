#include "modelsatellitelimiter.h"


ModelSatelliteLimiter::ModelSatelliteLimiter(int max, int type,
                                             QObject *parent)
    : QObject(parent)
    , _limit(max)
    , _current(0)
    , _type(type)
    , _clone(nullptr)
    , _firstCall(false)
{

}

void ModelSatelliteLimiter::on_itemChecked(bool b)
{

    if (!b && _current >= _limit)
    {
        emit limitReach(-_type);
    }


    if (b)
        ++_current;
    else
        --_current;

    if (b && _current >= _limit)
    {
        emit limitReach(_type);
    }
}

void ModelSatelliteLimiter::ping(bool status)
{
    if (status)
    {
        if(_current >= _limit)
            emit limitReach(_type);
    }
    else if (_clone)
    {
        emit sendConnection(_clone, _firstCall);
        if (_firstCall)
            _firstCall = false;
    }
    else
        emit sendConnection(this, false);
}

void ModelSatelliteLimiter::createClone()
{
    _clone = new ModelSatelliteLimiter(_limit, _type, this);
    _firstCall = true;
}

void ModelSatelliteLimiter::releaseClone()
{
    _clone = nullptr;
}
