#include "slotbutton.h"

#include <QPainter>

SlotButton::SlotButton(const QString &text, int amount, QWidget *parent)
    : QWidget(parent)
    , _text(text)
    , _spinner(nullptr)
    , _checked(false)
    , _amount(amount)
{
    setFixedHeight(20);
}

QSize SlotButton::sizeHint() const
{
    QSize ret;
    ret.setHeight(20);
    ret.setWidth(QFontMetrics(font()).width(_text)+10);
    if (_spinner)
        ret.rwidth() += _spinner->width();

    return ret;
}

void SlotButton::setText(QString text)
{
    _text = text;
    adjustSize();
    update();
}

QString SlotButton::getText() const
{
    return _text;
}

void SlotButton::setLimits(int min, int max)
{
    if (max < 0)
        max = min;
    if (max <= min)
        setDisabled(true);
    else
        setDisabled(false);
    createSpinner(min,max);
}

bool SlotButton::toggleCheck(bool check, bool inform)
{
    bool change = true;
    if (_spinner)
    {
        if (check)
        {
            if (_spinner->value() >= _spinner->maximum())
                change = false;
            else
                _spinner->stepUp();
        }
        else
        {
            if (_spinner->value())
                _spinner->stepDown();
            else
                change = false;
        }
/*        if (_spinner->value())
            _checked = true;
        else
            _checked = false;*/
    }
    else if (_amount >= 0)
    {
        if (check)
            ++_amount;
        else if (_amount > 0)
            --_amount;
        else change = false;

        if (_amount)
            _checked = true;
        else
            _checked = false;
    }
    else
    {
        if (_checked != check)
            _checked = !_checked;
        else
            change = false;
    }

    if (change)
    {
        emit toggled(check);
        if (inform)
        {
            if (_checked)
                emit checked(1);
            else
                emit checked(-1);
        }
    }
    update();
    return _checked;
}

void SlotButton::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QLinearGradient g(0,0, width(), height());
    if (_checked)
        g.setColorAt(0, QColor(200,255,200));
    else if (!isEnabled())
        g.setColorAt(0, Qt::lightGray);
    else
        g.setColorAt(0, Qt::white);

/*    if (_mouseIn)
        g.setColorAt(1, QColor(255,255,255));
    else*/
        g.setColorAt(1, QColor(230,240,255));

    QBrush b(g);
    p.setBrush(b);
    p.drawRect(rect());
/*    QFont f(QString(_settings->font),
            _settings->textFontSize);
    f.setBold(true);
    p.setFont(f);*/

    if (_spinner)
        p.drawText(_spinner->width()+10,
                   (height()/2)+(font().pointSize()/2),
                   _text);
    else
        p.drawText(10,
                   (height()/2)+(font().pointSize()/2),
                   _text);
}

void SlotButton::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        toggleCheck(true);

        e->accept();
    }
    else if (e->button() == Qt::RightButton)
    {
        toggleCheck(false);

        e->accept();
    }
}

void SlotButton::createSpinner(int min, int max)
{
    _spinner = new QSpinBox(this);
    connect(_spinner, SIGNAL(valueChanged(int)),
            this, SLOT(on_spinnerChanged(int)));
    _spinner->setRange(min, max);
    _spinner->setSuffix(QString("/")+QString::number(max));

//    _spinner->setMinimumHeight(_settings->itemHeight);
}

void SlotButton::changeLimit(int change)
{
    if (_spinner)
    {
        if (change > 0 || _spinner->value())
        {
            _spinner->setMaximum(_spinner->maximum()+change);
        }
    }
    else if (_amount >= 0)
    {
        if (change > 0 || _amount)
        {
            _amount += change;
        }
    }
    else
    {
        if (change < 0)
            setDisabled(true);
        else
            setEnabled(true);
        update();
    }
}

void SlotButton::on_spinnerChanged(int now)
{
    int change = now - _amount;
    emit checked(change);
}


SlotButtonGroup::SlotButtonGroup(QObject *parent)
    : QObject(parent)
    , _buttons(QList<SlotButton*>())
    , _exclusive(false)
{
}

void SlotButtonGroup::setExclusive(bool excl)
{
    _exclusive = excl;
}

QStringList SlotButtonGroup::checkedTexts()
{
    QStringList ret;
    foreach (SlotButton *b, _buttons)
    {
        if (b->isChecked())
            ret << b->getText();
    }
    return ret;
}

void SlotButtonGroup::addButton(SlotButton *but)
{
    int count = _buttons.count();
    connect(but, &SlotButton::checked,
            [=](int amount){buttonClick(count, amount);});
    _buttons << but;
}

void SlotButtonGroup::buttonClick(int index, int amount)
{
    SlotButton *button;
    if (amount != 0)
    {
        for (int i = 0; i < _buttons.count(); ++i)
        {
            button = _buttons.at(i);
            if (i != index)
            {
                if (!_exclusive)
                    button->changeLimit(-amount);
                else if (amount > 0)
                    button->toggleCheck(false, false);
            }
            else if (_exclusive && amount < 0)
                button->toggleCheck(true, false);
        }
    }
    if (!_exclusive || amount > 0)
        emit buttonClicked();
}
