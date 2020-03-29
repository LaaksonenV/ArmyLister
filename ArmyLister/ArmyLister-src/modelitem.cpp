#include "modelitem.h"

#include <QStringList>
#include <QList>
#include <QPushButton>
#include <QPainter>
#include <QSpinBox>
#include <QFontMetrics>
#include <QLabel>
#include <QTimer>
#include <QResizeEvent>
#include <QMouseEvent>

#include "settings.h"
#include "slotslabel.h"

TopModelItem::TopModelItem(Settings *set, QWidget *parent)
    : QWidget(parent)
    , _id(0)
    , _otherCosts(0)
    , _settings(set)
    , _belows(QList<ModelItem *>())
{
    setSizePolicy(QSizePolicy::Expanding,
                  QSizePolicy::Fixed);
}

TopModelItem::~TopModelItem(){}

void TopModelItem::reset()
{
    foreach (ModelItem *i, _belows)
        i->reset();
}

QSize TopModelItem::sizeHint() const
{
    return QSize(500, visibleUnders()*_settings->itemHeight);
}

int TopModelItem::addItem(ModelItem *item)
{
    item->move(_belows.count());
    _belows << item;
    setFixedHeight(sizeHint().height());
    return _belows.count()-1;
}

TopModelItem *TopModelItem::parentItem() const {return nullptr;}

void TopModelItem::insertItem(ModelItem *item, int to)
{
    for (int i = to; i < _belows.count(); ++i)
        _belows.at(i)->modifyIndex(1);

    int ret = 0;
    ModelItem *b;
    for (int i = 0; i < to; ++i)
    {
        b = _belows.at(i);
        ++ret;
        if (b->isExpanded())
            ret += b->visibleUnders();
    }
    item->move(ret);
    _belows.insert(to, item);
}

void TopModelItem::changeOtherCosts(int c, int role)
{
    _otherCosts += c;
    emit valueChanged(_otherCosts,-1);
    emit valueChanged(c,role);
}

int TopModelItem::visibleUnders() const
{
    int ret = 0;
    foreach (ModelItem *i, _belows)
    {
        ++ret;
        if (i->isExpanded())
            ret += i->visibleUnders();
    }
    return ret;
}

void TopModelItem::update()
{
    QWidget::update();
    foreach (ModelItem *i, _belows)
        i->update();
}

void TopModelItem::paintEvent(QPaintEvent*){}

void TopModelItem::resizeEvent(QResizeEvent *e)
{
    if (e->size().height() != e->oldSize().height())
        return;
    foreach (ModelItem *i, _belows)
        i->resize(e->size());
}

void TopModelItem::childExpanded(int item, int steps)
{

    setFixedHeight(height()+ steps*_settings->itemHeight);
    for (int i = item+1; i < _belows.count(); ++i)
        _belows.at(i)->move(steps);
}

void TopModelItem::childChecked(bool check, int role, int depth)
{
    if (depth == 1)
    {
    if (check)
        emit itemChecked(role,1);
    else
        emit itemChecked(role,-1);
    }
}

ModelItem* TopModelItem::getChild(int id)
{
    foreach (ModelItem *i, _belows)
        if (i->_id == id)
            return i;
    return nullptr;
}

ModelItem::ModelItem(Settings *set, TopModelItem *parent, bool)
    : TopModelItem(set, parent)
    , _above(parent)
//    , _limitClone(false)
    , _index(0)
    , _text(nullptr)//    , _texts(QList<SpecialLabel*>())
//    , _mshs(QList<MultiSelectionHandler*>())
    , _cost(0)
    , _basecost(0)
//    , _baseCostFor1(QList<int>())
    , _multiplier(1)
    , _multiplierismodels(false)
    , _baseModels(0)
//    , _modifier(0)
//    , _hasModifier(0)
//    , _takesModels(false)
//    , _takesModifier(false)
    , _spinner(nullptr)
//    , _hasModels(false)
//    , _hasLimit(false)
    , _expandButton(nullptr)
    , _expanded(false)
    , _checked(false)
    , _checkable(true)
    , _alwaysChecked(false)
//    , _clonable(nullptr)
    , _mouseIn(false)
{}

ModelItem::ModelItem(Settings *set, TopModelItem *parent)
    : ModelItem(set, parent, true)
{
    _index = _above->addItem(this);
    _id = _index+1;

    setFixedHeight(_settings->itemHeight);
    show();
}

