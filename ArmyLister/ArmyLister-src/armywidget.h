#ifndef ARMYWIDGET_H
#define ARMYWIDGET_H

#include <QWidget>

class Organisation;
class ArmyListWidget;
class Settings;

class QLabel;

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

    void on_valueChange(int amount, int role);

private:

    Organisation *create40k();
    Organisation *create9A(const QString &text, int limit);

    Settings *_settings;

    int _points;
//    QLabel *_points;

    Organisation *_army;
    ArmyListWidget *_list;
};

#endif // ARMYWIDGET_H
