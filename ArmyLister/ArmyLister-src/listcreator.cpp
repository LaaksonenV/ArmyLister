#include "listcreator.h"

#include <QHBoxLayout>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTreeWidget>
#include <QList>
#include <QFile>
#include <QTextStream>
#include <QLineEdit>
#include <QIntValidator>
#include <QRegExpValidator>
#include <QLabel>
#include <QString>
#include <QStringList>
#include <QTextEdit>
#include <QFileDialog>
#include <QKeyEvent>
#include <QMessageBox>

#include "textedit.h"
#include "listcreatorwidgets.h"
#include "settings.h"

void ListCreator::CreateArmy(QWidget *parent)
{
    ListCreator creator(QStringList("Name") << "Cost/Model#"
                        << "Models in unit#-#" << "Initial cost#", parent);

    creator.addIncl();
    creator.addOrg();

    creator.info_->setText(
                "This window is still under development.\n"
                "For now, a list is made by simply writing all "
                "neccessary info manually on its respective field.\n"
                "\n"
                "Army lists must contain information on how their "
                "organisation is defined. Before building the list, "
                "define the organisation, as this will reset the "
                "list!!\n"
                "List all neccessary files in the 'include' list. "
                "These are the item and rule/stat list files "
                "created previously (or later and also added later, "
                "previously created lists would help with auto"
                "completing words while building this list.\n"
                "\n"
                "Army list is comprised of categories, with unit "
                "entries under them, and items under them. If unit "
                "or item entries don't contain price or model "
                "information, these will be looked for in table "
                "files listed in the included section.\n"
                "A list can be included as an item with [] braces. "
                "Selections from lists may be limited with tags. "
                " Each entry has a list of tags, that by default "
                "contain their name and names of the entries above. "
                "Units add tags of selected items to their own, "
                "and items below may check if their tag limiters "
                "match with these. Tags may be added to units by "
                "listing them after their name, each tag capsulated "
                "by <> braces. Item entries' tags are added "
                "similarily, but with <+AA> adding a tag and <-AA> "
                "removing it. Tags are used "
                "to restrict selections based on the role and "
                "equipment a unit has.\n"
                "IMPORTANT ABOUT WEAPONS/ARMOUR. When "
                "a unit has access to "
                "special equipment, it must also contain "
                "information on what equipment it has. If a unit "
                "can "
                "select a weapon, this should be made with a "
                "selection/slot entry, with one of the options as "
                "default if it MUST, and 'Hand Weapon' (or listed "
                "default weapon if so) as default if it MAY. Same "
                "goes with Armour. If these aren't changeable, "
                "add the units default equipment as tags."
                "\n"
                "\n"
                "Entries may be controlled by adding one or more "
                "control elements before its name. Each control "
                "element starts with an exclamation point '!', and "
                "next whitespace (outside tag control, see later) "
                "denotes end of all control elements. X@Y means X "
                "for Y models, which is option for some items "
                "with definable amounts X.\n"
                "\n"
                "Control elements: \n"
                " !: This item is always selected, and cannot be "
                "unselected.\n"
                " !; This item is not manually selectable, but will "
                "be selected if its item is.\n"
                " !\\ This item contains changeable slots. *\n"
                " !/ This item changes an item in slots. *\n"
                " !. This item is for all models.\n"
                " !,X This item replaces X of for all. X=-1 "
                "replaces one for each selected item\n"
                " !+X This item is selectable X times. (X@Y)\n"
                " !#X Take up to X of items. (X@Y)\n"
                " !$X Tale up to X points worth of items.\n"
                " !^X This item selectable if unit has models <= "
                "X.\n"
                " !_X This item selectable if unit has models >= "
                "X.\n"
                " !€ Every copy of this item is selected at same "
                "time.\n"
                " !=X Item is limited to X selections for entire "
                "army.\n"
                " !£X Item is limited to X models for entire "
                "army.\n"
                " !?<>/{} Item is limited by tags or groups. *\n"
                " !~<AA> This items points count to category "
                "AA. Adding '|' before category name means "
                "not to count towards category\n"
                " !¨<AA> This item makes whole unit count as "
                "category AA. Adding '|' before category name means "
                "not to count towards category\n"
                "\n"
                "*Slots\n"
                "!\\ a(AA) b(BB, CC) makes an item with two slots, "
                "where slot named 'a' contains option AA and 'b' "
                "options BB and CC. Item may contain more or fewer "
                "slots, but all must be named differently.\n"
                "Under this !/a item starts a list of items that "
                "replace the option in slot 'a'. Items under !/a!/b "
                "replace options in both 'a' and 'b' slots.\n"
                "Iff !\\ contains only one slot, the slot name may "
                "be omitted along the parantheses, and !/ item is "
                "not needed, as replacing items may start "
                "immediately after !\\ item.\n"
                "\n"
                "*Tags\n"
                "Tags contain short additional information "
                "about items. Items tags contain their name and "
                "any provided by adding one or more '<AA>' after "
                "items "
                "name. Selected items will add their tags to to "
                "their parents. Adding '|' before tag name (<|AA>) "
                "will remove the tag from their parents if "
                "applicable.\n"
                "Limiting by a tag (!?<AA>) makes the item "
                "selectable only if it or its parent contains the "
                "tag, or doesn't contain the tag if preceded by '|' "
                "(!?<|AA>). Chaining tags with comma checks for all "
                "tags (eg. !?<AA, |BB> = if AA and not BB), and "
                "multiple limiters check for any tag (eg. "
                "!?<AA><|BB> = if AA or not BB). "
                "(Commas are not allowed in tag "
                "names!). '*' in a limiting tag means any word "
                "(eg. !?<A* C> accepts <AB C> and <A C>, but not "
                "<ABC> or <B C>).\n"
                "\n"
                "*Groups\n"
                "All items limited by the same group (!?{AA}) share "
                "the same limitation. Defining groups is done "
                "before compiling, see next part.\n"
                "\n"
                " § is a special control element which will take "
                "effect before lists are compiled:\n"
                " §L Creates a list out of the items under it, with the "
                "items text as lists name. These "
                "lists can then be used as "
                "items with just '[List name]' as text. List item may include "
                "tags that are common to all of lists items, and may be "
                "limited army wide (ie. only X items of list in army). Must be "
                "top level item (not indented). If item is left empty, each "
                "item below is considered "
                "separate list entry. Lists may contain other lists. "
                "Lists items may contain tag, group and armywide limit "
                "controls.\n"
                " §G{AA}?'Limit' Creates a grouplimiter with name 'AA'. "
                "Limit options are '=X' to limit groups items for "
                "whole army (eg. §G{AA}?=1), "
                "and '#X' for one unit (eg. §G{AA}?#2). Any text in group "
                "creation item is ignored. Must also be top level.\n"
                " §R Marks item(/unit) as retinue item, which may "
                "be included as an option for other item. If you give the "
                "units name in a tag as part of the control element, you"
                "may give alternative name to the unit as normal item name.\n"
                " §?<AA> Limit by tag, so if an army cannot have "
                "this item unless complemented by other lists "
                "(eg. Space Marines and their supplements), "
                "it may be omitted from whole process at the start.\n"
                " §¤ Starts values table. Items beneath this contain "
                "unit values, beneath them their options' point values. Top "
                "level. If item is left empty, each item below is considered "
                "separate table entry.\n"
                "\n"

                );

    creator.exec();
}

