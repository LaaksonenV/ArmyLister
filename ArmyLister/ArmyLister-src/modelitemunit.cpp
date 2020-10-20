#include "modelitemunit.h"

#include <QSpinBox>
#include <QMouseEvent>
#include <QTimer>
#include <QApplication>

#include "settings.h"
#include "itemsatellite.h"

ModelItemUnit::ModelItemUnit(ModelItemBase *parent)
    : ModelItemSpinner(parent)
    , _clickClock(new QTimer(this))
{
    _clickClock->setSingleShot(true);
    _clickClock->setInterval(QApplication::doubleClickInterval());
    connect(_clickClock, &QTimer::timeout,
            this, &ModelItemBasic::toggleCheck);
}

ModelItemUnit::ModelItemUnit(ModelItemUnit *source, ModelItemBase *parent)
    : ModelItemSpinner(source, parent)
    , _clickClock(new QTimer(this))
{
    _clickClock->setSingleShot(true);
    _clickClock->setInterval(QApplication::doubleClickInterval());
    connect(_clickClock, &QTimer::timeout,
            this, &ModelItemBasic::toggleCheck);

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

/*QString ModelItemUnit::getPrintText() const
{
    return ModelItemBasic::getPrintText();
}*/

void ModelItemUnit::passSpecialUp(const QStringList &list, bool check)
{
    dealWithSpecials(list, check);
}

bool ModelItemUnit::branchSelected(bool check, int i, int role)
{
    if (ModelItemBasic::branchSelected(check,i,role))
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

void ModelItemUnit::passCostUp(int c, bool perModel, int role)
{
    ModelItemSpinner::passCostUp(c, perModel, role);
    if (_unitCountsAs && -role != _unitCountsAs)
        ModelItemSpinner::passCostUp(c, perModel, _unitCountsAs);
}

void ModelItemUnit::printToStream(QTextStream &str)
{
    if (_checked)
    {
        endl(str);
        str.setPadChar('.');
        print(str, 1);
/*        if (_spinner && _current > _spinner->minimum())
        {
            str.setPadChar(' ');
            QString s = "+" + QString::number(_current-_spinner->minimum()) +
                    " models";
            QStringList ss(s);
            ss << QString::number(_initSingleCost*_current-_spinner->minimum());
            print(str, 2, ss);
        }*/
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

void ModelItemUnit::connectToSatellite(ItemSatellite *sat)
{
    connect(this, &ModelItemUnit::itemCloned,
            sat, &ItemSatellite::on_itemSelected);
}
