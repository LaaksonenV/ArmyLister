#include "itemfactory.h"

#include "modelitem.h"
#include "limitmihandler.h"
#include "settings.h"

#include <QFile>
#include <QTextStream>
#include <QRegExp>

ItemFactory::ItemFactory(TopModelItem* top, Settings *set)
    : _settings(set)
    , _topItem(top)
    , _pointList(QList<PointContainer*>())
    , _listList(QMap<QString, QStringList>())
{

}

ItemFactory::~ItemFactory()
{
    foreach (PointContainer *p, _pointList)
        delete p;
}

bool ItemFactory::addArmyFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::Text | QFile::ReadOnly))
        return false;

    QTextStream str(&file);
    TempTreeModelItem *tempitem = new TempTreeModelItem();
    QString line;

    line = str.readLine();
    while (!str.atEnd())
    {
        if (line.isEmpty())
        {
            line = str.readLine();
            continue;
        }
        else if (line == "INCLUDES")
            line = parseIncludes(str);
        else
            line = parseTextRec(line,str,tempitem);
    }

    ModelItem *newItem;
    foreach (TempTreeModelItem *itm, tempitem->_unders)
    {
        newItem = new ModelItem(_settings, _topItem);
        newItem->setText(itm->_text);

        foreach (TempTreeModelItem *itm2, itm->_unders)
            parseTree(itm2, newItem, 0);
    }

    return true;
}

QString ItemFactory::parseTextRec(const QString &line, QTextStream &str,
                                      TempTreeModelItem *current,
                                      QStringList prev)
{
    int tabCount = line.count('\t');
    QString nLine = line.trimmed();
    QStringList ctrl = parseControl(nLine);
    if (tabCount == 1)
    {
        bool found = false;
        foreach (QString s, ctrl)
            if (s.contains('='))
            {
                found = true;
                break;
            }
        if (!found)
            ctrl << "=-1";
    }
    QStringList newspec = parseSpecial(nLine);
    if (!newspec.isEmpty())
        prev.append(newspec);
    prev << nLine.trimmed();


    TempTreeModelItem *item = new TempTreeModelItem(nLine.trimmed(),ctrl,
                                                    prev, current);
    if (str.atEnd())
        return QString();
    nLine = str.readLine();
    while (!nLine.isEmpty() && nLine.count('\t') > tabCount)
    {
        nLine = parseTextRec(nLine, str, item, prev);
    }
    return nLine;
}

// Currently NA
QStringList ItemFactory::parseControl(QString &text)
{
    QStringList ret;
    if (text.startsWith('!'))
    {
        int prevsep = 0;
        int sep;
        while (true)
        {
            sep = text.indexOf(' ', prevsep);
            if (sep < 0)
            {
                sep = text.count();
                break;
            }
            if (text.indexOf('<', prevsep) < 0 ||
                sep < text.indexOf('<', prevsep))
                break;
            prevsep = text.indexOf('>', prevsep)+1;
        }
        ret = text.left(sep).split('!', QString::SkipEmptyParts);
        text.remove(0,sep);
    }
    return ret;
}

// Currently NA
QStringList ItemFactory::parseSpecial(QString &text)
{
    QStringList ret;
    if (text.isEmpty())
        return ret;
    QRegExp rx("<(.*)>");
    int pos = -1;
    while ((pos = rx.indexIn(text, pos+1)) > 0)
    {
        ret << rx.cap(1).trimmed();
    }
    text.remove(rx);
    return ret;
}

