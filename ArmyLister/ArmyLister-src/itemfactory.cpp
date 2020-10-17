#include "itemfactory.h"

#include "modelitemcat.h"
#include "itemsatellite.h"
#include "modelitemslot.h"
#include "modelitemselection.h"
#include "modelitemunit.h"
#include "settings.h"

#include <QFile>
#include <QTextStream>
#include <QRegExp>
#include <QDebug>
#include <QStringList>

ItemFactory::ItemFactory(Settings *set)
    : _settings(set)
    , _pointList(QList<PointContainer*>())
    , _listList(QMap<QString, QStringList>())
    , _globalLimiters(QMap<QString, ItemSatellite*>())
    , _nameMap(QMap<QString, int>())
{

}

ItemFactory::~ItemFactory()
{
    clear();
}

bool ItemFactory::addArmyFile(ModelItemBase* top, const QString &fileName)
{
    clear();
    QFile file(fileName);
    if (!file.open(QFile::Text | QFile::ReadOnly))
        return false;

    QTextStream str(&file);
    str.setCodec("utf-8");

    TempTreeModelItem *tempitem = new TempTreeModelItem();
    QString line = str.readLine();

    // Read through the list file. Each function reads lines until one starts
    // without an indent
    while (!line.isNull())
    {
        if (line.isEmpty())
            line = str.readLine();
        // First to create points tables from 8-9th ed. 40k codices,
        // separated into different files introduced before main list
        else if (line == "INCLUDES")
            line = parseIncludes(str);
        // ORGANISATION is used to map category names to a number for
        // units that count as other category in some cases
        else if (line == "ORGANISATION")
            line = mapOrganisation(str);
        else if (line.startsWith('['))
            line = parseList(str, line);
        else
            line = parseMainList(line,str,tempitem);
    }


    compileCategories(tempitem, top);

    foreach (ItemSatellite *lim, _globalLimiters.values())
        lim->setParent(top);

    return true;
}

QString ItemFactory::parseMainList(const QString &line, QTextStream &str,
                                      TempTreeModelItem *parentBranch,
                                      QStringList prev)
{
    // Main file is consructed with indented hierarchy
    int tabCount = line.count('\t');
    QString nLine = line.trimmed();

    // parseControl will list and remove control elements from the text
    QStringList ctrl = parseControl(nLine);

    // parseSpecial will list and remove special information from the text,
    // that is added to prev with the entrys name //NO, IF NEEDED, CREATE
    // SEPARATE SPECIAL ENTRY IN TEMP TREE FOR PRICE TABLE SEARCHING
    QStringList newspec = parseSpecial(nLine);
    QStringList splitText = nLine.split('|');
    QString text = splitText.at(0).trimmed();

    foreach (QString s, text.split(','))
    {
        s.remove(QRegExp("[\\d\\(\\)]"));
        s = "+"+s.trimmed();
        if (!newspec.contains(s))
            newspec << s;
    }
/*    if (!newspec.isEmpty())
        prev.append(newspec);
    prev << nLine.trimmed();*/

    TempTreeModelItem *item = new TempTreeModelItem(nLine.trimmed(),ctrl,
                                                    newspec, parentBranch);

    // Recursion control. EOF and empty lines will cease parsing
    if (str.atEnd())
        return QString();
    nLine = str.readLine();
    // As long as the new line is further indented, it will further the
    // recursion.
    while (!nLine.isEmpty() && nLine.count('\t') > tabCount)
    {
        nLine = parseMainList(nLine, str, item, prev);
    }
    return nLine;
}

QString ItemFactory::parseIncludes(QTextStream &str)
{
    QString line = str.readLine();
    while (!line.isNull() && line.startsWith('\t'))
    {
        parseFile(line.trimmed());
        line = str.readLine();
    }
    return line;
}

