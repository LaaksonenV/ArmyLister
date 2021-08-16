#ifndef MODELITEMSLOT_H
#define MODELITEMSLOT_H

#include "modelitembasic.h"

class ModelItemSlot : public ModelItemBasic
{
    Q_OBJECT
public:
    ModelItemSlot(ModelItemBase *parent, const QList<int> &slot);
    ModelItemSlot(ModelItemSlot *source, ModelItemBase *parent);
    virtual ~ModelItemSlot();

    virtual void clone(ModelItemBase *toRoot);

    virtual void setText(const QString &text, bool = false, int = -1);

    virtual void updateText();

    virtual QString getText() const;

    virtual int getCost() const;

    virtual bool branchSelected(int check, int role, int ind, int = -1);

    virtual void toggleCheck(int = -1);

private:

    QList<int> slots_;
    int currentIndex_;
    QString defaultText_;
};

#endif // MODELITEMSLOT_H
