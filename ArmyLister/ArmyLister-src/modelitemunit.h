#ifndef MODELITEMUNIT_H
#define MODELITEMUNIT_H

#include "modelitemspinner.h"

class QParallelAnimationGroup;

class ModelItemUnit : public ModelItemSpinner
{
    Q_OBJECT
public:
    ModelItemUnit(ModelItemBase *parent);

    ModelItemUnit(ModelItemUnit *source, ModelItemBase *parent);

    virtual ~ModelItemUnit();

    void setUpCloneAnimation();

    virtual void clone(ModelItemBase * = nullptr, int = -1);

    virtual void setTags(const QStringList &list);

    virtual void loadSelection(QString &str);

    virtual void saveSelection(QTextStream &str);

//    virtual QString getPrintText() const;

    virtual void passTagsUp(const QStringList &list, bool check);

    virtual bool branchSelected(bool check, int role, int, int = -1);

    virtual void passCostUp(int c, bool perModel = false, int role = 0);

    virtual void printToStream(QTextStream &str);

protected:

    virtual bool toggleSelected(int change, int slot);

    virtual void mousePressEvent(QMouseEvent*e);

    virtual void mouseReleaseEvent(QMouseEvent *e);

    void cloneEvent();

    virtual void connectToSatellite(ItemSatellite *sat);

signals:

    void itemCloned(bool = true, int = 0);

private:
    QParallelAnimationGroup *cloneAnimation_;

    int cloned_;

};

#endif // MODELITEMUNIT_H