QString ItemFactory::mapOrganisation(QTextStream &str)
{
    QString line = str.readLine();
    QStringList entryLine;
    while (!line.isNull() && line.startsWith('\t'))
    {
        entryLine = line.split('#');
        entryLine.removeFirst();
        foreach (QString entry, entryLine)
        {
            entry = entry.trimmed();
            if (!entry.isEmpty())
            {
                entry = entry.split(';').at(0);
                _nameMap.insert(entry, _nameMap.count());
            }
        }

        line = str.readLine();
    }
    return line;
}

QStringList ItemFactory::parseControl(QString &text)
{
    QStringList ret;
    text = text.trimmed();
    int sep;
    int prevsep = 0;
    while (text.startsWith('!'))
    {
        // look for a whitespace, if its between <> it's part of a name,
        // otherwise it separates a word, and thus also control elements
        while (true)
        {
            sep = text.indexOf(' ', prevsep);
            // if no whitespaces are found, the entire item is a control item
            if (sep < 0)
            {
                sep = text.count();
                break;
            }

            // if there are no <>, or whitespace is found before one, the
            // whitespace is a word/element separator
            if (text.indexOf('<', prevsep) < 0 ||
                sep < text.indexOf('<', prevsep))
                break;

            // otherwise found whitespace is inside <>, so search starts again
            // after <> is closed
            prevsep = text.indexOf('>', prevsep)+1;
        }
        // after a whitespace that separates an element from another of from
        // the rest of the line is found, the word until it is stored
        // as control elements and is removed from text
        ret << text.left(sep).split('!', QString::SkipEmptyParts);

        text.remove(0,sep);
    }

    text = text.trimmed();

    return ret;
}

QStringList ItemFactory::parseSpecial(QString &text)
{
    QStringList ret;
    if (text.isEmpty())
        return ret;
    QRegExp specialReg("<(.*)>");
    specialReg.setMinimal(true);
    int pos = -1;

    while ((pos = specialReg.indexIn(text, pos+1)) > 0)
    {
        ret << specialReg.cap(1).trimmed();
    }

    text.remove(specialReg);
    return ret;
}

void ItemFactory::compileCategories(TempTreeModelItem *temproot,
                                    ModelItemBase *root)
{

    ModelItemCategory *knot;
    foreach (TempTreeModelItem *itm, temproot->_unders)
    {
        knot = new ModelItemCategory(_settings, root);
        root->addItem(knot);
        qDebug() << "Compiling category: " << itm->_text;
        knot->setText(itm->_text);

        foreach (TempTreeModelItem *itm2, itm->_unders)
            compileUnit(itm2, knot);
    }

}

void ItemFactory::compileUnit(TempTreeModelItem *tempknot,
                              ModelItemCategory *trunk)
{
    ModelItemUnit *knot = new ModelItemUnit(_settings, trunk);

    trunk->addItem(knot);

// !=X cloning limit local & global and other possible control elements

    QString text = tempknot->_text;
    QStringList splitText = text.split('|');
    int specCost = 0;


    // if Unit stats are in base list (9A)
    // Name|permodelcost|modelss(|basecost(if different than permodelcost))
    if (splitText.count()>1)
    {
        text = splitText.at(0).trimmed();

        knot->setText(text);


        QStringList modelsSplit = splitText.at(2).split('-');
        int modelmin = modelsSplit.at(0).trimmed().toInt();
        int modelmax = 0;
        if (modelsSplit.count() > 1)
            modelmax = modelsSplit.at(1).trimmed().toInt();
        knot->setModels(modelmin, modelmax);


        int cost = splitText.at(1).trimmed().toInt();

        if (splitText.count()>3)
            specCost = splitText.at(3).trimmed().toInt()
                    -cost*modelmin;

        knot->setUnitCost(cost, specCost);
    }
    // otherwise the units stats are recorded in tables
    else
    {
        knot->setText(text);

        PointContainer *pr = findTableEntry(text, tempknot->_spec);

        if (pr)
        {
            knot->setModels(pr->min, pr->max);

            if (pr->specialPoints)
                specCost = pr->specialPoints - pr->points*pr->min;

            knot->setUnitCost(pr->points, specCost);
        }
    }

    knot->setSpecial(tempknot->_spec);

    checkControls(tempknot, knot);


    QMap<QString, int> slotmap;
    foreach (TempTreeModelItem *itm2, tempknot->_unders)
        compileItems(itm2, knot, slotmap);

    knot->passSpecialUp(QStringList(),true);
}

