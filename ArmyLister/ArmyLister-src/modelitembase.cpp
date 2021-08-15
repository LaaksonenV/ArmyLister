#include "modelitembase.h"

#include <QResizeEvent>

#include "settings.h"

ModelItemBase::ModelItemBase(QWidget *parent)
    : QWidget(parent)
    , cost_(0)
    , index_(-1)
    , branches_(QList<ModelItemBase *>())
    , takeLimit_(0)
    , initTakeLimit_(0)
    , takeBy_(0)
    , bDynamicText_(false)
    , initCost_(0)
{
    setFixedHeight(0);
    setSizePolicy(QSizePolicy::MinimumExpanding,
                  QSizePolicy::Fixed);
}

ModelItemBase::ModelItemBase(ModelItemBase *source, ModelItemBase *parent)
    : ModelItemBase(parent)
{
    setCost(source->initCost_);
    setTakeLimit(source->initTakeLimit_, source->takeBy_);
    setFixedHeight(0);
    setSizePolicy(QSizePolicy::MinimumExpanding,
                  QSizePolicy::Fixed);
}

ModelItemBase::~ModelItemBase()

{
}

void ModelItemBase::clone(ModelItemBase *toRoot, int i)
{
    for (int j = 0; j < branches_.count(); ++j)
        branches_.at(j)->clone(toRoot, i);
}

QSize ModelItemBase::sizeHint() const
{
    return QSize(300, visibleItems()*Settings::ItemMeta(Settings::eItem_Height));
}

ModelItemBase* ModelItemBase::getItem(const QString &text) const
{
    foreach (ModelItemBase *i, branches_)
    {
        if (i->getText() == text)
            return i;
    }
    return nullptr;
}

void ModelItemBase::addItem(ModelItemBase *item)
{
    item->moveSteps(branches_.count(),0);
    item->setIndex(branches_.count());
    branches_ << item;
    branchExpanded(-1,1);
    item->show();
}

void ModelItemBase::insertItem(ModelItemBase *item, int to)
{
    if (branches_.count() <= to)
        addItem(item);
    else
    {
        QPoint p = branches_.at(to)->pos();
        for (int i = to; i < branches_.count(); ++i)
            branches_.at(i)->setIndex(i+1);

        item->move(p);
        item->setIndex(to);
        branches_.insert(to, item);
        branchExpanded(to, 1);
        item->show();
    }
}

void ModelItemBase::moveSteps(short stepY, short stepX)
{
    QWidget::move(pos().x()+Settings::ItemMeta(Settings::eItemHPos)*stepX,
                  pos().y()+Settings::ItemMeta(Settings::eItem_Height)*stepY);
}

int ModelItemBase::visibleItems(bool) const
{
    int ret = 0;
    foreach (ModelItemBase *i, branches_)
    {
        ret += i->visibleItems();
    }
    return ret;
}

void ModelItemBase::setIndex(int i)
{
    index_ = i;
}

void ModelItemBase::setText(const QString &, int)
{
    bDynamicText_ = true;
}

void ModelItemBase::setCost(int i, int)
{
    cost_ += i;
    initCost_ += i;
    update();
}

void ModelItemBase::setTakeLimit(int take, int by)
{
    takeLimit_ = take;
    initTakeLimit_ = take;
    if (by)
    {
        takeLimit_ = (getCurrentCount()*take)/by;
        takeBy_ = by;
    }
    setText("Take up to " + QString::number(takeLimit_) + ": ");
}


void ModelItemBase::loadSelection(QString &str)
{
    foreach (ModelItemBase *i, branches_)
    {
        i->loadSelection(str);
    }
}

void ModelItemBase::saveSelection(QTextStream &str)
{
    foreach (ModelItemBase *i, branches_)
    {
        i->saveSelection(str);
    }
}

QString ModelItemBase::getText() const
{
     return QString();
}

