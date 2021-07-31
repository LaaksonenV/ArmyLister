#include "modelitemslot.h"

#include "settings.h"

ModelItemSlot::ModelItemSlot(ModelItemBase *parent)
    : ModelItemBasic(parent)
//    , _slot(slot)
    , _currentIndex(-1)
//    , _currentText(QString())
    , _expanded(false)
    , _trunk(parent)
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
    if (_expanded)
        return QSize(300,
                     (visibleItems())*Settings::ItemMeta(Settings::ItemHeight));
    return QSize(300, 0);
}

void ModelItemSlot::addItem(ModelItemBase *item)
{
    ModelItemBase::addItem(item);

    if (_branches.count() == 1)
        item->toggleCheck();
}

void ModelItemSlot::setTrunk(ModelItemBase *item)
{
    QWidget::setParent(item);
    _trunk = item;
}

QString ModelItemSlot::getText() const
{
    if (_currentIndex < 0)
        return QString();
    return _branches.at(_currentIndex)->getText();
}
int ModelItemSlot::getCurrentCount() const
{
    return _trunk->getCurrentCount();
}

void ModelItemSlot::passSpecialUp(const QStringList &list, bool check)
{
    _trunk->passSpecialUp(list, check);
}

void ModelItemSlot::passCostUp(int c, bool forall, int role)
{
    _trunk->passCostUp(c, forall, role);
}

int ModelItemSlot::getCost() const
{
    if (_currentIndex < 0)
        return 0;
    return _branches.at(_currentIndex)->getCost();
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
        setFixedHeight(Settings::ItemMeta(Settings::ItemHeight) * (toMove));

     _trunk->branchExpanded(_index, toMove);
}

bool ModelItemSlot::branchSelected(int check, int role, int ind, int)
{

    if (check > 0) // A branch is toggled to selected
    {
        // This case is only reached, if current index is changed
        // programmatically, meaning other than default was selected, causing
        // default (index 0) to be toggled
        if (_currentIndex == ind)
            return _trunk->branchSelected(check, role, _slot);

        // change current index to new one
        int oldIndex = _currentIndex;
        _currentIndex = ind;

        // if trunk disallows change, reset changes and return
        if (!_trunk->branchSelected(check, role, _slot))
        {
            _currentIndex = oldIndex;
            return false;
        }

        // if a branch was previously selected, it must be deselected
        // note, currentindex != oldindex
        if (oldIndex >= 0)
            _branches.at(oldIndex)->toggleCheck();

    }
    else // A branch is toggled to deselected
    {
        // if current index is same as branch's, toggle was manual
        if (_currentIndex == ind)
        {
            // the first choice is default, so it may not be deselected
            // manually
            if (_currentIndex == 0)
                return false;

            // if trunk for some reasons disallows deselection, return
            if (!_trunk->branchSelected(check, _slot, role))
                return false;

            // other deselections will cause the default to be selected
            _currentIndex = 0;
            _branches.at(_currentIndex)->toggleCheck();
        }
        // otherwise deselection was the effect of selecting other branch,
        // so nothing else should be done, just pass the effect on
        else
            return _trunk->branchSelected(check, _slot, role);
    }
    return true;

}

void ModelItemSlot::toggleExpand()
{
    _expanded = !_expanded;
    if (_expanded)
        expand(true);
    else
        expand(false);
}
