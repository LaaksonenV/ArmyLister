#include "battleforged.h"

#include <QList>
#include <algorithm>

#include "detachment.h"
#include "organisation.h"

BattleForged::BattleForged(QWidget *parent)
    : Organisation(parent)
//    , qv_partsList()
    , qv_currentMin_()
    , qv_currentMax_()
{
    for(int i = 0; i < BattlefieldRole::eBattlefieldRole_OverRoles; ++i)
    {
        qv_currentMax_ << 0;
        qv_currentMin_ << 0;
    }

}

BattleForged::~BattleForged()
{}


void BattleForged::addPart(const QStringList &args, int)
{
    Detachment *det = new Detachment(args, this);
//    int indx = qv_detachmentList.count();
    connect(det, &Detachment::toggled,
            this, &BattleForged::detachmentSelected);
//    connect(det, &Detachment::clone,
  //          this, &BattleForged::addPart);
//                    {detachmentSelected(indx,b);});
//    connect(this, &Organisation::roleSelected,
  //          det, &Detachment::roleSelected);
    connect(this, &BattleForged::selectionChanging,
            [=](Detachment *det2, QList<int> mins,
            QList<int> maxs){if (det!=det2)det->selectionChange(mins, maxs);});

    insertPart(det);
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

    for (int i = 0; i < qv_currentMax_.count(); ++i)
    {
        qv_currentMax_[i] += maxs.at(i);
        qv_currentMin_[i] += mins.at(i);
    }
/*
//    for (int i = 0; i < qv_detachmentList.count(); ++i)
    foreach (Detachment *d, qv_partsList)
//        if (i != index)
        if (d != det)
//            qv_detachmentList.at(i)->selectionChange(mins, maxs);
            d->selectionChange(mins, maxs);*/
}

/*void BattleForged::detachmentCloned(QStringList args)
{

//    int indx = qv_partsList.indexOf(det);
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
}*/