void ItemFactory::compileItems(TempTreeModelItem *tempknot,
                               ModelItemBase *trunk,
                               const QMap<QString, int> &slotmap,
                               ItemSatellite *sharedSat)
{
    if (tempknot->_text.startsWith('['))
    {
        compileList(tempknot, trunk, slotmap, sharedSat);
        return;
    }

    QChar ctrlchar;

    int group = -1;

    foreach (QString ctrl, tempknot->_control)
    {
        ctrlchar = ctrl.at(0);

        if (ctrlchar == '\\')
        {
            compileSelection(tempknot, trunk);
            return;
        }
        else if (ctrlchar == '/')
        {
            ctrl.remove(ctrlchar);
            if (!slotmap.contains(ctrl))
            {
                qDebug() << "Faulty slot name in main list: " << ctrl;
                return;
            }
            group = slotmap.value(ctrl);
        }
    }

    ModelItemBasic *knot = new ModelItemBasic(_settings, trunk);

    trunk->addItem(knot, group);

    QString text = tempknot->_text;

    QStringList splitText = text.split('|');
    int cost;


    // if item stats are in base list (9A)
    // Name|cost
    if (splitText.count()>1)
    {
        text = splitText.at(0).trimmed();

        knot->setText(text);

        cost = splitText.at(1).trimmed().toInt();
        knot->setCost(cost);
    }
    // otherwise the units stats are recorded in tables
    else
    {
        knot->setText(text);

        int cost = countItems(text, tempknot->_spec);
        if (cost >= 0)
            knot->setCost(cost);
    }

    knot->setSpecial(tempknot->_spec);

    if (sharedSat)
        knot->connectToSatellite(sharedSat);

    sharedSat = checkControls(tempknot, knot);

    foreach (TempTreeModelItem *itm2, tempknot->_unders)
        compileItems(itm2, knot, slotmap, sharedSat);
}

