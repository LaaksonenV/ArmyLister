#ifndef ITEMSATELLITE_H
#define ITEMSATELLITE_H

#include <QObject>


class ItemSatellite : public QObject
{
    Q_OBJECT
public:
    ItemSatellite(QObject *parent = nullptr);
    virtual ~ItemSatellite(){}

public slots:

    virtual void on_itemChecked(bool){}

    virtual void on_modelsChanged(int){}

    virtual void ping(bool);

    virtual void createClone() = 0;

    void releaseClone();

signals:

    void limitReach(int);

    void currentLimit(int);

    void check(bool);

    void sendConnection(ItemSatellite *, bool);

protected:

    bool _firstCall;

    ItemSatellite *_clone;
};


class ItemSatelliteSelectionLimiter : public ItemSatellite
{
    Q_OBJECT
public:
    ItemSatelliteSelectionLimiter(int max, int type, QObject *parent = nullptr);
    virtual ~ItemSatelliteSelectionLimiter(){}

public:

    virtual void on_itemChecked(bool b);

    virtual void ping(bool status);

    virtual void createClone();

protected:

    void editCurrent(int i);

    int _limit;
    int _current;
    int _type;
};

class ItemSatelliteModelMirror : public ItemSatellite
{
    Q_OBJECT
public:
    ItemSatelliteModelMirror(QObject *parent = nullptr);
    virtual ~ItemSatelliteModelMirror(){}

public:

    virtual void on_modelsChanged(int i);

    virtual void createClone();
};

class ItemSatelliteSelectionMirror : public ItemSatellite
{
    Q_OBJECT
public:
    ItemSatelliteSelectionMirror(QObject *parent = nullptr);
    virtual ~ItemSatelliteSelectionMirror(){}

public:

    virtual void on_itemChecked(bool b);

    virtual void createClone();
};

#endif // ITEMSATELLITE_H
