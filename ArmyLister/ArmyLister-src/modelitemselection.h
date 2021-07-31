#ifndef MODELITEMSELECTION_H
#define MODELITEMSELECTION_H

#include "modelitembasic.h"


struct SelectionSlot
{
    short _index;
    QString _defaultText;
    int _defaultCost;

    SelectionSlot()
        : _index(-1)
        , _defaultText(QString(""))
        , _defaultCost(0)
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

    virtual void setText(const QString &text, int slot);

    virtual void setCost(int i, int slot);

    virtual void updateItem();

    virtual bool branchSelected(int check, int role, int index, int slot);

    virtual void printToStream(QTextStream &str);

private:

    void fillSlots(int c);

private:

    QList<SelectionSlot> slots_;

};


#endif // MODELITEMSELECTION_H
