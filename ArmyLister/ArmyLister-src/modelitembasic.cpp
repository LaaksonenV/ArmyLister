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

    QFont f(_settings->font);
    f.setPointSize(_settings->textFontSize);
    f.setBold(true);
    _title->setFont(f);
    _title->setFixedHeight(_settings->itemHeight);
    _title->move(_settings->itemHeight,0);
    setFixedHeight(_settings->itemHeight);
}

ModelItemBasic::ModelItemBasic(Settings *set, ModelItemBase *parent)
    : ModelItemBase(set, parent)
    , _trunk(parent)
    , _checked(false)
    , _unitCountsAs(0)
    , _title(new QLabel(this))
    , _special(QStringList())
    , _specialLimiter(QStringList())
    , _initSpecial(QStringList())
    , _forAll(-1)
    , _costLimit(-1)
    , _modelLimitMin(0)
    , _hasModelLimitMin(false)
    , _modelLimitMax(0)
    , _hasModelLimitMax(false)
    , _skipChange(false)
    , _countsAs(0)
    , _expandButton(nullptr)
    , _expanded(false)
    , _alwaysChecked(false)
    , _mouseIn(false)
    , _limit(0)
{
    init();
}

ModelItemBasic::ModelItemBasic(ModelItemBasic *source, ModelItemBase *parent)
    : ModelItemBase(source, parent)
    , _trunk(parent)
    , _checked(false)
    , _unitCountsAs(0)
    , _title(new QLabel(this))
    , _special(QStringList())
    , _specialLimiter(QStringList())
    , _initSpecial(QStringList())
    , _forAll(-1)
    , _costLimit(-1)
    , _modelLimitMin(0)
    , _hasModelLimitMin(false)
    , _modelLimitMax(0)
    , _hasModelLimitMax(false)
    , _skipChange(false)
    , _countsAs(0)
    , _expandButton(nullptr)
    , _expanded(false)
    , _alwaysChecked(false)
    , _mouseIn(false)
    , _limit(0)
{
    init();

    setText(source->getText());
    setSpecial(source->_initSpecial);
    int min = 0;
    int max = 0;
    if (_hasModelLimitMin)
        min = source->_modelLimitMin+source->getCurrentCount();
    if (_hasModelLimitMax)
        min = source->_modelLimitMax+source->getCurrentCount();
    if (min || max)
        setModelLimiter(min, max);
    setAlwaysChecked(source->_alwaysChecked);
    if (source->_forAll >= 0)
        setForAll();
    setCostLimit(source->_costLimit);
    setCountsAs(source->_countsAs-1);
    setUnitCountsAs(source->_unitCountsAs-1);
}

ModelItemBasic::~ModelItemBasic()
{}

void ModelItemBasic::clone(ModelItemBase *toRoot, int i)
{
    ModelItemBasic *clone = new ModelItemBasic(this, toRoot);
    toRoot->addItem(clone, i);
    cloning(clone, i);

}

void ModelItemBasic::cloning(ModelItemBasic *clone, int i)
{
    connect(this, &ModelItemBasic::passConnection,
            clone, &ModelItemBasic::connectToSatellite);
    emit cloneSatellite();
    emit pingSatellite(false);
    ModelItemBase::clone(clone, i);
    emit pingSatellite(true);
    disconnect(this, &ModelItemBasic::passConnection,
            clone, &ModelItemBasic::connectToSatellite);

}

void ModelItemBasic::addItem(ModelItemBase *item,int)
{
    if (!_expandButton)
        _expandButton = createPlus();

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
    if (_expanded || underCover)
        return ModelItemBase::visibleItems()+1;
    else return 1;
}

void ModelItemBasic::setTrunk(ModelItemBase *item)
{
    QWidget::setParent(item);
    _trunk = item;
}

void ModelItemBasic::setText(const QString &text)
{
    QString name = text;
    name.remove(QRegExp("\\{.*\\}")).trimmed();

    _title->setText(name);
    _title->adjustSize();
    _title->show();
//    _special << "+" + name;
}