void ListCreator::CreateList(QWidget *parent)
{
    ListCreator creator(QStringList("Name") << "Cost", parent);

    creator.addIncl();

    creator.info_->setText(

                ""
                ""
                ""
                ""
                ""
                "TODO"
                ""
                ""
                ""
                ""
                ""




                           );

    creator.exec();
}

ListCreator::ListCreator(const QStringList &header,
                         QWidget *parent)
    : QDialog(parent)
    , lay_(new QGridLayout(this))
    , org_(nullptr)
    , incl_(nullptr)
    , list_(new QTreeWidget(this))
    , lineEdits_(QList<QLineEdit*>())
    , info_(new QTextEdit())
    , fileName_(QString())
    , clipboard_(nullptr)
    , editor_(new MCLineEdit(this))
{
    int columns = header.size();
    QVBoxLayout *lay2 = new QVBoxLayout();

    QHBoxLayout *chLay = new QHBoxLayout();

    QPushButton *button = new QPushButton("&Edit file", this);
    connect(button, &QPushButton::clicked,
            this, &ListCreator::on_loadFile);
    chLay->addWidget(button);

    lay2->addLayout(chLay);

    list_->setColumnCount(columns);

    list_->setSelectionMode(QAbstractItemView::SingleSelection);
    list_->setSelectionBehavior(QAbstractItemView::SelectRows);
    list_->setHeaderLabels(header);
    connect(list_, &QTreeWidget::currentItemChanged,
            this, &ListCreator::on_currentChanged);
    lay2->addWidget(list_,2);

    chLay = new QHBoxLayout();

    button = new QPushButton("&Copy", this);
    connect(button, &QPushButton::clicked,
            this, &ListCreator::on_copy);
    chLay->addWidget(button);

    button = new QPushButton("Cut", this);
    connect(button, &QPushButton::clicked,
            this, &ListCreator::on_cut);
    chLay->addWidget(button);

    button = new QPushButton("Delete", this);
    connect(button, &QPushButton::clicked,
            this, &ListCreator::on_delete);
    chLay->addWidget(button);

    chLay->addStretch(2);
    lay2->addLayout(chLay);

    chLay = new QHBoxLayout();

    button = new QPushButton("Paste Next", this);
    connect(button, &QPushButton::clicked,
            this, &ListCreator::on_pasteNext);
    chLay->addWidget(button);

    button = new QPushButton("Paste Under", this);
    connect(button, &QPushButton::clicked,
            this, &ListCreator::on_pasteUnder);
    chLay->addWidget(button);

    chLay->addStretch(2);
    lay2->addLayout(chLay);

    chLay = new QHBoxLayout();

    button = new QPushButton("Create &Next", this);
    connect(button, &QPushButton::clicked,
            this, &ListCreator::on_createNext);
    chLay->addWidget(button);

    button = new QPushButton("Create &Under", this);
    connect(button, &QPushButton::clicked,
            this, &ListCreator::on_createUnder);
    chLay->addWidget(button);

    button = new QPushButton("Create &Over", this);
    connect(button, &QPushButton::clicked,
            this, &ListCreator::on_createOver);
    chLay->addWidget(button);

    chLay->addStretch(2);
    lay2->addLayout(chLay);

    lay2->addSpacing(10);
    chLay = new QHBoxLayout();

    QLabel *label;
    QLineEdit *edit = nullptr, *edittemp = nullptr;
    for (int i = 0; i < columns; ++i)
    {

        if (i == 0)
        {
            edit = editor_;
            lay2->addWidget(edit);
//            setTabOrder(button, edit);
        }
        else
        {
            label = new QLabel(header.value(i) + ':', this);
            chLay->addWidget(label);
            edittemp = edit;
            edit = new QLineEdit(this);
            chLay->addWidget(edit);
            setTabOrder(edittemp,edit);
        }

        lineEdits_ << edit;

        connect(edit, &QLineEdit::textChanged,
                this, [=](const QString &text)
        {list_->currentItem()->setText(i,text);});
        //chLay->addWidget(edit);

        if (header.at(i).endsWith("#-#"))
        {
            QRegExp rx("\\d*(-\\d+)?");
            edit->setValidator(new QRegExpValidator(rx, this));
            edit->setFixedWidth(65);
        }
        else if (header.at(i).endsWith("#"))
        {
            edit->setValidator(new QIntValidator(this));
            edit->setFixedWidth(35);
        }
        else
            chLay->setStretch((i*2)+1,2);

    }
    editor_->setEnabled(false);
    list_->setColumnWidth(0,200);

    lay2->addLayout(chLay);

    lay_->addLayout(lay2,1,1,3,2);
    lay_->setColumnStretch(1,2);
    lay_->setRowStretch(3,2);

    info_->setReadOnly(true);
    lay_->addWidget(info_,3,0,4,1);

    chLay = new QHBoxLayout();

    chLay->addStretch(2);

    button = new QPushButton("&Save", this);
    connect(button, &QPushButton::clicked,
            this, &ListCreator::on_save);
    chLay->addWidget(button);

    button = new QPushButton("&Done", this);
    connect(button, &QPushButton::clicked,
            this, &ListCreator::on_OK);
    chLay->addWidget(button);

    button = new QPushButton("&Return", this);
    connect(button, &QPushButton::clicked,
            this, &QDialog::reject);
    chLay->addWidget(button);

    lay_->addLayout(chLay,5,1);

    setLayout(lay_);

    resize(800,800);
}