void ItemFactory::compileList(TempTreeModelItem *tempknot,
                              ModelItemBase *trunk,
                              const QMap<QString, int> &slotmap,
                              ItemSatellite *sharedSat)
{
    QString text = tempknot->_text;

    QString name = text;
    QString xgroup = QString();
    QRegExp listname("(\\[.*\\])");
    QRegExp extragroup("(\\{.*\\})");

    if (listname.indexIn(text) >= 0)
    {
        name = listname.cap(1);
        if (extragroup.indexIn(text) >= 0)
        {
            xgroup = extragroup.cap(1);
        }
    }

    if (!_listList.contains(name))
    {
        qDebug() << "Unknown listname: " << text;
        return;
    }

    QChar ctrlchar;
    int group = -1;

    foreach (QString ctrl, tempknot->_control)
    {
        ctrlchar = ctrl.at(0);

        if (ctrlchar == '/')
        {
            ctrl.remove(1,0);
            if (ctrl.isEmpty())
                ctrl = "null";
            if (!slotmap.contains(ctrl))
            {
                qDebug() << "Faulty slot name in main list: " << ctrl;
                return;
            }
            group = slotmap.value(ctrl);

        }
    }

    ModelItemBasic *knot = new ModelItemBasic(_settings, trunk);

    trunk->addItem(knot, group);

    knot->setText(name.remove("[\\[\\]]"));

    QStringList list = _listList.value(name);
    QStringList specials = tempknot->_spec;
    QStringList controls;
    QRegExp specialEntry("!<(.+)>");
    specialEntry.setMinimal(true);
    QRegExp controlEntry("^!(.+)\\s");
    controlEntry.setMinimal(true);;
    QRegExp limitReg("!(.*)\\?#(\\d+)");
    int pos;
    QMap<QString, ItemSatellite*> limits;
    ItemSatellite *limiter;
    int tries = 1;

    foreach (QString listEntry, list)
    {
        if (listEntry.startsWith('['))
        {
            compileList(tempknot, trunk, slotmap, sharedSat);
            continue;
        }

        ModelItemBasic *branch = new ModelItemBasic(_settings, knot);

        knot->addItem(branch, group);

        pos = 0;
        specials.clear();
        while ((pos = specialEntry.indexIn(listEntry,pos)) >= 0)
        {
            specials << specialEntry.cap(1);
            pos += specialEntry.matchedLength();
        }
        listEntry.remove(specialEntry);

        if (controlEntry.indexIn(listEntry) >= 0)
        {
            controls = controlEntry.cap(1).split('!');
            listEntry.remove(controlEntry);
        }

        QStringList splitText = listEntry.split('|');
        int cost;

        // if item stats are in base list (9A)
        // Name|cost
        if (splitText.count()>1)
        {
            listEntry = splitText.at(0).trimmed();

            branch->setText(listEntry);

            cost = splitText.at(1).trimmed().toInt();
            branch->setCost(cost);
        }
        // otherwise the units stats are recorded in tables
        else
        {
            branch->setText(listEntry);

            cost = countItems(listEntry, specials);
            if (cost >= 0)
                branch->setCost(cost);
        }

        branch->setSpecial(specials);

        if (sharedSat)
            branch->connectToSatellite(sharedSat);

        foreach (QString ctrl, controls)
        {
            ctrlchar = ctrl.at(0);
            ctrl.remove(0,1);

            if (ctrlchar == '=')
            {
                if (!_globalLimiters.contains(listEntry))
                    qDebug() << "Name not in lists: " << listEntry;
                else
                    branch->connectToSatellite(_globalLimiters[listEntry]);
            }

            else if (ctrlchar == '{' || !xgroup.isNull())
            {
                ctrl.prepend(ctrlchar);
                tries = 1;
                if (!xgroup.isNull())
                {
                    ctrl.append(xgroup);
                    tries = 2;
                }
                while (tries)
                {
                    if (!_globalLimiters.contains(ctrl))
                    {
                        if (!_listList.contains("[!]"))
                        {
                            qDebug() << "Faulty list name in list: " << ctrl;
                            break;
                        }

                        foreach (QString group, _listList.value("[!]"))
                        {
                            if (limitReg.indexIn(group) >= 0 &&
                                    limitReg.cap(1) == ctrl)
                            {
                                if (limits.contains(ctrl))
                               branch->connectToSatellite(limits[ctrl]);
                                else
                                {
                                    limiter = new ItemSatelliteSelectionLimiter(
                                                limitReg.cap(2).toInt(),
                                                ModelItemBasic::CountLimit,
                                                knot);

                               branch->connectToSatellite(limiter, true);

                                    limits.insert(ctrl,limiter);
                                }
                                tries = 0;
                                break;
                            }
                        }

                    }
                    else
                    {
                        branch->connectToSatellite(_globalLimiters[ctrl]);
                        tries = 0;
                    }

                    if (tries)
                    {
                        ctrl.remove(xgroup);
                        --tries;
                    }
                }
            }
        }
    }
}

void ItemFactory::compileSelection(TempTreeModelItem *tempknot,
                                   ModelItemBase *trunk)
{
    ModelItemSelection *knot = new ModelItemSelection(_settings, trunk);

    trunk->addItem(knot);

    QStringList texts = tempknot->_text.split(")", QString::SkipEmptyParts);
    QString text, group;
    QMap<QString,int> groupMap;
    QStringList splitText;
    int cost;

    ModelItemBasic *branch;

    for (int i = 0; i < texts.count(); ++i)
    {
        text = texts.at(i);
        group = text.section("(",0,0);
        if (group.isEmpty())
            group = "null";

        groupMap.insert(group,i);
        text = text.section("(",1);

        splitText = text.split('|');

        branch = new ModelItemBasic(_settings, knot);


        // if item stats are in base list (9A)
        // Name|cost
        if (splitText.count()>1)
        {
            text = splitText.at(0).trimmed();

            branch->setText(text);

            cost = splitText.at(1).trimmed().toInt();
            branch->passCostUp(cost);
        }
        // otherwise the units stats are recorded in tables
        else
        {
            branch->setText(text);

            int i = countItems(text, tempknot->_spec);
            if (i >= 0)
                branch->passCostUp(i);
        }

        knot->addItem(branch, i);

        branch->setSpecial(tempknot->_spec);


    }

    checkControls(tempknot, knot);

    foreach (TempTreeModelItem *itm2, tempknot->_unders)
        compileItems(itm2, knot, groupMap);
}

