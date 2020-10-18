#ifndef MODELITEMSLOT_H
#define MODELITEMSLOT_H

#include "modelitembase.h"

class ModelItemSlot : public ModelItemBase
{
    Q_OBJECT
public:
    ModelItemSlot(Settings *set, ModelItemBase *parent, int slot);
    virtual ~ModelItemSlot();

    virtual QSize sizeHint();

    virtual void clone(ModelItemBase *toRoot, int);

    virtual void addItem(ModelItemBase *item, int = -1);

    virtual void setTrunk(ModelItemBase *item);

    virtual QString getText() const;

    virtual int getCurrentCount() const;

    virtual void passSpecialUp(const QStringList & list, bool check);

    virtual void passCostUp(int c, bool forall = false, int role = -1);

    virtual int getCost() const;

    virtual void expand(bool expanse);

    virtual bool branchChecked(bool check, int ind, int role);

    virtual void toggleExpand();

private:

    int _slot;
    int _currentIndex;
//    QString _currentText;

    bool _expanded;

    ModelItemBase *_trunk;

};

#endif // MODELITEMSLOT_H
