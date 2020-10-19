#include "modelitembase.h"

#include <QResizeEvent>

#include "settings.h"

ModelItemBase::ModelItemBase(Settings *set, QWidget *parent)
    : QWidget(parent)
    , _settings(set)
    , _cost(0)
    , _index(-1)
    , _branches(QList<ModelItemBase *>())
    , _initCost(0)
{
    setFixedHeight(0);
    setSizePolicy(QSizePolicy::MinimumExpanding,
                  QSizePolicy::Fixed);
}

ModelItemBase::ModelItemBase(ModelItemBase *source, ModelItemBase *parent)
    : ModelItemBase(source->_settings, parent)
/*    , _settings(source->_settings)
    , _cost(source->_initCost)
    , _initCost(source->_initCost)
    , _index(-1)
    , _branches(QList<ModelItemBase *>())*/
{
    setCost(source->_initCost);
    setFixedHeight(0);
    setSizePolicy(QSizePolicy::MinimumExpanding,
                  QSizePolicy::Fixed);
}

ModelItemBase::~ModelItemBase()

{
}

void ModelItemBase::clone(ModelItemBase *toRoot, int i)
{
    for (int j = 0; j < _branches.count(); ++j)
        _branches.at(j)->clone(toRoot, i);
}

QSize ModelItemBase::sizeHint() const
{
    return QSize(300, visibleItems()*_settings->itemHeight);
}

void ModelItemBase::addItem(ModelItemBase *item,int)
{
    item->moveSteps(_branches.count(),0);
    item->setIndex(_branches.count());
    _branches << item;
    branchExpanded(-1,1);
    item->show();
}

void ModelItemBase::insertItem(ModelItemBase *item, int to)
{
    if (_branches.count() <= to)
        addItem(item);
    else
    {
        QPoint p = _branches.at(to)->pos();
        for (int i = to; i < _branches.count(); ++i)
            _branches.at(i)->setIndex(i+1);

        item->move(p);
        item->setIndex(to);
        _branches.insert(to, item);
        branchExpanded(to, 1);
        item->show();
    }
}

void ModelItemBase::moveSteps(int stepY, int stepX)
{
    QWidget::move(pos().x()+_settings->itemHPos*stepX,
                  pos().y()+_settings->itemHeight*stepY);
}

int ModelItemBase::visibleItems(bool) const
{
    int ret = 0;
    foreach (ModelItemBase *i, _branches)
    {
        ret += i->visibleItems();
    }
    return ret;
}

void ModelItemBase::setIndex(int i)
{
    _index = i;
}

void ModelItemBase::setCost(int i)
{
    _cost = i;
    _initCost = i;
    update();
}

QString ModelItemBase::getText() const
{
     return QString();
}

int ModelItemBase::getCost() const
{
    return _cost;
}

int ModelItemBase::getCurrentCount() const
{
    return -1;
}

void ModelItemBase::passSpecialUp(const QStringList &, bool)
{}

void ModelItemBase::passCostUp(int c, bool, int role)
{
    if (role <= 0)
    {
        _cost += c;
        update();
        emit valueChanged(c, -1);
    }
}

void ModelItemBase::passModelsDown(int models, bool push)
{
    foreach (ModelItemBase *i, _branches)
    {
        i->passModelsDown(models, push);
    }
}

void ModelItemBase::passSpecialDown(const QStringList &list)
{
    foreach (ModelItemBase *i, _branches)
    {
        i->passSpecialDown(list);
    }
}

void ModelItemBase::passCostDown(int left)
{
    foreach (ModelItemBase *i, _branches)
    {
        i->passCostDown(left);
    }
}

bool ModelItemBase::branchSelected(int, int, int)
{
    return true;
}

void ModelItemBase::resizeEvent(QResizeEvent *e)
{
    if (e->size().height() != e->oldSize().height())
        return;
    foreach (ModelItemBase *i, _branches)
        i->resize(e->size());
}

void ModelItemBase::printToStream(QTextStream &str)
{
    foreach (ModelItemBase *i, _branches)
        i->printToStream(str);
}

void ModelItemBase::endCloning()
{
    emit releaseCloneSatellite();
    foreach (ModelItemBase *i, _branches)
    {
        i->endCloning();
    }
}

void ModelItemBase::expand(bool)
{
}

void ModelItemBase::branchExpanded(int item, int steps)
{
    setFixedHeight(height()+ steps*_settings->itemHeight);
    if (item >= 0)
        for (int i = item+1; i < _branches.count(); ++i)
            _branches.at(i)->moveSteps(steps);
}
