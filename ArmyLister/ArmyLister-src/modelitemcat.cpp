#include "modelitemcat.h"

#include "settings.h"

ModelItemCategory::ModelItemCategory(ModelItemBase *parent)
    : ModelItemBasic(parent)
{
    setAlwaysChecked();
    connect(this, &ModelItemCategory::valueChanged,
            parent, &ModelItemBase::valueChanged);
    connect(this, &ModelItemCategory::itemSelected,
            parent, &ModelItemBase::itemSelected);
    connect(this, &ModelItemCategory::countsChanged,
            parent, &ModelItemBase::countsChanged);
}

ModelItemCategory::~ModelItemCategory()
{}

void ModelItemCategory::passCostUp(int c, bool, int role)
{
    if (role > 0 && role-1 != index_)
        emit valueChanged(c, role-1);
    else if (role == 0 || (role < 0 && 1+role != index_))
    {
        ModelItemBase::passCostUp(c);
        emit valueChanged(c, index_);
    }
}

bool ModelItemCategory::branchSelected(int check, int role, int index, int)
{
//    ModelItemBasic::branchChecked(check,0,0);
/*    if (check > 0)
    {
        if (role)
            emit itemSelected(1, role);
        else
            emit itemSelected(1, index_);
    }
    else
    {*/
    if (role)
        emit itemSelected(check, role);
    else
        emit itemSelected(check, index_);
    //    }
    ModelItemBase::branchSelected(check, role, index);
    return true;
}

void ModelItemCategory::printToStream(QTextStream &str)
{
    endl(str);
//    str.setPadChar('_');
    endl(str);
    str.setPadChar(' ');
    print(str, 0);
    ModelItemBase::printToStream(str);
}