void ItemFactory::parseFile(const QString &fileName)
{
    qDebug() << "parsingFile: " << fileName;
    QFile file(fileName);
    if (!file.open(QFile::Text | QFile::ReadOnly))
        return;
    QTextStream str(&file);
    str.setCodec("utf-8");

    QString line = str.readLine();

    while (!line.isNull())
    {
        // [AAA] denotes a category list entry
        if (line.startsWith('['))
            line = parseList(str, line);

        // Non-category list line without indent is only a heading.
        // Table entries are all indented
        else
        {
            if (line.startsWith('\t'))
                line = parseTableEntry(line);

            line = str.readLine();
        }
    }
}

ItemSatellite *ItemFactory::checkControls(TempTreeModelItem *tempknot,
                                                  ModelItemBasic *knot)
{
    QChar ctrlchar;
    ItemSatellite *ret = nullptr;
    short sign;

    foreach (QString ctrl, tempknot->_control)
    {
        sign = 1;
        ctrlchar = ctrl.at(0);
        ctrl.remove(0,1);

        if (ctrlchar == ';')
            knot->setCostLimit(ctrl.toInt());

        else if (ctrlchar == ':')
            knot->setAlwaysChecked();

        else if (ctrlchar == '#')
        {
            ret = new ItemSatelliteSelectionLimiter(ctrl.toInt(),
                                            ModelItemBase::CountLimit,
                                             knot);
            knot->limitedBy(ModelItemBase::SelectionLimit);
        }

        else if (ctrlchar == '€')
            knot->connectToSatellite(
                        new ItemSatelliteSelectionMirror(knot), true);

        else if (ctrlchar == '*')
            knot->setForAll();

        else if (ctrlchar == '^')
            knot->setModelLimiter(0,ctrl.toInt());

        else if (ctrlchar == '_')
            knot->setModelLimiter(ctrl.toInt(),0);

        else if (ctrlchar == '§')
        {
            knot->connectToSatellite(
                        new ItemSatelliteModelMirror(knot), true);
            knot->setModelLimiter(0,ctrl.toInt());
        }

        else if (ctrlchar == '{')
        {
            ctrl.prepend(ctrlchar);

            if (!_globalLimiters.contains(ctrl))
                qDebug() << "Faulty list name in main list: " << ctrl;
            else
                knot->connectToSatellite(_globalLimiters[ctrl]);
        }
        else if (ctrlchar == '~')
        {

            if (ctrl.startsWith('/'))
            {
                sign = -1;
                ctrl.remove(0,1);
            }

            ctrl.remove(QRegExp("[<>]"));

            if (!_nameMap.contains(ctrl))
                qDebug() << "Faulty role name in main list: " << ctrl;
            else
                knot->setCountsAs(sign* _nameMap.value(ctrl));

        }
        else if (ctrlchar == '¨')
        {
            ctrl.remove(QRegExp("[<>]"));

            if (!_nameMap.contains(ctrl))
                qDebug() << "Faulty role name in main list: " << ctrl;
            else
                knot->setUnitCountsAs(_nameMap.value(ctrl));

        }
        else if (ctrlchar == '=')
        {
            if (ctrl == "1")
                knot->limitedBy(ModelItemBase::NotClonable);
            else
                knot->connectToSatellite(
                            new ItemSatelliteSelectionLimiter(ctrl.toInt(),
                                                     ModelItemBase::NotClonable,
                                                      knot));
        }
    }
    return ret;
}

