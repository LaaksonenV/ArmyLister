#include "limithandler.h"

#include "detachmentrole.h"

LimitHandler::LimitHandler(RoleSlot *handleThis, QObject *parent)
    : QObject(parent)
    , _handled(handleThis)
    , _limited(0)
    , _limiter(0)
{

}

MinHandler::MinHandler(RoleSlot *handleThis, QObject *parent)
    : LimitHandler(handleThis, parent)
{
    _limited = _handled->getMin();
    _handled->setMin(0);
}

void MinHandler::selection(int count)
{
    _limiter += count;
    _handled->setMin(_limiter*_limited);
}

MaxHandler::MaxHandler(RoleSlot *handleThis, QObject *parent)
    : LimitHandler(handleThis, parent)
{
    _limited = _handled->getMax();
    _handled->setMax(0);
}

void MaxHandler::selection(int count)
{
    _limiter += count;
    _handled->setMax(_limiter*_limited);
}
