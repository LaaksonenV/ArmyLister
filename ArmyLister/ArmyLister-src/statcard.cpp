#include "statcard.h"

StatCard::StatCard(const QStringList& stats,
                   const QStringList& rules)
    : stats_(stats)
    , items_(QList<StatCard*>())
    , rules_(rules)
{}

void StatCard::addStat(StatCard *stat)
{
    items_ << stat;
}

void StatCard::removeStat(StatCard *stat)
{
    items_.removeOne(stat);
}

