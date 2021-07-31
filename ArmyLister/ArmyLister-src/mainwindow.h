#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class ArmyWidget;
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

    void on_createOrg();
//    void on_createWPList();
  //  void on_createUPList();
    void on_createWGList();
    void on_createArmyList();

private:
    Settings *settings_;
    ArmyWidget *mainwidget_;
};

#endif // MAINWINDOW_H
