#ifndef BATTLEFORGED_H
#define BATTLEFORGED_H

#include "organisation.h"

class QStringList;
class Detachment;

#include <QList>
#include <QWidget>

class BattleForged : public Organisation
{
    Q_OBJECT
public:
    explicit BattleForged(QWidget *parent = nullptr);
    virtual ~BattleForged();

//    virtual void setLists(const QString &detachments);

signals:
    void selectionChanging(Detachment *,const QList<int>&,
                           const QList<int>&);

public slots:
//    virtual void roleSelected(int role, int amount);

    virtual void detachmentSelected(Detachment *det, bool check);
//    virtual void detachmentCloned(QStringList args);
    
protected:

    virtual void addPart(const QStringList &args, int);

private:

//    QList<Detachment*> qv_partsList;

    QList<int> qv_currentMin_;
    QList<int> qv_currentMax_;
};

#endif // BATTLEFORGED_H