ListCreator::~ListCreator()
{
    if (clipboard_)
        delete clipboard_;
    clipboard_ = nullptr;
}

void ListCreator::on_orgChange()
{
    QStringList org = org_->getOrg().split('#');
    QString type = org.takeFirst();
    if (type == "40k")
        initialise40k();
    else if (type == "9A")
    {
        for (int i = 0; i < org.count(); ++i)
            org[i] = org.at(i).section(';',0,0);
        initialise9A(org);
    }
}

void ListCreator::on_includeAdd(const QString &filename)
{
    QFile file(filename);

    if (file.open(QFile::Text | QFile::ReadOnly))
    {
        QTextStream str(&file);
        QString line;
        QRegExp rx ("!?<(.*)>");
        rx.setMinimal(true);
        QRegExp ctrl ("!.+ ");
        ctrl.setMinimal(true);
        QStringList caps;
        QStringList strings;

        while (!str.atEnd())
        {
            line = str.readLine().section('|',0,0);
            if (rx.indexIn(line) > 0)
            {
                for (int i = 1; i < rx.captureCount(); ++i)
                {
                    caps = rx.cap(i).split(',');
                    foreach (QString wrd, caps)
                    {
                        wrd = wrd.trimmed();

                        if (wrd.count() > 3 && !wrd.toInt() &&
                                !strings.contains(wrd))
                            strings << wrd;
                    }
                }
            }
            line.remove(rx);
            line.remove(ctrl);
            line.remove(QRegExp("!\\w+"));
            line = line.trimmed();

            if (line.count() > 3 && !line.toInt() &&
                    !strings.contains(line))
                strings << line;

        }
        file.close();
        editor_->addToCompleter(strings);
    }
    else
        QMessageBox::critical(this, "File open error", "The specified file "
                                                       "couldn't be opened");
}

