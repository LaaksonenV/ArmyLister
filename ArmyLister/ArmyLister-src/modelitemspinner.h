#ifndef MODELITEMSPINNER_H
#define MODELITEMSPINNER_H

#include "modelitembasic.h"

class QSpinBox;

class ModelItemSpinner : public ModelItemBasic
{
    Q_OBJECT
public:
    ModelItemSpinner(Settings *set, ModelItemBase *parent);

    ModelItemSpinner(ModelItemSpinner *source, ModelItemBase *parent);

    virtual ~ModelItemUnit();

    virtual void clone(ModelItemBase * toRoot = nullptr, int i = -1);

    void setMultiCost(int base, int special);

    void setRange(int min, int max = 0);

    virtual int getCurrentCount() const;

    virtual QString getPrintText() const;

    virtual void passCostUp(int c, bool perModel = false, int role = -1);

private:

    virtual void createSpinner(int min, int max);

public slots:

    virtual void on_spinnerChanged(int now);

signals:

    void itemCloned(bool = true);

    void modelsChanged(int);

protected:

    int _current;

private:

    int _singleCost;
    int _otherCost;

    int _initSingleCost;
    int _initOtherCost;


    QSpinBox *_spinner;

    QTimer *_clickClock;
};

#endif // MODELITEMSPINNER_H
