#ifndef MODELITEM_H
#define MODELITEM_H

#include <QWidget>

class QStringList;
class QString;
template<typename T> class QList;
class QPushButton;
class QSpinBox;
class QLabel;

class Settings;
class SpecialLabel;
class MultiSelectionHandler;
class ModelItem;
class TopModelItem : public QWidget
{
    Q_OBJECT

public:
    TopModelItem(Settings *set ,QWidget *parent);
    virtual ~TopModelItem();

    void reset();

    virtual QSize sizeHint() const;

    virtual int addItem(ModelItem *item);
    virtual TopModelItem *parentItem() const;

    virtual void insertItem(ModelItem *item, int to);

    virtual void changeOtherCosts(int c, int role);
    virtual void setModifier(int,bool){}

    virtual void childExpanded(int item, int steps);
    virtual void childChecked(bool check, int role, int depth = 0);

    virtual ModelItem *getChild(int id);

public slots:
    virtual void update();

signals:
    void itemChecked(int item, int amount);
    void valueChanged(int value, int item);


protected:
    virtual void paintEvent(QPaintEvent*);
    virtual void resizeEvent(QResizeEvent *e);

    virtual int visibleUnders() const;

    int _id;
    int _otherCosts;
public: int getCost() const {return _otherCosts;}

public:
    Settings *_settings;

    QList<ModelItem *> _belows;

};



class ModelItem : public TopModelItem
{
    Q_OBJECT

private:
    ModelItem(Settings *set, TopModelItem *parent, bool);

public:
    ModelItem(Settings *set, TopModelItem *parent);
//    ModelItem(ModelItem *copy, TopModelItem *parent, int index = -1);
    virtual ~ModelItem();

    void reset();

    //Print function
    QStringList createTexts() const;

    virtual QSize sizeHint() const;

    virtual int addItem(ModelItem *item);
    virtual TopModelItem *parentItem() const;
    virtual void insertItem(ModelItem *item, int to);
    virtual void modifyIndex(int amount);

    //virtual ModelItem *cloneItem();

    virtual void setText(QString text);
//    virtual void removeText(int at);
//    virtual void setTexts(const QStringList &texts);
//    virtual QStringList getTexts() const;
//    virtual void updateTexts(int at = -1);
//    virtual void setModifier(int i, bool up);
//    virtual void setModifierGiver(int i);
//    virtual void adjustModifier(int i);
//    virtual void addPoint(int p);
//    virtual void setPoint(int p, int at);
//    virtual void setPoints(const QList<int> &ps);
 //   virtual void setModifierIntake(bool b = true);
    virtual void setCost(int c);
//    virtual void setAlwaysActive(bool b = true);
//    virtual void setClonable(int i);
//    virtual void setLimit(int limt, bool force = false);
//    virtual void setLimitable(bool = true);
//    virtual int getCurrentLimit() const;
    virtual void setValue(int val);
//    virtual void changeTexts(const QString &txts);

//    virtual int addSelection(const QString &text,
  //                    const QList<std::pair<QString,int> > &list);
//    virtual void changeSelection(int at,
  //                               const QString &text,
    //                             int points);
//    virtual void removeSelection(int at, const QString &text);
//    virtual void denySelection(int at, bool denied);
    virtual void setModels(int min, int max);

    virtual void updateCost();
    virtual void changeOtherCosts(int c, int);

    virtual void move(int step);

    virtual void expand(bool collapse);
    virtual bool isExpanded() const;
    virtual void childExpanded(int item, int steps);
    virtual void childChecked(bool check, int, int depth = 0);

protected:
    virtual void paintEvent(QPaintEvent*);
    virtual void resizeEvent(QResizeEvent *e);
    virtual void enterEvent(QEvent*);
    virtual void leaveEvent(QEvent*);
    virtual void mousePressEvent(QMouseEvent*e);
    virtual void mouseDoubleClickEvent(QMouseEvent *e);

    friend int visibleUnders();

private:
    virtual void createPlus();
    virtual void togglePlus();
public:
    virtual bool toggleCheck();
    virtual bool isChecked() const {return _checked;}
protected:

    virtual void createSpinner(int min, int max);

signals:
    void copied(ModelItem *);
    void cloning(ModelItem *);
    void removing(ModelItem *);
    void checked(int amount, ModelItem *);
    void multiplierChanged(int,ModelItem *);
    void modifierChanged(int);
    void selectionChanged(int at, const QString &slct, ModelItem *ths);

protected slots:
    virtual void on_spinnerChanged(int now);

public slots:
//    virtual void on_multiplierChange(int now, bool force = false);

private:

    TopModelItem *_above;
//    bool _limitClone;
    int _index;

    QLabel *_text;//    QList<SpecialLabel*> _texts;
//    QList<MultiSelectionHandler*> _mshs;

    int _cost;
    int _basecost;
//    QList<int> _baseCostFor1;

    int _multiplier;
    bool _multiplierismodels;
    int _baseModels;
//    int _modifier;
//    int _hasModifier;
//    bool _takesModifier;

    QSpinBox *_spinner;
//    bool _hasLimit;

    QPushButton *_expandButton;
    bool _expanded;
    bool _checked;
    bool _checkable;
//    bool _alwaysChecked;
//    int *_clonable;
    bool _mouseIn;
};


#endif // MODELITEM_H
