#ifndef MODELITEMCAT_H
#define MODELITEMCAT_H

#include "modelitembasic.h"

class Settings;

class ModelItemCategory : public ModelItemBasic
{
    Q_OBJECT

public:
    ModelItemCategory(Settings *set ,ModelItemBase *parent);

    virtual ~ModelItemCategory();

    virtual void passCostUp(int c, bool = false, int role = -1);

    virtual bool branchChecked(bool check,int);

    virtual void printToStream(QTextStream &str);


};

#endif // MODELITEMCAT_H