void ModelItemBasic::setSpecial(const QStringList &list)
{
    _initSpecial.append(list);
    foreach (QString s, list)
    {
        if (s.startsWith('-') || s.startsWith('+'))
            _special << s;
        else
            _specialLimiter << s;
    }
    if (list.count() && _checked)
        _trunk->passSpecialUp(_special,true);
}

void ModelItemBasic::setModelLimiter(int min, int max)
{
    if (min > 0)
    {
        _modelLimitMin = min;
        _hasModelLimitMin = true;
    }
    if (max > 0)
    {
        _modelLimitMax = max;
        _hasModelLimitMax = true;
    }

    if (_hasModelLimitMin || _hasModelLimitMax)
        passModelsDown(getCurrentCount());
}

void ModelItemBasic::setAlwaysChecked(bool b)
{
    if (b && !_checked)
    {
        bool fa = false;
        int c = _cost;
        if (_forAll >= 0)
        {
            fa = true;
            c = _forAll;
        }
        _checked = true;
        _trunk->passCostUp(c,fa);
    }

    _alwaysChecked = b;
    update();
}

void ModelItemBasic::setForAll(bool b)
{
    if (b && _forAll < 0)
    {
        _forAll = _cost;
        setCost(_forAll*getCurrentCount());
    }
}

void ModelItemBasic::setCostLimit(int limit)
{
    _costLimit = limit;
}

void ModelItemBasic::setCountsAs(int role)
{
    _countsAs = role+1;
}

void ModelItemBasic::setUnitCountsAs(int role)
{
    _unitCountsAs = role+1;
}

int ModelItemBasic::getCurrentCount() const
{
    return _trunk->getCurrentCount();
}

QString ModelItemBasic::getText() const
{
    return _title->text();
}

QString ModelItemBasic::getPrintText() const
{
    return _title->text();
}

void ModelItemBasic::passSpecialUp(const QStringList &list, bool check)
{
    if (check)
        _special << list;
    else
        foreach (QString s, list)
            _special.removeOne(s);

    if (_checked)
        _trunk->passSpecialUp(list, check);
}

void ModelItemBasic::passCostUp(int c, bool b, int role)
{
//    if (_countsAs >= 0)
  //      role = _countsAs;
    int change = c;
    bool fa = false;
    if (_forAll >= 0)
    {
        fa = true;
        change *= getCurrentCount();
        _forAll += c;
    }
    ModelItemBase::passCostUp(change,false, role);
    if (_checked)
        _trunk->passCostUp(c, fa || b, role);
    if (_countsAs && _countsAs != role)
        _trunk->passCostUp(c, fa || b, _countsAs);
    if (_costLimit > 0 && !role)
        ModelItemBase::passCostDown(_costLimit-_cost);
}

void ModelItemBasic::passModelsDown(int models)
{
    if (_hasModelLimitMin)
        _modelLimitMin -= models;
    if (_hasModelLimitMax)
        _modelLimitMax -= models;

    if (_checked)
    {
        if (_hasModelLimitMax)
            _skipChange = true;
        emit modelsChanged(models);
    }

    if ((_modelLimitMin > 0 || _modelLimitMax < 0) && !checkLimit(ModelsLimit))
        setHardLimit(ModelsLimit);
    else if (checkLimit(ModelsLimit))
        setHardLimit(-ModelsLimit);


    if (_forAll >= 0)
    {
        models *= _forAll;
        setCost(models);
    }
    else
        ModelItemBase::passModelsDown(models);
}

