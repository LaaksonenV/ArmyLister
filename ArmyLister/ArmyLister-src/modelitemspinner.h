#ifndef MODELITEMSPINNER_H
#define MODELITEMSPINNER_H

#include "modelitembasic.h"

class QSpinBox;

class ModelItemSpinner : public ModelItemBasic
{
    Q_OBJECT
public:
    ModelItemSpinner(ModelItemBase *parent);

    ModelItemSpinner(ModelItemSpinner *source, ModelItemBase *parent);

    virtual ~ModelItemSpinner();

    virtual void clone(ModelItemBase * toRoot = nullptr, int i = -1);

    virtual void setCost(int i, int = 0);

    void setMultiCost(int base, int special);

    virtual void setRange(int min, int max = -1);

    virtual void loadSelection(QString &str);

    virtual void saveSelection(QTextStream &str);

    virtual int getCurrentCount() const;

    virtual QString getPrintText() const;

    virtual void passCostUp(int c, bool perModel = false, int role = 0);

    virtual void currentLimitChanged(int current, bool whole);

private:

    void createSpinner(int min, int max);

    void updateSpinner();

public slots:

    virtual void on_spinnerChanged(int now);

private:

    int singleCost_;
    int otherCost_;

    int initialSingleCost_;
    int initialOtherCost_;


    QSpinBox *spinner_;

    QTimer *clickClock_;
};

#endif // MODELITEMSPINNER_H