// TODO CHECK
/*
ModelItem::ModelItem(ModelItem *copy, TopModelItem *parent, int index)
    : ModelItem(copy->_settings, parent, true)
{     
    if (index < 0)
    {
        _index = _above->addItem(this);

        // inform Limithandlers of a new customer
        emit copy->cloning(this);
    }
    else
    {
        _index = index;
        _above->insertItem(this, index);
    }

    // id
    _id = copy->_id;
    if (_id % 100)
    {
        _id += 1;
        _id *= 100;
    }

    _baseCostFor1 = copy->_baseCostFor1;
    setModifierIntake(copy->_takesModifier);

    foreach (ModelItem *i, copy->_belows)
        if (!i->_limitClone)
            new ModelItem(i, this);

    for (int i = 0; i < copy->_texts.count(); ++i)
    {
        _texts << new SpecialLabel(copy->_texts.at(i), this);
        if (index < 0)
            denySelection(i, true);
    }
    foreach (MultiSelectionHandler *h, copy->_mshs)
    {
        _mshs << new MultiSelectionHandler(h,this);
    }



    resize(copy->size());
//    setFixedHeight(_settings->itemHeight);



    if (copy->_spinner)
    {
        if (copy->_hasModels)
            setModels(copy->_spinner->minimum(), copy->_spinner->maximum());
        else
        {
            if (copy->_checked)
                setLimit(copy->_spinner->maximum()-copy->_spinner->value());
            else
                setLimit(copy->_spinner->maximum());
        }
    }
//    if (!copy->_hasLimit)
  //      on_multiplierChange(copy->_multiplier, true);

    setModelIntake(copy->_takesModels);

    if (index < 0)
        setAlwaysActive(copy->_alwaysChecked);


    _clonable = copy->_clonable;

    updateCost();
    show();
}*/

ModelItem::~ModelItem()
{
//    if (_clonable)
//        delete _clonable;
}

void ModelItem::reset()
{
    if (_checked)
        toggleCheck();
    if (_spinner)
        _spinner->setValue(_spinner->minimum());
    expand(true);
    ModelItem *itm;
    for(int i = 0; i < _belows.count(); ++i)
    {
        itm = _belows.at(i);
        itm->reset();
/*        if (itm->_limitClone)
        {
            delete _belows.takeAt(i);
            --i;
        }*/
    }
}

QStringList ModelItem::createTexts() const
{
    QStringList ret;

/*    foreach (SpecialLabel *l, _texts)
        ret << l->text();

    QString text = ret.join(", ");
    ret.clear();
*/
    ret << _text->text()
        << QString::number(_multiplier)// - _modifier)
        << QString::number(_cost + _otherCosts)
        << QString::number(_id);

    return ret;
}

QSize ModelItem::sizeHint() const
{
    if (!isExpanded())
        return QSize(300, (visibleUnders()+1)*_settings->itemHeight);
    return QSize(300, _settings->itemHeight);
}

int ModelItem::addItem(ModelItem *item)
{
    createPlus();
    item->QWidget::move(_settings->itemHPos,_settings->itemHeight);
    return TopModelItem::addItem(item);
}

TopModelItem *ModelItem::parentItem() const {return _above;}

void ModelItem::insertItem(ModelItem *item, int to)
{
    item->QWidget::move(_settings->itemHPos,_settings->itemHeight);
    TopModelItem::insertItem(item, to);

    childExpanded(to,1);
}

void ModelItem::modifyIndex(int amount)
{
    _index += amount;
}

/*ModelItem *ModelItem::cloneItem()
{
    ModelItem *ret = nullptr;
    if (_clonable && *_clonable)
    {
        ret = new ModelItem(this, parentItem(), _index+1);
        if (*_clonable > 0)
            --(*_clonable);
        ret->setFixedHeight(_settings->itemHeight);
        ret->setModelIntake(false);
        ret->_limitClone = true;
        if (*_clonable < -1)
            emit copied(ret);
    }

    return ret;
}*/

void ModelItem::setText(QString text)
{
    text = text.remove(QRegExp("[{}]")).trimmed();
    QRegExp xp("^\\|.+\\|$");
    int ind = -1;
    if (!text.isEmpty() && (ind = xp.indexIn(text)) < 0)
    {
/*        text = text.remove(QRegExp("\\|.+\\|"));
        if (at >= 0 && _texts.count() > at)
        {

            _texts.at(at)->setText(text.trimmed());
            _texts.at(at)->adjustSize();
            updateTexts(at);
        }

        else if (at < 0)
        {
            SpecialLabel *label = new SpecialLabel(text.trimmed(), this,
                                                   _texts.count());*/

        if (!_text)
             _text = new BaseLabel(this, _settings, text);
        else
            _text->setText(text);

        _text->show();
        //}
    }
    else
    {
        update();
    }
}