LimitMIhandler *ItemFactory::parseTree(TempTreeModelItem *branch,
                                     ModelItem *parent,
                                     int amount, LimitMIhandler *sharedLimit,
                                     ModelItem *itm)
{
//    QChar ctrlchar;
    bool setUpItem = true;

/*    foreach (QString ctrl, branch->_control)
    {
        ctrlchar = ctrl.at(0);

        if (ctrlchar == '/' || ctrlchar == '&')
        {
            sharedLimit = parseSelection(branch, parent, amount, sharedLimit);
            return sharedLimit;
        }
        if (ctrlchar == '#' || ctrlchar == '%')
            setUpItem = false;
    }*/

    ModelItem *newItem = itm;
    if (!newItem)
        newItem = new ModelItem(_settings, parent);

    int minMods = amount;

    if (setUpItem)
    {
        QString text = branch->_text;//.split(',');
        newItem->setText(text);

        PointContainer *pr = nullptr, *temppr;
        int points = 0;
        QStringList items;
        QList<int> modif; modif << 1;
        int effModif = 1;
        QRegExp xp("^(\\d+)");

QString s = text;//        foreach (QString s, text)

            if (modif.isEmpty())
                modif << 1;

            effModif = 1;

            s = s.remove('|').trimmed();
            if (xp.indexIn(s) >= 0)
            {
                effModif = modif.last() * xp.cap(1).toInt();
                s = s.remove(xp.cap(1));
                s = s.trimmed();
                if (s.startsWith('{'))
                {
                    modif << effModif;
                    s = s.remove(0,1);
                    s = s.trimmed();
                }
                if (s.endsWith('}'))
                {
                    modif.removeLast();
                    s.chop(1);
                    s = s.trimmed();
                }

            }
            items = s.split('&');
            points = 0;
            foreach (QString t, items)
            {
                t = t.trimmed();
                temppr = nullptr;
                if ((temppr = findPair(t,branch->_spec)))
                {
                    pr = temppr;
                    points += pr->points;
                }
            }
            if (pr)
            {
                newItem->setCost(points*effModif);
                if (pr->max)
                {
                    minMods = pr->min;
                    newItem->setModels(pr->min, pr->max);

                }
            }
    }

    QRegExp limitxp("\\((\\d+)@(\\d+)\\)");
/*    LimitMIhandler *limt;
    ModelItem *newitm2;

    foreach (QString ctrl, branch->_control)
    {
        ctrlchar = ctrl.at(0);
        ctrl.remove(0,1);

        if (ctrlchar == ':' || ctrlchar == '*')
        {
            if (ctrlchar == ':')
                newItem->setAlwaysActive();
            if (!ctrl.isEmpty())
            {
                if (ctrl.startsWith('A'))
                {
                    newItem->setModelIntake(true);
                    newItem->on_multiplierChange(amount-1);
                }
                else if (ctrl.startsWith('M'))
                {
                    newItem->setModelIntake(true);
                    newItem->setModifierIntake(true);
                    newItem->on_multiplierChange(amount-1);
                }
                else
                {
                    newItem->on_multiplierChange(ctrl.toInt()-1, true);
                }
            }
        }
        else if (ctrlchar == '=')
        {
            newItem->setClonable(ctrl.toInt());

        }
        else if (ctrlchar == '+')
        {
            if (limitxp.indexIn(ctrl) >= 0)
            {

                limt = createLimiter(parent, amount);
                limt->setLimits(limitxp.cap(1).toInt(),
                                limitxp.cap(2).toInt());
                limt->setAddSource(newItem);

            }
            else if (ctrl.startsWith('A') || ctrl.startsWith('M'))
            {
                limt = createLimiter(parent, amount);
                limt->setLimits(1, 1);
                if (ctrl.startsWith('M'))
                    limt->setModifiable(true);
                limt->setAddSource(newItem);

            }
            else
            {
                newItem->setLimit(ctrl.toInt());
            }
        }
        else if (ctrlchar == '#')
        {
            parseSelection(branch, newItem, amount);
        }
        else if (ctrlchar == '^')
        {
            newItem->setModifierGiver(1);
        }
        else if (ctrlchar == '%')
        {
            newItem->setText("Pick " + QString::number(ctrl.toInt()));
            newItem->setModifierGiver(-1);
            limt = new LimitMIhandler(this);
            limt->setPickSource(newItem);
            limt->setLimits(ctrl.toInt());
            foreach (TempTreeModelItem *i, branch->_unders)
            {
                newitm2 = new ModelItem(_settings, newItem);
                parseTree(i, newItem, minMods, nullptr, newitm2);
                limt->addPickTarget(newitm2);
            }
            return sharedLimit;
        }
    }*/

    foreach (TempTreeModelItem *i, branch->_unders)
        sharedLimit = parseTree(i, newItem, minMods, sharedLimit);
    return nullptr;
}

