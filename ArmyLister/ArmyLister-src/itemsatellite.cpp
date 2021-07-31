#include "itemsatellite.h"

ItemSatellite::ItemSatellite(QObject *parent)
    : QObject(parent)
    , firstCall_(false)
    , clone_(nullptr)
{

}

void ItemSatellite::ping(bool)
{
    if (clone_)
    {
        emit sendConnection(clone_, firstCall_);
        if (firstCall_)
            firstCall_ = false;
    }
    else
        emit sendConnection(this, false);
}

void ItemSatellite::releaseClone()
{
    clone_ = nullptr;
}

ItemSatelliteSelectionLimiter::ItemSatelliteSelectionLimiter(int max,
                                                             int type,
                                                             QObject *parent)
    : ItemSatellite(parent)
    , limit_(max)
    , current_(0)
    , type_(type)
{

}

void ItemSatelliteSelectionLimiter::on_itemSelected(int b, int)
{
    if (b < 0 && current_ >= limit_)
    {
        emit limitReach(-type_);
    }

    current_ += b;

    if (b > 0 && current_ >= limit_)
    {
        emit limitReach(type_);
    }
}

void ItemSatelliteSelectionLimiter::ping(bool status)
{
    if (status)
    {
        if(current_ >= limit_)
            emit limitReach(type_);
    }
    else ItemSatellite::ping(status);
}

void ItemSatelliteSelectionLimiter::createClone()
{
    clone_ = new ItemSatelliteSelectionLimiter(limit_, type_, this);
    firstCall_ = true;
}

ItemSatelliteModelMirror::ItemSatelliteModelMirror(QObject *parent)
    : ItemSatellite(parent)
{}

void ItemSatelliteModelMirror::on_modelsChanged(int i, bool check,
                                                bool toggle)
{
    if (check || toggle)
        emit currentLimit(i, false);
}

void ItemSatelliteModelMirror::createClone()
{
    clone_ = new ItemSatelliteModelMirror(this);
    firstCall_ = true;
}

ItemSatelliteSelectionMirror::ItemSatelliteSelectionMirror(QObject *parent)
    : ItemSatellite(parent)
    , selected_(false)
{}

void ItemSatelliteSelectionMirror::on_itemSelected(int b, int)
{
    if (b > 0)
        selected_ = true;
    else
        selected_ = false;
    emit check(b);
}

void ItemSatelliteSelectionMirror::createClone()
{
    clone_ = new ItemSatelliteSelectionMirror(this);
    firstCall_ = true;
}

void ItemSatelliteSelectionMirror::releaseClone()
{
    if (selected_)
        clone_->on_itemSelected(1,-1);
    ItemSatellite::releaseClone();
}

ItemSatelliteSelectionLimiterModels::ItemSatelliteSelectionLimiterModels
    (int one, int per, int type, int crit, QObject *parent)
    : ItemSatelliteSelectionLimiter(0, type, parent)
    , currentModels_(0)
    , criticalType_(crit)
    , limitFor_(one)
    , perModel_(per)
{}

void ItemSatelliteSelectionLimiterModels::on_itemSelected(int i, int)
{
    emit currentLimit(i, false);
    ItemSatelliteSelectionLimiter::on_itemSelected(i,-1);
}

void ItemSatelliteSelectionLimiterModels::on_modelsChanged(int i, bool,
                                                           bool toggle)
{
    if (!toggle)
    {
        int limitChange = limit_;
        currentModels_ += i;
        limit_ = limitFor_*(floor(currentModels_/perModel_));
        limitChange = limit_ - limitChange;
        emit currentLimit(-limitChange, true);

        if (limitChange < 0 && current_ > limit_)
        {
            emit limitReach(criticalType_);
        }

        if (limitChange > 0 && current_ <= limit_)
        {
            emit limitReach(-criticalType_);
        }
    }
}

void ItemSatelliteSelectionLimiterModels::createClone()
{
    clone_ = new ItemSatelliteSelectionLimiterModels(limitFor_, perModel_,
                                                     type_, criticalType_,
                                                     this);
    firstCall_ = true;
}
