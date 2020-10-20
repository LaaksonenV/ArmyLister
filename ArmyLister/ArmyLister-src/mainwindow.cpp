#include "mainwindow.h"

#include <QPushButton>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QInputDialog>
#include <QFileInfo>
#include <QScrollArea>

#include "listcreator.h"
#include "listcreatordetach.h"
#include "armywidget.h"
#include "settings.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , _settings(new Settings())
    , _mainwidget(nullptr)

{
    setWindowTitle(QString("ArmyLister ") + QString(APP_VERSION));
    _mainwidget = new ArmyWidget(this);

    setCentralWidget(_mainwidget);

    QMenuBar *menu = menuBar();
    QMenu *actMenu = menu->addMenu("Select");

    QAction *act = actMenu->addAction("New Army");
    connect(act, &QAction::triggered,
            this, &MainWindow::on_selectList);

    actMenu->addSeparator();

    QStringList armies = _settings->multiValue(SETTING_ARMY_GROUP);
    QFileInfo forName;
    for (int i = 0; i < std::min(5,armies.count()); ++i)
    {
        forName.setFile(armies.at(i));
        act = actMenu->addAction(forName.completeBaseName());
        connect(act, &QAction::triggered,
                [=](){on_selectArmy(armies.at(i));});
    }

//    actMenu = menu->addMenu("Save/Load List");

//    act = actMenu->addAction("Save");
//    connect(act, &QAction::triggered,
  //          [=](){_mainwidget->saveList(QString());});

//    act = actMenu->addAction("Save As");
//    connect(act, &QAction::triggered,
  //          this, &MainWindow::on_saveList);

//    actMenu->addSeparator();
//    actMenu->addSeparator();

//    act = actMenu->addAction("Load");
//    connect(act, &QAction::triggered,
  //          this, &MainWindow::on_loadList);

//    actMenu->addSeparator();

//    armies = _settings->multiValue(SETTING_LIST_GROUP);
//    for (int i = 0; i < std::min(5,armies.count()); ++i)
//    {
//        forName.setFile(armies.at(i));
//        act = actMenu->addAction(forName.completeBaseName());
//        connect(act, &QAction::triggered,
//                [=](){on_loadArmy(armies.at(i));});
//    }


    actMenu = menu->addMenu("Create");

    act = actMenu->addAction("Detachments");
    connect(act, &QAction::triggered,
            this, &MainWindow::on_createOrg);

    actMenu->addSeparator();

/*    act = actMenu->addAction("Wargear Points List");
    connect(act, &QAction::triggered,
            this, &MainWindow::on_createWPList);

    act = actMenu->addAction("Unit Points List");
    connect(act, &QAction::triggered,
            this, &MainWindow::on_createUPList);*/

    act = actMenu->addAction("Lists and Tables");
    connect(act, &QAction::triggered,
            this, &MainWindow::on_createWGList);

    act = actMenu->addAction("Army List");
    connect(act, &QAction::triggered,
            this, &MainWindow::on_createArmyList);

    actMenu = menu->addMenu("Print");

    act = actMenu->addAction("Txt file");
    connect(act, &QAction::triggered,
            _mainwidget, &ArmyWidget::printList);

    resize(500,500);
}

MainWindow::~MainWindow()
{
    delete _settings;
}

void MainWindow::on_selectList()
{
    QString file = QFileDialog::getOpenFileName
            (this, tr("Select Army to create"), QString(), "text (*txt)");

    if (!file.isEmpty() && _mainwidget->createArmy(file))
            _settings->addMultiValue(file, SETTING_ARMY_GROUP);
}

void MainWindow::on_selectArmy(const QString &fileName)
{
    _mainwidget->createArmy(fileName);
}

void MainWindow::on_saveList()
{
    QString file = QFileDialog::getSaveFileName
            (this, "Choose where to save", QString(), "Armylist (*alst)");
    if (!file.isEmpty())
    {
        _mainwidget->saveList(file);
        _settings->addMultiValue(file, SETTING_LIST_GROUP);
    }
}

void MainWindow::on_loadList()
{
    QString file = QFileDialog::getOpenFileName
            (this, "Select List to load", QString(), "Armylist (*alst)");
    if (!file.isEmpty())
    {
        _mainwidget->loadList(file);
        _settings->addMultiValue(file, SETTING_LIST_GROUP);
    }
}

void MainWindow::on_loadArmy(const QString &fileName)
{
    _mainwidget->loadList(fileName);
}

void MainWindow::on_createOrg()
{
    QString file = QFileDialog::getSaveFileName
            (this, "Choose Organisations filename", QString(),
             "(*.txt)", nullptr, QFileDialog::DontConfirmOverwrite);
    if (!file.isEmpty())
    {
    ListCreatorDetach dial(this,file);
    dial.open();
    }
}

/*void MainWindow::on_createWPList()
{
    QString file = QFileDialog::getSaveFileName
            (this, "Choose Wargear Points lists' name", QString(),
             "(*.txt)", nullptr, QFileDialog::DontConfirmOverwrite);
    if (!file.isEmpty())
    {
        ListCreator crt(file, QStringList("Name") << "Cost#", this);
        crt.initialiseWargear();
        crt.exec();
    }
}

void MainWindow::on_createUPList()
{
    QString file = QFileDialog::getSaveFileName
            (this, "Choose Units's Points lists' name", QString(),
             "(*.txt)", nullptr, QFileDialog::DontConfirmOverwrite);
    if (!file.isEmpty())
    {
    ListCreator crt(file,
                    QStringList("Name") << "Models/Unit#-#"
                    << "Cost/Model#",
                    this);
    crt.exec();
    }
}*/

void MainWindow::on_createWGList()
{
    QString file = QFileDialog::getSaveFileName
            (this, "Choose Wargear Lists lists' name", QString(),
             "(*.txt)", nullptr, QFileDialog::DontConfirmOverwrite);
    if (!file.isEmpty())
        ListCreator::CreateList(file, this);
}

void MainWindow::on_createArmyList()
{
    QString file = QFileDialog::getSaveFileName
            (this, "Choose Army Lists' name", QString(),
             "(*.txt)", nullptr, QFileDialog::DontConfirmOverwrite);
    if (!file.isEmpty())
        ListCreator::CreateArmy(file, this);

}
