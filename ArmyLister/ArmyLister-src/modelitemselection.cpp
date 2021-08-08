#include "modelitemselection.h"

#include <QPushButton>

#include "modelitemslot.h"
#include "settings.h"

ModelItemSelection::ModelItemSelection(ModelItemBase *parent)
    : ModelItemBasic(parent)
    , slots_(QList<SelectionSlot>())
{
    show();
}

ModelItemSelection::ModelItemSelection(ModelItemSelection *source,
                                       ModelItemBase *parent)
    : ModelItemBasic(source, parent)
    , slots_(QList<SelectionSlot>())
{
    show();

    for (int i = 0; i < source->slots_.count(); ++i)
    {
        setText(source->slots_.at(i).defaultText_, i);
        setCost(source->slots_.at(i).defaultCost_, i);
    }
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

    slots_[slot].defaultText_ = text;

    updateItem();
}

void ModelItemSelection::setCost(int i, int slot)
{
    if (slot < 0) // if slot is not defined, setting cost is meaningless
        return;

    fillSlots(slot);

    slots_[slot].defaultCost_ = i;

    ModelItemBase::setCost(i);
}

void ModelItemSelection::updateItem()
{
    QStringList newtext;

    foreach (SelectionSlot s, slots_)
    {
        if (s.index_ >= 0)
            newtext << branches_.at(current_)->getText();
        else if (s.index_ == -1)
            newtext << s.defaultText_;
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
    if (slot >= slots_.count())
    {
//        qDebug() << "Slot over board " + getText();
        return false;
    }

    // selections from non-slots don't matter to selection
    if (slot < 0)
        return ModelItemBasic::branchSelected(check, role, index);

    if (check >= 0)
    {
        if (slots_.at(slot).index_ == -1) // disable default if needed
            passCostUp(-slots_[slot].defaultCost_);

        // check if current slot is used by different item and needs to change
        if (slots_.at(slot).index_ != index)
        {
            slots_[slot].index_ = -2; // slot is changing
            branches_.at(slots_.at(slot).index_)->toggleCheck();
            slots_[slot].index_ = index;
        }
        updateItem();

    }
    // slot unchecked, enable default on slot if not currently changing
    else if (slots_.at(slot).index_ != -2)
    {
        slots_[slot].index_ = -1;
        passCostUp(slots_[slot].defaultCost_);
        updateItem();
    }

    if (role)
        ModelItemBasic::branchSelected(check, role, index);
    return true;
}

void ModelItemSelection::fillSlots(int c)
{
    while (slots_.count() <= c)
    {
        slots_ << SelectionSlot();
    }

    slots_[c].index_ = -1;
}
