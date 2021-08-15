#include "modelitembasic.h"

#include <QPainter>
#include <QLabel>
#include <QMouseEvent>
#include <QPushButton>
#include <QTextStream>

#include "settings.h"
#include "itemsatellite.h"

void ModelItemBasic::init()
{
    show();

    title_->setFont(Settings::Font(Settings::eFont_ItemFont));
    title_->setFixedHeight(Settings::ItemMeta(Settings::eItem_Height));
    title_->move(Settings::ItemMeta(Settings::eItem_Height),0);
    setFixedHeight(Settings::ItemMeta(Settings::eItem_Height));
}

ModelItemBasic::ModelItemBasic(ModelItemBase *parent)
    : ModelItemBase(parent)
    , trunk_(parent)
    , checked_(false)
    , current_(1)
    , title_(new QLabel(this))
    , unitCountsAs_(nullptr)
    , tags_(QStringList())
    , limitingTags_(QStringList())
    , initialTags_(QStringList())
    , forAllModels_(-1)
    , modelOverride_(0)
    , constantOverride_(true)
    , overriddenModels_(0)
    , costLimit_(-1)
    , modelLimitMin_(0)
    , bHasModelLimitMin_(false)
    , modelLimitMax_(0)
    , bHasModelLimitMax_(false)
    , countsAs_(nullptr)
    , expandButton_(nullptr)
    , bExpanded_(false)
    , bAlwaysChecked_(false)
    , bMouseIn_(false)
    , limit_(0)
    , autoToggle_(-1)
    , p_ItemHeight(Settings::ItemMeta(Settings::eItem_Height))
{
    init();
}

ModelItemBasic::ModelItemBasic(ModelItemBasic *source, ModelItemBase *parent)
    : ModelItemBase(source, parent)
    , trunk_(parent)
    , checked_(false)
    , current_(1)
    , title_(new QLabel(this))
    , unitCountsAs_(nullptr)
    , tags_(QStringList())
    , limitingTags_(QStringList())
    , initialTags_(QStringList())
    , forAllModels_(-1)
    , modelOverride_(0)
    , constantOverride_(true)
    , overriddenModels_(0)
    , costLimit_(-1)
    , modelLimitMin_(0)
    , bHasModelLimitMin_(false)
    , modelLimitMax_(0)
    , bHasModelLimitMax_(false)
    , countsAs_(nullptr)
    , expandButton_(nullptr)
    , bExpanded_(false)
    , bAlwaysChecked_(false)
    , bMouseIn_(false)
    , autoToggle_(-1)
    , limit_(0)
{
    init();

    setText(source->getText());
    setTags(source->initialTags_);
    int min = 0;
    int max = 0;
    if (bHasModelLimitMin_)
        min = source->modelLimitMin_+source->getCurrentCount();
    if (bHasModelLimitMax_)
        min = source->modelLimitMax_+source->getCurrentCount();
    if (min || max)
        setModelLimiter(min, max);
    setAlwaysChecked(source->bAlwaysChecked_);
    if (source->forAllModels_ >= 0)
        setForAll();
    setModelOverride(source->modelOverride_);
    setCostLimit(source->costLimit_);
    if (source->unitCountsAs_)
        foreach (int i, *(source->unitCountsAs_))
            setCountsAs(i-1);
    if (source->unitCountsAs_)
        foreach (int i, *(source->unitCountsAs_))
            setUnitCountsAs(i-1);
    if (source->autoToggle_ >= 0)
        setManualLock();
}

ModelItemBasic::~ModelItemBasic()
{
    if (countsAs_)
        delete countsAs_;
    if (unitCountsAs_)
        delete unitCountsAs_;
}

void ModelItemBasic::clone(ModelItemBase *toRoot)
{
    ModelItemBasic *clone = new ModelItemBasic(this, toRoot);
    toRoot->addItem(clone);
    cloning(clone);

}

