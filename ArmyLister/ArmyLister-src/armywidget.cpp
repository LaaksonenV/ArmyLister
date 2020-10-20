#include "armywidget.h"

#include <QHBoxLayout>
#include <QInputDialog>
#include <QFile>
#include <QTextStream>

#include "battleforged.h"
#include "organisation.h"

#include "armylistwidget.h"
#include "settings.h"
#include "listcreatordetach.h"


ArmyWidget::ArmyWidget(QWidget *parent)
    : QWidget(parent)
    , _points(0)
    , _army(nullptr)
    , _list(new ArmyListWidget(this))

{
    connect(_list, &ArmyListWidget::valueChanged,
            this, &ArmyWidget::on_valueChange);
}

ArmyWidget::~ArmyWidget()
{
}

bool ArmyWidget::createArmy(const QString &filename)
{
    QString org = ListCreatorDetach::getOrganisationList(filename);

    QString orgtype = org.split('#').first();

    // 1. Get army type from file 2. Get default size from settings
    // 3. Set default as initial
    bool ok = false;
    int limit;

    Organisation *army;
    if (orgtype == "9A")
    {
        limit = QInputDialog::getInt(this, tr("Army size"), tr("Points"),
                                 Settings::DefaultSize(Settings::DefaultArmySize9A),
                                     0, 10000, 100, &ok);
        army = create9A(org,limit);
    }
    else
        army = create40k();

    QHBoxLayout *lay = new QHBoxLayout();
    QScrollArea *armyarea = new QScrollArea();
    armyarea->setWidget(army);

    armyarea->setWidgetResizable(true);
    armyarea->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Preferred);

    armyarea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    lay->addWidget(armyarea);
    armyarea->show();

    _list->addArmyFile(filename);

    lay->addWidget(_list);

    setLayout(lay);

    resize(950,500);
    return true;
}

Organisation *ArmyWidget::create40k()
{
    BattleForged *army = new BattleForged(this);

    army->setLists(ListCreatorDetach::getOrganisationList("BattleForged.txt"),0);

    connect(_list, &ArmyListWidget::roleSelected,
            army, &BattleForged::onRoleSelection);

    return army;
}

Organisation *ArmyWidget::create9A(const QString &text, int limit)
{
    Organisation *army = new Organisation(this);

    army->setLists(text, limit);

    connect(_list, &ArmyListWidget::valueChanged,
            army, &Organisation::onRoleSelection);

    return army;
}

void ArmyWidget::saveList(const QString &filename)
{
    _list->saveListAs(filename);
}

void ArmyWidget::loadList(const QString &filename)
{
    _list->loadList(filename);
}

void ArmyWidget::printList()
{
    _list->printList();
}

void ArmyWidget::on_valueChange(int amount, int role)
{
    if (role < 0)
    {
        _points += amount;
//        setWindowTitle(QString::number(_points));
    }
}
