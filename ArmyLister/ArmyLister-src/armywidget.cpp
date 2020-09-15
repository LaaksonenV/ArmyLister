#include "armywidget.h"

#include <QHBoxLayout>

#include "battleforged.h"
#include "armylistwidget.h"
#include "settings.h"
#include "listcreatordetach.h"


ArmyWidget::ArmyWidget(Settings *set, QWidget *parent)
    : QWidget(parent)
    , _settings(set)
    , _list(new ArmyListWidget(_settings, this))

{

}

ArmyWidget::~ArmyWidget()
{
}

bool ArmyWidget::createArmy(const QString &filename)
{
    BattleForged *army;
    if (filename.startsWith("9A"))
        army = create9A(filename);
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

BattleForged *ArmyWidget::create40k()
{
    BattleForged *army = new ArmyListWidget(_settings, this);

    army->setLists(ListCreatorDetach::getDetachmentList("BattleForged.txt"));

    connect(_list, &ArmyListWidget::roleSelected,
            army, &BattleForged::roleSelected);

    return army;
}

BattleForged *ArmyWidget::create9A(const QString &file)
{
    BattleForged *army = new ArmyListWidget(_settings, this);

    // Org listan alkuun, Battleforge leikkaus modelitemin tapaan
    army->setLists(ListCreatorDetach::getDetachmentList("9thAgeOrg.txt"));

    connect(_list, &ArmyListWidget::valueChanged,
            this, &ArmyWidget::on_valueChange);

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

void ArmyWidget::on_valueChange(int, int)
{}
