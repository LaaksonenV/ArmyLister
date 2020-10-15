#include "modelitemunit.h"

#include <QSpinBox>
#include <QMouseEvent>
#include <QTimer>
#include <QApplication>

#include "settings.h"
#include "modelsatellitelimiter.h"

ModelItemUnit::ModelItemUnit(Settings *set, ModelItemBase *parent)
    : ModelItemBasic(set, parent)
    , _modelCost(0)
    , _otherCost(0)
    , _models(0)
    , _spinner(nullptr)
    , _clickClock(new QTimer(this))
{
    _clickClock->setSingleShot(true);
    _clickClock->setInterval(QApplication::doubleClickInterval());
    connect(_clickClock, &QTimer::timeout,
            this, &ModelItemBasic::toggleCheck);
}

ModelItemUnit::ModelItemUnit(ModelItemUnit *source, ModelItemBase *parent)
    : ModelItemBasic(source, parent)
    , _modelCost(0)
    , _otherCost(0)
    , _models(0)
    , _spinner(nullptr)
    , _clickClock(new QTimer(this))
{
    _clickClock->setSingleShot(true);
    _clickClock->setInterval(QApplication::doubleClickInterval());
    connect(_clickClock, &QTimer::timeout,
            this, &ModelItemBasic::toggleCheck);
    setUnitCost(source->_initModelCost, source->_initOtherCost);
    if (source->_spinner)
        setModels(source->_spinner->minimum(), source->_spinner->maximum());
    else if (source->_models)
        setModels(source->_models);
}

ModelItemUnit::~ModelItemUnit()
{}

void ModelItemUnit::clone(ModelItemBase*, int)
{
    ModelItemUnit *clone = new ModelItemUnit(this, _trunk);
    _trunk->insertItem(clone, _index+1);
    cloning(clone);
    clone->resize(size());
    clone->passSpecialUp(QStringList(), true);
    emit itemCloned();
}

/*void ModelItemUnit::setText(const QString &text)
{
    ModelItemBasic::setText(text);
//    dealWithSpecials(QStringList(text),true);
}*/

void ModelItemUnit::setSpecial(const QStringList &list)
{
    QStringList newlist;
    foreach (QString s, list)
    {
        if (s.startsWith("+"))
            s.remove(0,1);
        newlist << s.trimmed();
    }
    ModelItemBasic::setSpecial(newlist);
}

void ModelItemUnit::setUnitCost(int base, int special)
{
    _modelCost = base;
    _initModelCost = base;
    _otherCost = special;
    _initOtherCost = special;
    ModelItemBase::passCostUp((base*_models)+special);
}

void ModelItemUnit::passSpecialUp(const QStringList &list, bool check)
{
    dealWithSpecials(list, check);
}

bool ModelItemUnit::branchChecked(bool check, int i, int role)
{
    if (ModelItemBasic::branchChecked(check,i,role))
    {
        if (role)
        {
            if (check)
                _trunk->passCostUp(_cost, false, role);
            else
                _trunk->passCostUp(-_cost, false, role);
        }
        return true;
    }
    return false;
}

void ModelItemUnit::setModels(int min, int max)
{
    _models = min;
    if (max > 0)
        createSpinner(min,max);
    ModelItemBasic::passCostUp(min*_modelCost);
    ModelItemBase::passModelsDown(_models);
}

int ModelItemUnit::getModelCount() const
{
    return _models;
}

void ModelItemUnit::passCostUp(int c, bool perModel, int role)
{
    int change = c;
    if (perModel)
    {
        change = c*_models;
        if (role < 0)
            _modelCost += c;
    }
    else if (role < 0)
        _otherCost += change;
    ModelItemBasic::passCostUp(change, false, role);
    if (_unitCountsAs)
        ModelItemBasic::passCostUp(change, false, _unitCountsAs);
}

void ModelItemUnit::printToStream(QTextStream &str)
{
    if (_checked)
    {
        endl(str);
        str.setPadChar('.');
        print(str, 1);
        if (_spinner && _models > _spinner->minimum())
        {
            str.setPadChar(' ');
            QString s = "+" + QString::number(_models-_spinner->minimum()) +
                    " models";
            QStringList ss(s);
            ss << QString::number(_initModelCost*_models-_spinner->minimum());
            print(str, 2, ss);
        }
        ModelItemBase::printToStream(str);
    }
}

void ModelItemUnit::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        _clickClock->start();

        e->accept();
    }
}

void ModelItemUnit::mouseDoubleClickEvent(QMouseEvent *e)
{
    if (!checkLimit(NotClonable) &&
            e->button() == Qt::LeftButton)
    {
        _clickClock->stop();
        clone();

        e->accept();
    }
}

void ModelItemUnit::createSpinner(int min, int max)
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

void ModelItemUnit::on_spinnerChanged(int now)
{
    int change = now-_models;
    _models = now;
    ModelItemBasic::passCostUp(change*_modelCost);
    ModelItemBase::passModelsDown(_models);

}

void ModelItemUnit::connectToLimitSatellite(ModelSatelliteLimiter *sat)
{
    connect(this, &ModelItemUnit::itemCloned,
            sat, &ModelSatelliteLimiter::on_itemChecked);
}
