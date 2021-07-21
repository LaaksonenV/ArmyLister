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

void ListCreator::CreateArmy(QWidget *parent)
{
    ListCreator creator(QStringList("Name") << "Cost/Model#"
                        << "Models in unit#-#" << "Initial cost#", parent);

    creator.addIncl();
    creator.addOrg();

    creator._info->setText("This window is still under development.\n"
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
                           "denotes end of all control elements.\n"
                           "\n"
                           "Control elements: \n"
                           " !: Item is always selected, and cannot be "
                           "unselected.\n"
                           " !$X Item is selectable X times.\n"
                           " !$X@Y Item is selectable X times for Y models.\n"
                           " !€ Every instance of item is selected at same "
                           "time\n"
                           " !* Item is for all models.\n"
                           " !^X Item selectable if unit has models <= X.\n"
                           " !_X Item selectable if unit has models >= X.\n"
                           " !=X limits the entry to X selections for entire "
                           "army.\n"
                           " !§X limits the entry to X models for entire "
                           "army.\n"
                           " !~<AAA> Items points count toward AAA category "
                           "limits (9A specific).\n"
                           " !¨<AAA> Unit points count toward AAA category "
                           "limits (9A specific).\n"
                           " !~/<AAA> Items points do not count toward AAA "
                           "category limits if otherwise would (9A specific).\n"
                           " !{AAA} limits the item by by a group. Group "
                           "limits are defined separately in a list with name "
                           "[!] (list must be created manually).\n"
                           "  Group limit list entries are of form !{AAA}?=X, "
                           "where AAA is the name of the group, and X the "
                           "limit the item may be selected in army.\n"
                           " !\\ Entry contains one or more slots. Each slot "
                           "is denoted with AA(BBB, CCC), where AA is the "
                           "slots identifier and BBB, CCC is/are the default "
                           "item(s). If entry contains only one slot, AA may"
                           "be omitted.\n"
                           " !/AA Entry will replace the item in slot AA.\n"
                           " (!/AA/BB Entry will replace the item in slots AA "
                           "and BB TBA)\n"
                           "Following control elements limit the choises under "
                           "the item with the element:"
                           " !;X Select up to X points worth of items.\n"
                           " !#X Select up to X items.\n"
                           " !#X@Y Select up to X items for Y models.\n"
                           "A special control element staring with '?',"
                           " ?<AAA> "
                           "means item is only available on lists with "
                           "global AAA tag (40k specific). This must be "
                           "separated from other control by whitespace"

    );

    creator.exec();
}

void ListCreator::CreateList(QWidget *parent)
{
    ListCreator creator(QStringList("Name") << "Cost", parent);

    creator.addIncl();

    creator._info->setText("This window is still under development.\n"
                           "For now, a list is made by simply writing all "
                           "neccessary info manually on its respective field.\n"
                           "\n"
                           "This window will create a list or table file, that "
                           "may be included in the main list. Same file can "
                           "contain both lists and tables. Including "
                           "other files here will only benefit the "
                           "autocompletion function.\n"
                           "\n"
                           "A list is made of a name as root item, capsulated "
                           "in [] braces, followed by one or more items under "
                           "it. Writing the list in the main list will then "
                           "create an entry for each item in it."
                           "can be included with [] braces.\n"
                           "List entries may contain price information.\n"
                           "\n"
                           "List entries may be limited by adding one or more "
                           "control elements before its name. Each control "
                           "element starts with an exclamation point '!', and "
                           "next whitespace (outside tag control, see later) "
                           "denotes end of all control elements, with an "
                           "exception that any control elements after list "
                           "names ] will be inserted to each list entry.\n"
                           "\n"
                           "Control elements: \n"
                           " !=X limits the item to X selections for entire "
                           "army.\n"
                           " !#X limits the item to X selections for the "
                           "unit.\n"
                           " !<AAA> limits the item to entries according to "
                           "its tags:\n"
                           "  AAA a unit must have tag AAA to select this.\n"
                           "  /AAA a unit must not have tag AAA to select "
                           "this.\n"
                           "  AAA, BBB a unit must have both tags AAA and BBB "
                           "to select this. More tags may be chained and "
                           "preceded with /.\n"
                           "  * may be used as a wildcard in tags.\n"
                           " !{AAA} limits the item by by a group. Group "
                           "limits are defined separately in a list with name "
                           "[!] (list must be created manually).\n"
                           " !{AAA}{BBB} as above, groups can be stacked. Item "
                           "entries may add additional groups to list "
                           "entries.\n"
                           "  Group limit list entries are of form !{AAA}?#X, "
                           "where AAA is the name of the group, and X the "
                           "limit a unit may select items in this group.\n"
                           "\n"
                           "\n"
                           "Tables needed anymore?"

                           );

    creator.exec();
}