QString ItemFactory::parseList(QTextStream &str, QString line)
{

    QRegExp name("(\\[.*\\])");
    if (name.indexIn(line) < 0)
    {
        qDebug() << "Unviable list name";
        return QString();
    }
    QString listname = name.cap(1);

    // special entries common to all list members may be added after list name
    QString commonSpecial("");
    line = line.remove(name).trimmed();
    if (!line.isEmpty() && line.startsWith('!'))
        commonSpecial = line;

    QStringList list = _listList.value(listname);

    QRegExp groupLimitReg("!(\\{.*\\})\\?");
//    groupLimitReg.setMinimal(true);
    QRegExp limitReg("=(\\d+)");
    QRegExp specialEntry("!<(.+)>");
    specialEntry.setMinimal(true);
    QRegExp controlEntry("!.+\\s");
    controlEntry.setMinimal(true);
    QString entry;

    line = str.readLine();

    // read every indented line after listname, and put them through table
    // parser in case they contain price information
    while (!line.isNull() && line.startsWith('\t'))
    {
//        line = parseTableEntry(line).trimmed();
        line = line.trimmed();
        line = line.prepend(commonSpecial);

        // if the entry contains a global limit control entry, it must be
        // created. These will later be given to the top item, which will
        // take care of destroying them
        if (limitReg.indexIn(line) >= 0)
        {
            entry = "";
            if (groupLimitReg.indexIn(line) >= 0)
                entry = groupLimitReg.cap(1);
            if (entry.isEmpty())
            {
                entry = line.split('|').at(0).trimmed();
                entry.remove(specialEntry).remove(controlEntry);
            }

            if (_globalLimiters.contains(entry))
                qDebug() << "Multiple limit entries! " << line;
            else
                _globalLimiters.insert(entry,
                                       new ItemSatelliteSelectionLimiter(limitReg.cap(1)
                                                                 .toInt(),
                                                  ModelItemBasic::GlobalLimit));
        }

        list << line;
        line = str.readLine();
    }

    _listList.insert(listname, list);
    return line;
}

QString ItemFactory::parseTableEntry(const QString &line)
{

    QStringList splitLine = line.split("|");
    QRegExp specialEntry("!<(.+)>");
    specialEntry.setMinimal(true);

    QString name = splitLine.at(0);
    if (name.contains('!'))
        name = name.remove(specialEntry).trimmed();

    // if the entry doesn't include price information, nothing else needs
    // to be done
    if (splitLine.count() >= 1)
    {

        PointContainer *entry = new PointContainer();
        entry->text = name;
        QRegExp multiplier("^(\\d+)");
        if (multiplier.indexIn(name) > 0)
        {
            entry->multiplier = multiplier.cap(1).toInt();
            entry->text.remove(multiplier);
        }


        // look for special entries
        int pos = 0;
        while ((pos = specialEntry.indexIn(splitLine.at(0),pos)) >= 0)
        {
            entry->special << specialEntry.cap(1);
            pos += specialEntry.cap(1).count();
        }

        // we have confirmed that entry has at least a price
        entry->points = splitLine.at(1).toInt();

        // if entry is a unit, it will have a second point of information for
        // models
        if (splitLine.count() > 2)
        {
            if (splitLine.at(2).contains('-'))
            {
                QStringList splitEntry = splitLine.at(2).split('-');
                entry->min = splitEntry.at(0).toInt();
                entry->max = splitEntry.at(1).toInt();
            }
            else
                entry->min = splitLine.at(2).toInt();
        }

        // unit entries may also carry special cost aside from per model price
        if (splitLine.count() > 3)
            entry->specialPoints = splitLine.at(3).toInt();

        _pointList << entry;
    }
    return name;
}