void ModelItemBasic::cloning(ModelItemBasic *clone)
{
    connect(this, &ModelItemBasic::passConnection,
            clone, &ModelItemBasic::connectToSatellite);
    emit cloneSatellite();
    emit pingSatellite(false);
    ModelItemBase::clone(clone);
    emit pingSatellite(true);
    disconnect(this, &ModelItemBasic::passConnection,
            clone, &ModelItemBasic::connectToSatellite);

}

void ModelItemBasic::addItem(ModelItemBase *item)
{
    if (!expandButton_)
        expandButton_ = createPlus();

    ModelItemBase::addItem(item);
    item->moveSteps(1,1);

}

/*void ModelItemBasic::insertItem(ModelItemBase *item, int to)
{
    ModelItemBase::insertItem(item, to);
    item->moveSteps(1,1);

}*/

int ModelItemBasic::visibleItems(bool underCover) const
{
    if (bExpanded_ || underCover)
        return ModelItemBase::visibleItems()+1;
    else return 1;
}

void ModelItemBasic::setTrunk(ModelItemBase *item)
{
    QWidget::setParent(item);
    trunk_ = item;
}

void ModelItemBasic::setText(const QString &text, int)
{
    QString name = text;
    QRegExp ttip("\\{(.*)\\}");
    int pos = 0;
    while ((pos = ttip.indexIn(name,pos)) >= 0)
        setToolTip(ttip.cap(1));

    title_->setText(name.remove(ttip).trimmed());
    title_->adjustSize();
    title_->show();
//    _special << "+" + name;
}

void ModelItemBasic::setTags(const QStringList &list)
{
    initialTags_.append(list);
    tags_.append(list);
    if (list.count() && checked_)
        trunk_->passTagsUp(tags_,true);
}

void ModelItemBasic::setLimitingTags(const QStringList &list)
{
    limitingTags_.append(list);
}

void ModelItemBasic::setModelLimiter(int min, int max)
{
    if (min > 0)
    {
        modelLimitMin_ = min;
        bHasModelLimitMin_ = true;
    }
    if (max > 0)
    {
        modelLimitMax_ = max;
        bHasModelLimitMax_ = true;
    }

//    if (_hasModelLimitMin || _hasModelLimitMax)
  //      passModelsDown(getCurrentCount());
}

void ModelItemBasic::setAlwaysChecked(bool b)
{
    if (b && !checked_)
        toggleCheck();
/*    {
        bool fa = false;
        int c = cost_;
        if (forAllModels_ >= 0)
        {
            fa = true;
            c = forAllModels_;
        }
        checked_ = true;
        trunk_->passCostUp(c,fa);
    }*/


    bAlwaysChecked_ = b;
    update();
}

void ModelItemBasic::setForAll(bool b)
{
    if (b && forAllModels_ < 0)
    {
        forAllModels_ = cost_;
//        setCost(_forAll*getCurrentCount());
    }
}

void ModelItemBasic::setModelOverride(int models)
{
    if (models < 0)
        constantOverride_ = false;
    else
    {
        modelOverride_ = models;
        if (modelOverride_ && checked_)
            trunk_->overrideModels(modelOverride_);
    }
}

void ModelItemBasic::setCostLimit(int limit)
{
    costLimit_ = limit;
}

void ModelItemBasic::setCountsAs(int role)
{
    if (!countsAs_)
        countsAs_ = new QList<int>();
    countsAs_->append(role);
}

void ModelItemBasic::setUnitCountsAs(int role)
{
    if (!unitCountsAs_)
        unitCountsAs_ = new QList<int>();
    unitCountsAs_->append(role);
}

void ModelItemBasic::setManualLock(bool lock)
{
    if (lock)
        autoToggle_ = 0;
}

void ModelItemBasic::loadSelection(QString &str)
{
    if (str.startsWith("!"))
    {
        forceCheck(true);
        str.remove(0,1);
        ModelItemBase::loadSelection(str);
    }
    else
    {
        forceCheck(false);
        str.remove(0,1);
    }
}