ListCreator::ListCreator(const QStringList &header,
                         QWidget *parent)
    : QDialog(parent)
    , lay(new QGridLayout(this))
    , _org(nullptr)
    , _incl(nullptr)
    , _list(new QTreeWidget(this))
    , _lines(QList<QLineEdit*>())
    , _info(new QTextEdit())
    , _fileName(QString())
    , _clipboard(nullptr)
    , _editor(new MCLineEdit(this))
{
    int columns = header.size();
    QVBoxLayout *lay2 = new QVBoxLayout();

    QHBoxLayout *chLay = new QHBoxLayout();

    QPushButton *button = new QPushButton("&Edit file", this);
    connect(button, &QPushButton::clicked,
            this, &ListCreator::on_loadFile);
    chLay->addWidget(button);

    lay2->addLayout(chLay);

    _list->setColumnCount(columns);

    _list->setSelectionMode(QAbstractItemView::SingleSelection);
    _list->setSelectionBehavior(QAbstractItemView::SelectRows);
    _list->setHeaderLabels(header);
    connect(_list, &QTreeWidget::currentItemChanged,
            this, &ListCreator::on_currentChanged);
    lay2->addWidget(_list,2);

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
            edit = _editor;
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

        _lines << edit;

        connect(edit, &QLineEdit::textChanged,
                this, [=](const QString &text)
        {_list->currentItem()->setText(i,text);});
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
    _editor->setEnabled(false);
    _list->setColumnWidth(0,200);

    lay2->addLayout(chLay);

    lay->addLayout(lay2,1,1,3,2);
    lay->setColumnStretch(1,2);
    lay->setRowStretch(3,2);

    _info->setReadOnly(true);
    lay->addWidget(_info,3,0,4,1);

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

    lay->addLayout(chLay,5,1);

    setLayout(lay);

    resize(800,800);
}

ListCreator::~ListCreator()
{
    if (_clipboard)
        delete _clipboard;
    _clipboard = nullptr;
}

void ListCreator::on_orgChange()
{
    QStringList org = _org->getOrg().split('#');
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
        _editor->addToCompleter(strings);
    }
    else
        QMessageBox::critical(this, "File open error", "The specified file "
                                                       "couldn't be opened");
}

void ListCreator::on_loadFile()
{
    QString file = QFileDialog::getOpenFileName
            (this, "Choose file to load", QString(),
             "(*.txt)", nullptr, QFileDialog::DontConfirmOverwrite);
    if (!file.isEmpty())
        _fileName = file;
}

void ListCreator::initialise40k()
{
    // Take previous items and add them to end

    _list->clear();

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

    _list->clear();

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
    _list->addTopLevelItem(newItem);
    return newItem;
}

bool ListCreator::on_save()
{
    if (!checkFileName())
        return false;

    QFile f(_fileName);
    if (!f.open(QFile::Text | QFile::WriteOnly))
    {
        QMessageBox::critical(this, "File open error", "The specified file "
                                                       "couldn't be opened");
        return false;
    }
    f.flush();
    QTextStream str(&f);
    str.setCodec("utf-8");

    if (_org)
    {
        str << "ORGANISATION" << '\n';
        str << '\t' << _org->getOrg() << '\n';

        if (_incl)
        {
            str << "INCLUDES" << '\n';
            foreach (QString f, _incl->getFiles())
                str << '\t' << f << '\n';
        }
    }

    writeFileRecurse(str, _list->invisibleRootItem(), 0);

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
    QTreeWidgetItem *currentItem = _list->currentItem();
    if (!currentItem)
        currentItem = _list->invisibleRootItem();
    QTreeWidgetItem *newItem = new QTreeWidgetItem(QStringList("New"));
    QTreeWidgetItem *parentItem = currentItem->parent();
    if (!parentItem)
        parentItem = _list->invisibleRootItem();

    parentItem->addChild(newItem);

    _list->setCurrentItem(newItem);
    return newItem;
}

