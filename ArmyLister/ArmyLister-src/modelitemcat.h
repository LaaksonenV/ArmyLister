#ifndef MODELITEMCAT_H
#define MODELITEMCAT_H

#include "modelitembasic.h"

class ModelItemCategory : public ModelItemBasic
{
    Q_OBJECT

public:
    ModelItemCategory(ModelItemBase *parent);

    virtual ~ModelItemCategory();

    virtual void passCostUp(int c, bool = false, int role = 0);

    virtual bool branchSelected(int check, int role, int, int = 0);

    virtual void printToStream(QTextStream &str);


};

#endif // MODELITEMCAT_H