int ModelItemBase::getCost() const
{
    return cost_;
}

int ModelItemBase::getCurrentCount() const
{
    return -1;
}

void ModelItemBase::passTagsUp(const QStringList &, bool)
{}

void ModelItemBase::passCostUp(int c, bool, int role)
{
    if (!role)
    {
        cost_ += c;
        update();
        emit valueChanged(c, -1);
    }
}

void ModelItemBase::passModelsDown(int models, bool push)
{
    int change = takeLimit_;
    if (takeBy_)
    {
        takeLimit_ = (getCurrentCount()*initTakeLimit_)/takeBy_;
        if (change != takeLimit_)
            ModelItemBase::passTakeLimitDown(takeLimit_);
    }

    foreach (ModelItemBase *i, branches_)
    {
        i->passModelsDown(models, push);
    }
}

void ModelItemBase::passTagsDown(const QStringList &list)
{
    foreach (ModelItemBase *i, branches_)
    {
        i->passTagsDown(list);
    }
}

void ModelItemBase::passCostDown(int left)
{
    foreach (ModelItemBase *i, branches_)
    {
        i->passCostDown(left);
    }
}

void ModelItemBase::passTakeLimitDown(int limit)
{
    int newlimit = limit - selectedBranches_.count();
    while (newlimit < 0)
    {
        branches_.at(selectedBranches_.first())->toggleCheck();
        // as a branch deselects, it calls this's branchSelected() and removes
        // one from selecteds
        newlimit = limit - selectedBranches_.count();
    }
    for (int i = 0; i < branches_.count(); ++i)
    {
        if (!selectedBranches_.contains(i))
            branches_.at(i)->passTakeLimitDown(newlimit);
    }
}

bool ModelItemBase::branchSelected(int check, int, int index, int)
{
    if (initTakeLimit_ && selectedBranches_.count() + check > takeLimit_)
        return false;

    if (check > 0)
        for (int i = 0; i < check; ++i)
            selectedBranches_.append(index);
    if (check < 0)
        for (int i = 0; i > check; --i)
            selectedBranches_.removeOne(index);

    if (initTakeLimit_)
    {
        if (bDynamicText_)
        {
            QString text;
            QStringList texts;
            QList<int> done;
            int amount;
            foreach (int i, selectedBranches_)
            {
                if (!done.contains(i))
                {
                    done << i;
                    amount = selectedBranches_.count(i);
                    if (amount > 1)
                    {
                        text = QString::number(amount) + " " +
                                branches_.at(i)->getText();
                        if (!text.endsWith('s'))
                            text.append('s');
                        texts << text;
                    }
                    else
                        texts << branches_.at(i)->getText();
                }
            }
            if (texts.count() > 1)
            {
                text = texts.takeLast();
                texts.last().append(" and " + text);
            }
            setText("Take up to " + QString::number(takeLimit_) + ": " +
                    texts.join(', '));
        } // if (bDynamicText_)

        ModelItemBase::passTakeLimitDown(takeLimit_);
    }

    return true;
}

void ModelItemBase::resizeEvent(QResizeEvent *e)
{
    if (e->size().height() != e->oldSize().height())
        return;
    foreach (ModelItemBase *i, branches_)
        i->resize(e->size());
}

void ModelItemBase::printToStream(QTextStream &str)
{
    foreach (ModelItemBase *i, branches_)
        i->printToStream(str);
}

void ModelItemBase::endCloning()
{
    emit releaseCloneSatellite();
    foreach (ModelItemBase *i, branches_)
    {
        i->endCloning();
    }
}

void ModelItemBase::expand(bool)
{
}

void ModelItemBase::branchExpanded(int item, int steps)
{
    setFixedHeight(height()+ steps*Settings::ItemMeta(Settings::eItem_Height));
    if (item >= 0)
        for (int i = item+1; i < branches_.count(); ++i)
            branches_.at(i)->moveSteps(steps);
}
