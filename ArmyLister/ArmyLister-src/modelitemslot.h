#ifndef MODELITEMSLOT_H
#define MODELITEMSLOT_H

#include "modelitembasic.h"

class ModelItemSlot : public ModelItemBasic
{
    Q_OBJECT
public:
    ModelItemSlot(ModelItemBase *parent);
    virtual ~ModelItemSlot();

    virtual QSize sizeHint();

    virtual void clone(ModelItemBase *toRoot);

    virtual void addItem(ModelItemBase *item);

    virtual void setTrunk(ModelItemBase *item);

    virtual QString getText() const;

    virtual int getCurrentCount() const;

    virtual void passSpecialUp(const QStringList & list, bool check);

    virtual void passCostUp(int c, bool forall = false, int role = 0);

    virtual int getCost() const;

    virtual void expand(bool expanse);

    virtual bool branchSelected(int check, int role, int ind, int = 0);

    virtual void toggleExpand();

private:

//    int _slot;
    int currentIndex_;
//    QString _currentText;

    bool bExpanded_;

    ModelItemBase *trunk_;

};

#endif // MODELITEMSLOT_H
