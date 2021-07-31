#include "modelitemslot.h"

#include "settings.h"

ModelItemSlot::ModelItemSlot(ModelItemBase *parent)
    : ModelItemBasic(parent)
//    , _slot(slot)
    , currentIndex_(-1)
//    , _currentText(QString())
    , bExpanded_(false)
    , trunk_(parent)
{
    show();
}

ModelItemSlot::~ModelItemSlot()
{}

void ModelItemSlot::clone(ModelItemBase *toRoot, int)
{
    ModelItemBase::clone(toRoot, _slot);
}

QSize ModelItemSlot::sizeHint()
{
    if (bExpanded_)
        return QSize(300,
                     (visibleItems())*Settings::ItemMeta(Settings::eItem_Height));
    return QSize(300, 0);
}

void ModelItemSlot::addItem(ModelItemBase *item)
{
    ModelItemBase::addItem(item);

    if (branches_.count() == 1)
        item->toggleCheck();
}

void ModelItemSlot::setTrunk(ModelItemBase *item)
{
    QWidget::setParent(item);
    trunk_ = item;
}

QString ModelItemSlot::getText() const
{
    if (currentIndex_ < 0)
        return QString();
    return branches_.at(currentIndex_)->getText();
}
int ModelItemSlot::getCurrentCount() const
{
    return trunk_->getCurrentCount();
}

void ModelItemSlot::passSpecialUp(const QStringList &list, bool check)
{
    trunk_->passSpecialUp(list, check);
}

void ModelItemSlot::passCostUp(int c, bool forall, int role)
{
    trunk_->passCostUp(c, forall, role);
}

int ModelItemSlot::getCost() const
{
    if (currentIndex_ < 0)
        return 0;
    return branches_.at(currentIndex_)->getCost();
}

void ModelItemSlot::expand(bool expanse)
{
    int toMove = visibleItems();

    if (!expanse)
    {
        setFixedHeight(0);
        toMove = -toMove;
    }
    else
        setFixedHeight(Settings::ItemMeta(Settings::eItem_Height) * (toMove));

     trunk_->branchExpanded(index_, toMove);
}

bool ModelItemSlot::branchSelected(int check, int role, int ind, int)
{

    if (check > 0) // A branch is toggled to selected
    {
        // This case is only reached, if current index is changed
        // programmatically, meaning other than default was selected, causing
        // default (index 0) to be toggled
        if (currentIndex_ == ind)
            return trunk_->branchSelected(check, role, _slot);

        // change current index to new one
        int oldIndex = currentIndex_;
        currentIndex_ = ind;

        // if trunk disallows change, reset changes and return
        if (!trunk_->branchSelected(check, role, _slot))
        {
            currentIndex_ = oldIndex;
            return false;
        }

        // if a branch was previously selected, it must be deselected
        // note, currentindex != oldindex
        if (oldIndex >= 0)
            branches_.at(oldIndex)->toggleCheck();

    }
    else // A branch is toggled to deselected
    {
        // if current index is same as branch's, toggle was manual
        if (currentIndex_ == ind)
        {
            // the first choice is default, so it may not be deselected
            // manually
            if (currentIndex_ == 0)
                return false;

            // if trunk for some reasons disallows deselection, return
            if (!trunk_->branchSelected(check, _slot, role))
                return false;

            // other deselections will cause the default to be selected
            currentIndex_ = 0;
            branches_.at(currentIndex_)->toggleCheck();
        }
        // otherwise deselection was the effect of selecting other branch,
        // so nothing else should be done, just pass the effect on
        else
            return trunk_->branchSelected(check, _slot, role);
    }
    return true;

}

void ModelItemSlot::toggleExpand()
{
    bExpanded_ = !bExpanded_;
    if (bExpanded_)
        expand(true);
    else
        expand(false);
}
