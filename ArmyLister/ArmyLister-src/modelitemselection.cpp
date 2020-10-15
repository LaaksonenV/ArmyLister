#include "modelitemselection.h"

#include <QPushButton>

#include "modelitemslot.h"
#include "settings.h"

ModelItemSelection::ModelItemSelection(Settings *set, ModelItemBase *parent)
    : ModelItemBasic(set, parent)
    , _nextButton(nullptr)
    , _currentSlot(-1)
{
    show();
    setAlwaysChecked();
}

ModelItemSelection::ModelItemSelection(ModelItemSelection *source,
                                       ModelItemBase *parent)
    : ModelItemBasic(source, parent)
    , _nextButton(nullptr)
    , _currentSlot(-1)
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

void ModelItemSelection::addItem(ModelItemBase *item, int slot)
{


    ModelItemBase *slotItem;
    while (_branches.count() <= slot)
    {
        slotItem = new ModelItemSlot(_settings, this, slot);
        ModelItemBasic::addItem(slotItem);

        slotItem->move(0,0);
        slotItem->moveSteps(1,1);

    }

    if (_branches.count() > 1 && !_nextButton)
    {
        _nextButton = createNext();
    }

    slotItem = _branches.at(slot);

    item->setTrunk(slotItem);
    slotItem->addItem(item);

}

int ModelItemSelection::visibleItems(bool) const
{
    if (_currentSlot >= 0)
        return _branches.at(_currentSlot)->visibleItems()+1;    
    else return 1;
}

void ModelItemSelection::printToStream(QTextStream &str)
{
    str.setPadChar(' ');
    print(str, 2);
}

void ModelItemSelection::expand(bool expanse)
{

    if (expanse && _currentSlot < 0)
    {
        if (_nextButton)
            _nextButton->show();
        _currentSlot = 0;
        _branches.at(_currentSlot)->toggleExpand();
    }
    else if (!expanse && _currentSlot >= 0)
    {
        _branches.at(_currentSlot)->toggleExpand();
        if (_nextButton)
            _nextButton->hide();
        _currentSlot = -1;
    }
}

void ModelItemSelection::nextSlot()
{
    _branches.at(_currentSlot)->toggleExpand();

    ++_currentSlot;
    if (_branches.count() <= _currentSlot)
        _currentSlot = 0;

    _branches.at(_currentSlot)->toggleExpand();
}

bool ModelItemSelection::branchChecked(bool check, int, int role)
{
    if (check)
    {
        QStringList newtext;
        //int total = 0;
        foreach (ModelItemBase* m, _branches)
      //  {
            newtext << m->getText();
    //        total += m.getCost();
  //      }
        setText(newtext.join(", "));
//        _cost = total;
    }
    if (role)
        ModelItemBasic::branchChecked(check, 0, role);
    return true;
}

QPushButton* ModelItemSelection::createNext()
{
    QPushButton *nxt = new QPushButton(">", this);

    fitButton(nxt);

    connect(nxt, &QPushButton::pressed,
            this, &ModelItemSelection::nextSlot);

    return nxt;
}
