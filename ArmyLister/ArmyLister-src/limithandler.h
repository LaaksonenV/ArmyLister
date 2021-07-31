#ifndef LIMITHANDLER_H
#define LIMITHANDLER_H

#include <QObject>

class DetachmentRoleType;

class LimitHandler : public QObject
{
public:
    LimitHandler(DetachmentRoleType *handleThis, QObject *parent = nullptr);
    virtual ~LimitHandler(){}

public slots:
    virtual void selection(int count) = 0;

protected:
    DetachmentRoleType *_handled;

    int limited_;
    int limiter_;
};

class MinHandler : public LimitHandler
{
public:
    MinHandler(DetachmentRoleType *handleThis, QObject *parent = nullptr);
    virtual ~MinHandler(){}

public slots:
    virtual void selection(int count);

};

class MaxHandler : public LimitHandler
{
public:
    MaxHandler(DetachmentRoleType *handleThis, QObject *parent = nullptr);
    virtual ~MaxHandler(){}

public slots:
    virtual void selection(int count);

};


#endif // LIMITHANDLER_H
