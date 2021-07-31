#include "listcreatordeforg.h"

#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QButtonGroup>
#include <QRadioButton>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QFileDialog>

ListCreatorDefOrg::ListCreatorDefOrg(QWidget *parent)
    : QDialog(parent)
{
    QGridLayout *mainlay = new QGridLayout(this);
    mainlay->setSizeConstraint(QLayout::SetFixedSize);

    orgtype_ = new QButtonGroup(this);
    orgtype_->setExclusive(true);
    QRadioButton *typebut = new QRadioButton("40k", this);
    orgtype_->addButton(typebut);
    //typebut->setChecked(true);

  // FOR NOW
    typebut->setDisabled(true);

    mainlay->addWidget(typebut,1,0);

    QHBoxLayout *lay = new QHBoxLayout(this);

    detachment_ = new QLineEdit("BattleForged.txt", this);
    lay->addWidget(detachment_);

    QPushButton *but = new QPushButton("Browse file", this);
    connect(but, &QPushButton::clicked,
            this, &ListCreatorDefOrg::on_selectFile);
    lay->addWidget(but);

    mainlay->addLayout(lay,1,1,1,2);


    QFrame *line = new QFrame(this);
    line->setLineWidth(2);
    line->setMidLineWidth(1);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Raised);
//    linea1->setPalette(QPalette(QColor(255, 0, 0)));
    mainlay->addWidget(line,2,0,1,3);


    typebut = new QRadioButton("9A", this);
    orgtype_->addButton(typebut);
    typebut->setChecked(true);

    mainlay->addWidget(typebut,3,0);


    9AOrg_ = new QGridLayout(this);

    QLabel *lab = new QLabel("Characters", this);
    9AOrg_->addWidget(lab,0,0,Qt::AlignRight);

    QComboBox *drop = new QComboBox(this);
    types_ << drop;
    drop->addItems(items);
    drop->setCurrentIndex(0);
    9AOrg_->addWidget(drop,0,1);

    QSpinBox *value = new QSpinBox(this);
    values_ << value;
    value->setRange(0,100);
    value->setSuffix("%");
    value->setValue(40);
    value->setSingleStep(5);
    9AOrg_->addWidget(value,0,2);


    lab = new QLabel("Core", this);
    9AOrg_->addWidget(lab,1,0,Qt::AlignRight);

    drop = new QComboBox(this);
    types_ << drop;
    drop->addItems(items);
    drop->setCurrentIndex(1);
    9AOrg_->addWidget(drop,1,1);

    value = new QSpinBox(this);
    values_ << value;
    value->setRange(0,100);
    value->setSuffix("%");
    value->setValue(20);
    value->setSingleStep(5);
    9AOrg_->addWidget(value,1,2);


    lab = new QLabel("Special", this);
    9AOrg_->addWidget(lab,2,0,Qt::AlignRight);

    drop = new QComboBox(this);
    types_ << drop;
    drop->addItems(items);
    drop->setCurrentIndex(2);
    9AOrg_->addWidget(drop,2,1);

    value = new QSpinBox(this);
    values_ << value;
    value->setRange(0,100);
    value->setSuffix("%");
    value->setValue(0);
    value->setSingleStep(5);
    9AOrg_->addWidget(value,2,2);


    QLineEdit *txt = new QLineEdit("Army-Specific", this);
    names_ << txt;
    9AOrg_->addWidget(txt,3,0);

    drop = new QComboBox(this);
    types_ << drop;
    drop->addItems(items);
    drop->setCurrentIndex(0);
    9AOrg_->addWidget(drop,3,1);

    value = new QSpinBox(this);
    values_ << value;
    value->setRange(0,100);
    value->setSuffix("%");
    value->setValue(0);
    value->setSingleStep(5);
    9AOrg_->addWidget(value,3,2);


    add9AOrg_ = new QPushButton("+", this);
    connect(add9AOrg_, &QPushButton::clicked,
            this, &ListCreatorDefOrg::on_add9AAS);
    9AOrg_->addWidget(add9AOrg_,4,0);


    mainlay->addLayout(n9AOrg_,3,1,1,2);

    mainlay->addItem(new QSpacerItem(5,5),4,0);


    but = new QPushButton("Cancel", this);
    connect(but, &QPushButton::clicked,
            this, &QDialog::reject);
    mainlay->addWidget(but,5,1);

    but = new QPushButton("Ok", this);
    connect(but, &QPushButton::clicked,
            this, &QDialog::accept);
    mainlay->addWidget(but,5,2);
}

