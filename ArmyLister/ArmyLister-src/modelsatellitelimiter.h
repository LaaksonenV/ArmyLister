#ifndef MODELITEMLIMITER_H
#define MODELITEMLIMITER_H

#include <QObject>


class ModelSatelliteLimiter : public QObject
{
    Q_OBJECT
public:
    ModelSatelliteLimiter(int max, int type, QObject *parent = nullptr);
    virtual ~ModelSatelliteLimiter(){}

public slots:

    void on_itemChecked(bool b);

    void ping(bool status);

    void createClone();

    void releaseClone();

signals:

    void limitReach(int);

    void sendConnection(ModelSatelliteLimiter *, bool);

private:
    int _limit;
    int _current;

    int _type;

    ModelSatelliteLimiter *_clone;
    bool _firstCall;
};

#endif // MODELITEMLIMITER_H
