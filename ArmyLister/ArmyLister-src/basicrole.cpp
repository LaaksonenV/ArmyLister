#include "basicrole.h"

#include <QStringList>
#include <QPainter>
#include <QMargins>

#include "settings.h"

BasicRole::BasicRole(const QStringList &args, QWidget *parent)
    : OrganisationRole(args, parent)
    , printText_(QString())
    , max_(0)
    , current_(0)
    , globalMax_(0)
 //   , _currentMax(0)
{
    printText_ = name_ + "0";
    if (args.count() > 1)
    {
        max_ = args.at(1).toInt();
        printText_ = name_ + " 0/" + QString::number(max_);
    }
}

BasicRole::~BasicRole()
{

}

void BasicRole::setGlobalMax(int n)
{
    globalMax_ = n;

    if (max_ != 0)
        max_ = globalMax_*max_/100;

    if (max_ == 0)
        printText_ = name_ + "   " + QString::number(current_)
                + "/" + QString::number(globalMax_);
    else if (max_ < 0)
        printText_ = name_ + "   " + QString::number(-max_)
                + "/" + QString::number(current_)
                + "/" + QString::number(globalMax_);
    else
        printText_ = name_ + "   " + QString::number(current_)
                + "/" + QString::number(max_)
                + "/" + QString::number(globalMax_);
    update();
}

void BasicRole::roleSelected(int, int amount)
{
    current_ += amount;

    if (max_ == 0)
        printText_ = name_ + "   " + QString::number(current_)
                + "/" + QString::number(globalMax_);
    else if (max_ < 0)
        printText_ = name_ + "   " + QString::number(-max_)
                + "/" + QString::number(current_)
                + "/" + QString::number(globalMax_);
    else
        printText_ = name_ + "   " + QString::number(current_)
                + "/" + QString::number(max_)
                + "/" + QString::number(globalMax_);

    update();
}

void BasicRole::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QPen pen;
    QRect r(rect());
    QMargins m(s_frameWidth_,s_frameWidth_,s_frameWidth_,s_frameWidth_);

    // paint borders
    pen.setColor(Qt::blue);
    pen.setWidth(s_frameWidth_/2);
    p.setPen(pen);
    m -= s_frameWidth_/2;
    p.drawRect(r.marginsRemoved(m));

    pen.setColor(Qt::gray);
    pen.setWidth(1);
    p.setPen(pen);
    m += s_frameWidth_/2;
    p.drawRect(r.marginsRemoved(m));

    // paint filler
    r = r.marginsRemoved(m);
    p.fillRect(r,Settings::Color(Settings::eColor_Neutral));
    if (globalMax_)
    {
        r.setWidth(r.width()*current_/globalMax_);
        if (max_ > 0)
        {
            if (current_ <= max_)
                p.fillRect(r, Settings::Color(Settings::eColor_Ok));
            else
                p.fillRect(r, Settings::Color(Settings::eColor_Not));
        }
        else if (max_ < 0)
        {
            if (current_ <= -max_)

                p.fillRect(r, Settings::Color(Settings::eColor_Not));
            else
                p.fillRect(r, Settings::Color(Settings::eColor_Ok));
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

    if (max_ != 0)
    {
        int d = r.width()*max_/globalMax_;
        if (max_ < 0)
            d = -d;

        p.drawLine(d,r.top(),d,r.bottom());
    }

    pen.setColor(Qt::black);
    pen.setWidth(1);
    p.setPen(pen);

    p.setFont(Settings::Font(Settings::eFont_OrgFont));
    p.drawText(r, printText_);

}
