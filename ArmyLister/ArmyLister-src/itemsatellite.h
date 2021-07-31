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

    virtual void on_itemSelected(int,int){}

    virtual void on_modelsChanged(int, bool, bool){}

    virtual void ping(bool);

    virtual void createClone() = 0;

    virtual void releaseClone();

signals:

    void limitReach(int);

    void currentLimit(int, bool);

    void check(bool);

    void sendConnection(ItemSatellite *, bool);

protected:

    bool firstCall_;

    ItemSatellite *clone_;
};


class ItemSatelliteSelectionLimiter : public ItemSatellite
{
    Q_OBJECT
public:
    ItemSatelliteSelectionLimiter(int max, int type, QObject *parent = nullptr);
    virtual ~ItemSatelliteSelectionLimiter(){}

    virtual void on_itemSelected(int b,int);

    virtual void ping(bool status);

    virtual void createClone();

protected:

    int limit_;
    int current_;
    int type_;
};

class ItemSatelliteModelMirror : public ItemSatellite
{
    Q_OBJECT
public:
    ItemSatelliteModelMirror(QObject *parent = nullptr);
    virtual ~ItemSatelliteModelMirror(){}

    virtual void on_modelsChanged(int i, bool check, bool toggle);

    virtual void createClone();
};

class ItemSatelliteSelectionMirror : public ItemSatellite
{
    Q_OBJECT
public:
    ItemSatelliteSelectionMirror(QObject *parent = nullptr);
    virtual ~ItemSatelliteSelectionMirror(){}

public:

    virtual void on_itemSelected(int b, int);

    virtual void createClone();

    virtual void releaseClone();

private:
    bool selected_;
};

class ItemSatelliteSelectionLimiterModels : public ItemSatelliteSelectionLimiter
{
    Q_OBJECT
public:
    ItemSatelliteSelectionLimiterModels(int one, int per, int type, int crit,
                                        QObject *parent = nullptr);
    virtual ~ItemSatelliteSelectionLimiterModels(){}

    virtual void on_itemSelected(int i, int);

    virtual void on_modelsChanged(int i, bool, bool toggle);

    virtual void createClone();

private:

    int currentModels_;
    int criticalType_;
    int limitFor_;
    int perModel_;
};

#endif // ITEMSATELLITE_H
