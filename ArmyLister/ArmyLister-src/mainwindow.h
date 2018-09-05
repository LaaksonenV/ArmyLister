#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class BattleForged;
class ArmyListWidget;
class Settings;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void on_selectList();
    void on_selectArmy(const QString &fileName);
    void on_saveList();
    void on_loadList();
    void on_loadArmy(const QString &fileName);

    void on_createBattleforge();
    void on_createWPList();
    void on_createUPList();
    void on_createWGList();
    void on_createArmyList();

    void on_valueChange(int i, int r);

private:
    Settings *_settings;
    BattleForged *_army;
    ArmyListWidget *_list;

};

#endif // MAINWINDOW_H