void ModelItemBasic::passSpecialDown(const QStringList &list)
{
    if (_specialLimiter.count())
    {
        bool ok = false;
        bool no = false;
        int ind = 0;
        QStringList l;
        while (ind < _specialLimiter.count() && !ok)
        {
            l = _specialLimiter.at(ind).split(",");
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
            setHardLimit(-SpecialLimit);
        else
            setHardLimit(SpecialLimit);
    }
    if (!_checked)
    {
        QStringList newlist(list);
        updateSpecials(_special, newlist, true);
        ModelItemBase::passSpecialDown(newlist);
    }
    else
        ModelItemBase::passSpecialDown(list);
}


void ModelItemBasic::passCostDown(int left)
{
    if (_cost > left)
        limitedBy(CostLimit);
    else
        limitedBy(-CostLimit);
    ModelItemBase::passCostDown(left);
}

void ModelItemBasic::branchExpanded(int item, int steps)
{
    if (_expanded)
    {
        ModelItemBase::branchExpanded(item, steps);
        _trunk->branchExpanded(_index, steps);
    }
    update();
}

bool ModelItemBasic::branchChecked(bool check, int, int role)
{
    if (role)
    {
        if (check)
            _unitCountsAs = role;
        else
            _unitCountsAs = 0;
    }
    if(!_trunk->branchChecked(check, _index, role))
        return false;

    if ((check && !_checked) || checkLimit(SelectionLimit))
        toggleCheck();

    return true;
}

bool ModelItemBasic::checkLimit(int limit)
{
    if (_limit & limit)
        return true;
    return false;
}

void ModelItemBasic::resizeEvent(QResizeEvent *e)
{
    if (e->size().height() != e->oldSize().height())
        return;
    QSize sz = e->size();
    sz.setWidth(sz.width()-_settings->itemHPos);
    foreach (ModelItemBase *i, _branches)
        i->resize(sz);
}

void ModelItemBasic::printToStream(QTextStream &str)
{
    if (_checked)
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
        str.setFieldWidth(Settings::Number(Settings::PlainTextWidth)-pre*10);
        str << override.at(0);
        str.setFieldWidth(0);
        str.setFieldAlignment(QTextStream::AlignRight);

        if (override.count() > 1)
        {
            str << override.at(1);
        }
        endl(str);
    }
    else if (list.indexIn(_title->text()) < 0)
    {
        str << QString("  ").repeated(pre);
        str.setFieldAlignment(QTextStream::AlignLeft);
        str.setFieldWidth(Settings::Number(Settings::PlainTextWidth)-pre*10);
        str << _title->text();
        str.setFieldWidth(0);
        str.setFieldAlignment(QTextStream::AlignRight);

        if (_cost)
        {
            str << _cost;
        }
        endl(str);
    }
}

void ModelItemBasic::paintEvent(QPaintEvent *)
{
    if (isVisible())
    {
        QPainter p(this);
        QLinearGradient g(0,0, width(), _settings->itemHeight);
        if (_checked)
            g.setColorAt(0, QColor(200,255,200));
        else if (checkLimit(GlobalLimit))
            g.setColorAt(0, QColor(255,200,200));
        else if (_limit > Checkable)
            g.setColorAt(0, QColor(150,150,150));
        else
            g.setColorAt(0, Qt::white);

        if (_mouseIn)
            g.setColorAt(1, QColor(255,255,255));
        else
            g.setColorAt(1, QColor(220,230,245));

        QBrush b(g);
        p.setBrush(b);
        p.drawRect(rect());

        QFont f(_settings->font);
        f.setPointSize(_settings->textFontSize);
        f.setBold(true);

        p.setFont(f);

        p.drawLine(width()-_settings->costFieldWidth,0,
                   width()-_settings->costFieldWidth,height());

        if (checkLimit(CostLimit))
           p.setPen(Qt::red);

        p.drawText(width()-_settings->costFieldWidth + 10,
                   (_settings->itemHeight/2)+(_settings->textFontSize/2),
                   QString::number(_cost));
    }
}

void ModelItemBasic::enterEvent(QEvent*)
{
    _mouseIn = true;
    update();
}

void ModelItemBasic::leaveEvent(QEvent*)
{
    _mouseIn = false;
    update();
}

void ModelItemBasic::mousePressEvent(QMouseEvent *e)
{
    if (!_alwaysChecked && !checkLimit(SelectionLimit) &&
            e->button() == Qt::LeftButton)
    {
        toggleCheck();

        e->accept();
    }
}

