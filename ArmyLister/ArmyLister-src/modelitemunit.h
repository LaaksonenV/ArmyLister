#ifndef MODELITEMUNIT_H
#define MODELITEMUNIT_H

#include "modelitemspinner.h"

class ModelItemUnit : public ModelItemSpinner
{
    Q_OBJECT
public:
    ModelItemUnit(ModelItemBase *parent);

    ModelItemUnit(ModelItemUnit *source, ModelItemBase *parent);

    virtual ~ModelItemUnit();

    virtual void clone(ModelItemBase * = nullptr, int = -1);

    virtual void setSpecial(const QStringList &list);

//    virtual QString getPrintText() const;

    virtual void passSpecialUp(const QStringList &list, bool check);

    virtual bool branchSelected(bool check, int i, int role);

    virtual void passCostUp(int c, bool perModel = false, int role = 0);

    virtual void printToStream(QTextStream &str);

protected:

    virtual void mousePressEvent(QMouseEvent*e);

    virtual void mouseDoubleClickEvent(QMouseEvent *e);

    virtual void connectToSatellite(ItemSatellite *sat);

signals:

    void itemCloned(bool = true, int = 0);

private:
    QTimer *_clickClock;
};

#endif // MODELITEMUNIT_H
