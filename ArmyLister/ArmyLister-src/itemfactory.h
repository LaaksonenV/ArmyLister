#ifndef ITEMFACTORY_H
#define ITEMFACTORY_H

#include <QMap>

#define CONTROL_C ":/#~^+*?"

class TopModelItem;
class Settings;
class QTextStream;
struct TempTreeModelItem;
class ModelItem;
class LimitMIhandler;
#include "pointcontainer.h"

class ItemFactory
{

public:
    ItemFactory(TopModelItem *top, Settings *set);
    ~ItemFactory();

    bool addArmyFile(const QString &fileName);

private:
    QString parseTextRec(const QString &line, QTextStream &str,
                          TempTreeModelItem *current,
                          QStringList prev = QStringList());

    QStringList parseControl(QString &text);
    QStringList parseSpecial(QString &text);

    LimitMIhandler *parseTree(TempTreeModelItem *branch,
                         ModelItem *parent, int amount,
                         LimitMIhandler *sharedLimit = nullptr,
                         ModelItem *itm = nullptr);
//    LimitMIhandler *parseSelection(TempTreeModelItem *branch,
  //                      ModelItem *parent,
    //                    int amount,
      //                  LimitMIhandler *sharedLimit = nullptr);
//    LimitMIhandler *createLimiter(ModelItem *par,
  //                                int models = 0);

/*signals:
    void roleSelected(int role, int amount);
    void valueChanged(int value, int role);*/

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

#endif // ITEMFACTORY_H
