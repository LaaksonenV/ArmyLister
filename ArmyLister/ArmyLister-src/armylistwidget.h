#ifndef ARMYLISTWIDGET_H
#define ARMYLISTWIDGET_H

#include <QScrollArea>


class TopModelItem;
class Settings;
class QTextStream;

const int printwidth = 80;

class ArmyListWidget : public QScrollArea
{
    Q_OBJECT
public:
    ArmyListWidget(Settings *set, QWidget *parent);
    ~ArmyListWidget();

public slots:
    void printList() const;
    void saveList() const;
    void saveListAs(QString filename) const;
    void loadList(const QString &filename);

    void on_valueChange(int i, int r);

private:
    void printRecurseText(QTextStream &str, ModelItem *itm, int d,
                           QFont &f) const;
    void printRecurseSave(QTextStream &str, ModelItem *itm, int d) const;
    QString recurseLoad(QTextStream &str, ModelItem *parnt, int d);

signals:
    void roleSelected(int role, int amount);
    void valueChanged(int value, int role);

private:
    Settings *_settings;
    TopModelItem *_topItem;

    QString _name;
    int _points;
};

#endif // ARMYLISTWIDGET_H
