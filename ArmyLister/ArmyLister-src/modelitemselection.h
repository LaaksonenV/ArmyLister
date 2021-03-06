#ifndef MODELITEMSELECTION_H
#define MODELITEMSELECTION_H

#include "modelitembasic.h"

class ModelItemSelection : public ModelItemBasic
{
    Q_OBJECT

public:
    ModelItemSelection(ModelItemBase *parent);

    ModelItemSelection(ModelItemSelection *source, ModelItemBase *parent);

    virtual ~ModelItemSelection();

    virtual void clone(ModelItemBase *toRoot, int = 0);

    virtual void addItem(ModelItemBase* item, int slot);

    virtual int visibleItems(bool = false) const;

//    virtual void setText(const QString &text);


    virtual bool branchSelected(int check, int, int role);

    virtual void printToStream(QTextStream &str);

private:

    virtual void expand(bool expanse);

public slots:

    virtual void nextSlot();

private:

    QPushButton *createNext();

private:

    QPushButton *_nextButton;

    int _currentSlot;
};

#endif // MODELITEMSELECTION_H