void ModelItemBasic::saveSelection(QTextStream &str)
{
    if (checked_)
    {
        str << "!";
        ModelItemBase::saveSelection(str);
    }
    else
        str << "/";
}

int ModelItemBasic::getCurrentCount() const
{
    return trunk_->getCurrentCount();
}

QString ModelItemBasic::getText() const
{
    return title_->text();
}

QString ModelItemBasic::getPrintText() const
{
    return title_->text();
}

void ModelItemBasic::passTagsUp(const QStringList &list, bool check)
{
    if (check)
        tags_ << list;
    else
        foreach (QString s, list)
            tags_.removeOne(s);

    if (checked_)
        trunk_->passTagsUp(list, check);
}

void ModelItemBasic::passCostUp(int c, bool b, int role)
{
//    if (_countsAs >= 0)
  //      role = _countsAs;
    int change = c;
    bool fa = false;
    if (forAllModels_ >= 0)
    {
        fa = true;
        change *= getCurrentCount()-overriddenModels_;
        forAllModels_ += c;
    }
    ModelItemBase::passCostUp(change,false, role);
    if (checked_)
        trunk_->passCostUp(c, fa || b, role);
    if (countsAs_)
        foreach (int i, *countsAs_)
            if (i != role)
                trunk_->passCostUp(c, fa || b, i);
    if (costLimit_ > 0 && !role)
        ModelItemBase::passCostDown(costLimit_-cost_);
}

void ModelItemBasic::passModelsDown(int models, bool push)
{
    if (bHasModelLimitMin_)
        modelLimitMin_ -= models;
    if (bHasModelLimitMax_)
        modelLimitMax_ -= models;

    emit modelsChanged(models, checked_, false);


    if ((modelLimitMin_ > 0 || modelLimitMax_ < 0) && !checkLimit(eModelsLimit))
        setHardLimit(eModelsLimit);
    else if (checkLimit(eModelsLimit))
        setHardLimit(-eModelsLimit);

    if (forAllModels_ >= 0)
    {
        if (push)
            models -= overriddenModels_;
        models *= forAllModels_;
        ModelItemBase::passCostUp(models);
    }
    else
        ModelItemBase::passModelsDown(models, push);
}

void ModelItemBasic::passTagsDown(const QStringList &list)
{
    if (limitingTags_.count())
    {
        bool ok = false;
        int ind = 0;
        while (ind < limitingTags_.count() && !ok)
        {
            ok = true;
            foreach (QString ss, limitingTags_.at(ind).split(","))
            {
                ss = ss.trimmed();
                if (ss.startsWith('|'))
                {
                    // don't bother with limiting tag if its item itself checked
                    if (checked_ && ss == getText())
                        break;
                    ok = false;
                    ss = ss.remove(0,1);
                }
//?                ss.replace("**", ".*");
                ss.replace('*', "\\w*");
                if (list.indexOf(QRegExp(ss)) < 0)
                    ok = !ok;

                if (!ok)
                    break;
            }
            ++ind;
        }
        if (ok)
            setHardLimit(-eSpecialLimit);
        else
            setHardLimit(eSpecialLimit);
    }
    if (!checked_)
    {
        QStringList newlist(list);
        updateTags(tags_, newlist, true);
        ModelItemBase::passTagsDown(newlist);
    }
    else
        ModelItemBase::passTagsDown(list);
}


void ModelItemBasic::passCostDown(int left)
{
    if (cost_ > left)
        limitedBy(eCostLimit);
    else
        limitedBy(-eCostLimit);
    ModelItemBase::passCostDown(left);
}

void ModelItemBasic::passTakeLimitDown(int left)
{
    if (left <= 0)
        limitedBy(eTakeLimit);
    else
        limitedBy(-eTakeLimit);
}