/*void ModelItem::removeText(int at)
{
    SpecialLabel *removing;
    if (at >= 0 && _texts.count() > at)
    {
        removing = _texts.takeAt(at);
        _baseCostFor1.removeAt(at);
    }
    else
        return;
    for (int i = at; i < _texts.count(); ++i)
        _texts.at(i)->move(_texts.at(i)->pos().x()-removing->width(),0);

    if (removing)
        delete removing;
    updateCost();
}*/

/*void ModelItem::setTexts(const QStringList &texts)
{
    _texts.clear();
    for (int i = 0; i < texts.count(); ++i)
        setText(texts.at(i));

    update();
}*/

/*QStringList ModelItem::getTexts() const
{
    QStringList ret;
    for (int i = 0; i < _texts.count(); ++i)
    {
        ret << _texts.at(i)->text();
    }
    return ret;
}*/

/*void ModelItem::updateTexts(int at)
{
    if (_texts.count())
    {
        if (at >= 0 && _texts.count() > at+1)
        {
            QFont f(QString(_settings->font),
                    _settings->textFontSize);
            f.setBold(true);
            int toMove = _texts.at(at)->width() + QFontMetrics(f).width(", ")
                    - (_texts.at(at+1)->pos().x()
                    - _texts.at(at)->pos().x());
            for (int i = at+1; i < _texts.count(); ++i)
            {
                _texts.at(i)->move(_texts.at(i)->pos() + QPoint(toMove,0));
            }
        }
    }
    update();
}*/
/* Was ist modifier
void ModelItem::setModifier(int i, bool up)
{
    if (_takesModifier)
    {
        _modifier += i;
        updateCost();
        emit modifierChanged(i);
    }
    else if (_hasModifier < 0 && up)
        _above->setModifier(i, true);
    if (!_hasModifier)
        foreach (ModelItem *bel, _belows)
        {
            bel->setModifier(i, false);
        }
}

void ModelItem::setModifierGiver(int i)
{
    _hasModifier = i;
    _takesModifier = false;
    if (_hasModifier > 0 && _checked)
        _above->setModifier(i,true);
}

void ModelItem::adjustModifier(int i)
{
    _modifier += i;
    updateCost();
    emit modifierChanged(i);

}

void ModelItem::setModifierIntake(bool b)
{
    _takesModifier = b;
}*/

/*void ModelItem::addPoint(int p)
{
    _cost += p;

    if (_texts.count() > _baseCostFor1.count())
    {
        _texts.at(_baseCostFor1.count())->setPoint(p);
        _baseCostFor1 << p;
    }
}

void ModelItem::setPoint(int p, int at)
{
    if (_baseCostFor1.count() > at)
    {
        _baseCostFor1.replace(at, p);
        _texts.at(at)->setPoint(p);
        updateCost();
    }
}

void ModelItem::setPoints(const QList<int> &ps)
{
    int p;
    for (int i = 0; i < ps.count(); ++i)
    {
        p = ps.at(i);
        _cost += p;
        _texts.at(i)->setPoint(p);
    }
    _costFor1 = _cost;
    _baseCostFor1 = ps;
}*/

void ModelItem::setCost(int c)
{
    _cost = c;
    _basecost = c;
}

void ModelItem::setAlwaysChecked(bool b)
{

    if (b && !_checked)
    {
        _checked = true;
        _above->changeOtherCosts(_cost,_index);
    }

    _alwaysChecked = b;
    update();
}

/*void ModelItem::setClonable(int i)
{
    if (_clonable)
        *_clonable = i;
    else
        _clonable = new int(i);
}

void ModelItem::setLimit(int limt, bool force)
{
    int limit = limt;

    if (_spinner)
    {
        if (_checked && !force)
            limit += _spinner->value();
        _spinner->setMaximum(limit);
        _spinner->setSuffix(QString("/")+QString::number(limit));
        if (limit > 1)
            _spinner->setVisible(true);
        else
            _spinner->setVisible(false);
    }
    else if (limt > 1)
        createSpinner(0,limt);
    if (limit)
        _checkable = true;
    else if (!_checked)
        _checkable = false;

}

void ModelItem::setLimitable(bool )
{
    if (!_clonable)
        _clonable = new int(-2);
    for (int i = 0; i < _texts.count(); ++i)
        denySelection(i, true);
    _hasLimit = true;
}

int ModelItem::getCurrentLimit() const
{
    if (!_hasLimit)
        return -1;
    if (_spinner)
    {
        if (_checked)
            return _spinner->maximum()-_spinner->value();
        else
            return _spinner->maximum();
    }
    if (_checkable)
        return 1;
    return 0;
}*/

