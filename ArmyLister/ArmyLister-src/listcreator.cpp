#include "listcreator.h"

#include <QHBoxLayout>
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

#include "textedit.h"
#include "listcreatorwidgets.h"

void ListCreator::CreateArmy(const QString &file, QWidget *parent)
{
    ListCreator creator(file,  QStringList("Name") << "Cost/Model#"
                        << "Models in unit#-#" << "Initial cost#", parent);

    creator.addIncl();
    creator.addOrg();

    creator.readFile();

    creator._info->setText("This window is still under development.\n"
                           "For now, a list is made by simply writing all "
                           "neccessary info manually on its respective field.\n"
                           "\n"
                           "\n"
                           "Army lists must contain information on how their "
                           "organisation is defined. Before building the list, "
                           "define the organisation, as this will reset the "
                           "list!!\n"
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
                           "removing it.\n"
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
                           " !* Item is for all models.\n"
                           " !^X Item selectable if unit has models <= X.\n"
                           " !_X Item selectable if unit has models >= X.\n"
                           " !=X limits the entry to X selections for entire "
                           "army.\n"
                           " !~<AAA> Items points count toward AAA categorys "
                           "limits (9A specific).\n"
                           " !¨<AAA> Unit points count toward AAA categorys "
                           "limits (9A specific).\n"
                           " !{AAA} limits the item by by a group. Group "
                           "limits are defined separately in a list with name "
                           "[!] (list must be created manually).\n"
                           "  Group limit list entries are of form !{AAA}?=X, "
                           "where AAA is the name of the group, and X the "
                           "limit the item may be selected in army.\n"
                           " !\\ Entry contains one or more slots. Each slot "
                           "is denoted with AA(BBB, CCC), where AA is the "
                           "slots identifier and BBB, CCC is the default "
                           "item(s).\n"
                           " !/AA Entry will replace the item in slot AA.\n"
                           " (!/AA/BB Entry will replace the item in slots AA "
                           "and BB TBA)"
                           "Following control elements limit the choises under "
                           "the item with the element:"
                           " !;X Select up to X points worth of items.\n"
                           " !#X Select up to X items.\n"

    );

    creator.exec();
}

void ListCreator::CreateList(const QString &file, QWidget *parent)
{
    ListCreator creator(file,  QStringList("Name") << "Cost/Model#"
                        << "Models in unit#-#" << "Initial cost#", parent);

    creator.addIncl();

    creator.readFile();

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

ListCreator::ListCreator(const QString &file,
                         const QStringList &header,
                         QWidget *parent)
    : QDialog(parent)
    , lay(new QVBoxLayout(this))
    , _org(nullptr)
    , _incl(nullptr)
    , _list(new QTreeWidget(this))
    , _lines(QList<QLineEdit*>())
    , _info(new QTextEdit())
    , _fileName(file)
    , _clipboard(nullptr)
    , _editor(new MCLineEdit(this))
{
    int columns = header.size();
    _list->setColumnCount(columns);

    _list->setSelectionMode(QAbstractItemView::SingleSelection);
    _list->setSelectionBehavior(QAbstractItemView::SelectRows);
    _list->setHeaderLabels(header);
    connect(_list, &QTreeWidget::currentItemChanged,
            this, &ListCreator::on_currentChanged);
    lay->addWidget(_list,3);

    QHBoxLayout *chLay = new QHBoxLayout();

    QPushButton *button = new QPushButton("&Copy", this);
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
    lay->addLayout(chLay);

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
    lay->addLayout(chLay);

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
    lay->addLayout(chLay);

    lay->addSpacing(10);
    chLay = new QHBoxLayout();

    QLabel *label;
    QLineEdit *edit = nullptr, *edittemp = nullptr;
    for (int i = 0; i < columns; ++i)
    {
        label = new QLabel(header.value(i) + ':', this);
        chLay->addWidget(label);

        if (i == 0)
        {
            edit = _editor;
            chLay->addWidget(edit);
//            setTabOrder(button, edit);
        }
        else
        {
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

    lay->addLayout(chLay);

    _info->setReadOnly(true);
    lay->addWidget(_info);

    lay->addSpacing(10);
    chLay = new QHBoxLayout();

    chLay->addStretch(2);

    button = new QPushButton("&Ok", this);
    connect(button, &QPushButton::clicked,
            this, &ListCreator::on_OK);
    chLay->addWidget(button);

    button = new QPushButton("&Cancel", this);
    connect(button, &QPushButton::clicked,
            this, &QDialog::reject);
    chLay->addWidget(button);

    lay->addLayout(chLay);

    setLayout(lay);

    resize(400,800);
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
            org[i] = org.at(i).section(';',0,1);
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
        QRegExp rx ("!<(.*)>");
        QStringList caps;
        QStringList strings;

        while (!str.atEnd())
        {
            line = str.readLine().section('|',0,0);
            if (rx.indexIn(line) > 0)
            {
                caps = rx.cap(1).split(',');
                foreach (QString wrd, caps)
                {
                    wrd = wrd.trimmed();

                    if (wrd.count() > 3 && !wrd.toInt() &&
                            !strings.contains(wrd))
                        strings << wrd;
                }
            }
            line.remove(rx);
            line.remove(QRegExp("!\\w+"));
            line = line.trimmed();

            if (line.count() > 3 && !line.toInt() &&
                    !strings.contains(line))
                strings << line;

        }
        file.close();
        _editor->addToCompleter(strings);
    }
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

void ListCreator::on_OK()
{
    QFile f(_fileName);
    if (!f.open(QFile::Text | QFile::WriteOnly))
        return;
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
    lay->insertWidget(0, _org);
}

void ListCreator::addIncl()
{
    _incl = new ListCreatorWidgetIncl(this);
    lay->insertWidget(0, _incl);
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

    while (!str.atEnd())
    {
        line = str.readLine();
        if (line == "ORGANISATION")
        {
            if (_org)
                _org->setOrg(str.readLine().trimmed());
            line = str.readLine();

        }
        else if (line == "INCLUDES")
        {
            if (_incl)
            {
                line = str.readLine();
                while (line.startsWith('\t'))
                {
                    _incl->addFile(line.trimmed());
                    line = str.readLine();
                }
            }
            else
                while (line.startsWith('\t'))
                    line = str.readLine();

        }
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
    }

    f.close();
}

QString ListCreator::textForFile(const QStringList &str)
{
    QStringList ret;
    foreach (QString temp, str)
    {
        temp = temp.trimmed();
            temp.replace('|',"||");
            temp.replace('\t', "  ");
            ret << temp;

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
