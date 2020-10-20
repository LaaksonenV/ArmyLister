#ifndef ARMYLISTWIDGET_H
#define ARMYLISTWIDGET_H

#include <QScrollArea>


class ModelItemBase;
class QTextStream;

//const int printwidth = 80;

class ArmyListWidget : public QScrollArea
{
    Q_OBJECT
public:
    ArmyListWidget(QWidget *parent);
    ~ArmyListWidget();

    bool addArmyFile(const QString &fileName);

public slots:
    void printList() const;
    void saveList() const;
    void saveListAs(QString filename) const;
    void loadList(const QString &filename);

    void on_valueChange(int i, int r);

private:
    void printRecurseSave(QTextStream &str, ModelItemBase *itm, int d) const;
    QString recurseLoad(QTextStream &str, ModelItemBase *parnt, int d);

signals:
    void roleSelected(int amount, int role);
    void valueChanged(int value, int role);

private:
    ModelItemBase *_topItem;

    QString _name;
    int _points;
};

#endif // ARMYLISTWIDGET_H
