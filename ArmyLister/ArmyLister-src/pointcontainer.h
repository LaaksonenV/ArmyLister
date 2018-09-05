#ifndef POINTCONTAINER_H
#define POINTCONTAINER_H

#include <QStringList>

struct PointContainer
{
    QString text;
    QStringList special;
    int points;
    int min;
    int max;

    PointContainer()
        : text(QString())
        , special(QStringList())
        , points(0)
        , min(0)
        , max(0)
    {
    }

    PointContainer(const PointContainer &c)
        : text(c.text)
        , special(c.special)
        , points(c.points)
        , min(c.min)
        , max(c.max)
    {
    }


};

#endif // POINTCONTAINER_H
