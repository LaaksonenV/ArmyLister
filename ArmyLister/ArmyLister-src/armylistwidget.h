#ifndef ARMYLISTWIDGET_H
#define ARMYLISTWIDGET_H

#include <QScrollArea>
#include <QMap>

#define CONTROL_C ":/#~^+*?"

class TopModelItem;
class Settings;
class QTextStream;
class TempTreeModelItem;
class ModelItem;
class LimitMIhandler;
#include "pointcontainer.h"

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

public: void addArmyFile(const QString &fileName);

private:
    QString recurserParse(const QString &line, QTextStream &str,
                          TempTreeModelItem *current,
                          QStringList prev = QStringList());

    QStringList parseControl(QString &text);
    QStringList parseSpecial(QString &text);

    LimitMIhandler *parseTree(TempTreeModelItem *branch,
                         ModelItem *parent, int amount,
                         LimitMIhandler *sharedLimit = nullptr,
                         ModelItem *itm = nullptr);
    LimitMIhandler *parseSelection(TempTreeModelItem *branch,
                        ModelItem *parent,
                        int amount,
                        LimitMIhandler *sharedLimit = nullptr);
    LimitMIhandler *createLimiter(ModelItem *par,
                                  int models = 0);

signals:
    void roleSelected(int role, int amount);
    void valueChanged(int value, int role);

private:
    QString parseIncludes(QTextStream &str);

    void parseFile(const QString &fileName);
    QString parseList(QTextStream &str, QString line);

    PointContainer* findPair(const QString &text,
                            const QStringList &special);


private:
    Settings *_settings;
    TopModelItem *_topItem;

    QList<PointContainer*> _pointList;
    QMap<QString, QStringList> _listList;

    QString _name;
    int _points;
};

struct TempTreeModelItem
{
    TempTreeModelItem(const QString &text = QString(),
                      const QStringList &ctrl = QStringList(),
                      const QStringList &spec = QStringList(),
                      TempTreeModelItem *parent = nullptr);

    void addChild(TempTreeModelItem *item);

    QString _text;
    QStringList _control;
    QStringList _spec;
    QList<TempTreeModelItem*> _unders;

};

#endif // ARMYLISTWIDGET_H
