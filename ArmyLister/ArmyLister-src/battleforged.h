#ifndef BATTLEFORGED_H
#define BATTLEFORGED_H

//#include "organisation.h"

class Organisation;
class QStringList;
class Detachment;
class QWidget;

class BattleForged : public Organisation
{
    Q_OBJECT
public:
    explicit BattleForged(QWidget *parent = nullptr);
    virtual ~BattleForged();

//    virtual void setLists(const QString &detachments);

signals:

public slots:
    virtual void roleSelected(int role, int amount);

    virtual void detachmentSelected(Detachment *det, bool check);
    virtual void detachmentCloned(Detachment *det, QStringList args);
    
protected:

    virtual void addPart(const QStringList &args);

private:

    QList<int> qv_currentMin;
    QList<int> qv_currentMax;
};

#endif // BATTLEFORGED_H
