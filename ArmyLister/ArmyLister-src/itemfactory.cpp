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
#include <QMessageBox>

ItemFactory::ItemFactory()
    : pointList_(QList<PointContainer*>())
    , unitMap_(QMap<QString, UnitContainer>())
    , listMap_(QMap<QString, QStringList>())
    , globalLimiterMap_(QMap<QString, ItemSatellite*>())
    , nameMap_(QMap<QString, int>())
    , retinueMap_(QMap<QString, TempTreeModelItem*>)
{

}

ItemFactory::~ItemFactory()
{
    clear();
}

bool ItemFactory::addArmyFile(ModelItemBase* top, const QString &fileName,
                              bool newArmy, const QString &tag)
{
    if (newArmy)
        clear();

    QFile file(fileName);
    if (!file.open(QFile::Text | QFile::ReadOnly))
        return false;

    QTextStream str(&file);
    str.setCodec("utf-8");

    TempTreeModelItem *tempitem = new TempTreeModelItem();
    QString line = str.readLine();
    QStringList suptags;
    QString suptag = QString();

    // Read through the list file. Each function reads lines until one starts
    // without an indent
    while (!line.isNull())
    {
        if (line.isEmpty())
            line = str.readLine();

        // First to create lists and tables,
        // separated into different files introduced before main list.
        // The lists will contain common items selectable by multiple entries,
        // and their costs. If an item has different costs depending on entry,
        // these can be defined in a separate table.
        else if (line == "INCLUDES")
            line = parseIncludes(str);

        // ORGANISATION is used to map category names to a number for
        // units that count as other category in some cases (9th Age)
        else if (line == "ORGANISATION")
            line = mapOrganisation(str);

        // SUPPLEMENT denotes that current list adds content to other list.
        // Supplemented lists may be given a special tag, if needed (ie.
        // Spacemarines.txt <Blood Angels>)
        else if (line == "SUPPLEMENT")
        {
            line = str.readLine();
            suptags = parseSpecial(line);
            if (!suptags.isEmpty())
                suptag = suptags.first();
            addArmyFile(top, line.trimmed(), false, suptag);
            line = str.readLine();
        }

        // Lists may be defined in main file, though it should be on their own
        else if (line.startsWith('['))
            line = parseList(str, line);


        else
            line = parseMainList(line,str,tempitem, tag);
    }


    compileCategories(tempitem, top);

    foreach (ItemSatellite *lim, globalLimiterMap_.values())
        lim->setParent(top);

    return true;
}

QString ItemFactory::parseMainList(const QString &line, QTextStream &str,
                                   TempTreeModelItem *parentBranch,
                                   const QString &supTag)
{
    // Main file is consructed with indented hierarchy
    int tabCount = line.count('\t');
    QString nLine = line.trimmed();

    // Check for compiletime control elements
    QStringList tags = parseControl(nLine, '?');

    // Retinue units may be included in other unit entries as options, so are
    // saved also separately
    bool retinue = false;
    if (tags.contains("R"))
    {
        retinue = true;
        tags.removeAll("R");
    }
    // Check for supplement/faction specific entry. If specific and creating
    // a different list, don't bother with the entry.
    if (!supTag.isEmpty() && !tags.isEmpty() && !tags.contains(supTag))
    {
        // Recursion control. EOF and empty lines will cease parsing
        if (str.atEnd())
            return QString();
        nLine = str.readLine();
        // As long as the new line is further indented, it would further the
        // recursion.
        while (!nLine.isEmpty() && nLine.count('\t') > tabCount)
        {
            nLine = str.readLine();
        }
        return nLine;
    }

    QStringList ctrl = parseControl(nLine);
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

    TempTreeModelItem *item = new TempTreeModelItem(nLine.trimmed(),ctrl,
                                                    newspec, parentBranch);

    if (retinue)
        retinueMap_.insert(text, item);

    // Recursion control. EOF and empty lines will cease parsing
    if (str.atEnd())
        return QString();
    nLine = str.readLine();
    // As long as the new line is further indented, it will further the
    // recursion.
    while (!nLine.isEmpty() && nLine.count('\t') > tabCount)
    {
        nLine = parseMainList(nLine, str, item, supTag);
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
                nameMap_.insert(entry, nameMap_.count());
            }
        }

        line = str.readLine();
    }
    return line;
}

