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
    , specials_(QStringList())
    , specialLimiters_(QStringList())
    , initialSpecials_(QStringList())
    , forAllModels_(-1)
    , modelOverride_(0)
    , overriddenModels_(0)
    , costLimit_(-1)
    , modelLimitMin_(0)
    , bHasModelLimitMin_(false)
    , modelLimitMax_(0)
    , bHasModelLimitMax_(false)
    , countsAs_(0)
    , expandButton_(nullptr)
    , bExpanded_(false)
    , bAlwaysChecked_(false)
    , bMouseIn_(false)
    , limit_(0)
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
    , specials_(QStringList())
    , specialLimiters_(QStringList())
    , initialSpecials_(QStringList())
    , forAllModels_(-1)
    , modelOverride_(0)
    , overriddenModels_(0)
    , costLimit_(-1)
    , modelLimitMin_(0)
    , bHasModelLimitMin_(false)
    , modelLimitMax_(0)
    , bHasModelLimitMax_(false)
    , countsAs_(0)
    , expandButton_(nullptr)
    , bExpanded_(false)
    , bAlwaysChecked_(false)
    , bMouseIn_(false)
    , limit_(0)
{
    init();

    setText(source->getText());
    setSpecial(source->initialSpecials_);
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
    setCountsAs(source->countsAs_-1);
}

ModelItemBasic::~ModelItemBasic()
{}

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

void ModelItemBasic::setSpecial(const QStringList &list)
{
    initialSpecials_.append(list);
    foreach (QString s, list)
    {
        if (s.startsWith('-') || s.startsWith('+'))
            specials_ << s;
        else
            specialLimiters_ << s;
    }
    if (list.count() && checked_)
        trunk_->passSpecialUp(specials_,true);
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
    modelOverride_ = models;
    if (modelOverride_ && checked_)
        trunk_->overrideModels(modelOverride_);
}

void ModelItemBasic::setCostLimit(int limit)
{
    costLimit_ = limit;
}

void ModelItemBasic::setCountsAs(int role)
{
    countsAs_ = role+1;
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

void ModelItemBasic::passSpecialUp(const QStringList &list, bool check)
{
    if (check)
        specials_ << list;
    else
        foreach (QString s, list)
            specials_.removeOne(s);

    if (checked_)
        trunk_->passSpecialUp(list, check);
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
    if (countsAs_ && countsAs_ != role)
        trunk_->passCostUp(c, fa || b, countsAs_);
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
        models *= forAllModels_;
        ModelItemBase::passCostUp(models);
    }
    else
        ModelItemBase::passModelsDown(models, push);
}

void ModelItemBasic::passSpecialDown(const QStringList &list)
{
    if (specialLimiters_.count())
    {
        bool ok = false;
        bool no = false;
        int ind = 0;
        QStringList l;
        while (ind < specialLimiters_.count() && !ok)
        {
            l = specialLimiters_.at(ind).split(",");
            foreach (QString ss, l)
            {
                ss = ss.trimmed();
                if (ss.startsWith('/'))
                {
                    no = true;
                    ss = ss.remove(0,1);
                }
                ss.replace("**", ".*");
                ss.replace('*', "\\w+");
                if (list.indexOf(QRegExp(ss)) >= 0)
                    ok = true;
                else
                    ok = false;
                if (no)
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
        updateSpecials(specials_, newlist, true);
        ModelItemBase::passSpecialDown(newlist);
    }
    else
        ModelItemBase::passSpecialDown(list);
}


void ModelItemBasic::passCostDown(int left)
{
    if (cost_ > left)
        limitedBy(eCostLimit);
    else
        limitedBy(-eCostLimit);
    ModelItemBase::passCostDown(left);
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

bool ModelItemBasic::branchSelected(int check, int role, int, int slot)
{
    if(!trunk_->branchSelected(check, role, index_, slot))
        return false;

    if ((check > 0 && !checked_) || checkLimit(eSelectionLimit))
        toggleCheck();

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
    if (!bAlwaysChecked_ && !checkLimit(eSelectionLimit) &&
            e->button() == Qt::LeftButton)
    {
        toggleCheck();

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

void ModelItemBasic::dealWithSpecials(const QStringList &list, bool check)
{
    updateSpecials(list, specialLimiters_, check);

    ModelItemBase::passSpecialDown(specialLimiters_);
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

void ModelItemBasic::updateSpecials(const QStringList &list,
                                    QStringList &to, bool check)
{
    foreach (QString s, list)
    {
        if (s.startsWith('-'))
        {
            s.remove(0,1);

            if (!check)
                to << s;
            else
                to.removeOne(s);
        }
        else if (s.startsWith('+'))
        {
            s.remove(0,1);

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
    if (countsAs_ > 0)
        trunk_->passCostUp(c, fa, countsAs_);
    if (specials_.count())
        trunk_->passSpecialUp(specials_, checked_);
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
    return trunk_->branchSelected(change, 0, index_, slot);
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
