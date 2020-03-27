#ifndef SLOTSELECTION_H
#define SLOTSELECTION_H

#include <QWidget>

class QPushButton;
class QButtonGroup;
class QHBoxLayout;
class QVBoxLayout;
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
    void selected(const QStringList &text, int cost);

private:
    void addToSelection(const QStringList &list, int at);

private:
    const unsigned short itemHeight = 24;
    QFont _f;
    QHBoxLayout *_lay;
    QList <QVBoxLayout *> _laycols;
    QList <QButtonGroup *> _labels;
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
