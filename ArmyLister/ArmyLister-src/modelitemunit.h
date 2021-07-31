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

    /*!
     * \brief setUnitCountsAs simple setter
     * \param role counts also as
     *
     * If \a role is set != 0, when the item is checked its unit will
     * push its cost towards the role
     */
    void setUnitCountsAs(int role);

    virtual void loadSelection(QString &str);

    virtual void saveSelection(QTextStream &str);

//    virtual QString getPrintText() const;

    virtual void passSpecialUp(const QStringList &list, bool check);

    virtual bool branchSelected(bool check, int role, int, int = 0);

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

    int _cloned;

    int _unitCountsAs;
};

#endif // MODELITEMUNIT_H
