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

    _orgtype = new QButtonGroup(this);
    _orgtype->setExclusive(true);
    QRadioButton *typebut = new QRadioButton("40k", this);
    _orgtype->addButton(typebut);
    //typebut->setChecked(true);

  // FOR NOW
    typebut->setDisabled(true);

    mainlay->addWidget(typebut,1,0);

    QHBoxLayout *lay = new QHBoxLayout(this);

    _detachment = new QLineEdit("BattleForged.txt", this);
    lay->addWidget(_detachment);

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
    _orgtype->addButton(typebut);
    typebut->setChecked(true);

    mainlay->addWidget(typebut,3,0);


    _9AOrg = new QGridLayout(this);

    QLabel *lab = new QLabel("Characters", this);
    _9AOrg->addWidget(lab,0,0,Qt::AlignRight);

    QComboBox *drop = new QComboBox(this);
    _types << drop;
    drop->addItems(_items);
    drop->setCurrentIndex(0);
    _9AOrg->addWidget(drop,0,1);

    QSpinBox *value = new QSpinBox(this);
    _values << value;
    value->setRange(0,100);
    value->setSuffix("%");
    value->setValue(40);
    value->setSingleStep(5);
    _9AOrg->addWidget(value,0,2);


    lab = new QLabel("Core", this);
    _9AOrg->addWidget(lab,1,0,Qt::AlignRight);

    drop = new QComboBox(this);
    _types << drop;
    drop->addItems(_items);
    drop->setCurrentIndex(1);
    _9AOrg->addWidget(drop,1,1);

    value = new QSpinBox(this);
    _values << value;
    value->setRange(0,100);
    value->setSuffix("%");
    value->setValue(20);
    value->setSingleStep(5);
    _9AOrg->addWidget(value,1,2);


    lab = new QLabel("Special", this);
    _9AOrg->addWidget(lab,2,0,Qt::AlignRight);

    drop = new QComboBox(this);
    _types << drop;
    drop->addItems(_items);
    drop->setCurrentIndex(2);
    _9AOrg->addWidget(drop,2,1);

    value = new QSpinBox(this);
    _values << value;
    value->setRange(0,100);
    value->setSuffix("%");
    value->setValue(0);
    value->setSingleStep(5);
    _9AOrg->addWidget(value,2,2);


    QLineEdit *txt = new QLineEdit("Army-Specific", this);
    _names << txt;
    _9AOrg->addWidget(txt,3,0);

    drop = new QComboBox(this);
    _types << drop;
    drop->addItems(_items);
    drop->setCurrentIndex(0);
    _9AOrg->addWidget(drop,3,1);

    value = new QSpinBox(this);
    _values << value;
    value->setRange(0,100);
    value->setSuffix("%");
    value->setValue(0);
    value->setSingleStep(5);
    _9AOrg->addWidget(value,3,2);


    _add9AOrg = new QPushButton("+", this);
    connect(_add9AOrg, &QPushButton::clicked,
            this, &ListCreatorDefOrg::on_add9AAS);
    _9AOrg->addWidget(_add9AOrg,4,0);


    mainlay->addLayout(_9AOrg,3,1,1,2);

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
    return _orgtype->checkedButton()->text();
}

QStringList ListCreatorDefOrg::getOrg() const
{
    if (_orgtype->checkedId() == 0)
        return QStringList(_detachment->text());
    QStringList lst;
    QString ret;
    int i = 0;
    while (i < 3)
    {
        ret = _coreNames.at(i);
        ret += ";";
        if (_types.at(i)->currentText()== "min")
        {
            ret += "-";
            ret += QString::number(_values.at(i)->value());
        }
        else if (_types.at(i)->currentText()== "max")
            ret += QString::number(_values.at(i)->value());
        else
            ret += "0";
        lst << ret;
        ++i;
    }
    while (i < _types.count())
    {
        ret = _names.at(i-3)->text();
        ret += ";";
        if (_types.at(i)->currentText()== "min")
        {
            ret += "-";
            ret += QString::number(_values.at(i)->value());
        }
        else if (_types.at(i)->currentText()== "max")
            ret += QString::number(_values.at(i)->value());
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
        _orgtype->button(0)->setChecked(true);
        _detachment->setText(org.first());
    }
    else
    {
        _orgtype->button(1)->setChecked(true);

        int i = 0;
        int num;
        while (i < 3)
        {
            num = org.at(i).section(';',1).toInt();
            if (num < 0)
            {
                _types.at(i)->setCurrentText("min");
                _values.at(i)->setValue(-num);
            }
            else if (num > 0)
            {
                _types.at(i)->setCurrentText("max");
                _values.at(i)->setValue(num);
            }
            else
            {
                _types.at(i)->setCurrentText("-");
                _values.at(i)->setValue(0);
            }
            ++i;
        }
        while (i < org.count())
        {
            _names[i-3]->setText(org.at(i).section(';',0,1));

            num = org.at(i).section(';',1).toInt();
            if (num < 0)
            {
                _types.at(i)->setCurrentText("min");
                _values.at(i)->setValue(-num);
            }
            else if (num > 0)
            {
                _types.at(i)->setCurrentText("max");
                _values.at(i)->setValue(num);
            }
            else
            {
                _types.at(i)->setCurrentText("-");
                _values.at(i)->setValue(0);
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
        _detachment->setText(file);
}

void ListCreatorDefOrg::on_add9AAS()
{
    int row = _9AOrg->rowCount()-1;

    QLineEdit *txt = new QLineEdit("Army-Specific", this);
    _names << txt;
    _9AOrg->addWidget(txt,row,0);

    QComboBox *drop = new QComboBox(this);
    _types << drop;
    drop->addItems(_items);
    drop->setCurrentIndex(0);
    _9AOrg->addWidget(drop,row,1);

    QSpinBox *value = new QSpinBox(this);
    _values << value;
    value->setRange(0,100);
    value->setSuffix("%");
    value->setValue(0);
    value->setSingleStep(5);
    _9AOrg->addWidget(value,row,2);

    _9AOrg->addWidget(_add9AOrg,row+1,0);
}