QTreeWidgetItem *ListCreator::on_createUnder()
{
    QTreeWidgetItem *currentItem = _list->currentItem();
    if (!currentItem)
        currentItem = _list->invisibleRootItem();
    QTreeWidgetItem *newItem = new QTreeWidgetItem(QStringList("New"));
    currentItem->addChild(newItem);

    _list->setCurrentItem(newItem);
    return newItem;
}

QTreeWidgetItem *ListCreator::on_createOver()
{
    QTreeWidgetItem *currentItem = _list->currentItem();
    if (!currentItem)
        return on_createUnder();
    else if (currentItem->type() == preMadeItem)
        return nullptr;
    QTreeWidgetItem *newItem = new QTreeWidgetItem(QStringList("New"));
    QTreeWidgetItem *parentItem = _list->currentItem()->parent();
    if (!parentItem)
        parentItem = _list->invisibleRootItem();

    parentItem->addChild(newItem);
    parentItem->removeChild(currentItem);
    newItem->addChild(currentItem);

    _list->setCurrentItem(newItem);
    return newItem;
}

void ListCreator::on_copy()
{
    QTreeWidgetItem *currentItem = _list->currentItem();
    if (!currentItem)
        return;

    if (_clipboard)
        delete _clipboard;
    _clipboard = currentItem->clone();
}

void ListCreator::on_delete()
{
    QTreeWidgetItem *currentItem = _list->currentItem();
    if (!currentItem || currentItem->type() == preMadeItem)
        return;
    QTreeWidgetItem *parentItem = currentItem->parent();
    if (!parentItem)
        parentItem = _list->invisibleRootItem();

    _list->setCurrentItem(_list->itemBelow(currentItem));

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
    QTreeWidgetItem *currentItem = _list->currentItem();
    if (!currentItem || !_clipboard)
        return;

    currentItem->addChild(_clipboard->clone());
}

void ListCreator::on_pasteNext()
{
    QTreeWidgetItem *currentItem = _list->currentItem();
    if (!currentItem || !_clipboard)
        return;
    QTreeWidgetItem *parentItem = currentItem->parent();
    if (!parentItem)
        parentItem = _list->invisibleRootItem();

    parentItem->addChild(_clipboard->clone());
}

void ListCreator::on_labelChange(const QString &text)
{
    if (!_list->currentItem())
        return;
    _list->currentItem()->setText(0,text);
}

void ListCreator::on_costChange(const QString &text)
{
    if (!_list->currentItem())
        return;
    _list->currentItem()->setText(1,text);
}

void ListCreator::on_currentChanged(QTreeWidgetItem *now, QTreeWidgetItem *)
{
    if (!now)
        return;
    for (int i = 0; i < _lines.size(); ++i)
        _lines.at(i)->setText(now->text(i));

    _lines.at(0)->setFocus();
    _lines.at(0)->selectAll();
    _editor->setEnabled(true);
}

void ListCreator::addOrg()
{
    _org = new ListCreatorWidgetOrg(this);
    connect(_org, &ListCreatorWidgetOrg::finished,
            this, &ListCreator::on_orgChange);
    lay->addWidget(_org,0,0);
}

void ListCreator::addIncl()
{
    _incl = new ListCreatorWidgetIncl(this);
    connect(_incl, &ListCreatorWidgetIncl::fileAdded,
            this, &ListCreator::on_includeAdd);
    lay->addWidget(_incl,1,0);
}

bool ListCreator::checkFileName()
{
    if (_fileName.isNull())
    {
        QString file = QFileDialog::getSaveFileName
                (this, "Choose filename", QString(),
                 "(*.txt)", nullptr, QFileDialog::DontConfirmOverwrite);
        if (file.isEmpty())
            return false;
        else
            _fileName = file;
    }
    return true;
}

void ListCreator::readFile()
{
    QFile f(_fileName);
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
        if (_org)
            _org->setOrg(str.readLine().trimmed());
        line = str.readLine();

    }

    if (!line.isNull() && line == "INCLUDES")
    {
        if (_incl)
        {
            line = str.readLine();
            while (!line.isNull() && line.startsWith('\t'))
            {
                _incl->addFile(line.trimmed());
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
            _list->addTopLevelItem(newItem);

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
