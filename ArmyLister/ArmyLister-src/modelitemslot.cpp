#include "modelitemslot.h"

#include "settings.h"

ModelItemSlot::ModelItemSlot(ModelItemBase *parent, const QList<int> &slot)
    : ModelItemBasic(parent)
    , slots_(slot)
    , currentIndex_(-1)
    , defaultText_(QString())
{
    show();
}

ModelItemSlot::ModelItemSlot(ModelItemSlot *source, ModelItemBase *parent)
    : ModelItemBasic(parent)
    , slots_(source->slots_)
    , currentIndex_(-1)
    , defaultText_(source->defaultText_)
{
    show();
    updateText();
}

ModelItemSlot::~ModelItemSlot()
{}

void ModelItemSlot::clone(ModelItemBase *toRoot)
{
    ModelItemSlot *clone = new ModelItemSlot(this, toRoot);
    toRoot->addItem(clone);
    cloning(clone);
}

void ModelItemSlot::setText(const QString &text, int)
{
    defaultText_ = text;
    updateText();
}

void ModelItemSlot::updateText()
{
    ModelItemBasic::setText(QString("Replace " + defaultText_
                                    + " with: " + getText()));
}

QString ModelItemSlot::getText() const
{
    if (currentIndex_ < 0)
        return QString();
    return branches_.at(currentIndex_)->getText();
}

int ModelItemSlot::getCost() const
{
    if (currentIndex_ < 0)
        return 0;
    return branches_.at(currentIndex_)->getCost();
}

bool ModelItemSlot::branchSelected(int check, int role, int ind, int)
{

    if (check > 0)
    {
        // change current index to new one, so when selection permit is asked
        // right info will be transferred
        int oldIndex = currentIndex_;
        currentIndex_ = ind;

/*        // if trunk disallows change, reset changes and return
        if (!ModelItemBasic::branchSelected(check, role, index_, slots_.at(0)))
        {
            currentIndex_ = oldIndex;
            return false;
        }
*/
        // if a branch was previously selected, it must be deselected
        if (oldIndex >= 0)
            branches_.at(oldIndex)->toggleCheck();

    }
    else
    {
        // if current index is same as branch's, toggle was manual
        if (currentIndex_ == ind)
        {
            // if no selections are made, slot is inactive
            if (checked_)
                toggleCheck();
            currentIndex_ = -1;
        }
        // otherwise deselection was the effect of selecting other branch,
        // so nothing else should be done, just pass the effect on
        else
            return trunk_->branchSelected(check, role, index_, -1);
    }
    updateText();
    ModelItemBase::branchSelected(check, role, ind);
    return true;

}

void ModelItemSlot::toggleCheck(int)
{
    // slot selectable only if selection is made
    if (currentIndex_ < 0)
        return;

    // toggle with first slot
    ModelItemBasic::toggleCheck(slots_.at(0));

    // inform trunk of other slots used
    for (int i = 1; i < slots_.count(); ++i)
        trunk_->branchSelected(int(checked_), 0, index_, slots_.at(i));
}
