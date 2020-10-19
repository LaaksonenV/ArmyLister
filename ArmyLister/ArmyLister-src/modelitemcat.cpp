#include "modelitemcat.h"

#include "settings.h"

ModelItemCategory::ModelItemCategory(Settings *set, ModelItemBase *parent)
    : ModelItemBasic(set, parent)
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
    if (role >= 0)
        emit valueChanged(c, role);
    else
    {
        ModelItemBase::passCostUp(c);
        emit valueChanged(c, _index);
    }
}

bool ModelItemCategory::branchSelected(int check, int, int role)
{
//    ModelItemBasic::branchChecked(check,0,0);
    if (check > 0)
    {
        if (role)
            emit itemSelected(1, role-1);
        else
            emit itemSelected(1, _index);
    }
    else
    {
        if (role)
            emit itemSelected(-1, role-1);
        else
            emit itemSelected(-1, _index);
    }

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
