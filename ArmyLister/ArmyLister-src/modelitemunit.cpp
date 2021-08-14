#include "modelitemunit.h"

#include <QSpinBox>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QLabel>
#include <QTimer>

#include "settings.h"
#include "itemsatellite.h"

ModelItemUnit::ModelItemUnit(ModelItemBase *parent)
    : ModelItemSpinner(parent)
    , cloned_(0)
    , cloneAnimation_(nullptr)
    , cloneTimer_(new QTimer(this))
{
    setUpCloneAnimation();
    cloneTimer_->setSingleShot(true);
}

ModelItemUnit::ModelItemUnit(ModelItemUnit *source, ModelItemBase *parent)
    : ModelItemSpinner(source, parent)
    , cloned_(0)
    , cloneAnimation_(nullptr)
    , cloneTimer_(new QTimer(this))
{
    setUpCloneAnimation();
    cloneTimer_->setSingleShot(true);
}

ModelItemUnit::~ModelItemUnit()
{}

void ModelItemUnit::setUpCloneAnimation()
{

}

void ModelItemUnit::clone(ModelItemBase*, int)
{
    ModelItemUnit *clone = new ModelItemUnit(this, trunk_);
    trunk_->insertItem(clone, index_+1);
    cloning(clone);
    clone->resize(size());
    clone->passTagsUp(QStringList(), true);
    ++cloned_;
    emit itemCloned();
}

void ModelItemUnit::setTags(const QStringList &list)
{
    QStringList newlist;
    foreach (QString s, list)
    {
        if (s.startsWith("+"))
            s.remove(0,1);
        newlist << s.trimmed();
    }
    ModelItemBasic::setTags(newlist);
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

void ModelItemUnit::passTagsUp(const QStringList &list, bool check)
{
    dealWithTags(list, check);
}

bool ModelItemUnit::branchSelected(int check, int role, int, int)
{
    if (role)
    {
        if (check > 0)
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

bool ModelItemUnit::toggleSelected(int change, int slot)
{
    return trunk_->branchSelected(change, unitCountsAs_, index_, slot);
}

void ModelItemUnit::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {

        if (!checkLimit(eNotClonable))
        {
            cloneAnimation_ = new QParallelAnimationGroup(this);

            QPropertyAnimation *grow;

            grow = new QPropertyAnimation(this, "itemHeight");
            grow->setDuration(Settings::ItemMeta(Settings::eItem_MouseHoldTime));
            grow->setKeyValueAt(0,Settings::ItemMeta(Settings::eItem_Height));
            grow->setKeyValueAt(0.95,2*Settings::ItemMeta(Settings::eItem_Height));
            grow->setKeyValueAt(1,Settings::ItemMeta(Settings::eItem_Height));
            cloneAnimation_->addAnimation(grow);

            connect(cloneAnimation_, &QParallelAnimationGroup::finished,
                    this, &ModelItemUnit::cloneEvent);
            connect(cloneTimer_, &QTimer::timeout,
                    this, &ModelItemUnit::cloneTimerTimeout);

            cloneTimer_->start(200);
        }

        e->accept();
    }
}

void ModelItemUnit::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {

        if (cloneTimer_->isActive() ||
                (cloneAnimation_ &&
            cloneAnimation_->state() == QAbstractAnimation::Running))
        {
            cloneTimer_->stop();

            if (cloneAnimation_)
            {
                cloneAnimation_->setCurrentTime(0);
                cloneAnimation_->stop();
                delete cloneAnimation_;

            }
            toggleCheck();

        }


//        if (!checkLimit(eSelectionLimit))

        e->accept();
    }
}

void ModelItemUnit::cloneEvent()
{
    delete cloneAnimation_;
    clone();
}

void ModelItemUnit::connectToSatellite(ItemSatellite *sat)
{
    connect(this, &ModelItemUnit::itemCloned,
            sat, &ItemSatellite::on_itemSelected);
}

void ModelItemUnit::cloneTimerTimeout()
{
    cloneAnimation_->start();
}
