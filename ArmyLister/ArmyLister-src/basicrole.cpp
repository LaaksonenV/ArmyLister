#include "basicrole.h"

#include <QStringList>
#include <QPainter>
#include <QMargins>

#include "settings.h"

BasicRole::BasicRole(const QStringList &args, QWidget *parent)
    : OrganisationRole(args, parent)
    , _printText(QString())
    , _max(0)
    , _current(0)
    , _globalMax(0)
 //   , _currentMax(0)
{
    _printText = vq_name + "0";
    if (args.count() > 1)
    {
        _max = args.at(1).toInt();
        _printText = vq_name + " 0/" + QString::number(_max);
    }
}

BasicRole::~BasicRole()
{

}

void BasicRole::setGlobalMax(int n)
{
    _globalMax = n;

    if (_max != 0)
        _max = _globalMax*_max/100;

    if (_max == 0)
        _printText = vq_name + "   " + QString::number(_current)
                + "/" + QString::number(_globalMax);
    else if (_max < 0)
        _printText = vq_name + "   " + QString::number(-_max)
                + "/" + QString::number(_current)
                + "/" + QString::number(_globalMax);
    else
        _printText = vq_name + "   " + QString::number(_current)
                + "/" + QString::number(_max)
                + "/" + QString::number(_globalMax);
    update();
}

void BasicRole::roleSelected(int, int amount)
{
    _current += amount;

    if (_max == 0)
        _printText = vq_name + "   " + QString::number(_current)
                + "/" + QString::number(_globalMax);
    else if (_max < 0)
        _printText = vq_name + "   " + QString::number(-_max)
                + "/" + QString::number(_current)
                + "/" + QString::number(_globalMax);
    else
        _printText = vq_name + "   " + QString::number(_current)
                + "/" + QString::number(_max)
                + "/" + QString::number(_globalMax);

    update();
}

void BasicRole::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QPen pen;
    QRect r(rect());
    QMargins m(c_frameWidth,c_frameWidth,c_frameWidth,c_frameWidth);

    // paint borders
    pen.setColor(Qt::blue);
    pen.setWidth(c_frameWidth/2);
    p.setPen(pen);
    m -= c_frameWidth/2;
    p.drawRect(r.marginsRemoved(m));

    pen.setColor(Qt::gray);
    pen.setWidth(1);
    p.setPen(pen);
    m += c_frameWidth/2;
    p.drawRect(r.marginsRemoved(m));

    // paint filler
    r = r.marginsRemoved(m);
    p.fillRect(r,Settings::Color(Settings::ColorNeutral));
    if (_globalMax)
    {
        r.setWidth(r.width()*_current/_globalMax);
        if (_max > 0)
        {
            if (_current <= _max)
                p.fillRect(r, Settings::Color(Settings::ColorOk));
            else
                p.fillRect(r, Settings::Color(Settings::ColorNot));
        }
        else if (_max < 0)
        {
            if (_current <= -_max)

                p.fillRect(r, Settings::Color(Settings::ColorNot));
            else
                p.fillRect(r, Settings::Color(Settings::ColorOk));
        }
        else
        {
            p.fillRect(r, Qt::white);
        }
    }

    // paint text
    pen.setColor(Qt::gray);
    pen.setWidth(2);
    p.setPen(pen);

    r = rect().marginsRemoved(m);

    if (_max != 0)
    {
        int d = r.width()*_max/_globalMax;
        if (_max < 0)
            d = -d;

        p.drawLine(d,r.top(),d,r.bottom());
    }

    QFont f(Settings::font);
    f.setPointSize(c_fontsize);
    f.setBold(true);
    pen.setColor(Qt::black);
    pen.setWidth(1);
    p.setPen(pen);

    p.setFont(f);
    p.drawText(r, _printText);

}