int ItemFactory::countItems(QString text,
                             const QStringList &special)
{
    PointContainer *pr = nullptr;
    int points = 0;
    QStringList items = text.split(QRegExp("[&,]"));

    int costModifier;
    int foundModifier;
    int newModifier;
    QRegExp multiplier("^(\\d+)");
    QString tempText;

    for (int i = 0; i < items.count(); ++i)
    {
        costModifier = 1;
        text = items.at(i).trimmed();
        tempText = text;

        // search for the item in tables
        if ((pr = findTableEntry(tempText,special)))
        {
            if (multiplier.indexIn(text) > 0)
            {
                newModifier = multiplier.cap(1).toInt();

                // findTableEntry edits text to have the greatest multiplier
                // less than original found in tables if applicable
                if (multiplier.indexIn(tempText) >= 0)
                    foundModifier = multiplier.cap(1).toInt();
                else
                    foundModifier = 1;

                costModifier = int(floor(newModifier/foundModifier));


                // if found entry does not count for all items, insert the
                // item with remaining multiples back to list
                newModifier = newModifier - foundModifier * costModifier;
                if (newModifier > 0)
                {
                    text.replace(multiplier, QString::number(newModifier));
                    items << text;
                }
            }

            points += pr->points*costModifier;
        }
        // items may not have costs at all
    }

    return points;
}

PointContainer *ItemFactory::findTableEntry(QString &text,
                                            const QStringList &special)
{
    QRegExp multiplier("^(\\d+)");
    PointContainer *found = nullptr;
    PointContainer *candidate = nullptr;
    int foundMultiplier = 0;
    int originalMultiplier = 1;
    if (multiplier.indexIn(text) > 0)
        originalMultiplier = multiplier.cap(1).toInt();
    text.remove(multiplier);
    text = text.trimmed();

    bool ok = false;
    int ind = 0;

    // look through all entries
    foreach (PointContainer *pr, _pointList)
    {
        // if item names match..
        if (pr->text == text)
        {
            // check if the entry has special limitations
            if (pr->special.count())
            {
                ok = false;
                ind = 0;

                QStringList l;
                // go through all limitations, if multiple limitations need
                // matching, they are divided by a comma
                // Search stops when list ends, or special limits are met
                while (ind < pr->special.count() && !ok)
                {
                    l = pr->special.at(ind).split(",");
                    // for each multiple limitations (or just one)
                    foreach (QString s, l)
                    {
                        // compare with given specials
                        foreach (QString p, special)
                        {
                            // if specials match, we can continue on
                            if (p == s)
                            {
                                ok = true;
                                break;
                            }
                            else
                                ok = false;
                        }
                        // if no special matched for one multiple, no need to
                        // check others
                        if (!ok)
                            break;
                    }
                    ++ind;
                }
                // if we found one group of limitations that match with
                // special entries, we have a candidate
                if (ok)
                    candidate = pr;
            }
            // if no limitations, the entry is a candidate
            else
                candidate = pr;

            if (candidate)
            {
            // check if the candidates multiplier is same (best candidate)
            // or at least greater than previously founds
                if (candidate->multiplier == originalMultiplier)
                    return candidate;
                else if (candidate->multiplier < originalMultiplier &&
                         candidate->multiplier > foundMultiplier)
                {
                    foundMultiplier = candidate->multiplier;
                    found = candidate;
                }
                candidate = nullptr;
            }
        } // items names don't match
    }
    if (found)
    {
        text.prepend(" ");
        text.prepend(QString::number(foundMultiplier));
    }
    return found;
}

void ItemFactory::clear()
{
    foreach (PointContainer *p, _pointList)
        delete p;
    _listList.clear();
    _globalLimiters.clear();
    _nameMap.clear();
}

TempTreeModelItem::TempTreeModelItem(const QString &text,
                                     const QStringList &ctrl,
                                     const QStringList &spec,
                                     TempTreeModelItem *parent)
    : _text(text)
    , _control(ctrl)
    , _spec(spec)
    , _unders(QList<TempTreeModelItem*>())
{
    if (parent)
        parent->addChild(this);
}

void TempTreeModelItem::addChild(TempTreeModelItem *item)
{
    _unders << item;
}