void ListCreator::on_loadFile()
{
    QString file = QFileDialog::getOpenFileName
            (this, "Choose file to load", fileName_,
             "(*.txt)", nullptr, QFileDialog::DontConfirmOverwrite);
    if (!file.isEmpty())
        fileName_ = file;
}

void ListCreator::initialise40k()
{
    // Take previous items and add them to end

    list_->clear();

    createPreMadeItem(HQ_ROLE);
    createPreMadeItem(TROOP_ROLE);
    createPreMadeItem(ELITE_ROLE);
    createPreMadeItem(FAST_ROLE);
    createPreMadeItem(HEAVY_ROLE);
    createPreMadeItem(TRANSPOR_ROLE);
    createPreMadeItem(FLYER_ROLE);
    createPreMadeItem(FORT_ROLE);
    createPreMadeItem(LORD_ROLE);

}

void ListCreator::initialise9A(const QStringList &org)
{

    list_->clear();

    for (int i = 0; i < org.count(); ++i)
    {
        createPreMadeItem(org.at(i));
    }


    // INFO?
}

QTreeWidgetItem *ListCreator::createPreMadeItem(const QString &text)
{
    QTreeWidgetItem *newItem = new QTreeWidgetItem(preMadeItem);
    newItem->setText(0,text);
    list_->addTopLevelItem(newItem);
    return newItem;
}

bool ListCreator::on_save()
{
    if (!checkFileName())
        return false;

    QFile f(fileName_);
    if (!f.open(QFile::Text | QFile::WriteOnly))
    {
        QMessageBox::critical(this, "File open error", "The specified file "
                                                       "couldn't be opened");
        return false;
    }
    f.flush();
    QTextStream str(&f);
    str.setCodec("utf-8");

    if (org_)
    {
        str << "ORGANISATION" << '\n';
        str << '\t' << org_->getOrg() << '\n';

        if (incl_)
        {
            str << "INCLUDES" << '\n';
            foreach (QString f, incl_->getFiles())
                str << '\t' << f << '\n';
        }
    }

    writeFileRecurse(str, list_->invisibleRootItem(), 0);

    f.close();
    return true;
}

