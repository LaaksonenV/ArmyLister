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

ListCreatorDetach::ListCreatorDetach(QWidget *parent, const QString &filename)
    : QDialog(parent)
    , currentIndex_(-1)
    , mins_(QList<QSpinBox*>())
    , maxs_(QList<QSpinBox*>())
    , specials_(QList<QComboBox*>())
    , structs_(QList<DetachmentStruct>())
    , currentFile_(filename)
{
    QGridLayout *lay = new QGridLayout(this);

    detachmentList_ = new QListWidget(this);
    connect(detachmentList_, &QListWidget::currentRowChanged,
            this, &ListCreatorDetach::itemSelected);
    lay->addWidget(detachmentList_,1,0,20,1);

    QPushButton *button = new QPushButton("Add Detachment");
    lay->addWidget(button,0,0);
    connect(button, &QPushButton::clicked,
            this, &ListCreatorDetach::on_AddItem);

    button = new QPushButton("Remove Detachment");
    lay->addWidget(button,0,1,1,5);
    connect(button, &QPushButton::clicked,
            this, &ListCreatorDetach::removeItem);

    lay->addWidget(new QLabel("Command Points"),1,1,1,2);
    cp_ = new QLineEdit("0", this);
    cp_->setValidator(new QIntValidator());
    cp_->setFixedWidth(30);
    lay->addWidget(cp_,1,3,1,2);

    int row;
    QSpinBox *spin;
    QComboBox *box;
    for (int i = 0; i < BattlefieldRole::eBattlefieldRole_OverRoles; ++i)
    {
        row = i+2;
//      row = (((i)/3)*2)+2;
  //      int column = (((i)%3)*2)+10;

        QLabel *lab = new QLabel(this);
        lab->setPixmap(Detachment::getIcon(i));
        lay->addWidget(lab,row,1);

        spin = new QSpinBox(this);
        spin->setMinimum(0);
        mins_.append(spin);
        lay->addWidget(spin,row,2);

        lay->addWidget(new QLabel("-"),row,3);

        spin = new QSpinBox(this);
        spin->setMinimum(-1);
        maxs_ << spin;
        lay->addWidget(spin,row,4);

        QStringList list;
        for (int i = 0; i < SpecialSlot::eSpecialSlot_OverSlot; ++i)
            list << "";

        box = new QComboBox(this);
        list[SpecialSlot::eSpecialSlot_Null] = "Standard";
        list[SpecialSlot::eSpecialSlot_LimitByMin] = "Min by other slots";
        list[SpecialSlot::eSpecialSlot_LimitByMax] = "Max by other slots";

        box->addItems(list);

        specials_ << box;
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

QString ListCreatorDetach::getOrganisationList(const QString &filename)
{
    QFile f(filename);
    if (!f.open(QFile::Text | QFile::ReadOnly))
        return QString();
    QTextStream str(&f);
    str.setCodec("utf-8");
    QString line;
    QString ret;

    bool ok;

    // NEEDS WORK
    while (!str.atEnd() && line != "ORGANISATION")
    {
        line = str.readLine();
    }
    line = str.readLine();
    while (!line.isNull() && line.startsWith('\t'))
    {
        ret += line.trimmed();
        ret += ';';

        line = str.readLine();
    }
    if (!ret.isEmpty())
            ret.remove(ret.count()-1,1);
    f.close();
    return ret;
}

int ListCreatorDetach::addItem()
{
    DetachmentStruct strct;
    strct.cp_ = 0;
    for (int i = 0; i < BattlefieldRole::eBattlefieldRole_OverRoles; ++i)
    {
        strct.roles_ << RoleSlotStruct{0,0,0};
    }
    structs_ << strct;
    QListWidgetItem *item = new QListWidgetItem("New Detachment");
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    detachmentList_->addItem(item);
    return structs_.count()-1;
}

void ListCreatorDetach::on_AddItem()
{
    addItem();
}

void ListCreatorDetach::removeItem()
{
    if (currentIndex_ < 0)
        return;
    structs_.removeAt(currentIndex_);
    detachmentList_->takeItem(currentIndex_);
}

void ListCreatorDetach::specialSelected(int index)
{
    Q_UNUSED(index)
}

void ListCreatorDetach::itemSelected(int index)
{
    saveCurrent();
    currentIndex_ = index;
    cp_->setText(QString::number(structs_.at(index).cp_));
    const QList<RoleSlotStruct> &list = structs_.at(index).roles_;
    RoleSlotStruct strct;
    for (int i = 0; i < list.count(); ++i)
    {
        strct = list.at(i);
        mins_.at(i)->setValue(strct.min_);
        maxs_.at(i)->setValue(strct.max_);
        specials_.at(i)->setCurrentIndex(strct.special_);
    }
}

void ListCreatorDetach::on_OK()
{
    saveCurrent();
    writeFile();
    accept();
}

void ListCreatorDetach::writeFile()
{
    QFile f(currentFile_);
    if (!f.open(QFile::Text | QFile::WriteOnly))
        return;
    QTextStream str(&f);
    str.setCodec("utf-8");
    QList<RoleSlotStruct> list;
    RoleSlotStruct strct;
    QListWidgetItem *item;
    for (int i = 0; i < structs_.count(); ++i)
    {
        item = detachmentList_->item(i);
        if (!item)
            continue;
        str << '#' << item->text()
            << ',' << QString::number(structs_.at(i).cp_)
            << '\n';
        list = structs_.at(i).roles_;
        for (int j = 0; j < list.count(); ++j)
        {
            strct = list.at(j);
            if (strct.min_ | strct.max_ | strct.special_)
                str << '\t' << QString::number(j)
                    << ','  << QString::number(strct.min_)
                    << ','  << QString::number(strct.max_)
                    << ','  << QString::number(strct.special_)
                    << '\n';
        }
    }
    f.close();
}

void ListCreatorDetach::readFile()
{
    QFile f(currentFile_);
    if (!f.open(QFile::Text | QFile::ReadOnly))
        return;
    QTextStream str(&f);
    QString line;
    QStringList splitline;
    int indx;
    QList<RoleSlotStruct> *list = nullptr;
    while (!str.atEnd())
    {
        line = str.read(1);
        if (line == "#")
        {
            indx = addItem();
            splitline = str.readLine().split(',');
            detachmentList_->item(indx)->setText(splitline.at(0));
            if (splitline.count()>1)
                cp_->setText(splitline.at(1));
            else
                cp_->setText("0");

            list = &(structs_[indx].roles_);
        }
        else
        {
            splitline = str.readLine().split(',');
            (*list)[splitline.at(0).toInt()].min_ = splitline.at(1).toInt();
            (*list)[splitline.at(0).toInt()].max_ = splitline.at(2).toInt();
            (*list)[splitline.at(0).toInt()].special_ = splitline.at(3).toInt();
        }
    }

    f.close();
}

void ListCreatorDetach::saveCurrent()
{
    if (currentIndex_ < 0)
        return;
    structs_[currentIndex_].cp_ = cp_->text().toInt();
    QList<RoleSlotStruct> &list = structs_[currentIndex_].roles_;
    for (int i = 0; i < list.count(); ++i)
    {
        list[i].min_ =  mins_.at(i)->value();
        list[i].max_ =  maxs_.at(i)->value();
        list[i].special_ =  specials_.at(i)->currentIndex();
    }
}
