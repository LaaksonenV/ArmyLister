#ifndef MODELITEMUNIT_H
#define MODELITEMUNIT_H

#include "modelitembasic.h"

class QSpinBox;

class ModelItemUnit : public ModelItemBasic
{
    Q_OBJECT
public:
    ModelItemUnit(Settings *set, ModelItemBase *parent);

    ModelItemUnit(ModelItemUnit *source, ModelItemBase *parent);

    virtual ~ModelItemUnit();

    virtual void clone(ModelItemBase * = nullptr, int = -1);

//    virtual void setText(const QString &text);

    void setSpecial(const QStringList &list);

    void setUnitCost(int base, int special);

    virtual void passSpecialUp(const QStringList &list, bool check);

    virtual bool branchChecked(bool check, int i, int role);

    void setModels(int min, int max = 0);

    virtual int getModelCount() const;

    virtual void passCostUp(int c, bool perModel = false, int role = -1);

    virtual void printToStream(QTextStream &str);

protected:

    virtual void mousePressEvent(QMouseEvent*e);

    virtual void mouseDoubleClickEvent(QMouseEvent *e);

    virtual void createSpinner(int min, int max);

    virtual void connectToSatellite(ItemSatellite *sat);

public slots:

    virtual void on_spinnerChanged(int now);

signals:

    void itemCloned(bool = true);

    void modelsChanged(int);

private:

    int _modelCost;
    int _otherCost;

    int _initModelCost;
    int _initOtherCost;

    int _models;

    QSpinBox *_spinner;

    QTimer *_clickClock;
};

#endif // MODELITEMUNIT_H