void ModelItemBasic::overrideModels(int models)
{
    overriddenModels_ += models;
}

void ModelItemBasic::branchExpanded(int item, int steps)
{
    if (bExpanded_)
    {
        ModelItemBase::branchExpanded(item, steps);
        trunk_->branchExpanded(index_, steps);
    }
    update();
}

bool ModelItemBasic::branchSelected(int check, int role, int index, int slot)
{
//    if(!trunk_->branchSelected(check, role, index_, slot))
  //      return false;

    if (!ModelItemBase::branchSelected(check, role, index))
        return false;

    if (!constantOverride_)
        modelOverride_ += check;

    if ((check > 0 && !checked_))// || checkLimit(eSelectionLimit))
        toggleCheck();

    if (autoToggle_ >= 0)
    {
        autoToggle_ += check;
        if (autoToggle_ < 0)
        {
//            qDebug() << "autotoggle dysfunction";
            autoToggle_ = 0;
        }

        if (autoToggle_ == 0 && checked_)
            toggleCheck();
    }

    if (role)
        trunk_->branchSelected(check, role, index_, slot);

    return true;
}

bool ModelItemBasic::checkLimit(int limit)
{
    if (limit_ & limit)
        return true;
    return false;
}

void ModelItemBasic::resizeEvent(QResizeEvent *e)
{
    if (e->size().height() != e->oldSize().height())
        return;
    QSize sz = e->size();
    sz.setWidth(sz.width()-Settings::ItemMeta(Settings::eItemHPos));
    foreach (ModelItemBase *i, branches_)
        i->resize(sz);
}

void ModelItemBasic::printToStream(QTextStream &str)
{
    if (checked_)
    {
        str.setPadChar(' ');
        print(str,2);
        ModelItemBase::printToStream(str);
    }
}

void ModelItemBasic::print(QTextStream &str, int pre,
                           const QStringList &override)
{
    QRegExp list("^\\[.+\\]");

    if (!override.isEmpty())
    {
        str << QString("  ").repeated(pre);
        str.setFieldAlignment(QTextStream::AlignLeft);
        str.setFieldWidth(Settings::ItemMeta(Settings::eItem_PlainTextWidth)
                          -pre*10);
        str << override.at(0);
        str.setFieldWidth(0);
        str.setFieldAlignment(QTextStream::AlignRight);

        if (override.count() > 1)
        {
            str << override.at(1);
        }
        endl(str);
    }
    else if (list.indexIn(title_->text()) < 0)
    {
        str << QString("  ").repeated(pre);
        str.setFieldAlignment(QTextStream::AlignLeft);
        str.setFieldWidth(Settings::ItemMeta(Settings::eItem_PlainTextWidth)
                          -pre*10);
        str << title_->text();
        str.setFieldWidth(0);
        str.setFieldAlignment(QTextStream::AlignRight);

        if (cost_)
        {
            str << cost_;
        }
        endl(str);
    }
}

void ModelItemBasic::paintEvent(QPaintEvent *)
{
    if (isVisible())
    {
        QPainter p(this);
        QLinearGradient g(0,0, width(),
                          p_ItemHeight);
        if (checked_)
        {
            if (limit_ == eCriticalLimit)
                g.setColorAt(0, Settings::Color(Settings::eColor_Not));
            else
                g.setColorAt(0, Settings::Color(Settings::eColor_Ok));
        }
        else if (limit_ > eCheckable)
            g.setColorAt(0, Settings::Color(Settings::eColor_Neutral));
        else
            g.setColorAt(0, Qt::white);

        if (bMouseIn_)
            g.setColorAt(1, QColor(255,255,255));
        else
            g.setColorAt(1, QColor(220,230,245));

        QBrush b(g);
        p.setBrush(b);
        p.drawRect(rect());

        p.setFont(Settings::Font(Settings::eFont_ItemFont));

        p.drawLine(width()-Settings::ItemMeta(Settings::eItem_CostFieldWidth),0,
                   width()-Settings::ItemMeta(Settings::eItem_CostFieldWidth),
                     height());

        if (checkLimit(eCostLimit))
           p.setPen(Qt::red);

        p.drawText(width()-Settings::ItemMeta(Settings::eItem_CostFieldWidth) + 10,
                   (Settings::ItemMeta(Settings::eItem_Height)/2)
                   +(Settings::ItemMeta(Settings::eItem_FontSize)/2),
                   QString::number(cost_));
    }
}

