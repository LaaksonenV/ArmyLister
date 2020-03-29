#ifndef STRUCTS_H
#define STRUCTS_H

#include <QStringList>

struct Gear
{
    QString name;
    int cost;
};

struct PointContainer
{
    QString text;
    QStringList special;
    int points;
    int min;
    int max;
    Gear specialcase;

    PointContainer()
        : text(QString())
        , special(QStringList())
        , points(0)
        , min(0)
        , max(0)
        , specialcase({"",0})
    {
    }

    PointContainer(const PointContainer &c)
        : text(c.text)
        , special(c.special)
        , points(c.points)
        , min(c.min)
        , max(c.max)
        , specialcase(c.specialcase)
    {
    }


};


#endif // STRUCTS_H