QStringList ItemFactory::parseControl(QString &text, const QChar &ctrl)
{
    QStringList ret;
    text = text.trimmed();
    int sep;

    QRegExp contrl("\\b" + ctrl);
    int prevsep = contrl.indexIn(text);

    while (prevsep >= 0)
    {
        // look for a whitespace, if its between <> it's part of a name,
        // otherwise it separates a word, and thus also ends control elements
        while (true)
        {
            sep = text.indexOf(' ', prevsep);
            // if no whitespaces are found, the entire item is a control item
            // This also ensures of breaking loop, as sep will eventually <=-1
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
        ret << text.left(sep).split(ctrl, QString::SkipEmptyParts);

        text.remove(0,sep);
        prevsep = contrl.indexIn(text);
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

void ItemFactory::compileCategories(const TempTreeModelItem *temproot,
                                    ModelItemBase *root)
{

    ModelItemCategory *knot;
    foreach (TempTreeModelItem *itm, temproot->unders_)
    {
        // Check if category already exists..
        knot = static_cast<ModelItemCategory*> (root->getItem(itm->text_));

        // ..if not, create
        if (!knot)
        {
            knot = new ModelItemCategory(root);
            root->addItem(knot);
            qDebug() << "Compiling category: " << itm->text_;
            knot->setText(itm->text_);
        }

        foreach (TempTreeModelItem *itm2, itm->unders_)
            compileUnit(itm2, knot);
    }

}

ModelItemBasic *ItemFactory::compileUnit(TempTreeModelItem *tempknot,
                              ModelItemBase *trunk)
{
    ModelItemUnit *knot = new ModelItemUnit(trunk);

    trunk->addItem(knot);

    UnitContainer *ucont = nullptr;

    checkCost(knot, tempknot->text_, ucont);

    knot->setSpecial(tempknot->spec_);

    checkControls(tempknot, knot);

    QMap<QString, int> slotmap;
    foreach (TempTreeModelItem *itm2, tempknot->unders_)
        compileItems(itm2, knot, slotmap, ucont);

    knot->passSpecialUp(QStringList(),true);
    knot->passModelsDown(modelmin, true);

    return knot;
}

void ItemFactory::compileItems(const TempTreeModelItem *tempknot,
                               ModelItemBase *trunk,
                               const QMap<QString, int> &slotmap,
                               const UnitContainer *ucont,
                               ItemSatellite *sharedSat)
{
    // lists are compiled separately
    if (tempknot->text_.startsWith('['))
    {
        compileList(tempknot, trunk, slotmap, ucont, sharedSat);
        return;
    }

    QChar ctrlchar;
    bool spin = false;
    TempTreeModelItem *retinue = nullptr;
    int group = -1;

    QString text = tempknot->text_;

    foreach (QString ctrl, tempknot->control_)
    {
        ctrlchar = ctrl.at(0);

        // selection roots are compiled differently
        if (ctrlchar == '\\')
        {
            compileSelection(tempknot, trunk, ucont);
            return;
        }
        // compileSelection gives slotmap, which includes all available
        // slotnames mapped to an integer
        else if (ctrlchar == '/')
        {
            ctrl.remove(ctrlchar);
            if (ctrl.isEmpty())
                ctrl = "null";
            if (!slotmap.contains(ctrl))
            {
                qDebug() << "Faulty slot name in main list: " << ctrl;
                return;
            }
            group = slotmap.value(ctrl);
        }
        // items selectable multiple times need spinners
        else if (ctrlchar == '$')
            spin = true;

        // retinue items are compiled as units, and edited as needed
        else if (ctrlchar == 'R')
        {
            ctrl.remove(ctrlchar);
            // retinue name may be omitted, if option text is the name, nothing
            // else
            if (ctrl.isEmpty())
                ctrl = text;
            else
                ctrl.remove(QRegExp("[<>]"));

            retinue = retinueMap_.value(ctrl);
            if (!retinue)
            {
                qDebug() << "Faulty retinue name in main list: " << ctrl;
                return;
            }
        }
    }

    ModelItemBasic *knot;

    if (retinue)
        knot = compileUnit(retinue, trunk);
    else
    {
        if (spin)
            knot = new ModelItemSpinner(trunk);
        else
            knot = new ModelItemBasic(trunk);

        trunk->addItem(knot, group);
    }

    checkCost(knot, text, ucont);

    knot->setSpecial(tempknot->spec_);

    if (sharedSat)
        knot->connectToSatellite(sharedSat);

    sharedSat = checkControls(tempknot, knot);

    foreach (TempTreeModelItem *itm2, tempknot->unders_)
        compileItems(itm2, knot, slotmap, ucont, sharedSat);
}

void ItemFactory::compileList(const TempTreeModelItem *tempknot,
                              ModelItemBase *trunk,
                              const QMap<QString, int> &slotmap,
                              const UnitContainer *ucont,
                              ItemSatellite *sharedSat)
{
    QString text = tempknot->text_;

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

    if (!listMap_.contains(name))
    {
        qDebug() << "Unknown listname: " << text;
        return;
    }

    QChar ctrlchar;
    int group = -1;

    foreach (QString ctrl, tempknot->control_)
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

    ModelItemBasic *knot = new ModelItemBasic(trunk);

    trunk->addItem(knot, group);

    knot->setText(name.remove("[\\[\\]]"));

    QStringList list = listMap_.value(name);
    QStringList specials = tempknot->spec_;
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
            compileList(tempknot, trunk, slotmap, ucont, sharedSat);
            continue;
        }

        ModelItemBasic *branch = new ModelItemBasic(knot);

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

        checkCost(branch, listEntry);

        branch->setSpecial(specials);

        if (sharedSat)
            branch->connectToSatellite(sharedSat);

        foreach (QString ctrl, controls)
        {
            ctrlchar = ctrl.at(0);
            ctrl.remove(0,1);

            if (ctrlchar == '=')
            {
                if (!globalLimiterMap_.contains(listEntry))
                    qDebug() << "Name not in lists: " << listEntry;
                else
                    branch->connectToSatellite(globalLimiterMap_[listEntry]);
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
                    if (!globalLimiterMap_.contains(ctrl))
                    {
                        if (!listMap_.contains("[!]"))
                        {
                            qDebug() << "Faulty list name in list: " << ctrl;
                            break;
                        }

                        foreach (QString group, listMap_.value("[!]"))
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
                                                ModelItemBasic::eCountLimit,
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
                        branch->connectToSatellite(globalLimiterMap_[ctrl]);
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

void ItemFactory::compileSelection(const TempTreeModelItem *tempknot,
                                   ModelItemBase *trunk,
                                   const UnitContainer *ucont)
{
    ModelItemSelection *knot = new ModelItemSelection(trunk);

    trunk->addItem(knot);

    QStringList texts = tempknot->text_.split(")", QString::SkipEmptyParts);
    QString text, group;
    QMap<QString,int> groupMap;
    QStringList splitText;
    int cost;

    ModelItemBasic *branch;

    for (int i = 0; i < texts.count(); ++i)
    {
        text = texts.at(i);
        group = text.section("(",0,0);
        if (!text.contains('(') || group.isEmpty())
        {
            if (texts.count() == 1)
                group = "null";
            else
            {
                QMessageBox::warning(nullptr, "Erroneous selection",
                                     "Omiting a slot name "
                                     "or parantheses with multiple slots "
                                     "is not currently allowed. Skipping " +
                                     tempknot->text_);
                return;
            }
        }

        groupMap.insert(group,i);
        if (text.contains('('))
            text = text.section("(",1);

        splitText = text.split('|');

        branch = new ModelItemBasic(knot);


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

            int i = countItems(text, ucont);
            if (i >= 0)
                branch->passCostUp(i);
        }

        knot->addItem(branch, i);

        branch->setSpecial(tempknot->spec_);

    }

    checkControls(tempknot, knot);

    foreach (TempTreeModelItem *itm2, tempknot->unders_)
        compileItems(itm2, knot, groupMap, ucont);
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

        else
            line = parseTable(str, line);
    }
}

ItemSatellite *ItemFactory::checkControls(const TempTreeModelItem *tempknot,
                                                  ModelItemBasic *knot)
{
    QChar ctrlchar;
    ItemSatellite *ret = nullptr;
    short sign;
    QRegExp permodel("(\\d+)@(\\d+)");

    foreach (QString ctrl, tempknot->control_)
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
            if (permodel.indexIn(ctrl) >= 0)
                ret = new ItemSatelliteSelectionLimiterModels(
                            permodel.cap(1).toInt(), permodel.cap(2).toInt(),
                            ModelItemBase::eModelsLimit,
                            ModelItemBase::eCriticalLimit, knot);
            else
                ret = new ItemSatelliteSelectionLimiter(ctrl.toInt(),
                                            ModelItemBase::eCountLimit,
                                             knot);
            knot->limitedBy(ModelItemBase::eSelectionLimit);
        }

        else if (ctrlchar == '€')
            knot->connectToSatellite(
                        new ItemSatelliteSelectionMirror(knot), true);

        else if (ctrlchar == '$')
        {
            if (permodel.indexIn(ctrl) >= 0)
            {
                knot->setRange(0,0);
                knot->connectToSatellite(
                            new ItemSatelliteSelectionLimiterModels(
                                permodel.cap(1).toInt(),
                                permodel.cap(2).toInt(),
                                ModelItemBase::eModelsLimit,
                                ModelItemBase::eCriticalLimit, knot), true);
            }
            else
                knot->setRange(0,ctrl.toInt());
        }

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

            if (!globalLimiterMap_.contains(ctrl))
                qDebug() << "Faulty list name in main list: " << ctrl;
            else
                knot->connectToSatellite(globalLimiterMap_[ctrl]);
        }
        else if (ctrlchar == '~')
        {

            if (ctrl.startsWith('/'))
            {
                sign = -1;
                ctrl.remove(0,1);
            }

            ctrl.remove(QRegExp("[<>]"));

            if (!nameMap_.contains(ctrl))
                qDebug() << "Faulty role name in main list: " << ctrl;
            else
                knot->setCountsAs(sign* nameMap_.value(ctrl));

        }
        else if (ctrlchar == '¨')
        {
            ctrl.remove(QRegExp("[<>]"));

            if (!nameMap_.contains(ctrl))
                qDebug() << "Faulty role name in main list: " << ctrl;
            else
                knot->setUnitCountsAs(nameMap_.value(ctrl));

        }
        else if (ctrlchar == '=')
        {
            if (ctrl == "1")
                knot->limitedBy(ModelItemBase::eNotClonable);
            else
                knot->connectToSatellite(
                            new ItemSatelliteSelectionLimiter(ctrl.toInt(),
                                                     ModelItemBase::eNotClonable,
                                                      knot));
        }
    }
    return ret;
}

void ItemFactory::checkCost(ModelItemBasic *knot,
                                            const QString &text,
                                            UnitContainer *&ucont)
{
    QStringList splitText = text.split('|');

    int specCost = 0;
    int modelmin = 0;

    // if Item stats are in base list (9A)
    // for items Name|cost
    if (splitText.count()>1)
    {
        text = splitText.at(0).trimmed();

        knot->setText(text);

        int cost = splitText.at(1).trimmed().toInt();

        // for units
        // Name|permodelcost|modelss(|basecost(if different than permodelcost))
        if (splitText.count()>2)
        {
            QStringList modelsSplit = splitText.at(2).split('-');
            modelmin = modelsSplit.at(0).trimmed().toInt();
            int modelmax = -1;
            if (modelsSplit.count() > 1)
                modelmax = modelsSplit.at(1).trimmed().toInt();
            knot->setRange(modelmin, modelmax);

            if (splitText.count()>3)
                specCost = splitText.at(3).trimmed().toInt()
                        -cost*modelmin;

            knot->setMultiCost(cost, specCost);
        }
        else
            knot->setCost(cost);

        return;

    }
    // otherwise the units stats are recorded in tables (40k)
    else
    {
        knot->setText(text);

        // if unitcontainer is given, use that
        if (ucont)
        {
            cost = countItems(text, ucont);
            if (cost >= 0)
                knot->setCost(cost);

            return;
        }
        // otherwise look for unitcontainer in table
        else if (unitMap_.contains(text))
        {
            ucont = unitMap_.value(text);

            modelmin = ucontret.min;
            knot->setRange(ucont->min_, ucont->max_);

            if (ucont->specialPoints_)
                specCost = ucont->specialPoints_ - ucont->points_*ucont->min_;

            knot->setMultiCost(ucont->points_, specCost);
        }
    }
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

    QStringList list = listMap_.value(listname);

    QRegExp groupLimitReg("!(\\{.*\\})\\?");
//    groupLimitReg.setMinimal(true);
    QRegExp limitReg("=(\\d+)");
    QRegExp specialEntry("!<(.+)>");
    specialEntry.setMinimal(true);
    QRegExp controlEntry("!.+\\s");
    controlEntry.setMinimal(true);
    QString entry;

    line = str.readLine();

    // read every indented line after listname
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

            if (globalLimiterMap_.contains(entry))
                qDebug() << "Multiple limit entries! " << line;
            else
                globalLimiterMap_.insert(entry,
                               new ItemSatelliteSelectionLimiter(limitReg.cap(1)
                                                                 .toInt(),
                                                  ModelItemBasic::eGlobalLimit));
        }

        list << line;
        line = str.readLine();
    }

    listMap_.insert(listname, list);
    return line;
}

