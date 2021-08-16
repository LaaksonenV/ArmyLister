#ifndef STATCARD_H
#define STATCARD_H

#include <QStringList>
#include <QList>

class QPainter;

class StatCard
{
public:
    StatCard(const QStringList& stats,
             const QStringList& rules);
    virtual ~StatCard() = default;

    void addStat(StatCard* stat);
    void removeStat(StatCard* stat);

    virtual void print(QPainter *) = 0;

protected:

    QStringList stats_;

    QList<StatCard*> items_;

    QStringList rules_;

};

class StatCard9A : public StatCard
{
public:
    StatCard9A();
    virtual ~StatCard9A() = default;

    void print(QPainter *){}
};

#endif // STATCARD_H
