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

#include "textedit.h"

ListCreator::ListCreator(const QString &file,
                         const QStringList &header,
                         QWidget *parent)
    : QDialog(parent)
    , _list(new QTreeWidget(this))
    , _lines(QList<QLineEdit*>())
    , _info(new QTextEdit())
    , _fileName(file)
    , _clipboard(nullptr)
    , _editor(new MCLineEdit(this))
{
    QVBoxLayout *lay = new QVBoxLayout(this);
    int columns = header.size();
    _list->setColumnCount(columns);

    _list->setSelectionMode(QAbstractItemView::SingleSelection);
    _list->setSelectionBehavior(QAbstractItemView::SelectRows);
    _list->setHeaderLabels(header);
    readFile();
    connect(_list, &QTreeWidget::currentItemChanged,
            this, &ListCreator::on_currentChanged);
    lay->addWidget(_list,2);

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
//    button->setDefault(true);
//    setTabOrder(edit,button);

    button = new QPushButton("&Cancel", this);
    connect(button, &QPushButton::clicked,
            this, &QDialog::reject);
    chLay->addWidget(button);

    lay->addLayout(chLay);

    resize(400,600);
}

ListCreator::~ListCreator()
{
    if (_clipboard)
        delete _clipboard;
    _clipboard = nullptr;
}

void ListCreator::initialiseArmy()
{
    _editor->setMultipleCompleter(new QCompleter(include(true),this));

    if (!_list->topLevelItemCount())
    {

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


    _info->setText("A list can be included with [] braces.\n"
                   "Start a name with reserved characters '!' to add control."
                   "\n"
                   "Control characters are then added after ! with no"
                   " whitespaces.\n"
                   "X denotes a number, usually replacable with:\n"
                   " A for every model,\n"
                   " M for every model except leader, \n"
                   " (X@Y) X for every Y models.\n"
                   "Items without ! and control characters are normal"
                   " selectable items.\n"
                   "'=X' limited to X selections. (e.g. named characters\n"
                   "':' item is not selectable (always selected).\n"
                   "'/' item replaces previous items.\n"
                   " Optional:\n"
                   " '<AAA>' replaces item AAA;\n"
                   " '<AAA,BBB,...>' replaces all items;\n"
                   " 'X' replace up to X.\n"
                   "'&X' add X items from a list.\n"
                   "'+X' item is selectable X times.\n"
                   "'%X' select up to X items from below.\n"
                   "'*X' item cost multiplied by X.\n\n"
                   "You can prefix an item with an amount number, and group "
                   "items with curly braces {}.\n"
                   "Putting items inside straight lines || means it will not "
                   "be shown."

                 );
}

void ListCreator::initialiseWargear()
{
    if (!_list->topLevelItemCount())
    {
        _editor->setCompleter(new QCompleter(include(),this));
        createPreMadeItem("Ranged Weapons");
        createPreMadeItem("Melee Weapons");
        createPreMadeItem("Other Wargear");
    }

    _info->setText("Begin a name with following for effect:\n"
                   "'!<AAA,BBB>' Cost for keyword or name AAA or BBB");
}

void ListCreator::initialiseList()
{
    _editor->setCompleter(new QCompleter(include(),this));
    _info->setText("Put list names inside [] brackets.\n"
                   "Begin a name with following for effect:"
                   "'!+<AAA,BBB>' item only available for keywords.\n"
                   "'!-<AAA,BBB>' item not available for keywords.\n");
}

QStringList ListCreator::include(bool print)
{
    QStringList files = QFileDialog::getOpenFileNames(
                this, "include files", QDir::currentPath(),
                "(*txt)");

    QTreeWidgetItem *itm = _list->topLevelItem(0);
    int oldindx = 0;
    if (print)
    {
        if (itm)
        {
            oldindx = itm->childCount();
            for (int i = oldindx; i > 0; --i)
                files.prepend(itm->child(i-1)->text(0));
        }
        else
            itm = createPreMadeItem("INCLUDES");
    }
    QFile fl;
    QTextStream str;
    QStringList ret;
    QString line;
    QRegExp rx ("!<(.*)>");
    QStringList caps;
    QDir cur;
    for (int i = 0; i < files.count(); ++i)
    {
        if (print && i >= oldindx)
            itm->addChild(new QTreeWidgetItem(QStringList(
                             cur.relativeFilePath(files.at(i)))));

        fl.setFileName(files.at(i));
        if (fl.open(QFile::Text | QFile::ReadOnly))
        {
            str.setDevice(&fl);
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
                                !ret.contains(wrd))
                            ret << wrd;
                    }
                }
                line.remove(rx);
                line.remove(QRegExp("!\\w+"));
                line = line.trimmed();

                if (line.count() > 3 && !line.toInt() &&
                        !ret.contains(line))
                    ret << line;

            }
            fl.close();
        }
    }
    return ret;
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