QString ItemFactory::parseTable(QTextStream &str, QString line)
{
    // Unit entries have data after their name, otherwise the line is a heading
    // and may be ignored
    QStringList splitLine = line.split("|");
    QString name;

    if (splitLine.count() >= 1)
    {
        UnitContainer ucont = UnitContainer();
        name = splitLine.at(0).trimmed();

        ucont.points_ = splitLine.at(1).toInt();

        if (splitLine.count() > 2)
        {
            if (splitLine.at(2).contains('-'))
            {
                QStringList splitEntry = splitLine.at(2).split('-');
                ucont.min_ = splitEntry.at(0).toInt();
                ucont.max_ = splitEntry.at(1).toInt();
            }
            else
                ucont.min_ = splitLine.at(2).toInt();
        }
        else
        {
            qDebug() << "Unit entry not a unit! " << line;
        }

        // unit entries may also carry special cost aside from per model
        // price
        if (splitLine.count() > 3)
            ucont.specialPoints_ = splitLine.at(3).toInt();

        unitMap_.insert(name, ucont);

        line = str.readLine();

        ItemContainer item;

        while (!line.isNull() && line.startsWith('\t'))
        {
            splitLine = line.split("|");

            name = splitLine.at(0).trimmed();

            item = ItemContainer();
            item.points_ = splitLine.at(1).toInt();

            ucont.items_.insert(name, item);

            line = str.readLine();
        }
    }
/*    // Entries that are not unit may or may not contain different information?
    // Propably unnecessarily complicated, SM need only for 'Chapter Command',
    // May be cut down in future.
    else
    {

        QRegExp specialEntry("!<(.+)>");
        specialEntry.setMinimal(true);

        QRegExp multiplier("^(\\d+)");
        QStringList splitEntry;

        PointContainer *entry;

        line = str.readLine();

        while (!line.isNull() && line.startsWith('\t'))
        {

            splitLine = line.split("|");

            name = splitLine.at(0);
            if (name.contains('!'))
                name = name.remove(specialEntry).trimmed();

            // if the entry doesn't include price information, nothing else needs
            // to be done
            if (splitLine.count() >= 1)
            {

                entry = new PointContainer();
                entry->text = name;

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

                // if entry is a unit, it will have a second point of
                // information for models
                if (splitLine.count() > 2)
                {
                    if (splitLine.at(2).contains('-'))
                    {
                        splitEntry = splitLine.at(2).split('-');
                        entry->min = splitEntry.at(0).toInt();
                        entry->max = splitEntry.at(1).toInt();
                    }
                    else
                        entry->min = splitLine.at(2).toInt();
                }

                // unit entries may also carry special cost aside from per model
                // price
                if (splitLine.count() > 3)
                    entry->specialPoints = splitLine.at(3).toInt();

                _pointList << entry;
            }

            line = str.readLine();
        }

    }*/
    return line;
}

