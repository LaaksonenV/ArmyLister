#include "limithandler.h"

#include "detachmentrole.h"

LimitHandler::LimitHandler(DetachmentRoleType *handleThis, QObject *parent)
    : QObject(parent)
    , _handled(handleThis)
    , limited_(0)
    , limiter_(0)
{

}

MinHandler::MinHandler(DetachmentRoleType *handleThis, QObject *parent)
    : LimitHandler(handleThis, parent)
{
    limited_ = _handled->getMin();
    _handled->setMin(0);
}

void MinHandler::selection(int count)
{
    limiter_ += count;
    _handled->setMin(limiter_*limited_);
}

MaxHandler::MaxHandler(DetachmentRoleType *handleThis, QObject *parent)
    : LimitHandler(handleThis, parent)
{
    limited_ = _handled->getMax();
    _handled->setMax(0);
}

void MaxHandler::selection(int count)
{
    limiter_ += count;
    _handled->setMax(limiter_*limited_);
}
