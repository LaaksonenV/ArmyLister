#include "listcreatordetach.h"

#include <QGridLayout>
#include <QListWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QPixmap>
#include <QPushButton>
#include <QSpinBox>
#include <QComboBox>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QLabel>
#include <QLineEdit>
#include <QIntValidator>
#include <QStringList>

#include "detachment.h"

ListCreatorDetach::ListCreatorDetach(QWidget *parent)
    : QDialog(parent)
    , v_currentIndex(-1)
    , qv_mins(QList<QSpinBox*>())
    , qv_maxs(QList<QSpinBox*>())
    , qv_spes(QList<QComboBox*>())
    , qv_structs(QList<DetachmentStruct>())
{
    QGridLayout *lay = new QGridLayout(this);

    qv_detachmentList = new QListWidget(this);
    connect(qv_detachmentList, &QListWidget::currentRowChanged,
            this, &ListCreatorDetach::itemSelected);
    lay->addWidget(qv_detachmentList,1,0,20,1);

    QPushButton *button = new QPushButton("Add Detachment");
    lay->addWidget(button,0,0);
    connect(button, &QPushButton::clicked,
            this, &ListCreatorDetach::on_AddItem);

    button = new QPushButton("Remove Detachment");
    lay->addWidget(button,0,1,1,5);
    connect(button, &QPushButton::clicked,
            this, &ListCreatorDetach::removeItem);

    lay->addWidget(new QLabel("Command Points"),1,1,1,2);
    qv_cp = new QLineEdit("0", this);
    qv_cp->setValidator(new QIntValidator());
    qv_cp->setFixedWidth(30);
    lay->addWidget(qv_cp,1,3,1,2);

    int row;
    QSpinBox *spin;
    QComboBox *box;
    for (int i = 0; i < BattlefieldRole::OverRoles; ++i)
    {
        row = i+2;
//      row = (((i)/3)*2)+2;
  //      int column = (((i)%3)*2)+10;

        QLabel *lab = new QLabel(this);
        lab->setPixmap(Detachment::getIcon(i));
        lay->addWidget(lab,row,1);

        spin = new QSpinBox(this);
        spin->setMinimum(0);
        qv_mins.append(spin);
        lay->addWidget(spin,row,2);

        lay->addWidget(new QLabel("-"),row,3);

        spin = new QSpinBox(this);
        spin->setMinimum(-1);
        qv_maxs << spin;
        lay->addWidget(spin,row,4);

        QStringList list;
        for (int i = 0; i < SpecialSlot::OverSlot; ++i)
            list << "";

        box = new QComboBox(this);
        list[SpecialSlot::Null] = "Standard";
        list[SpecialSlot::LimitByMin] = "Min by other slots";
        list[SpecialSlot::LimitByMax] = "Max by other slots";

        box->addItems(list);

        qv_spes << box;
        connect(box, SIGNAL(currentIndexChanged(int)),
                this, SLOT(specialSelected(int)));
        lay->addWidget(box,row,5);
    }

    button = new QPushButton("OK");
    connect(button, &QPushButton::clicked,
            this, &ListCreatorDetach::on_OK);
    lay->addWidget(button,19,1,1,4);

    button = new QPushButton("Cancel");
    connect(button, &QPushButton::clicked,
            this, &ListCreatorDetach::reject);
    lay->addWidget(button,19,5,1,1);

    readFile();
}

void ListCreatorDetach::writeFile()
{
    QFile f(c_fileName);
    if (!f.open(QFile::Text | QFile::WriteOnly))
        return;
    QTextStream str(&f);
    QList<RoleSlotStruct> list;
    RoleSlotStruct strct;
    QListWidgetItem *item;
    for (int i = 0; i < qv_structs.count(); ++i)
    {
        item = qv_detachmentList->item(i);
        if (!item)
            continue;
        str << '#' << item->text()
            << ',' << QString::number(qv_structs.at(i).cp)
            << '\n';
        list = qv_structs.at(i).roles;
        for (int j = 0; j < list.count(); ++j)
        {
            strct = list.at(j);
            if (strct.min | strct.max | strct.special)
                str << '\t' << QString::number(j)
                    << ','  << QString::number(strct.min)
                    << ','  << QString::number(strct.max)
                    << ','  << QString::number(strct.special)
                    << '\n';
        }
    }
    f.close();
}