/*LimitMIhandler *ItemFactory::parseSelection(TempTreeModelItem *branch,
                                    ModelItem *parent, int amount,
                                    LimitMIhandler *sharedLimit)
{
    QStringList contrl;
    QChar ctrlchar;
    QString splitters = "/&#";
    bool NAs = false;
    foreach (QString ctrl, branch->_control)
    {
        ctrlchar = ctrl.at(0);
        if (splitters.contains(ctrlchar))
        {
            contrl = ctrl.split(ctrlchar, QString::SkipEmptyParts);
            if (ctrlchar == '&' || ctrlchar == '#')
                NAs = true;
            break;
        }
    }
    QStringList replaced;
    int amnt = amount;
    int modif = 1;
    int times = 0, divs = 0;
    bool limitmod = false;
    QRegExp xp("^(\\d+)");
    QRegExp limitxp("\\((\\d+)@(\\d+)\\)");

    bool limitFound = false;

    foreach (QString s, contrl)
    {
        if (s.startsWith('<'))
            replaced << s.mid(1,s.count()-2);

        else
        {
            if (limitxp.indexIn(s) >= 0)
            {
                if (!sharedLimit)
                    sharedLimit = createLimiter(parent, amount);
                times = limitxp.cap(1).toInt();
                divs = limitxp.cap(2).toInt();
            }
            else if (NAs)
            {
                modif = 1;
                if (xp.indexIn(s) >= 0)
                    modif = xp.cap(1).toInt();
                for (int i = 0; i < modif; ++i)
                {
                    parent->setText("( )");
                    parent->addPoint(0);
                }
                replaced << "( )";
            }
            else if (xp.indexIn(s) >= 0)
            {
                if (!sharedLimit)
                    sharedLimit = createLimiter(parent, amount);

                times = xp.cap(1).toInt(); divs = 0;

            }
            else if (s.startsWith('A') || s.startsWith('M'))
            {
                if (!sharedLimit)
                    sharedLimit = createLimiter(parent, amount);
                times = divs = 1;
                if (s.startsWith('M'))
                    limitmod = true;
            }
            else
            {
                amnt = s.toInt();
                if (amnt > amount)
                    amnt = amount;
            }
            limitFound = true;
            if(!NAs)
                parent->setLimitable();

        }
    }

    if (!limitFound)
    {
        if (!sharedLimit)
        {
        sharedLimit = createLimiter(parent);
        times = -1; divs = 0;
//        parent->setLimitable();

        }
    }


    if (!replaced.count())
        replaced << QString();


    if (!branch->_text.isEmpty())
    {
        QList<std::pair<QString,int> > list;
        PointContainer *pr, *temppr;
        QStringList text = branch->_text.split(',');
        QStringList texts;
        QStringList items;
        int points = 0;
        QString s, u;
        for(int i = 0; i < text.count(); ++i)
        {
            s = u = text.at(i).trimmed();
            modif = 1;
            if (xp.indexIn(u) >= 0)
            {
                modif = xp.cap(1).toInt();
                u = u.remove(xp.cap(1));
                u = u.trimmed();
            }

            if (u.startsWith('['))
            {
                foreach (QString t, _listList.value(u))
                    if (!text.contains(t))
                        text << t;
                continue;
            }

            items = u.split('&');
            points = 0;
            foreach (QString t, items)
            {
                t = t.remove('|').trimmed();
                temppr = nullptr;
                if ((temppr = findPair(t,branch->_spec)))
                {
                    pr = temppr;
                    points += pr->points;
                }
            }
            if (pr)
            {
                list << std::make_pair(s, points*modif);
                texts << items.at(0);
            }


        }
        int j;
        foreach (QString str, replaced)
        {
            j = parent->addSelection(str,list);
            if (sharedLimit)
                sharedLimit->addSelections(j, texts, times, divs, limitmod);
        }
    }
    foreach (TempTreeModelItem *it, branch->_unders)
    {
//        it->_control.append(branch->_control);
        sharedLimit = parseSelection(it, parent, amnt, sharedLimit);
    }
    return sharedLimit;
}*/

/*LimitMIhandler *ItemFactory::createLimiter(ModelItem *par,
                                              int models)
{
    LimitMIhandler *limt = new LimitMIhandler(this);
    limt->setFollowing(par);
    limt->on_modelChange(models, nullptr);
    return limt;
}*/

QString ItemFactory::parseIncludes(QTextStream &str)
{
    QString line = str.readLine();
    while (!str.atEnd() && line.startsWith('\t'))
    {
        parseFile(line.trimmed());
        line = str.readLine();
    }
    return line;
}

void ItemFactory::parseFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::Text | QFile::ReadOnly))
        return;
    QTextStream str(&file);
    QString line;
    QStringList splitLine;
    PointContainer text;
    QRegExp xp("!<(.+)>");
    int pos = 0;
    while (!str.atEnd())
    {
        line = str.readLine();
        while (line.startsWith('['))
            line = parseList(str, line);

        if (line.startsWith('\t'))
        {
            splitLine = line.split("|");
            text.special.clear();
            if (line.startsWith("\t\t"))
                text.special << text.text;
            pos = 0;
            while ((pos = xp.indexIn(splitLine.at(0),pos)) >= 0)
            {
                foreach (QString s, xp.cap(1).split(','))
                    text.special << s.trimmed();
                pos += xp.cap(0).count();
            }
            line = splitLine.at(0);
            text.text = line.remove(xp).trimmed();
            text.points = 0;
            text.min = 0;
            text.max = 0;
            if (splitLine.count() > 2)
            {
                if (splitLine.at(1) != "0")
                {
                text.points = splitLine.at(2).toInt();
                splitLine = splitLine.at(1).split('-');
                text.min = splitLine.at(0).toInt();
                if (splitLine.count() > 1)
                    text.max = splitLine.at(1).toInt();
                }
                else
                    text.points = splitLine.at(2).toInt();
            }
            else if (splitLine.count() > 1)
                text.points = splitLine.at(1).toInt();

            _pointList << new PointContainer(text);
        }
    }
}

QString ItemFactory::parseList(QTextStream &str, QString line)
{
    QString listname = line;
    QStringList list;
    line = str.readLine();
    while (line.startsWith('\t'))
    {
        list << line;
        if (str.atEnd())
            break;
        line = str.readLine();
    }
    _listList.insert(listname, list);
    return line;
}

PointContainer *ItemFactory::findPair(const QString &text,
                                        const QStringList &special)
{
    PointContainer *found = nullptr;
    foreach (PointContainer *pr, _pointList)
    {
        if (pr->text == text)
        {
            if (pr->special.count())
            {
                foreach (QString s, pr->special)
                    foreach (QString p, special)
                        if (p==s)
                            return pr;
            }
            else
                found = pr;
        }
    }
    return found;
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
