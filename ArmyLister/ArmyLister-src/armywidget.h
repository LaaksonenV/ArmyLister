#ifndef ARMYWIDGET_H
#define ARMYWIDGET_H

#include <QWidget>

class BattleForged;
class ArmyListWidget;
class Settings;

class ArmyWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ArmyWidget(Settings *set, QWidget *parent = nullptr);
    virtual ~ArmyWidget();

    bool createArmy(const QString &filename);

    void saveList(const QString &filename);
    void loadList(const QString &filename);

signals:

public slots:

    void printList();

    void on_valueChange(int , int );


private:

    BattleForged *create40k();
    BattleForged *create9A(const QString &file);

    Settings *_settings;

    BattleForged *_army;
    ArmyListWidget *_list;
};

#endif // ARMYWIDGET_H