void ModelItem::setValue(int val)
{
    if (_spinner)
        _spinner->setValue(val);
}

/*void ModelItem::changeTexts(const QString &txts)
{
    QStringList lst = txts.split(", ");
    for (int i = 0; i < lst.count(); ++i)
    {
        if(_texts.count() > i &&
           _texts.at(i)->text() != lst.at(i))
            _texts.at(i)->select(lst.at(i));
    }
}*/

void ModelItem::setSelection(Gear& pr, int at)
{

    checkLabel();

    _text->hide();
    setCost(_cost+pr.cost);
    _text->setDefault(pr, at);
    _text->show();
}

void ModelItem::addSelection(const QList<Gear> &list, int at)
{
    _text->addSelection(list, at);
//    _text->show();
}

void ModelItem::addSpecialCase(Gear pr)
{
    checkLabel();
    _text->addSpecial(pr);
}
/*
void ModelItem::changeSelection(int at, const QString &text,
                                int points)
{
    if (_checked)
    {
        toggleCheck();
        setText(text, at);
        setPoint(points, at);
        emit selectionChanged(at, text, this);
        toggleCheck();
    }
    else
    {
    setText(text, at);
    setPoint(points, at);
    emit selectionChanged(at, text, this);
    }
}

void ModelItem::removeSelection(int at, const QString &text)
{
    _texts.at(at)->removeSelection(text);
}

void ModelItem::denySelection(int at, bool denied)
{
    _texts.at(at)->denySelection(denied);
}*/

void ModelItem::setModels(int min,int max)
{
    _multiplierismodels = true;
    if (max < 0)
        max = min;
    createSpinner(min,max);
}

void ModelItem::updateCost()
{
    int change = _cost;

//    _costFor1 = 0;

//    foreach (int i, _baseCostFor1)
  //      _costFor1 += i;

    _cost = (_basecost+_otherCosts)*_multiplier;

    change = _cost - change;

    if (_checked)
        _above->changeOtherCosts(change,_index);

    update();
}

void ModelItem::changeOtherCosts(int c, int)
{
    _otherCosts += c;
    if (_checked)
        _above->changeOtherCosts(c,_index);
    update();
}

void ModelItem::move(int step)
{
    QWidget::move(pos().x(),pos().y()+_settings->itemHeight*step);
    update();
}

void ModelItem::expand(bool collapse)
{
    if (!_expandButton || collapse != _expanded)
        return;
    int toMove = visibleUnders();

    if (collapse)
        setFixedHeight(_settings->itemHeight);
    else
        setFixedHeight(_settings->itemHeight* (toMove+1));

    if (collapse)
        toMove = -toMove;

    if (_above)
        _above->childExpanded(_index, toMove);

    togglePlus();
}

bool ModelItem::isExpanded() const
{
    if (!_expandButton)
        return false;
    return _expandButton->isChecked();
}

void ModelItem::childExpanded(int item, int steps)
{
    if (_expanded)
    {
        TopModelItem::childExpanded(item, steps);
        _above->childExpanded(_index, steps);
    }
    else
        for (int i = item+1; i < _belows.count(); ++i)
            _belows.at(i)->move(steps);
    update();
}

void ModelItem::childChecked(bool check, int, int depth)
{
    _above->childChecked(check, _index, depth+1);
    if (check && !_checked)
        toggleCheck();
}

void ModelItem::paintEvent(QPaintEvent *)
{
    if (isVisible())
{
        QPainter p(this);
    QLinearGradient g(0,0, width(), _settings->itemHeight);
    if (_checked)
        g.setColorAt(0, QColor(200,255,200));
    else if (!_checkable)
        g.setColorAt(0, QColor(255,200,200));
    else
        g.setColorAt(0, Qt::white);

    if (_mouseIn)
        g.setColorAt(1, QColor(255,255,255));
    else
        g.setColorAt(1, QColor(230,240,255));

    QBrush b(g);
    p.setBrush(b);
    p.drawRect(rect());
    QFont f(QString(_settings->font),
            _settings->textFontSize);
    f.setBold(true);
    p.setFont(f);

/*    QRect comma(0,0,QFontMetrics(f).width(", ")
                ,_settings->itemHeight);

    for (int i = 0; i < _texts.count()-1; ++i)
    {
        comma.moveLeft(_texts.at(i)->x() + _texts.at(i)->width());
        p.drawText(comma, Qt::AlignVCenter | Qt::AlignLeft, ", ");
    }*/


    p.drawLine(width()-_settings->costFieldWidth,0,
               width()-_settings->costFieldWidth,height());

    p.drawText(width()-_settings->costFieldWidth + 10,
               (_settings->itemHeight/2)+(_settings->textFontSize/2),
               QString::number(_cost + _otherCosts));
}
}