void ListCreatorDetach::readFile()
{
    QFile f(c_fileName);
    if (!f.open(QFile::Text | QFile::ReadOnly))
        return;
    QTextStream str(&f);
    QString line;
    QStringList splitline;
    int indx;
    QList<RoleSlotStruct> *list;
    while (!str.atEnd())
    {
        line = str.read(1);
        if (line == "#")
        {
            indx = addItem();
            splitline = str.readLine().split(',');
            qv_detachmentList->item(indx)->setText(splitline.at(0));
            if (splitline.count()>1)
                qv_cp->setText(splitline.at(1));
            else
                qv_cp->setText("0");

            list = &(qv_structs[indx].roles);
        }
        else
        {
            splitline = str.readLine().split(',');
            (*list)[splitline.at(0).toInt()].min = splitline.at(1).toInt();
            (*list)[splitline.at(0).toInt()].max = splitline.at(2).toInt();
            (*list)[splitline.at(0).toInt()].special = splitline.at(3).toInt();
        }
    }

    f.close();
}

QString ListCreatorDetach::getDetachmentList()
{
    QFile f(c_fileName);
    if (!f.open(QFile::Text | QFile::ReadOnly))
        return QString();
    QTextStream str(&f);
    QString line;
    QString ret;
    while (!str.atEnd())
    {
        line = str.read(1);
        if (line == "#")
        {
            ret += '#';
            ret += str.readLine();
        }
        else
        {
            ret += ';';
            ret += str.readLine();
        }
    }
    f.close();
    return ret;
}

int ListCreatorDetach::addItem()
{
    DetachmentStruct strct;
    strct.cp = 0;
    for (int i = 0; i < BattlefieldRole::OverRoles; ++i)
    {
        strct.roles << RoleSlotStruct{0,0,0};
    }
    qv_structs << strct;
    QListWidgetItem *item = new QListWidgetItem("New Detachment");
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    qv_detachmentList->addItem(item);
    return qv_structs.count()-1;
}

void ListCreatorDetach::on_AddItem()
{
    addItem();
}

void ListCreatorDetach::removeItem()
{
    if (v_currentIndex < 0)
        return;
    qv_structs.removeAt(v_currentIndex);
    qv_detachmentList->takeItem(v_currentIndex);
}

void ListCreatorDetach::specialSelected(int index)
{
    Q_UNUSED(index)
}

void ListCreatorDetach::itemSelected(int index)
{
    saveCurrent();
    v_currentIndex = index;
    qv_cp->setText(QString::number(qv_structs.at(index).cp));
    const QList<RoleSlotStruct> &list = qv_structs.at(index).roles;
    RoleSlotStruct strct;
    for (int i = 0; i < list.count(); ++i)
    {
        strct = list.at(i);
        qv_mins.at(i)->setValue(strct.min);
        qv_maxs.at(i)->setValue(strct.max);
        qv_spes.at(i)->setCurrentIndex(strct.special);
    }
}

void ListCreatorDetach::on_OK()
{
    saveCurrent();
    writeFile();
    accept();
}

void ListCreatorDetach::saveCurrent()
{
    if (v_currentIndex < 0)
        return;
    qv_structs[v_currentIndex].cp = qv_cp->text().toInt();
    QList<RoleSlotStruct> &list = qv_structs[v_currentIndex].roles;
    for (int i = 0; i < list.count(); ++i)
    {
        list[i].min =  qv_mins.at(i)->value();
        list[i].max =  qv_maxs.at(i)->value();
        list[i].special =  qv_spes.at(i)->currentIndex();
    }
}