void ListCreator::on_OK()
{
    if(on_save())
        accept();
}

void ListCreator::writeFileRecurse(QTextStream &str,
                                   QTreeWidgetItem *item,
                                   int depth)
{
    QStringList textList;
    QTreeWidgetItem *currentItem;

    for (int i = 0; i < item->childCount(); ++i)
    {
        currentItem = item->child(i);
        textList.clear();

        for (int i = 0; i < depth; ++i)
            str << '\t';
        for (int i = 0; i < currentItem->columnCount(); ++i)
            textList << currentItem->text(i);
        str << textForFile(textList) << '\n';

        writeFileRecurse(str, currentItem, depth+1);
    }
}

QTreeWidgetItem *ListCreator::on_createNext()
{
    QTreeWidgetItem *currentItem = list_->currentItem();
    if (!currentItem)
        currentItem = list_->invisibleRootItem();
    QTreeWidgetItem *newItem = new QTreeWidgetItem(QStringList("New"));
    QTreeWidgetItem *parentItem = currentItem->parent();
    if (!parentItem)
        parentItem = list_->invisibleRootItem();

    parentItem->addChild(newItem);

    list_->setCurrentItem(newItem);
    return newItem;
}

QTreeWidgetItem *ListCreator::on_createUnder()
{
    QTreeWidgetItem *currentItem = list_->currentItem();
    if (!currentItem)
        currentItem = list_->invisibleRootItem();
    QTreeWidgetItem *newItem = new QTreeWidgetItem(QStringList("New"));
    currentItem->addChild(newItem);

    list_->setCurrentItem(newItem);
    return newItem;
}

QTreeWidgetItem *ListCreator::on_createOver()
{
    QTreeWidgetItem *currentItem = list_->currentItem();
    if (!currentItem)
        return on_createUnder();
    else if (currentItem->type() == preMadeItem)
        return nullptr;
    QTreeWidgetItem *newItem = new QTreeWidgetItem(QStringList("New"));
    QTreeWidgetItem *parentItem = list_->currentItem()->parent();
    if (!parentItem)
        parentItem = list_->invisibleRootItem();

    parentItem->addChild(newItem);
    parentItem->removeChild(currentItem);
    newItem->addChild(currentItem);

    list_->setCurrentItem(newItem);
    return newItem;
}

void ListCreator::on_copy()
{
    QTreeWidgetItem *currentItem = list_->currentItem();
    if (!currentItem)
        return;

    if (clipboard_)
        delete clipboard_;
    clipboard_ = currentItem->clone();
}

void ListCreator::on_delete()
{
    QTreeWidgetItem *currentItem = list_->currentItem();
    if (!currentItem || currentItem->type() == preMadeItem)
        return;
    QTreeWidgetItem *parentItem = currentItem->parent();
    if (!parentItem)
        parentItem = list_->invisibleRootItem();

    list_->setCurrentItem(list_->itemBelow(currentItem));

    parentItem->removeChild(currentItem);
    delete currentItem;
}

void ListCreator::on_cut()
{
    on_copy();
    on_delete();
}

void ListCreator::on_pasteUnder()
{
    QTreeWidgetItem *currentItem = list_->currentItem();
    if (!currentItem || !clipboard_)
        return;

    currentItem->addChild(clipboard_->clone());
}

void ListCreator::on_pasteNext()
{
    QTreeWidgetItem *currentItem = list_->currentItem();
    if (!currentItem || !clipboard_)
        return;
    QTreeWidgetItem *parentItem = currentItem->parent();
    if (!parentItem)
        parentItem = list_->invisibleRootItem();

    parentItem->addChild(clipboard_->clone());
}

void ListCreator::on_labelChange(const QString &text)
{
    if (!list_->currentItem())
        return;
    list_->currentItem()->setText(0,text);
}