QString ListCreatorDefOrg::getType() const
{
    return orgtype_->checkedButton()->text();
}

QStringList ListCreatorDefOrg::getOrg() const
{
    if (orgtype_->checkedId() == 0)
        return QStringList(detachment_->text());
    QStringList lst;
    QString ret;
    int i = 0;
    while (i < 3)
    {
        ret = coreNames.at(i);
        ret += ";";
        if (types_.at(i)->currentText()== "min")
        {
            ret += "-";
            ret += QString::number(values_.at(i)->value());
        }
        else if (types_.at(i)->currentText()== "max")
            ret += QString::number(values_.at(i)->value());
        else
            ret += "0";
        lst << ret;
        ++i;
    }
    while (i < types_.count())
    {
        ret = names_.at(i-3)->text();
        ret += ";";
        if (types_.at(i)->currentText()== "min")
        {
            ret += "-";
            ret += QString::number(values_.at(i)->value());
        }
        else if (types_.at(i)->currentText()== "max")
            ret += QString::number(values_.at(i)->value());
        else
            ret += "0";
        lst << ret;
        ++i;
    }
    return lst;
}

void ListCreatorDefOrg::init(const QString &orgtype, const QStringList &org)
{
    if (orgtype == "40k")
    {
        orgtype_->button(0)->setChecked(true);
        detachment_->setText(org.first());
    }
    else
    {
        orgtype_->button(1)->setChecked(true);

        int i = 0;
        int num;
        while (i < 3)
        {
            num = org.at(i).section(';',1).toInt();
            if (num < 0)
            {
                types_.at(i)->setCurrentText("min");
                values_.at(i)->setValue(-num);
            }
            else if (num > 0)
            {
                types_.at(i)->setCurrentText("max");
                values_.at(i)->setValue(num);
            }
            else
            {
                types_.at(i)->setCurrentText("-");
                values_.at(i)->setValue(0);
            }
            ++i;
        }
        while (i < org.count())
        {
            names_[i-3]->setText(org.at(i).section(';',0,1));

            num = org.at(i).section(';',1).toInt();
            if (num < 0)
            {
                types_.at(i)->setCurrentText("min");
                values_.at(i)->setValue(-num);
            }
            else if (num > 0)
            {
                types_.at(i)->setCurrentText("max");
                values_.at(i)->setValue(num);
            }
            else
            {
                types_.at(i)->setCurrentText("-");
                values_.at(i)->setValue(0);
            }
            ++i;
        }
    }
}

void ListCreatorDefOrg::on_selectFile()
{
    QString file = QFileDialog::getOpenFileName(this,
                       tr("Select Detachment file"), QString(), "text (*txt)");
    if (!file.isNull())
        detachment_->setText(file);
}

void ListCreatorDefOrg::on_add9AAS()
{
    int row = n9AOrg_->rowCount()-1;

    QLineEdit *txt = new QLineEdit("Army-Specific", this);
    names_ << txt;
    9AOrg_->addWidget(txt,row,0);

    QComboBox *drop = new QComboBox(this);
    types_ << drop;
    drop->addItems(items);
    drop->setCurrentIndex(0);
    9AOrg_->addWidget(drop,row,1);

    QSpinBox *value = new QSpinBox(this);
    values_ << value;
    value->setRange(0,100);
    value->setSuffix("%");
    value->setValue(0);
    value->setSingleStep(5);
    9AOrg_->addWidget(value,row,2);

    9AOrg_->addWidget(add9AOrg_,row+1,0);
}