void ModelItem::resizeEvent(QResizeEvent *e)
{
    if (e->size().height() != e->oldSize().height())
        return;
    foreach (ModelItem *i, _belows)
        i->resize(e->size().width()-_settings->itemHPos,
                  e->size().height());
}

void ModelItem::enterEvent(QEvent*)
{
    _mouseIn = true;
    update();
}

void ModelItem::leaveEvent(QEvent*)
{
    _mouseIn = false;
    update();
}

void ModelItem::mousePressEvent(QMouseEvent *e)
{
    if (!_alwaysChecked &&
            e->button() == Qt::LeftButton)
    {
        if (!_checked && !_checkable)
            return;
        toggleCheck();

        e->accept();
    }
}

void ModelItem::mouseDoubleClickEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
//        ModelItem *cp = cloneItem();
  //      if (cp)
            e->accept();
    }
}

void ModelItem::checkLabel()
{
    if (!_text)
    {
         _text = new SlotLabel(this, _settings);
         connect(_text, &BaseLabel::selectedCost,
                 this, &ModelItem::on_specialCost);
    }
}

void ModelItem::createPlus()
{
    if (_expandButton)
        return;
    _expandButton = new QPushButton("+", this);
    _expandButton->setFixedSize(_settings->expandButtonSize,
                                _settings->expandButtonSize);
    _expandButton->setCheckable(true);
    _expandButton->setFlat(true);
    _expandButton->move((_settings->itemHeight/2)
                            -(_settings->expandButtonSize/2),
                        (_settings->itemHeight/2)
                            -(_settings->expandButtonSize/2));
    _expandButton->show();
    connect(_expandButton, &QPushButton::toggled,
            this, [=](bool b){expand(!b);});
}

void ModelItem::togglePlus()
{
    if (_expandButton->isChecked())
    {
        _expandButton->setText("-");
        _expanded = true;
    }
    else
    {
        _expandButton->setText("+");
        _expanded = false;
    }
}

bool ModelItem::toggleCheck()
{
    if ((!_checked && !_checkable)
            //|| (_alwaysChecked && _checked)
            )
        return false;
    if (_spinner && !_spinner->value() && _spinner->maximum() && !_checked)
        _spinner->setValue(1);
    _checked = !_checked;
    _above->childChecked(_checked, _index);
    int total = _cost;
    int sign = 1;
    if (!_checked)
        sign = -1;
    emit checked(_multiplier*sign, this);
//    if (_checked)
//    if (_hasModifier > 0)
//        _above->setModifier(sign*_hasModifier,true);
    _above->changeOtherCosts(total*sign, _index);
    update();
   // if (_checked && _clonable)
   //     cloneItem();


    return true;
}


void ModelItem::createSpinner(int min, int max)
{
    _spinner = new QSpinBox(this);
    connect(_spinner, SIGNAL(valueChanged(int)),
            this, SLOT(on_spinnerChanged(int)));
    _spinner->setRange(min, max);
    _spinner->setSuffix(QString("/")+QString::number(max));
//    if (_texts.count())
        _spinner->move(_text->pos().x() +
                       _text->width() + 20,
                       0);
    _spinner->setMinimumHeight(_settings->itemHeight);
    if (min < max)
        _spinner->show();
    else
        _spinner->hide();
}

void ModelItem::on_spinnerChanged(int now)
{
//    int change = now-_multiplier;
//    if (_hasModels || _checked)
  //      emit multiplierChanged(change, this);
    _multiplier = now;

    //if(_multiplierismodels)
    //{
    // Inform children for cost/model and limit
    // Inform parent for model count
    //}
    updateCost();
//    foreach (ModelItem *i, _belows)
  //      i->on_multiplierChange(change);

}

void ModelItem::on_specialCost(int c)
{
    _basecost = c;
    updateCost();
}

/*void ModelItem::on_multiplierChange(int now, bool force)
{
//    int change = now-_multiplier;

    if (_takesModels || force)
    {
        _multiplier += now;
        emit multiplierChanged(now, this);
        updateCost();
    }

    foreach (ModelItem *i, _belows)
        i->on_multiplierChange(now);
}*/