void ListCreator::on_costChange(const QString &text)
{
    if (!list_->currentItem())
        return;
    list_->currentItem()->setText(1,text);
}

void ListCreator::on_currentChanged(QTreeWidgetItem *now, QTreeWidgetItem *)
{
    if (!now)
        return;
    for (int i = 0; i < lineEdits_.size(); ++i)
        lineEdits_.at(i)->setText(now->text(i));

    lineEdits_.at(0)->setFocus();
    lineEdits_.at(0)->selectAll();
    editor_->setEnabled(true);
}

void ListCreator::addOrg()
{
    org_ = new ListCreatorWidgetOrg(this);
    connect(org_, &ListCreatorWidgetOrg::finished,
            this, &ListCreator::on_orgChange);
    lay_->addWidget(org_,0,0);
}

void ListCreator::addIncl()
{
    incl_ = new ListCreatorWidgetIncl(this);
    connect(incl_, &ListCreatorWidgetIncl::fileAdded,
            this, &ListCreator::on_includeAdd);
    lay_->addWidget(incl_,1,0);
}

bool ListCreator::checkFileName()
{
    if (fileName_.isNull())
    {
        QString file = QFileDialog::getSaveFileName
                (this, "Choose filename", QString(),
                 "(*.txt)", nullptr, QFileDialog::DontConfirmOverwrite);
        if (file.isEmpty())
            return false;
        else
            fileName_ = file;
    }
    return true;
}

void ListCreator::readFile()
{
    QFile f(fileName_);
    QTreeWidgetItem *currentItem = nullptr;
    if (!f.exists())
    {
 //       currentItem = new QTreeWidgetItem(QStringList("New"));
   //     _list->addTopLevelItem(currentItem);
        return;
    }
    if (!f.open(QFile::Text | QFile::ReadOnly))
        return;
    QTextStream str(&f);
    QString line;

    int depth = 0;
    int count = 0;
    QTreeWidgetItem *newItem;

    line = str.readLine();
    if (!line.isNull() && line == "ORGANISATION")
    {
        if (org_)
            org_->setOrg(str.readLine().trimmed());
        line = str.readLine();

    }

    if (!line.isNull() && line == "INCLUDES")
    {
        if (incl_)
        {
            line = str.readLine();
            while (!line.isNull() && line.startsWith('\t'))
            {
                incl_->addFile(line.trimmed());
                line = str.readLine();
            }
        }
        else
            while (!line.isNull() && line.startsWith('\t'))
                line = str.readLine();

    }

    while (!line.isNull())
    {


        if (line.trimmed().isEmpty())
            continue;
        count = line.count('\t');
        if (count > depth +1)
            count = depth +1;

        newItem = new QTreeWidgetItem(textFromFile(line));

        if (count == 0)
            list_->addTopLevelItem(newItem);

        else
        {
            while (count <= depth)
            {
                currentItem = currentItem->parent();
                --depth;
            }
            currentItem->addChild(newItem);
        }

        depth = count;
        currentItem = newItem;
        line = str.readLine();
    }

    f.close();
}

QString ListCreator::textForFile(const QStringList &str)
{
    QStringList ret;
    int empt = 0;
    foreach (QString temp, str)
    {
        temp = temp.trimmed();
        if (temp.isEmpty())
            ++empt;
        else
        {
            for (int i = 0; i < empt; ++i)
                ret << "";
            empt = 0;
            temp.replace('|',"||");
            temp.replace('\t', "  ");
            ret << temp;
        }
    }
    return ret.join(" | ");
}

QStringList ListCreator::textFromFile(const QString &str)
{
    QStringList templist = str.split(" | ");
    QStringList ret;
    QString temp;
    for (int i = 0; i < templist.count(); ++i)
    {
        temp = templist.at(i).trimmed();
        temp.replace("||", "|");
        ret << temp;
    }
    return ret;
}

void ListCreator::keyPressEvent(QKeyEvent *e)
{
    switch (e->key())
    {
    case Qt::Key_Enter: case Qt::Key_Return:
        on_createNext();
        e->accept();
        return;
    }
    QDialog::keyPressEvent(e);
}
