#include "modelitemspinner.h"

#include <QSpinBox>
#include <QMouseEvent>
#include <QTimer>
#include <QApplication>

#include "settings.h"
#include "itemsatellite.h"

ModelItemSpinner::ModelItemSpinner(ModelItemBase *parent)
    : ModelItemBasic(parent)
    , singleCost_(0)
    , initialSingleCost_(0)
    , otherCost_(0)
    , initialOtherCost_(0)
    , spinner_(nullptr)
{
}

ModelItemSpinner::ModelItemSpinner(ModelItemSpinner *source, ModelItemBase *parent)
    : ModelItemBasic(source, parent)
    , singleCost_(0)
    , initialSingleCost_(0)
    , otherCost_(0)
    , initialOtherCost_(0)
    , spinner_(nullptr)
{
    setMultiCost(source->initialSingleCost_, source->initialOtherCost_);
    if (source->spinner_)
        setRange(source->spinner_->minimum(), source->spinner_->maximum());
    else if (source->current_)
        setRange(source->current_);
}

ModelItemSpinner::~ModelItemSpinner()
{}

void ModelItemSpinner::clone(ModelItemBase*toRoot, int i)
{
    ModelItemSpinner *clone = new ModelItemSpinner(this, trunk_);
    toRoot->insertItem(clone,i);
    cloning(clone);
}

void ModelItemSpinner::setCost(int i, int)
{
    setMultiCost(i,0);
}

void ModelItemSpinner::setMultiCost(int base, int special)
{
    singleCost_ = base;
    initialSingleCost_ = base;
    otherCost_ = special;
    initialOtherCost_ = special;
    ModelItemBase::passCostUp((base*current_)+special);
    updateSpinner();
}

void ModelItemSpinner::setRange(int min, int max)
{
    current_ = min;
    ModelItemBase::setCost((singleCost_*current_)+otherCost_);
    if (max >= 0)
        createSpinner(min,max);
}

void ModelItemSpinner::loadSelection(QString &str)
{
    int pos = str.indexOf(";");
    if (spinner_)
        spinner_->setValue(str.left(pos).toInt());
    str.remove(0,pos+1);

    ModelItemBasic::loadSelection(str);
}

void ModelItemSpinner::saveSelection(QTextStream &str)
{
    if (spinner_)
        str << QString::number(spinner_->value());
    str << ";";
    ModelItemBasic::saveSelection(str);
}

int ModelItemSpinner::getCurrentCount() const
{
    return current_;
}

QString ModelItemSpinner::getPrintText() const
{
    if (spinner_ && !spinner_->value())
        return QString();
    QString ret = ModelItemBasic::getPrintText();
    if (spinner_ && current_ > spinner_->minimum())
        ret.prepend(QString::number(spinner_->value()) + " ");
    return ret;
}

void ModelItemSpinner::passCostUp(int c, bool perModel, int role)
{
    int change = c;
    if (perModel)
    {
        change = c*current_;
        if (!role)
        {
            singleCost_ += c;
            updateSpinner();
        }
    }
    else if (!role)
        otherCost_ += change;
    ModelItemBasic::passCostUp(change, false, role);
}

void ModelItemSpinner::createSpinner(int min, int max)
{
    spinner_ = new QSpinBox(this);
    connect(spinner_, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &ModelItemSpinner::on_spinnerChanged);
    spinner_->setFont(Settings::Font(Settings::eFont_SpinnerFont));
    spinner_->setRange(min, max);
    updateSpinner();

    spinner_->move(endOfText() + 20, 1);
    spinner_->setMinimumHeight(Settings::ItemMeta(Settings::eItem_Height)-2);

    spinner_->show();

}

void ModelItemSpinner::updateSpinner()
{
    if (spinner_)
    {
        QString suffix("/");
        suffix += QString::number(spinner_->maximum());
        suffix += " @ ";
        suffix += QString::number(singleCost_);

        spinner_->setSuffix(suffix);
    }
}

void ModelItemSpinner::on_spinnerChanged(int now)
{
    int change = now-current_;
    current_ = now;
    ModelItemBasic::branchSelected(change,0,0);
    ModelItemBasic::passCostUp(change*singleCost_);
    ModelItemBasic::passModelsDown(change);

}

void ModelItemSpinner::currentLimitChanged(int current, bool whole)
{
    if (spinner_)
    {
        int change = spinner_->maximum()-current;
        if (!whole)
            change += spinner_->value();
        spinner_->setMaximum(change);
        updateSpinner();
    }
    else
        ModelItemBasic::currentLimitChanged(current, true);
}
