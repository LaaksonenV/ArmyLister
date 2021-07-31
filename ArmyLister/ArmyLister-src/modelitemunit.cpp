#include "modelitemunit.h"

#include <QSpinBox>
#include <QMouseEvent>
#include <QTimer>
#include <QApplication>

#include "settings.h"
#include "itemsatellite.h"

ModelItemUnit::ModelItemUnit(ModelItemBase *parent)
    : ModelItemSpinner(parent)
    , clickClock_(new QTimer(this))
    , cloned_(0)
    , unitCountsAs_(0)
{
    clickClock_->setSingleShot(true);
    clickClock_->setInterval(QApplication::doubleClickInterval());
    connect(clickClock_, &QTimer::timeout,
            this, &ModelItemBasic::toggleCheck);
}

ModelItemUnit::ModelItemUnit(ModelItemUnit *source, ModelItemBase *parent)
    : ModelItemSpinner(source, parent)
    , clickClock_(new QTimer(this))
    , cloned_(0)
    , unitCountsAs_(0)
{
    clickClock_->setSingleShot(true);
    clickClock_->setInterval(QApplication::doubleClickInterval());
    connect(clickClock_, &QTimer::timeout,
            this, &ModelItemBasic::toggleCheck);
    setUnitCountsAs(source->unitCountsAs_-1);

}

ModelItemUnit::~ModelItemUnit()
{}

void ModelItemUnit::clone(ModelItemBase*, int)
{
    ModelItemUnit *clone = new ModelItemUnit(this, trunk_);
    trunk_->insertItem(clone, index_+1);
    cloning(clone);
    clone->resize(size());
    clone->passSpecialUp(QStringList(), true);
    ++cloned_;
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

void ModelItemUnit::setUnitCountsAs(int role)
{
    unitCountsAs_ = role+1;
}

void ModelItemUnit::loadSelection(QString &str)
{
    int pos = str.indexOf("#");
    for (int i = 0; i < str.left(pos).toInt(); ++i)
        clone();
    str.remove(0,pos+1);

    ModelItemSpinner::loadSelection(str);
}

void ModelItemUnit::saveSelection(QTextStream &str)
{
    str << QString::number(cloned_);
    str << "#";
    ModelItemSpinner::saveSelection(str);
}

void ModelItemUnit::passSpecialUp(const QStringList &list, bool check)
{
    dealWithSpecials(list, check);
}

bool ModelItemUnit::branchSelected(bool check, int role, int, int)
{
    if (ModelItemBasic::branchSelected(check, role, 0))
    {
        if (role)
        {
            if (check)
            {
                unitCountsAs_ = role;
                trunk_->passCostUp(cost_, false, role);
            }
            else
            {
                trunk_->passCostUp(-cost_, false, role);
                unitCountsAs_ = 0;
            }
        }
        return true;
    }
    return false;
}

void ModelItemUnit::passCostUp(int c, bool perModel, int role)
{
    ModelItemSpinner::passCostUp(c, perModel, role);
    if (unitCountsAs_ && -role != unitCountsAs_)
        ModelItemSpinner::passCostUp(c, perModel, unitCountsAs_);
}

void ModelItemUnit::printToStream(QTextStream &str)
{
    if (checked_)
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
        clickClock_->start();

        e->accept();
    }
}

void ModelItemUnit::mouseDoubleClickEvent(QMouseEvent *e)
{
    if (!checkLimit(eNotClonable) &&
            e->button() == Qt::LeftButton)
    {
        clickClock_->stop();
        clone();

        e->accept();
    }
}

void ModelItemUnit::connectToSatellite(ItemSatellite *sat)
{
    connect(this, &ModelItemUnit::itemCloned,
            sat, &ItemSatellite::on_itemSelected);
}
