#ifndef SLOTSELECTION_H
#define SLOTSELECTION_H

#include <QWidget>
#include <QMap>

class SlotButton;
class SlotButtonGroup;
class QHBoxLayout;
class QVBoxLayout;
class SelectionLimiter;
#include "structs.h"

class SlotSelection : public QWidget
{
    Q_OBJECT

public:
    SlotSelection(const QFont &f);
//    Selection(Selection *copy);

    void setDefault(Gear& def, int at, int models);
    void addSelection(const QList<Gear> &list, int at, int perModels = 0);
    bool hasSelections() const;

    void modelsChange(int change);

    void mousePressEvent(QMouseEvent *e);
//    void select(const QString &s);

public slots:
    void on_selection();

signals:
    void selected(const QStringList &text, QList<int> cost);
    void modelsChanged(int change);

private:
    SlotButtonGroup *createGroup(int at);
    SlotButton *createBlanc();
    SlotButton *createButton(QString text);

private:
    const unsigned short itemHeight = 24;
    QFont _f;
    QHBoxLayout *_lay;
    QList <QVBoxLayout *> _laycols;
    QList <SlotButtonGroup *> _labels;
    SelectionLimiter *_limiter;

    int _models;
};

struct Limit
{
    int current;
    int max;
    int per;
};

class SelectionLimiter : public QObject
{
    Q_OBJECT

public:
    SelectionLimiter(QObject *parent);

    void addButton(SlotButton *label, int group, int col);
    void addCrossLimit(int group, int limit, int perModel);
    int addLimit(int limit, int perModel);

    void modelsChanged(int models);

public slots:
    void buttonPress(bool check, int group, int col, int index);

signals:
    void disableButtons(bool check, int group, int col, int index);
    void modelChange(int change, int group);

private:
    QMap<int,Limit> _limits;
    int _indexCounter;
};

/*class MultiSelectionHandler : public QObject
{
    Q_OBJECT

public:
    MultiSelectionHandler(ModelItem *parent);
    MultiSelectionHandler(MultiSelectionHandler *copy, ModelItem *parent);

    void addText(const QList<std::pair<QString, int> > &list);
    void addAt(int at);

public slots:
    void selectionChanged(int at, const QString &text, ModelItem *copy);

private:
    QList<int> _ats;
    QStringList _texts;
};*/

#endif // SLOTSELECTION_H
