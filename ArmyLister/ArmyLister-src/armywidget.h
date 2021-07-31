#ifndef ARMYWIDGET_H
#define ARMYWIDGET_H

#include <QWidget>

class Organisation;
class ArmyListWidget;

class QLabel;

class ArmyWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ArmyWidget(QWidget *parent = nullptr);
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

    int points_;
//    QLabel *_points;

    Organisation *army_;
    ArmyListWidget *list_;
};

#endif // ARMYWIDGET_H
