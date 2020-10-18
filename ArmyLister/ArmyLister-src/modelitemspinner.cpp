#include "modelitemspinner.h"

#include <QSpinBox>
#include <QMouseEvent>
#include <QTimer>
#include <QApplication>

#include "settings.h"
#include "itemsatellite.h"

ModelItemSpinner::ModelItemSpinner(Settings *set, ModelItemBase *parent)
    : ModelItemBasic(set, parent)
    , _current(0)
    , _singleCost(0)
    , _initSingleCost(0)
    , _otherCost(0)
    , _initOtherCost(0)
    , _spinner(nullptr)
{
}

ModelItemSpinner::ModelItemSpinner(ModelItemSpinner *source, ModelItemBase *parent)
    : ModelItemBasic(source, parent)
    , _current(0)
    , _singleCost(0)
    , _initSingleCost(0)
    , _otherCost(0)
    , _initOtherCost(0)
    , _spinner(nullptr)
{
    setMultiCost(source->_initSingleCost, source->_initOtherCost);
    if (source->_spinner)
        setRange(source->_spinner->minimum(), source->_spinner->maximum());
    else if (source->_current)
        setRange(source->_current);
}

ModelItemSpinner::~ModelItemSpinner()
{}

void ModelItemSpinner::clone(ModelItemBase*toRoot, int i)
{
    ModelItemSpinner *clone = new ModelItemSpinner(this, _trunk);
    toRoot->insertItem(clone,i);
    cloning(clone,i);
}

void ModelItemSpinner::setMultiCost(int base, int special)
{
    _singleCost = base;
    _initSingleCost = base;
    _otherCost = special;
    _initOtherCost = special;
    ModelItemBase::passCostUp((base*_current)+special);
}

void ModelItemSpinner::setRange(int min, int max)
{
    _current = min;
    if (max > 0)
        createSpinner(min,max);
}

int ModelItemSpinner::getCurrentCount() const
{
    return _current;
}

QString ModelItemSpinner::getPrintText() const
{
    QString ret = ModelItemBasic::getPrintText();
    if (_spinner && _current > _spinner->minimum())
        ret.prepend(QString::number(_spinner->value()) + " ");
    return ret;
}

void ModelItemSpinner::passCostUp(int c, bool perModel, int role)
{
    int change = c;
    if (perModel)
    {
        change = c*_current;
        if (!role)
            _singleCost += c;
    }
    else if (!role)
        _otherCost += change;
    ModelItemBasic::passCostUp(change, false, role);
}

void ModelItemSpinner::createSpinner(int min, int max)
{
    _spinner = new QSpinBox(this);
    connect(_spinner, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &ModelItemUnit::on_spinnerChanged);
    _spinner->setRange(min, max);
    _spinner->setSuffix(QString("/")+QString::number(max));

    _spinner->move(endOfText() + 20, 1);
    _spinner->setMinimumHeight(_settings->itemHeight-2);

    _spinner->show();

}

void ModelItemSpinner::on_spinnerChanged(int now)
{
    int change = now-_current;
    _current = now;
    ModelItemBasic::passCostUp(change*_singleCost);
    ModelItemBasic::passModelsDown(change);

}
