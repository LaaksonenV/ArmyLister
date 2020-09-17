#include "battleforged.h"

#include <QList>
#include <algorithm>

#include "detachment.h"
#include "organisation.h"

BattleForged::BattleForged(QWidget *parent)
    : Organisation(parent)
    , qv_currentMin()
    , qv_currentMax()
{
    for(int i = 0; i < BattlefieldRole::OverRoles; ++i)
    {
        qv_currentMax << 0;
        qv_currentMin << 0;
    }

}

BattleForged::~BattleForged()
{}


void BattleForged::addPart(const QStringList &args)
{
    Detachment *det = new Detachment(args, this);
//    int indx = qv_detachmentList.count();
    connect(det, &Detachment::toggled,
            this, &BattleForged::detachmentSelected);
    connect(det, &Detachment::clone,
            this, &BattleForged::detachmentCloned);
//                    {detachmentSelected(indx,b);});
    det->move(0,det->sizeHint().height()*qv_partsList.count());
    qv_partsList << det;

}

void BattleForged::roleSelected(int role, int amount)
{
    foreach (Detachment *d, qv_partsList)
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
    foreach (Detachment *d, qv_partsList)
//        if (i != index)
        if (d != det)
//            qv_detachmentList.at(i)->selectionChange(mins, maxs);
            d->selectionChange(mins, maxs);
}

void BattleForged::detachmentCloned(Detachment *det, QStringList args)
{
    int indx = qv_partsList.indexOf(det);
    Detachment *d = new Detachment(args, this);
    connect(d, &Detachment::toggled,
            this, &BattleForged::detachmentSelected);
    connect(d, &Detachment::clone,
            this, &BattleForged::detachmentCloned);
    ++indx;
    qv_partsList.insert(indx, d);
    d->selectionChange(qv_currentMin, qv_currentMax);
    d->show();
    for (int i = indx;
         i < qv_partsList.count(); ++i)
    {
        d = qv_partsList.at(i);
        d->move(0,d->sizeHint().height()*i);
    }
}