void ModelItemBasic::enterEvent(QEvent*)
{
    bMouseIn_ = true;
    update();
}

void ModelItemBasic::leaveEvent(QEvent*)
{
    bMouseIn_ = false;
    update();
}

void ModelItemBasic::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        e->accept();
    }
}

void ModelItemBasic::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        if (!bAlwaysChecked_ && autoToggle_ < 0
                //&& !checkLimit(eSelectionLimit)
                )
        {
            toggleCheck();

        }
        e->accept();
    }
}

QPushButton *ModelItemBasic::createPlus()
{
    QPushButton *plus = new QPushButton("+", this);
    plus->setFixedSize(Settings::ItemMeta(Settings::eItem_ExpandButtonSize),
                                Settings::ItemMeta(Settings::eItem_ExpandButtonSize));
//    plus->setCheckable(true);
    plus->setFlat(true);
    plus->move((Settings::ItemMeta(Settings::eItem_Height)/2)
                          -(Settings::ItemMeta(Settings::eItem_ExpandButtonSize)/2),
                        (Settings::ItemMeta(Settings::eItem_Height)/2)
                          -(Settings::ItemMeta(Settings::eItem_ExpandButtonSize)/2));

    connect(plus, &QPushButton::pressed,
            this, &ModelItemBasic::toggleExpand);

    plus->show();

    return plus;
}

void ModelItemBasic::toggleExpand()
{
    if (!bExpanded_)
    {
        expandButton_->setText("-");
        bExpanded_ = true;
        expand(true);

    }
    else
    {

        expandButton_->setText("+");

        expand(false);
        bExpanded_ = false;
    }
}

void ModelItemBasic::dealWithTags(const QStringList &list, bool check)
{
    updateTags(list, tags_, check);

    ModelItemBase::passTagsDown(tags_);
}


int ModelItemBasic::endOfText() const
{
    if (title_)
        return title_->pos().x() + title_->width();
    return 0;
}

void ModelItemBasic::fitButton(QPushButton *but)
{
    but->setFixedSize(Settings::ItemMeta(Settings::eItem_ExpandButtonSize),
                                Settings::ItemMeta(Settings::eItem_ExpandButtonSize));

    but->setFlat(true);
    but->move((Settings::ItemMeta(Settings::eItem_Height)*3/2)
                          -(Settings::ItemMeta(Settings::eItem_ExpandButtonSize)/2),
                        (Settings::ItemMeta(Settings::eItem_Height)/2)
                          -(Settings::ItemMeta(Settings::eItem_ExpandButtonSize)/2));

    title_->move(title_->pos().x()+
                 Settings::ItemMeta(Settings::eItem_ExpandButtonSize),
                 title_->pos().y());
}

void ModelItemBasic::expand(bool expanse)
{

    int toMove = visibleItems(true)-1;

    if (!expanse)
    {
        setFixedHeight(Settings::ItemMeta(Settings::eItem_Height));
        toMove = -toMove;
    }
    else
        setFixedHeight(Settings::ItemMeta(Settings::eItem_Height)* (toMove+1));

    trunk_->branchExpanded(index_, toMove);
}

void ModelItemBasic::updateTags(const QStringList &list,
                                    QStringList &to, bool check)
{
    foreach (QString s, list)
    {
        if (s.startsWith('|'))
        {
            s.remove(0,1);

            if (!check)
                to << s;
            else
                to.removeOne(s);
        }
        else
        {
            if (check)
                to << s;
            else
                to.removeOne(s);
        }
    }
}

