#include "battleforged.h"

#include <QList>
#include <algorithm>

#include "detachment.h"

BattleForged::BattleForged(QWidget *parent)
    : QWidget(parent)
    , qv_detachmentList()
    , qv_currentMin()
    , qv_currentMax()
{
    for(int i = 0; i < BattlefieldRole::OverRoles; ++i)
    {
        qv_currentMax << 0;
        qv_currentMin << 0;
    }
    setMinimumSize(100,200);
    setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}

QSize BattleForged::sizeHint() const
{
    if (!qv_detachmentList.count())
        return QSize();
    QSize ret(qv_detachmentList.at(0)->sizeHint());
    ret.rheight() *= qv_detachmentList.count();
    ret.rwidth() += 15;
    return ret;
}

void BattleForged::setLists(const QString &detachments)
{
    clear();
    if (detachments.isEmpty())
        return;
    QStringList lst = detachments.split('#');
    lst.removeFirst();
    foreach (QString s, lst)
    {
        addDetachment(s.split(';'));
    }
    setMinimumSize(sizeHint());
}

void BattleForged::addDetachment(const QStringList &args)
{
    Detachment *det = new Detachment(args, this);
//    int indx = qv_detachmentList.count();
    connect(det, &Detachment::toggled,
            this, &BattleForged::detachmentSelected);
    connect(det, &Detachment::clone,
            this, &BattleForged::detachmentCloned);
//                    {detachmentSelected(indx,b);});
    det->move(0,det->sizeHint().height()*qv_detachmentList.count());
    qv_detachmentList << det;

}

void BattleForged::clear()
{
    foreach (Detachment *d, qv_detachmentList)
    {
        delete d;
    }
    qv_detachmentList.clear();
}

void BattleForged::roleSelected(int role, int amount)
{
    foreach (Detachment *d, qv_detachmentList)
    {
        d->roleSelected(role, amount);
    }
}

void BattleForged::detachmentSelected(Detachment *det, bool check)
{
    QList<int> mins = det->getMinimums();
    QList<int> maxs = det->getMaximums();
    if (!check)
        for (int i = 0; i < mins.count(); ++i)
        {
            mins[i] *= -1;
            if (maxs.at(i) > 0)
                maxs[i] *= -1;
        }

    for (int i = 0; i < qv_currentMax.count(); ++i)
    {
        qv_currentMax[i] += maxs.at(i);
        qv_currentMin[i] += mins.at(i);
    }

//    for (int i = 0; i < qv_detachmentList.count(); ++i)
    foreach (Detachment *d, qv_detachmentList)
//        if (i != index)
        if (d != det)
//            qv_detachmentList.at(i)->selectionChange(mins, maxs);
            d->selectionChange(mins, maxs);
}

void BattleForged::detachmentCloned(Detachment *det, QStringList args)
{
    int indx = qv_detachmentList.indexOf(det);
    Detachment *d = new Detachment(args, this);
    connect(d, &Detachment::toggled,
            this, &BattleForged::detachmentSelected);
    connect(d, &Detachment::clone,
            this, &BattleForged::detachmentCloned);
    ++indx;
    qv_detachmentList.insert(indx, d);
    d->selectionChange(qv_currentMin, qv_currentMax);
    d->show();
    for (int i = indx;
         i < qv_detachmentList.count(); ++i)
    {
        d = qv_detachmentList.at(i);
        d->move(0,d->sizeHint().height()*i);
    }
}
