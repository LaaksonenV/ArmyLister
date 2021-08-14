#include "modelitembase.h"

#include <QResizeEvent>

#include "settings.h"

ModelItemBase::ModelItemBase(QWidget *parent)
    : QWidget(parent)
    , cost_(0)
    , index_(-1)
    , branches_(QList<ModelItemBase *>())
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

void ModelItemBase::setCost(int i, int)
{
    cost_ += i;
    initCost_ += i;
    update();
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

bool ModelItemBase::branchSelected(int, int, int, int)
{
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
