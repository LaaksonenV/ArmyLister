#ifndef LIMITHANDLER_H
#define LIMITHANDLER_H

#include <QObject>

class RoleSlot;

class LimitHandler : public QObject
{
public:
    LimitHandler(RoleSlot *handleThis, QObject *parent = nullptr);
    virtual ~LimitHandler(){}

public slots:
    virtual void selection(int count) = 0;

protected:
    RoleSlot *_handled;

    int _limited;
    int _limiter;
};

class MinHandler : public LimitHandler
{
public:
    MinHandler(RoleSlot *handleThis, QObject *parent = nullptr);
    virtual ~MinHandler(){}

public slots:
    virtual void selection(int count);

};

class MaxHandler : public LimitHandler
{
public:
    MaxHandler(RoleSlot *handleThis, QObject *parent = nullptr);
    virtual ~MaxHandler(){}

public slots:
    virtual void selection(int count);

};


#endif // LIMITHANDLER_H
