/*#include "limitmihandler.h"

#include <QMutableMapIterator>

#include "modelitem.h"

LimitMIhandler::LimitMIhandler(QObject *parent)
    : QObject(parent)
//    , _current(0)
//    , _limit(0)
//    , _timer(0)
//    , _divider(0)
    , _targets(QList<ModelItem*>())
    , _selectables(QMap<int, QList<Limiter*>* >())
    , _globalLimits(QList<Limiter*>())
    , _globallim(-1)
    , _modifier(0)
//    , _modifiable(false)
    , _models(0)
//    , _selections(QStringList())
    , _followed(nullptr)
    , _limitClone(nullptr)
{

}

LimitMIhandler::~LimitMIhandler()
{
    foreach (QList<Limiter*> *l, _selectables)
    {
        foreach (Limiter *lim, *l)
            delete lim;
        delete l;
    }
    foreach (Limiter *lim, _globalLimits)
        delete lim;
}

void LimitMIhandler::setLimits(int limit, int by, int of)
{
    Limiter *newlim;

    if (of >= 0)
    {
        QList<Limiter*> *limlist;
        if (_selectables.contains(of))
            limlist = _selectables.value(of);
        else
        {
            limlist =  new QList<Limiter*>();
            _selectables.insert(of, limlist);
        }

        if (limlist->count())
            newlim = limlist->first();
        else
        {
            newlim =  new Limiter();
            limlist->append(newlim);
        }
    }
    else
    {
        if (_globalLimits.count())
            newlim = _globalLimits.first();
        else
        {
            newlim =  new Limiter();
            _globalLimits.append(newlim);
        }
    }

    newlim->_timer = limit;
    newlim->_divider = by;
    updateLimit(newlim, (of < 0));
}

void LimitMIhandler::setModifiable(bool b, int of)
{
    Limiter *newlim;

    if (of < 0)
    {
        if (_globalLimits.count())
            newlim = _globalLimits.first();
        else
        {
            newlim =  new Limiter();
            _globalLimits.append(newlim);
        }
    }
    else
    {
        QList<Limiter*> *limlist;
        if (_selectables.contains(of))
            limlist = _selectables.value(of);
        else
        {
            limlist =  new QList<Limiter*>();
            _selectables.insert(of, limlist);
        }

        if (limlist->count())
            newlim = limlist->first();
        else
        {
            newlim =  new Limiter();
            limlist->append(newlim);
        }
    }    

    newlim->_modifiable = b;
    updateLimit(newlim, (of < 0));
}

void LimitMIhandler::addSelections(int of, const QStringList &texts,
                                   int limit, int by, bool modif)
{
    Limiter *newlim =  new Limiter();

    newlim->_timer = limit;
    newlim->_divider = by;
    newlim->_modifiable = modif;
    newlim->_selections = texts;
    updateLimit(newlim, (of < 0));

    if (of >= 0)
    {
        QList<Limiter*> *limlist;
        if (_selectables.contains(of))
            limlist = _selectables.value(of);
        else
        {
            limlist =  new QList<Limiter*>();
            _selectables.insert(of, limlist);
        }
        limlist->append(newlim);
    }
}

void LimitMIhandler::setAddSource(ModelItem *item)
{
    _targets << item;

    inputLimit(item);

    connect(item, &ModelItem::cloning,
            this, [=](ModelItem *to){on_cloning(to, 0);});
    connect(item, &ModelItem::removing,
            this, &LimitMIhandler::on_remove);
}

void LimitMIhandler::addPickTarget(ModelItem *item)
{
    _targets << item;
    connect(item, &ModelItem::checked,
            this, &LimitMIhandler::on_check);

    inputLimit(item);

    connect(item, &ModelItem::cloning,
            this, [=](ModelItem *to){on_cloning(to, 1);});
    connect(item, &ModelItem::removing,
            this, &LimitMIhandler::on_remove);
}

void LimitMIhandler::setPickSource(ModelItem *item)
{
    connect(item, &ModelItem::cloning,
            this, [=](ModelItem *to){on_cloning(to, 2);});
}

void LimitMIhandler::setFollowing(ModelItem *item)
{
    _followed = item;

    connect(_followed, &ModelItem::copied,
            this, &LimitMIhandler::on_copy);
    connect(_followed, &ModelItem::multiplierChanged,
            this, &LimitMIhandler::on_modelChange);
    connect(_followed, &ModelItem::modifierChanged,
            this, &LimitMIhandler::on_modifierChange);
    connect(_followed, &ModelItem::selectionChanged,
            this, &LimitMIhandler::on_selection);

    connect(item, &ModelItem::cloning,
            this, [=](ModelItem *to){on_cloning(to, 3);});
    connect(item, &ModelItem::removing,
            this, &LimitMIhandler::on_remove);
}

void LimitMIhandler::inputLimit(ModelItem *item)
{
    int cur, lim = _globallim;
    QStringList texts = item->getTexts();

    for (int i = 0; i < texts.count(); ++i)
    {
        if (_selectables.contains(i))
        {
            foreach (Limiter *limt, *(_selectables.value(i)))
                if(limt->_selections.contains(texts.at(i)))
                {
                    cur = limt->_limit;
                    if (cur >= 0 &&
                            (lim < 0 || (cur - limt->_current) < lim))
                        lim = cur - limt->_current;
                }
        }
    }
    if (lim >= 0)
        item->setLimit(lim);
}

void LimitMIhandler::on_copy(ModelItem *cp)
{
    _targets << cp;

    connect(cp, &ModelItem::copied,
            this, &LimitMIhandler::on_copy);
    connect(cp, &ModelItem::checked,
            this, &LimitMIhandler::on_check);
    connect(cp, &ModelItem::multiplierChanged,
            this, &LimitMIhandler::on_check);
    connect(cp, &ModelItem::selectionChanged,
            this, &LimitMIhandler::on_selection);

    inputLimit(cp);

}

void LimitMIhandler::on_cloning(ModelItem *to, int type)
{
    LimitMIhandler *clone = _limitClone;
    if (type != 1)
    {
        _limitClone = (clone = new LimitMIhandler(parent()));
        QList<Limiter*> *limlist;
        foreach (int k, _selectables.keys())
        {
            limlist = new QList<Limiter*>();
            foreach (Limiter *limt, *(_selectables.value(k)))
                limlist->append(new Limiter(limt));
            clone->_selectables.insert(k, limlist);
        }
        foreach (Limiter *limt, _globalLimits)
            clone->_globalLimits.append(new Limiter(limt));

        if(_models)
            clone->_models = 1;
    }

    if (type == 0)
        clone->setAddSource(to);
    else if (type == 1)
        clone->addPickTarget(to);
    else if (type == 2)
        clone->setPickSource(to);
    else if (type == 3)
        clone->setFollowing(to);
}

void LimitMIhandler::on_remove(ModelItem *itm)
{
    if (_followed == itm)
        deleteLater();
    else
        _targets.removeOne(itm);
}

void LimitMIhandler::on_selection(int at, const QString &text,
                                  ModelItem *by)
{
    int lim = -1;
    int cur = 0;

    foreach (Limiter *limt, _globalLimits)
    {
        cur = limt->_limit;
        if (cur >= 0)
            lim = cur - limt->_current;
    }

    if (_selectables.contains(at))
    {
        foreach (Limiter *limt, *(_selectables.value(at)))
            if(limt->_selections.contains(text))
            {
                cur = limt->_limit;
                if (cur >= 0 &&
                        (lim < 0 || (cur - limt->_current) < lim))
                    lim = cur - limt->_current;
            }
    }
    by->setLimit(lim);

}

void LimitMIhandler::on_check(int chk, ModelItem *by)
{

    foreach (Limiter *limt, _globalLimits)
        limt->_current += chk;

    QStringList texts = by->getTexts();

    for (int i = 0; i < texts.count(); ++i)
    {
        if (_selectables.contains(i))
        {
            foreach (Limiter *limt, *(_selectables.value(i)))
                if(limt->_selections.contains(texts.at(i)))
                    limt->_current += chk;
        }
    }

    if (_followed)
        _followed->adjustModifier(chk);
}

void LimitMIhandler::on_modelChange(int m, ModelItem *)
{
    _models += m;

    foreach (Limiter *limt, _globalLimits)
        updateLimit(limt, true);


    foreach (QList<Limiter*> *limlist, _selectables.values())
        foreach (Limiter *limt, *limlist)
            updateLimit(limt, false);

    foreach (ModelItem *itm, _targets)
    {
        inputLimit(itm);
    }
}

void LimitMIhandler::on_modifierChange(int m)
{
    _modifier += m;
    on_modelChange(0);
}

int LimitMIhandler::updateLimit(Limiter *limt, bool global)
{
    int lim = -1;
    if (limt->_divider)
    {
        if (limt->_modifiable)
            lim = limt->_limit = (_models-_modifier)
                    *limt->_timer/limt->_divider;
        else
            lim = limt->_limit = _models
                    *limt->_timer/limt->_divider;
    }
    if (global && lim >= 0)
    {
        lim = lim - limt->_current;
        if (_globallim <= 0 || lim < _globallim)
            _globallim = lim;
    }
    return lim;
}*/
