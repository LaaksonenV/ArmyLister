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

const QChar& ItemFactory::CCharacter(ControlCharacters c)
{
    if (c == eControl_NULL)
        return s_controlcharacters.last();
    return s_controlcharacters.at(int(c));
}

const ItemFactory::ControlCharacters ItemFactory::ControlC(const QChar &c)
{
    return ControlCharacters(s_controlcharacters.indexOf(c));
}

ItemFactory::ItemFactory()
    : pointList_(QList<PointContainer*>())
    , unitMap_(QMap<QString, UnitContainer*>())
    , listMap_(QMap<QString, QList<TempTreeModelItem*> >())
    , localLimitGroupMap_(QMap<QString, int>())
    , globalLimitMap_(QMap<QString, ItemSatellite*>())
    , nameMap_(QMap<QString, int>())
    , retinueMap_(QMap<QString, TempTreeModelItem*>())
{

}

ItemFactory::~ItemFactory()
{
    clear();
}

bool ItemFactory::addArmyFile(ModelItemBase* top, const QString &fileName,
                              bool newArmy, const QStringList &tags)
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
    QStringList suptags(tags);

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
            line = parseIncludes(str, tags);

        // ORGANISATION is used to map category names to a number for
        // units that count as other category in some cases (9th Age)
        else if (line == "ORGANISATION")
            line = parseOrganisation(str);

        // SUPPLEMENT denotes that current list adds content to other list.
        // Supplemented lists may be given a special tag, if needed (ie.
        // Spacemarines.txt <Blood Angels>).
        else if (line == "SUPPLEMENT")
        {
            line = str.readLine();
            suptags += parseTags(line);
            addArmyFile(top, line.trimmed(), false, suptags);
            line = str.readLine();
        }

/*        // Lists may be defined in main file, though it should be on their own
        else if (line.startsWith('['))
            line = parseList(str, line);
*/

        else
            line = parseMainList(line,str,tempitem, tags);
    }


    compileCategories(tempitem, top);

    foreach (ItemSatellite *lim, globalLimitMap_.values())
        lim->setParent(top);

    return true;
}

QString ItemFactory::parseMainList(const QString &line, QTextStream &str,
                                   TempTreeModelItem *parentBranch,
                                   const QStringList &supTags)
{
    // Main file is consructed with indented hierarchy
    int tabCount = line.count('\t');
    QString parsingLine = line.trimmed();

    bool retinue = false;

    parseGlobalControl(parsingLine, str, supTags, retinue);

    QStringList control = parseControl(parsingLine);
    QStringList tags = parseTags(parsingLine);
    QStringList splitText = parsingLine.split('|');
    QString text = splitText.at(0).trimmed();

    // pick up each separate listed item for the tag list
    foreach (QString s, text.split(','))
    {
        s.remove(QRegExp("\\w*" +
                         QRegExp::escape(CCharacter(eControl_SlotStart))));
        s.remove(QRegExp("[\\d" +
                         QRegExp::escape(CCharacter(eControl_SlotEnd)) +
                         "\\)]"));
        s = s.trimmed();
        if (!tags.contains(s))
            tags << s;
    }

    TempTreeModelItem *item = new TempTreeModelItem(parsingLine.trimmed()
                                                    ,control, tags,
                                                    parentBranch);

    if (retinue)
        retinueMap_.insert(text, item);

    parsingLine = str.readLine();

    while (!parsingLine.isEmpty() && parsingLine.count('\t') > tabCount)
    {
        parsingLine = parseMainList(parsingLine, str, item, supTags);
    }
    return parsingLine;
}

void ItemFactory::parseGlobalControl(QString &line,
                                        QTextStream &str,
                                        const QStringList &supTags,
                                        bool &retinue)
{
    QStringList control;

    ControlCharacters ctrlchar;
    QStringList tags;
    int tabCount;
    bool done = false;

    // keep parsing globally controlled items until done
    // done when stream is not read in loop ie. when item is not handled
    // elsewhere
    while (!done && !line.isEmpty())
    {
        done = true;
        retinue = false;
        tabCount = line.count('\t');
        control = parseControl(line, CCharacter(eControl_Global));
        foreach (QString ctrl, control)
        {
            ctrlchar = ControlC(ctrl.at(0));
            ctrl.remove(0,1);
            if (ctrlchar == eControl_Retinue)
            {
                retinue = true;
            }

            else if (ctrlchar == eControl_Limit)
            {
                tags = parseTags(ctrl);

                if (!checkTagLimiter(tags, supTags))
                {
                    // skip item and branches
                    line = str.readLine();
                    while (!line.isEmpty()
                           && line.count('\t') > tabCount)
                    {
                        line = str.readLine();
                    }
                    done = false;
                }
            }

            else if (ctrlchar == eControl_CreateList)
            {
                line = parseList(str, line, supTags);
                done = false;
            }

            else if (ctrlchar == eControl_CreateGroup)
            {
                line = parseGroup(str, ctrl);
                done = false;
            }

            else if (ctrlchar == eControl_CreateUnitValues)
            {
                line = parseTable(str, line);
                done = false;
            }
        }
    }
}

