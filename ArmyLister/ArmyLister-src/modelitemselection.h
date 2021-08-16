#ifndef MODELITEMSELECTION_H
#define MODELITEMSELECTION_H

#include "modelitembasic.h"


struct SelectionSlot
{
    short index_;
    QString defaultText_;
    int defaultCost_;

    SelectionSlot()
        : index_(-1)
        , defaultText_(QString(""))
        , defaultCost_(0)
    {}
};

class ModelItemSelection : public ModelItemBasic
{
    Q_OBJECT

public:
    ModelItemSelection(ModelItemBase *parent);

    ModelItemSelection(ModelItemSelection *source, ModelItemBase *parent);

    virtual ~ModelItemSelection();

    virtual void clone(ModelItemBase *toRoot, int = 0);

    virtual void setText(const QString &text, bool, int slot);

    virtual void setCost(int i, int slot = -1);

    virtual void updateItem();

    virtual bool branchSelected(int check, int role, int index, int slot);

    virtual void printToStream(QTextStream &str);

private:

    void fillSlots(int c);

private:

    QList<SelectionSlot> slots_;

};


#endif // MODELITEMSELECTION_H
