#ifndef SLOTBUTTON_H
#define SLOTBUTTON_H

#include <QWidget>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QString>
#include <QSpinBox>

class SlotButton : public QWidget
{
    Q_OBJECT

public:
    SlotButton(const QString& text, QWidget *parent = nullptr);

    virtual QSize sizeHint() const;

    virtual void setText(QString text);
    virtual QString getText() const;

    virtual void setAmount(int amount);

    virtual void setLimits(int min, int max);

    virtual bool toggleCheck(bool check, bool inform = true);
    virtual bool isChecked() const {return _checked;}


private:
    virtual void paintEvent(QPaintEvent *);

protected:
    virtual void mousePressEvent(QMouseEvent*e);

    virtual void createSpinner(int min, int max);

public slots:
    virtual void changeLimit(int change);
    virtual void changeModels(int change);

protected slots:
    virtual void on_spinnerChanged(int now);

signals:
    void checked(int amount);
    void toggled(bool chk);

private:
    QString _text;
    QSpinBox *_spinner;

    bool _checked;
    int _amount;
};

class SlotButtonGroup : public QObject
{
    Q_OBJECT

public:
    SlotButtonGroup(QObject *parent);

    void setExclusive(bool excl = true);

    QStringList checkedTexts();

    void addButton(SlotButton *but);


signals:
    void buttonClicked();

private:
    void buttonClick(int index, int amount);

    QList<SlotButton*> _buttons;
    bool _exclusive;
};

#endif // SLOTBUTTON_H
