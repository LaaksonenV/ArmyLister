#ifndef SLOTSELECTION_H
#define SLOTSELECTION_H

#include <QWidget>
#include <QMap>

class QPushButton;
class QButtonGroup;
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

    void addSelection(const QList<Gear> &list, int at);
//    void removeSelection(const QString &text);
    bool hasSelections() const;

    void mousePressEvent(QMouseEvent *e);
//    void select(const QString &s);

public slots:
    void on_selection(int);

signals:
    void selected(const QStringList &text, QList<int> cost);

private:
    void addToSelection(const QStringList &list, int at);

private:
    const unsigned short itemHeight = 24;
    QFont _f;
    QHBoxLayout *_lay;
    QList <QVBoxLayout *> _laycols;
    QList <QButtonGroup *> _labels;
    SelectionLimiter *_limiter;
};

struct Limit
{
    int current;
    int max;
};

class SelectionLimiter : public QObject
{
    Q_OBJECT

public:
    SelectionLimiter(QObject *parent);

    void addButton(QPushButton *label, int group, int col);
    void addLimit(int group, int limit);

public slots:
    void buttonPress(bool check, int group, int col);
signals:
    void disableButtons(bool check, int group, int col);

private:
    QMap<int,Limit> _limits;
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