int ItemFactory::countItems(QString text,
                            const UnitContainer *ucont)
{
  //  PointContainer *pr = nullptr;
    int points = 0;
    QStringList items = text.split(QRegExp("[&,]"));

    int costModifier;
 //   int foundModifier;
    int newModifier;
    QRegExp multiplier("^(\\d+)");
    QString tempText;

    for (int i = 0; i < items.count(); ++i)
    {
        costModifier = 1;
        text = items.at(i).trimmed();
        tempText = text;

        // check if item has multiplier, and make a text entry without it
        if (multiplier.indexIn(text) > 0)
        {
            newModifier = multiplier.cap(1).toInt();
            tempText.remove(multiplier);
        }

        // search for the item in units table, first if one with multiplier
        // exists
        if (ucont->items_.contains(text))
        {
            points += ucont->items_.value(text).points_;
        }
        else if (ucont->items_.contains(tempText))
        {
            points += ucont->items_.value(text).points_ * newModifier;
        }

    }

    return points;
}

void ItemFactory::clear()
{
    foreach (PointContainer *p, pointList_)
    {
        delete p;
        p = nullptr;
    }
    unitMap_.clear();
    listMap_.clear();
    globalLimiterMap_.clear();
    nameMap_.clear();
}

TempTreeModelItem::TempTreeModelItem(const QString &text,
                                     const QStringList &ctrl,
                                     const QStringList &spec,
                                     TempTreeModelItem *parent)
    : text_(text)
    , control_(ctrl)
    , spec_(spec)
    , unders_(QList<TempTreeModelItem*>())
{
    if (parent)
        parent->addChild(this);
}

void TempTreeModelItem::addChild(TempTreeModelItem *item)
{
    unders_ << item;
}