QPushButton *ModelItemBasic::createPlus()
{
    QPushButton *plus = new QPushButton("+", this);
    plus->setFixedSize(_settings->expandButtonSize,
                                _settings->expandButtonSize);
//    plus->setCheckable(true);
    plus->setFlat(true);
    plus->move((_settings->itemHeight/2)
                            -(_settings->expandButtonSize/2),
                        (_settings->itemHeight/2)
                            -(_settings->expandButtonSize/2));

    connect(plus, &QPushButton::pressed,
            this, &ModelItemBasic::toggleExpand);

    plus->show();

    return plus;
}

void ModelItemBasic::toggleExpand()
{
    if (!_expanded)
    {
        _expandButton->setText("-");
        _expanded = true;
        expand(true);

    }
    else
    {

        _expandButton->setText("+");

        expand(false);
        _expanded = false;
    }
}

void ModelItemBasic::dealWithSpecials(const QStringList &list, bool check)
{
    updateSpecials(list, _specialLimiter, check);

    ModelItemBase::passSpecialDown(_specialLimiter);
}


int ModelItemBasic::endOfText() const
{
    if (_title)
        return _title->pos().x() + _title->width();
    return 0;
}

void ModelItemBasic::fitButton(QPushButton *but)
{
    but->setFixedSize(_settings->expandButtonSize,
                                _settings->expandButtonSize);

    but->setFlat(true);
    but->move((_settings->itemHeight*3/2)
                            -(_settings->expandButtonSize/2),
                        (_settings->itemHeight/2)
                            -(_settings->expandButtonSize/2));

    _title->move(_title->pos().x()+_settings->expandButtonSize,
                 _title->pos().y());
}

void ModelItemBasic::expand(bool expanse)
{

    int toMove = visibleItems(true)-1;

    if (!expanse)
    {
        setFixedHeight(_settings->itemHeight);
        toMove = -toMove;
    }
    else
        setFixedHeight(_settings->itemHeight* (toMove+1));

    _trunk->branchExpanded(_index, toMove);
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

void ModelItemBasic::toggleCheck()
{
    if (!_checked && _limit > Checkable)
        return;

    if (!_trunk->branchChecked(!_checked, _index, _unitCountsAs))
        return;

    _checked = !_checked;

    if (_hasModelLimitMax)
    {
        _skipChange = true;
        if (_checked)
            emit modelsChanged(getCurrentCount());
        else
            emit modelsChanged(-getCurrentCount());

    }

    int c = _cost;
    bool fa = false;
    if (_forAll >= 0)
    {
        fa = true;
        c = _forAll;
    }

    if (!_checked)
        c = -c;

    _trunk->passCostUp(c, fa);
    if (_countsAs >= 0)
        _trunk->passCostUp(c, fa, _countsAs);
    if (_special.count())
        _trunk->passSpecialUp(_special, _checked);
    update();

    emit itemChecked(_checked);
}

void ModelItemBasic::forceCheck(bool check)
{
    if ((check && !_checked) || (!check && _checked))
        toggleCheck();
}

void ModelItemBasic::currentLimitChanged(int current)
{
    if (_skipChange)
        _skipChange = false;
    else
    {
        if (_hasModelLimitMax)
            _modelLimitMax -= current;
        if (_modelLimitMax < 0 && !checkLimit(ModelsLimit))
            limitedBy(ModelsLimit);
        else if (checkLimit(ModelsLimit))
            limitedBy(-ModelsLimit);
    }
}

void ModelItemBasic::connectToSatellite(ItemSatellite *sat,
                                             bool responsible)
{
    connect(this, &ModelItemBasic::itemChecked,
            sat, &ItemSatellite::on_itemChecked);

    connect(this, &ModelItemBasic::modelsChanged,
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
    if (limit > 0 && _checked)
        toggleCheck();
    limitedBy(limit);
    update();
}

void ModelItemBasic::limitedBy(short flag)
{
    if (flag > 0)
        _limit = _limit | flag;
    else
        _limit = _limit & ~flag;
}