void ModelItemBasic::toggleCheck(int slot)
{
    if (!checked_ && limit_ > eCheckable)
        return;

    int change = current_;
    if (checked_)
        change = -change;

    if (!toggleSelected(change, slot))
        return;

    checked_ = !checked_;

    if (checked_)
        emit modelsChanged(getCurrentCount(), checked_, true);
    else
        emit modelsChanged(-getCurrentCount(), checked_, true);

    int c = cost_;
    bool fa = false;
    if (forAllModels_ >= 0)
    {
        fa = true;
        c = forAllModels_;
    }

    if (!checked_)
        c = -c;

    trunk_->passCostUp(c, fa);
    if (countsAs_)
        foreach (int i, *countsAs_)
            trunk_->passCostUp(c, fa, i);
    if (tags_.count())
        trunk_->passTagsUp(tags_, checked_);
    update();

    if (checked_)
        emit itemSelected(current_,0);
    else
        emit itemSelected(-current_,0);

    if (modelOverride_ > 0)
    {
        if (checked_)
            trunk_->overrideModels(modelOverride_);
        else
            trunk_->overrideModels(-modelOverride_);
    }
}

bool ModelItemBasic::toggleSelected(int change, int slot)
{
    bool ok = true;
    if (unitCountsAs_)
    {
        int i = 0;
        while (ok && i < unitCountsAs_->count())
        {
            ok = trunk_->branchSelected(change, unitCountsAs_->at(i),
                                        index_, slot);
            ++i;
        }
        if (!ok)
        {
            --i;
            while (i >= 0)
            {
                trunk_->branchSelected(-change, unitCountsAs_->at(i),
                                                        index_, slot);
                --i;
            }
        }
    }
    return ok;
}

void ModelItemBasic::forceCheck(bool check)
{
    if ((check && !checked_) || (!check && checked_))
        toggleCheck();
}

void ModelItemBasic::currentLimitChanged(int current, bool)
{
    if (bHasModelLimitMax_)
        modelLimitMax_ -= current;
    if (modelLimitMax_ < 0 && !checkLimit(eModelsLimit))
        limitedBy(eModelsLimit);
    else if (checkLimit(eModelsLimit))
        limitedBy(-eModelsLimit);
}

void ModelItemBasic::connectToSatellite(ItemSatellite *sat,
                                             bool responsible)
{
    connect(this, &ModelItemBasic::itemSelected,
            sat, &ItemSatellite::on_itemSelected);

    connect(trunk_, &ModelItemBasic::modelsChanged,
            sat, &ItemSatellite::on_modelsChanged);

    connect(sat, &ItemSatellite::currentLimit,
            this, &ModelItemBasic::currentLimitChanged);

    connect(sat, &ItemSatellite::check,
            this, &ModelItemBasic::forceCheck);

    connect(sat, &ItemSatellite::limitReach,
            this, &ModelItemBasic::limitedBy);

    connect(this, &ModelItemBasic::pingSatellite,
            sat, &ItemSatellite::ping);

    connect(sat, &ItemSatellite::sendConnection,
            this, &ModelItemBasic::passConnection);

    if (responsible)
    {
        connect(this, &ModelItemBasic::cloneSatellite,
                sat, &ItemSatellite::createClone);

        connect(this, &ModelItemBase::releaseCloneSatellite,
                sat, &ItemSatellite::releaseClone);
    }
}

void ModelItemBasic::setHardLimit(short limit)
{
    if (limit > 0 && checked_)
        toggleCheck();
    limitedBy(limit);
    update();
}

void ModelItemBasic::limitedBy(short flag)
{
    if (flag > 0)
        limit_ = limit_ | flag;
    else
        limit_ = limit_ & ~flag;

    if (abs(flag) == eCriticalLimit)
        trunk_->limitedBy(flag);
}