QString ItemFactory::parseIncludes(QTextStream &str,
                                   const QStringList &tags)
{
    QString line = str.readLine();
    while (!line.isNull() && line.startsWith('\t'))
    {
        parseFile(line.trimmed(), tags);
        line = str.readLine();
    }
    return line;
}

void ItemFactory::parseFile(const QString &fileName,
                            const QStringList &supTags)
{
    qDebug() << "parsingFile: " << fileName;
    QFile file(fileName);
    if (!file.open(QFile::Text | QFile::ReadOnly))
        return;
    QTextStream str(&file);
    str.setCodec("utf-8");

    QString line = str.readLine();
    bool b;

    parseGlobalControl(line, str, supTags, b);
}

QString ItemFactory::parseOrganisation(QTextStream &str)
{
    QString line = str.readLine();
    QStringList entryLine;
    while (!line.isEmpty() && line.startsWith('\t'))
    {
        entryLine = line.split(CCharacter(eControl_OrgMainSplit));
        entryLine.removeFirst();
        foreach (QString entry, entryLine)
        {
            entry = entry.trimmed();
            if (!entry.isEmpty())
            {
                entry = entry.split(CCharacter(eControl_OrgPointSplit)).at(0);
                nameMap_.insert(entry, nameMap_.count());
            }
        }

        line = str.readLine();
    }
    return line;
}

QStringList ItemFactory::parseControl(QString &text, QChar ctrl)
{
    QStringList ret;
    int pos = 0;
    int endPos = 0;

    QString part;

    if (ctrl.isNull())
        ctrl = CCharacter(eControl_Local);

    QRegExp contrl(QString("(?:^| |\\t)") + QRegExp::escape(ctrl) +
                   "(.+)(?=" + QRegExp::escape(ctrl) + "| |$)");
    contrl.setMinimal(true);
    QRegExp contrlEnd(QString("(.*)(?=") + QRegExp::escape(ctrl) +
                      "| |$)");
    contrlEnd.setMinimal(true);
    QRegExp delimStart(QString("[") +
                   CCharacter(eControl_TagStart) +
                   CCharacter(eControl_GroupStart) +
                   CCharacter(eControl_SlotStart) +
                   "]");
    QRegExp delimEnd(QString("[") +
                   CCharacter(eControl_TagEnd) +
                   CCharacter(eControl_GroupEnd) +
                   CCharacter(eControl_SlotEnd) +
                   "]");

    while ((pos = contrl.indexIn(text, pos)) >= 0)
    {
        endPos = pos+contrl.matchedLength();
        part = contrl.cap(1);
        if (text.contains(delimStart))
        {
            while (delimStart.lastIndexIn(text, endPos) >
                   delimEnd.lastIndexIn(text, endPos))
            {
                endPos = contrlEnd.indexIn(text, endPos+1) +
                        contrlEnd.matchedLength();
                part += (QString(" ") + contrlEnd.cap(1));
            }
        }
        ret << part;
        text.remove(pos, endPos);
    }

    text = text.trimmed();

    return ret;
}

QStringList ItemFactory::parseTags(QString &text)
{
    QStringList ret;
    if (text.isEmpty())
        return ret;

    QString tag = QRegExp::escape(CCharacter(eControl_TagStart)) +
            "(.*)" +
            QRegExp::escape(CCharacter(eControl_TagEnd));

    QRegExp controlReg("!\\w+"+tag);
    controlReg.setMinimal(true);
    QRegExp tagReg(tag);
    tagReg.setMinimal(true);
    int pos = 0, controlPos;

    // skip control element
    while ((controlPos = controlReg.indexIn(text, pos)) >= 0)
    {
        pos = controlPos + controlReg.matchedLength();
    }


    while ((pos = tagReg.indexIn(text, pos)) >= 0)
    {
        text.remove(pos, tagReg.matchedLength());
        ret << tagReg.cap(1).trimmed();
    }

    text.remove(tagReg);
    text = text.trimmed();
    return ret;
}

