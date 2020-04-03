#ifndef SPECIALLABEL_H
#define SPECIALLABEL_H

#include <QLabel>
#include <QList>

class SlotSelection;
class ModelItem;
class Settings;
#include "structs.h"

class BaseLabel : public QLabel
{
    Q_OBJECT

public:
    BaseLabel(QWidget *parent, Settings *set, const QString& text = "");

    virtual void setDefault(Gear&, int, int = -1){}
    virtual void addSelection(QList<Gear>, int, int = 0){}
    virtual void addSpecial(Gear &){}
    virtual bool hasSelections() const{return false;}
    virtual void changeMultiplier(int){}

public slots:
    virtual void setTexts(const QStringList&){}
    virtual void on_selection(const QStringList &, QList<int> ){}

signals:
    void selectedCost(int c);
};

class SlotLabel : public BaseLabel
{
    Q_OBJECT

public:
    SlotLabel(QWidget *parent, Settings *set);
//    SlotLabel(SlotLabel *copy, ModelItem *parent);
    ~SlotLabel();

//    void reset();

    void setDefault(Gear& def, int at, int models = -1);
    void addSelection(QList<Gear> list, int at, int perModels = 0);
    void addSpecial(Gear &gr);
//    void removeSelection(const QString &text);
//    void denySelection(bool denied);
    bool hasSelections() const;
//    void select(const QString &s);
    void changeMultiplier(int change);

    void enterEvent(QEvent*);
    void leaveEvent(QEvent*);
    void mousePressEvent(QMouseEvent*e);
    void paintEvent(QPaintEvent *e);

public slots:
    void setText(const QString& );
    void setTexts(const QStringList& s);
//    void setPoint(int p);
    void on_selection(const QStringList &text, QList<int> cost);



private:
//    ModelItem *_modelParent;
    QStringList _defaultTexts;
    QList<int> _defaultPoints;

    SlotSelection *_selection;
    QList<Gear> _specials;

//    bool _selectionDenied;

    bool _mouseIn;
};



#endif // SPECIALLABEL_H
