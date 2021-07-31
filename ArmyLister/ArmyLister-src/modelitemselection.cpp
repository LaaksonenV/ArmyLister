#include "modelitemselection.h"

#include <QPushButton>

#include "modelitemslot.h"
#include "settings.h"

ModelItemSelection::ModelItemSelection(ModelItemBase *parent)
    : ModelItemBasic(parent)
    , _defaults(QStringList())
    , _defaultCosts(QList<int>())
    , _slots(QList<short>())
{
    show();
    setAlwaysChecked();
}

ModelItemSelection::ModelItemSelection(ModelItemSelection *source,
                                       ModelItemBase *parent)
    : ModelItemBasic(source, parent)
    , _defaults(QStringList())
    , _defaultCosts(QList<int>())
    , _slots(QList<short>())
{
    show();
    setAlwaysChecked();
}

ModelItemSelection::~ModelItemSelection()
{}

void ModelItemSelection::clone(ModelItemBase *toRoot, int)
{
    ModelItemSelection *clone = new ModelItemSelection(this, toRoot);
    toRoot->addItem(clone);
    cloning(clone);
}

void ModelItemSelection::printToStream(QTextStream &str)
{
    str.setPadChar(' ');
    print(str, 2);
}

void ModelItemSelection::setText(const QString &text, int slot)
{
    fillSlots(slot);

    _slots[slot]._defaultText = text;

    updateItem();
}

void ModelItemSelection::setCost(int i, int slot)
{
    fillSlots(slot);

    _slots[slot]._defaultCost = i;

    passCostUp(i);
}

void ModelItemSelection::updateItem()
{
    QStringList newtext;

    foreach (SelectionSlot s, _slots)
    {
        if (s._index >= 0)
            newtext << _branches.at(_current)->getText();
        else if (s._type == -1)
            newtext << s._defaultText;
        // else nonting
    }

    if (newtext.count() > 1)
    {
        QString last = newtext.takeLast();
        newtext.last().append(" and " + last);
    }

    ModelItemBasic::setText(newtext.join(", "));
}

bool ModelItemSelection::branchSelected(int check, int role, int index,
                                        int slot)
{
    if (check >= 0)
    {
        // check if able to select
        if (!_trunk->branchSelected(check, slot, role))
            return false;

        if (_slots.at(slot)._index == -1) // disable default if needed
            passCostUp(-_slots[slot]._defaultCost);

        // check if current slot is used by different item and needs to change
        if (_slots.at(slot)._index != index)
        {
            _slots[slot]._index = -2; // slot is changing
            _branches.at(_slots.at(slot)._index)->toggleCheck();
            _slots[slot]._index = index;
        }
        updateItem();

    }
    // slot unchecked, enable default on slot if not currently changing
    else if (_slots.at(slot)._index != -2)
    {
        _slots[slot]._index = -1;
        passCostUp(_slots[slot]._defaultCost);
        updateItem();
    }

    if (role)
        ModelItemBasic::branchSelected(check, role, index);
    return true;
}

void ModelItemSelection::fillSlots(int c)
{
    while (_slots.count() <= c)
    {
        _slots << SelectionSlot();
    }

    _slots[c]._index = -1;
}