QString ItemFactory::parseList(QTextStream &str, QString line,
                               const QStringList &suptag)
{
    if (line.isEmpty())
    {
        line = str.readLine();
        while (!line.isEmpty() && line.startsWith('\t'))
            parseList(str, line, suptag);
        return line;
    }

    int tabCount = line.count('\t');

    TempTreeModelItem *item = nullptr;

    QStringList controls = parseControl(line);
    QStringList commonTags = parseTags(line);
    QStringList tags;

    QChar ctrlchar;

    QList<TempTreeModelItem*> &list = listMap_[line];

    if (list.isEmpty())
    {
        item = new TempTreeModelItem(line, controls, tags);
        list << item;
    }
    else if (!controls.isEmpty())
        item->control_ = controls;

    QRegExp limitReg(QRegExp::escape(CCharacter(eControl_GlobalLimit)) +
                     "(\\d+)");

    QString entry = CCharacter(eControl_ListStart) +
            line +
            CCharacter(eControl_ListEnd);

    if (controls.indexOf(limitReg) >= 0)
    {
        if (globalLimitMap_.contains(entry))
            qDebug() << "Multiple limit entries! " << line;
        else
            globalLimitMap_.insert(entry,
                     new ItemSatelliteSelectionLimiter(limitReg.cap(1).toInt(),
                                                ModelItemBasic::eGlobalLimit));
    }

    line = str.readLine();
    int itemTabCount;
    bool skip;

    // read every indented line after listname
    while (!line.isEmpty() && line.count('\t') > tabCount)
    {
        // Check for compiletime control elements
        controls = parseControl(line, CCharacter(eControl_Global));
        skip = false;
        foreach (QString ctrl, controls)
        {
            ctrlchar = ControlC(ctrl.at(0));
            ctrl.remove(0,1);

            if (ctrlchar == eControl_Limit &&
                    !checkTagLimiter(parseTags(ctrl), suptag))
            {
                skip = true;
                line = str.readLine();
                itemTabCount = line.count('\t');
                while (!line.isEmpty()
                       && line.count('\t') > itemTabCount)
                {
                    line = str.readLine();
                }
                break;
            }
        }

        if (!skip)
        {
            controls = parseControl(line);
            tags = parseTags(line) + commonTags;
            item = new TempTreeModelItem(line.trimmed(), controls, tags);
            entry = line.split('|').at(0).trimmed();

            if (controls.indexOf(limitReg) >= 0)
            {
                if (globalLimitMap_.contains(entry))
                    qDebug() << "Multiple limit entries! " << line;
                else
                    globalLimitMap_.insert(entry,
                     new ItemSatelliteSelectionLimiter(limitReg.cap(1).toInt(),
                                                ModelItemBasic::eGlobalLimit));
            }

            list << item;
            line = str.readLine();
        }
    }

    return line;
}

QString ItemFactory::parseGroup(QTextStream &str, QString line)
{
    QRegExp groupLimitReg("(" +
                          QRegExp::escape(CCharacter(eControl_GroupStart)) +
                          ".+" +
                          QRegExp::escape(CCharacter(eControl_GroupEnd)) +
                          ")" +
                          QRegExp::escape(CCharacter(eControl_Limit)) +
                          "([" +
                          CCharacter(eControl_GlobalLimit) +
                          CCharacter(eControl_LocalLimit) +
                          "])(\\d+)"
                          );

    if (line.isEmpty())
    {
        line = str.readLine();
        while (!line.isEmpty() && line.startsWith('\t'))
            parseGroup(str, line.trimmed());
        return line;
    }

    if (groupLimitReg.indexIn(line) < 0)
    {
        qDebug() << "Faulty group entry " + line;
        return line;
    }
    QString group = groupLimitReg.cap(1);

    if (ControlC(groupLimitReg.cap(2).at(0)) == eControl_GlobalLimit)
    {
        // if the entry contains a global limit control entry, it must be
        // created. These will later be given to the top item, which will
        // take care of destroying them
        if (globalLimitMap_.contains(group))
            qDebug() << "Multiple limit entries! " << line;
        else
            globalLimitMap_.insert(group,
                new ItemSatelliteSelectionLimiter(groupLimitReg.cap(3).toInt(),
                                                ModelItemBasic::eGlobalLimit));
    }
    else if (ControlC(groupLimitReg.cap(2).at(0)) == eControl_LocalLimit)
    {
        if (localLimitGroupMap_.contains(group))
            qDebug() << "Multiple group entries! " << line;
        else
            localLimitGroupMap_.insert(group, groupLimitReg.cap(3).toInt());
    }
    line = str.readLine();

    return line;
}


QString ItemFactory::parseTable(QTextStream &str, QString line)
{

    if (line.isEmpty())
    {
        line = str.readLine();
        while (!line.isEmpty() && line.startsWith('\t'))
            parseTable(str, line);
        return line;
    }

    int tabCount = line.count('\t');

    QStringList splitLine = line.split("|");
    QString name;

    if (splitLine.count() > 1)
    {
        UnitContainer *ucont = new UnitContainer();
        name = splitLine.at(0).trimmed();

        ucont->points_ = splitLine.at(1).toInt();

        if (splitLine.count() > 2)
        {
            if (splitLine.at(2).contains('-'))
            {
                QStringList splitEntry = splitLine.at(2).split('-');
                ucont->min_ = splitEntry.at(0).toInt();
                ucont->max_ = splitEntry.at(1).toInt();
            }
            else
                ucont->min_ = splitLine.at(2).toInt();
        }
        else
        {
            qDebug() << "Unit entry not a unit! " << line;
        }

        if (splitLine.count() > 3)
            ucont->specialPoints_ = splitLine.at(3).toInt();

        unitMap_.insert(name, ucont);

        ItemContainer item;

        line = str.readLine();

        while (!line.isEmpty() && line.count('\t') > tabCount)
        {
            splitLine = line.split("|");

            name = splitLine.at(0).trimmed();

            item = ItemContainer();
            item.points_ = splitLine.at(1).toInt();

            ucont->items_.insert(name, item);

            line = str.readLine();
        }
    }
    else
        qDebug() << "Table entry without data " + line;

    return line;
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

ModelItemBasic *ItemFactory::compileUnit(const TempTreeModelItem *tempknot,
                              ModelItemBase *trunk)
{
    ModelItemUnit *knot = new ModelItemUnit(trunk);

    trunk->addItem(knot);

    int models;

    const UnitContainer *ucont = checkCost(knot, tempknot->text_, models);

    knot->setTags(tempknot->tags_);

    QMap<QString, ItemSatellite *> localGroupLimiters;

    checkControls(tempknot, knot, localGroupLimiters);

    foreach (TempTreeModelItem *itm2, tempknot->unders_)
        compileItems(itm2, knot, ucont, localGroupLimiters);

    knot->passTagsUp(QStringList(),true);
    knot->passModelsDown(models, true);

    return knot;
}

void ItemFactory::compileItems(const TempTreeModelItem *tempknot,
                               ModelItemBase *trunk,
                               const UnitContainer *ucont,
                               QMap<QString, ItemSatellite *>
                               &localGroupLimiters,
                               ItemSatellite *takeLimiter)
{
    // lists are compiled separately
    if (tempknot->text_.contains(CCharacter(eControl_ListStart)))
    {
        compileList(tempknot, trunk, ucont, localGroupLimiters, takeLimiter);
        return;
    }

    ControlCharacters ctrlchar = eControl_NULL;
    bool spin = false;
    TempTreeModelItem *retinue = nullptr;

    QString text = tempknot->text_;

    foreach (QString ctrl, tempknot->control_)
    {
        ctrlchar = ControlC(ctrl.at(0));

        // selection roots are compiled differently
        if (ctrlchar == eControl_Selection)
        {
            compileSelection(tempknot, trunk, ucont, localGroupLimiters,
                             takeLimiter);
            return;
        }
        else if (ctrlchar == eControl_Slot)
        {
            qDebug() << "Slot item without Selection " << text;
            return;
        }
        // items selectable multiple times need spinners
        else if (ctrlchar == eControl_Multiple)
            spin = true;

        // retinue items are compiled as units, and edited as needed
        else if (ctrlchar == eControl_Retinue)
        {
            ctrl.remove(1,0);
            // retinue name may be omitted, if option text is the name, nothing
            // else
            if (ctrl.isEmpty())
                ctrl = text;
            else
            {
                ctrl.remove(CCharacter(eControl_TagStart));
                ctrl.remove(CCharacter(eControl_TagEnd));
            }

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

        trunk->addItem(knot);
    }

    int temp;

    checkCost(knot, text, temp, ucont);

    knot->setTags(tempknot->tags_);

    if (takeLimiter)
        knot->connectToSatellite(takeLimiter);

    takeLimiter = checkControls(tempknot, knot, localGroupLimiters);

    foreach (TempTreeModelItem *itm2, tempknot->unders_)
        compileItems(itm2, knot, ucont, localGroupLimiters, takeLimiter);
}

void ItemFactory::compileList(const TempTreeModelItem *tempknot,
                              ModelItemBase *trunk,
                              const UnitContainer *ucont,
                              QMap<QString, ItemSatellite *>
                              &localGroupLimiters,
                              ItemSatellite *takeLimiter)
{
    QString name = tempknot->text_;
    name.remove(CCharacter(eControl_ListStart));
    name.remove(CCharacter(eControl_ListEnd));

    if (!listMap_.contains(name))
    {
        qDebug() << "Unknown listname: " << tempknot->text_;
        return;
    }

    ControlCharacters ctrlchar = eControl_NULL;

    ModelItemBasic *knot = new ModelItemBasic(trunk);

    trunk->addItem(knot);

    knot->setText(name);

    ItemSatellite *globalLimiter = globalLimitMap_.value(tempknot->text_,
                                                         nullptr);

    QList<TempTreeModelItem*> list = listMap_.value(name);

    TempTreeModelItem *listItem = list.takeFirst();
    QString parameter = QString();
    QString parameterValue;
    QRegExp parameterReg("\\((\\w+)=?(\\w*)\\)");

    if (listItem->control_.indexOf(parameterReg) >= 0)
    {
        parameter = parameterReg.cap(1);
        parameterValue = parameterReg.cap(2);
    }
    if (tempknot->control_.indexOf(parameterReg) >= 0)
        parameterValue = parameterReg.cap(1);

    ItemSatellite *limiter;
    QRegExp tagReg(QRegExp::escape(CCharacter(eControl_TagStart)) +
                   "(.+)" +
                   QRegExp::escape(CCharacter(eControl_TagEnd)));
    tagReg.setMinimal(true);
    QStringList tagList;
    int pos;

    foreach (TempTreeModelItem *listEntry, list)
    {
        if (listEntry->text_.contains(CCharacter(eControl_ListStart)))
        {
            compileList(listEntry, knot, ucont, localGroupLimiters,
                        takeLimiter);
            continue;
        }

        ModelItemBasic *branch = new ModelItemBasic(knot);

        knot->addItem(branch);

        int temp;
        checkCost(branch, listEntry->text_, temp);

        branch->setTags(listEntry->tags_);

        if (takeLimiter)
            branch->connectToSatellite(takeLimiter);
        if (globalLimiter)
            branch->connectToSatellite(globalLimiter);

        foreach (QString ctrl, listEntry->control_)
        {
            ctrlchar = ControlC(ctrl.at(0));
            ctrl.remove(0,1);

            if (!parameterValue.isEmpty())
                ctrl.replace(parameter, parameterValue);

            if (ctrlchar == eControl_GlobalLimit)
            {
                // global limiters were created while parsing
                if (!globalLimitMap_.contains(listEntry->text_))
                    qDebug() << "Name not in lists: " << listEntry->text_;
                else
                    branch->connectToSatellite(
                                globalLimitMap_[listEntry->text_]);
            }

            else if (ctrlchar == eControl_Limit)
            {
                // check if limit is by tags
                if ((pos = tagReg.indexIn(ctrl)) >= 0)
                {
                    tagList.clear();
                    tagList << tagReg.cap(1);
                    pos += tagReg.matchedLength();
                    while ((pos = tagReg.indexIn(ctrl, pos)) >= 0)
                    {
                        tagList << tagReg.cap(1);
                        pos += tagReg.matchedLength();
                    }
                    branch->setLimitingTags(tagList);
                }

                // if not then it's by group
                else if (globalLimitMap_.contains(ctrl))
                    branch->connectToSatellite(globalLimitMap_[ctrl]);

                // local if not global
                else if (localLimitGroupMap_.contains(ctrl))
                {

                    if (localGroupLimiters.contains(ctrl))
                        branch->connectToSatellite(localGroupLimiters[ctrl]);
                    else
                    {
                        limiter = new ItemSatelliteSelectionLimiter(
                                    localLimitGroupMap_.value(ctrl),
                                    ModelItemBasic::eCountLimit,
                                    branch);

                        branch->connectToSatellite(limiter, true);

                        localGroupLimiters.insert(ctrl,limiter);
                    }
                }

                else
                     qDebug() << "Faulty limit " << listEntry->text_;
            }
        }
    }
}

void ItemFactory::compileSelection(const TempTreeModelItem *tempknot,
                                   ModelItemBase *trunk,
                                   const UnitContainer *ucont,
                                   QMap<QString, ItemSatellite *>
                                   &localGroupLimiters,
                                   ItemSatellite *takeLimiter)
{
    ModelItemSelection *knot = new ModelItemSelection(trunk);

    trunk->addItem(knot);

    // Different slots are of form "group(item) group2(item2) ..."
    // or if only one slot group and parantheses may be omitted
    QStringList texts = tempknot->text_.split(CCharacter(eControl_SlotEnd),
                                              QString::SkipEmptyParts);
    QString text;
    QStringList grouped;
    QMap<QString,int> groupMap;
    QStringList defaults;
    int temp;

    for (int i = 0; i < texts.count(); ++i)
    {
        text = texts.at(i);

        // text splitting removed slot ending character, now remove beginning
        grouped = text.split(CCharacter(eControl_SlotStart),
                             QString::SkipEmptyParts);
        // extract group name if applicable
        if (grouped.count() > 1)
        {
            // groups are mapped to growing integer for programmatical reasons
            groupMap.insert(grouped.takeFirst(), i);
        }            

        text = grouped.at(0);

        defaults << text;

        checkCost(knot, text, temp, ucont, i);
    }

    knot->setTags(tempknot->tags_);

    if (takeLimiter)
        knot->connectToSatellite(takeLimiter);

    takeLimiter = checkControls(tempknot, knot, localGroupLimiters);


    if (groupMap.isEmpty())
        // only one slot with no group defined, all children are replacing
        // items
        compileSlots(tempknot, knot, groupMap, defaults, ucont,
                     localGroupLimiters, takeLimiter);
    else
    {
        QRegExp slotItem(QRegExp::escape(CCharacter(eControl_Slot)) +
                         ".*");
        slotItem.setMinimal(true);

        foreach (TempTreeModelItem *itm2, tempknot->unders_)
        {
            // selections may contain other items than replacing
            // items
            if (itm2->control_.indexOf(slotItem) < 0)
                compileItems(itm2, knot, ucont, localGroupLimiters,
                             takeLimiter);
            else
                compileSlots(itm2, knot, groupMap, defaults, ucont,
                             localGroupLimiters, takeLimiter);
        }
    }
}

void ItemFactory::compileSlots(const TempTreeModelItem *tempknot,
                               ModelItemBase *trunk,
                               const QMap<QString, int> &slotmap,
                               const QStringList &defaults,
                               const UnitContainer *ucont,
                            QMap<QString, ItemSatellite *> &localGroupLimiters,
                               ItemSatellite *takeLimiter)
{
    QList<int> slotList;
    // empty slotmap means only one unnamed slot without treemodelitem, and
    // tempknot is still selection item
    if (slotmap.isEmpty())
        slotList << 0;
    else
    {
        ControlCharacters ctrlchar;

        foreach (QString ctrl, tempknot->control_)
        {
            ctrlchar = ControlC(ctrl.at(0));

            // compileSelection gives slotmap, which includes all available
            // slotnames mapped to an integer
            if (ctrlchar == eControl_Slot)
            {
                ctrl.remove(ctrlchar);
                if (ctrl.isEmpty())
                    ctrl = "null";
                foreach (QString c, ctrl.split(CCharacter(eControl_Slot)))
                {
                    if (!slotmap.contains(c))
                    {
                        qDebug() << "Faulty slot name in main list: " << c;
                        return;
                    }
                    slotList << slotmap.value(c);
                }
            }
        }
    }

    ModelItemSlot *knot = new ModelItemSlot(trunk, slotList);

    trunk->addItem(knot);

    QStringList texts;

    foreach (int i, slotList)
        texts << defaults.at(i);
    if (texts.count() > 1)
    {
        QString last = texts.takeLast();
        texts.last().append(" and " + last);
    }
    knot->setText(texts.join(", "));

    if (!slotmap.isEmpty())
    {
        knot->setTags(tempknot->tags_);

        if (takeLimiter)
            knot->connectToSatellite(takeLimiter);

        takeLimiter = checkControls(tempknot, knot, localGroupLimiters);
    }

    foreach (TempTreeModelItem *itm2, tempknot->unders_)
        compileItems(itm2, knot, ucont, localGroupLimiters, takeLimiter);
}

bool ItemFactory::checkTagLimiter(const QStringList &limitingTags,
                                  const QStringList &tags)
{
    if (limitingTags.count() < 1)
        return true;

    int i = 0;
    bool ok = false;

    // check every limiting tag if any one is ok
    while (i < limitingTags.count() && !ok)
    {
        ok = true;
        // split each tag by comma to check if any separate tags are
        // not ok
        foreach (QString tag, limitingTags.at(i).split(','))
        {
            if (tag.startsWith(CCharacter(eControl_NOT)))
            {
                ok = false;
                tag = tag.remove(0,1);
            }
            // if any separated tag fails, no need to check others
            if (!tags.contains(tag.trimmed()))
            {
                ok = !ok;
            }
            if (!ok)
                break;
        }

        ++i;
    }

    return ok;
}

ItemSatellite *ItemFactory::checkControls(const TempTreeModelItem *tempknot,
                                          ModelItemBasic *knot,
                                          QMap<QString, ItemSatellite *>
                                          &localGroupLimiters)
{
    ControlCharacters ctrlchar;
    ItemSatellite *ret = nullptr;
    ItemSatellite *limiter;

    short sign;
    QRegExp permodel("(\\d+)" +
                     QRegExp::escape(CCharacter(eControl_Model)) +
                     "(\\d+)");

    QRegExp tagReg(QRegExp::escape(CCharacter(eControl_TagStart)) +
                   "(.+)" +
                   QRegExp::escape(CCharacter(eControl_TagEnd)));
    tagReg.setMinimal(true);
    QRegExp groupReg("(" +
                     QRegExp::escape(CCharacter(eControl_GroupStart)) +
                     ".+" +
                     QRegExp::escape(CCharacter(eControl_GroupEnd)) +
                     ")");
    groupReg.setMinimal(true);
    QString group;
    QStringList tagList;
    int pos;

    foreach (QString ctrl, tempknot->control_)
    {
        sign = 1;
        ctrlchar = ControlC(ctrl.at(0));
        ctrl.remove(0,1);

        if (ctrlchar == eControl_Take)
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

        else if (ctrlchar == eControl_GlobalModelLimit)
        {
            knot->connectToSatellite(
                        new ItemSatelliteModelMirror(knot), true);
            knot->setModelLimiter(0,ctrl.toInt());
        }

        else if (ctrlchar == eControl_GlobalCheck)
            knot->connectToSatellite(
                        new ItemSatelliteSelectionMirror(knot), true);

        else if (ctrlchar == eControl_SelectPoints)
            knot->setCostLimit(ctrl.toInt());





        else if (ctrlchar == eControl_GlobalLimit)
        {
            if (ctrl == "1")
                knot->limitedBy(ModelItemBase::eNotClonable);
            else
                knot->connectToSatellite(new ItemSatelliteSelectionLimiter(
                                             ctrl.toInt(),
                                             ModelItemBase::eNotClonable,
                                             knot));
        }

        else if (ctrlchar == eControl_Limit)
        {
            // check if limit is by tags
            if ((pos = tagReg.indexIn(ctrl)) >= 0)
            {
                tagList.clear();
                tagList << tagReg.cap(1);
                pos += tagReg.matchedLength();
                while ((pos = tagReg.indexIn(ctrl, pos)) >= 0)
                {
                    tagList << tagReg.cap(1);
                    pos += tagReg.matchedLength();
                }
                knot->setLimitingTags(tagList);
            }

            // if not then it's by group
            else if ((pos = groupReg.indexIn(ctrl)) >= 0)
            {
                do
                {
                    pos += groupReg.matchedLength();
                    group = groupReg.cap(1);

                    if (globalLimitMap_.contains(group))
                        knot->connectToSatellite(globalLimitMap_[group]);

                    // local if not global
                    else if (localLimitGroupMap_.contains(group))
                    {

                        if (localGroupLimiters.contains(group))
                            knot->connectToSatellite(localGroupLimiters[group]);
                        else
                        {
                            limiter = new ItemSatelliteSelectionLimiter(
                                        localLimitGroupMap_.value(group),
                                        ModelItemBasic::eCountLimit,
                                        knot);

                            knot->connectToSatellite(limiter, true);

                            localGroupLimiters.insert(group,limiter);
                        }
                    }
                } while ((pos = groupReg.indexIn(ctrl, pos)) >= 0);
            }
            else
                 qDebug() << "Faulty limit " << tempknot->text_;

        }

        else if (ctrlchar == eControl_Multiple)
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

        else if (ctrlchar == eControl_ModelMax)
            knot->setModelLimiter(0,ctrl.toInt());

        else if (ctrlchar == eControl_UnitCat)
        {
            ctrl.remove(CCharacter(eControl_TagStart));
            ctrl.remove(CCharacter(eControl_TagEnd));

            sign = 1;
            if (ctrl.startsWith(CCharacter(eControl_NOT)))
            {
                sign = -1;
                ctrl.remove(0,1);
            }

            if (!nameMap_.contains(ctrl))
                qDebug() << "Faulty role name in main list: " << ctrl;
            else
                knot->setUnitCountsAs(sign* nameMap_.value(ctrl));

        }

        else if (ctrlchar == eControl_ItemCat)
        {
            ctrl.remove(CCharacter(eControl_TagStart));
            ctrl.remove(CCharacter(eControl_TagEnd));

            sign = 1;
            if (ctrl.startsWith(CCharacter(eControl_NOT)))
            {
                sign = -1;
                ctrl.remove(0,1);
            }

            if (!nameMap_.contains(ctrl))
                qDebug() << "Faulty role name in main list: " << ctrl;
            else
                knot->setCountsAs(sign* nameMap_.value(ctrl));

        }

        else if (ctrlchar == eControl_ForAll)
            knot->setForAll();

        else if (ctrlchar == eControl_AutoCheck)
            knot->setManualLock(true);

        else if (ctrlchar == eControl_Replace)
        {
            if (ctrl.isEmpty())
                knot->setModelOverride(1);
            else
                knot->setModelOverride(ctrl.toInt());
        }

        else if (ctrlchar == eControl_AlwaysCheck)
            knot->setAlwaysChecked();

        else if (ctrlchar == eControl_ModelMin)
            knot->setModelLimiter(ctrl.toInt(),0);

    }
    return ret;
}

const UnitContainer *ItemFactory::checkCost(ModelItemBasic *knot,
                                            const QString &text, int &models,
                                            const UnitContainer *ucont,
                                      int slot)
{
    QStringList splitText = text.split('|');

    int specCost = 0;

    // if Item stats are in base list (9A)
    // for items Name|cost
    if (splitText.count()>1)
    {
        knot->setText(splitText.at(0).trimmed(), slot);

        int cost = splitText.at(1).trimmed().toInt();

        // for units
        // Name|permodelcost|modelss(|basecost(if different than permodelcost))
        if (splitText.count()>2)
        {
            QStringList modelsSplit = splitText.at(2).split('-');
            models = modelsSplit.at(0).trimmed().toInt();
            int modelmax = -1;
            if (modelsSplit.count() > 1)
                modelmax = modelsSplit.at(1).trimmed().toInt();
            knot->setRange(models, modelmax);

            if (splitText.count()>3)
                specCost = splitText.at(3).trimmed().toInt()
                        -cost*models;

            knot->setMultiCost(cost, specCost);
        }
        else
            knot->setCost(cost, slot);

    }
    // otherwise the units stats are recorded in tables (40k)
    else
    {
        knot->setText(text, slot);

        // if unitcontainer is given, use that
        if (ucont)
        {
            int cost = countItems(text, ucont);
            if (cost >= 0)
                knot->setCost(cost, slot);

            return ucont;
        }
        // otherwise look for unitcontainer in table
        else if (unitMap_.contains(text))
        {
            ucont = unitMap_.value(text);

            int models = ucont->min_;
            knot->setRange(models, ucont->max_);

            if (ucont->specialPoints_)
                specCost = ucont->specialPoints_ - ucont->points_*models;

            knot->setMultiCost(ucont->points_, specCost);

            return ucont;
        }
    }
    return nullptr;
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
    QRegExp multiplier("^(\\d+) ");
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
    pointList_.clear();
    foreach (UnitContainer *p, unitMap_.values())
    {
        delete p;
        p = nullptr;
    }
    unitMap_.clear();
    foreach (QList<TempTreeModelItem*> l, listMap_.values())
        foreach (TempTreeModelItem* i, l)
            if (i)
            {
                delete i;
                i = nullptr;
            }
    listMap_.clear();
    localLimitGroupMap_.clear();
    globalLimitMap_.clear();
    nameMap_.clear();
    foreach (TempTreeModelItem* i, retinueMap_.values())
        if (i)
        {
            delete i;
            i = nullptr;
        }
    retinueMap_.clear();
}

TempTreeModelItem::TempTreeModelItem(const QString &text,
                                     const QStringList &ctrl,
                                     const QStringList &spec,
                                     TempTreeModelItem *parent)
    : text_(text)
    , control_(ctrl)
    , tags_(spec)
    , unders_(QList<TempTreeModelItem*>())
{
    if (parent)
        parent->addChild(this);
}

void TempTreeModelItem::addChild(TempTreeModelItem *item)
{
    unders_ << item;
}
