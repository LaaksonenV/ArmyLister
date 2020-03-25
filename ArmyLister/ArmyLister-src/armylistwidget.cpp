#include "armylistwidget.h"
#include "modelitem.h"
#include "limitmihandler.h"
#include "settings.h"
#include "itemfactory.h"

#include <QFile>
#include <QTextStream>
#include <QRegExp>
#include <QtPrintSupport/QPrinter>
#include <QFileInfo>

ArmyListWidget::ArmyListWidget(Settings *set, QWidget *parent)
    : QScrollArea(parent)
    , _settings(set)
    , _topItem(new TopModelItem(set, this))
    , _name(QString())
    , _points(0)
{
    setWidget(_topItem);
    setWidgetResizable(true);
    setMinimumWidth(500);
    setSizePolicy(QSizePolicy::Expanding,
                  QSizePolicy::MinimumExpanding);

    connect(_topItem, &TopModelItem::itemChecked,
            [this](int i, int b){emit roleSelected(i,b);});
    connect(_topItem, &TopModelItem::valueChanged,
            this, &ArmyListWidget::on_valueChange);

}

ArmyListWidget::~ArmyListWidget()
{
//    foreach (PointContainer *p, _pointList)
  //      delete p;
}

bool ArmyListWidget::addArmyFile(const QString &fileName)
{
    ItemFactory fctr(_topItem, _settings);
    return fctr.addArmyFile(fileName);
}

void ArmyListWidget::printList() const
{
    if (!_topItem)
        return;
    QString filename = _name;
    filename.append("-");
    filename.append(QString::number(_points));
    filename.append(".txt");
    QFile file(filename);
    if (!file.open(QFile::WriteOnly | QFile::Text))
        return;
    QTextStream str(&file);

    QFont f("Consolas",11);


    int count = _name.size()+5;
    str.setFieldWidth((printwidth-count)/2);
    str << _name;
    str.setFieldWidth(0);
    str << " " << _points;
 //   endl(str);

    foreach (ModelItem *i, _topItem->_belows)
    {
        printRecurseText(str, i, 0, f);
    }
    file.close();
}

void ArmyListWidget::saveList() const
{
    QString filename = _name;
    filename.append("-");
    filename.append(QString::number(_points));
    filename.append(".alst");
    saveListAs(filename);
}

void ArmyListWidget::saveListAs(QString filename) const
{
    if (!_topItem)
        return;

    QFile file(filename);
    if (!file.open(QFile::WriteOnly | QFile::Text))
        return;
    QTextStream str(&file);

    str << _name;
    endl(str);

    foreach (ModelItem *i, _topItem->_belows)
    {
        printRecurseSave(str, i, 0);
    }
    file.close();
}

void ArmyListWidget::loadList(const QString &filename)
{

    QFile file(filename);
    if (!file.open(QFile::ReadOnly | QFile::Text))
        return;
    QTextStream str(&file);

    QString line = str.readLine();

    if (str.atEnd())
        return;

    ItemFactory fctr(_topItem, _settings);
    if (!fctr.addArmyFile(line + ".txt"))
        return;

    resize(500,400);

    _name = QFileInfo(filename).baseName();

    ModelItem *itm;
    QStringList dat;

    line = str.readLine();

    while(!line.isNull())
    {
        dat = line.split(';');

        itm = _topItem->getChild(dat.at(3).toInt());
        itm->toggleCheck();
        line = recurseLoad(str, itm, 0);
    }

    file.close();
}

void ArmyListWidget::on_valueChange(int i, int r)
{
    if (r<0)
        _points = i;

    emit valueChanged(i,r);
}

void ArmyListWidget::printRecurseText(QTextStream &str, ModelItem *itm, int d,
                                  QFont &f) const
{
    if (!itm->isChecked())
        return;

    int textwidth = 0;

    QStringList texts = itm->createTexts();

    if (d <= 1)
        endl(str);

    for (int i = 0; i < d; ++i)
    {
        str << "  ";
        textwidth += 4;
    }
    if (texts.at(1).toInt() > 1)
    {
        str << texts.at(1) << "X ";
        textwidth += texts.at(1).size() + 2;
    }

    QStringList ret(texts.at(0).split(", "));
    QString text("");
    QString temp("");
    int count;

    while (ret.count())
    {
        if (!text.isEmpty())
            text.append(", ");
        temp = ret.first();
        count = ret.count(temp);
        ret.removeAll(temp);

        if (count > 1)
        {
            if (!temp.endsWith('s'))
                temp.append('s');
            temp.prepend(QString::number(count) + " ");
        }
        text.append(temp);
    }

    str << text;
    textwidth += text.size();

    if (d == 1)
        str.setPadChar('.');
    str.setFieldWidth(printwidth-textwidth);
    right(str);

    str << texts.at(2);

    str.setFieldWidth(0);
    if (d == 1)
        str.setPadChar(' ');
    left(str);
    endl(str);

    foreach (ModelItem *i, itm->_belows)
    {
        printRecurseText(str, i, d+1, f);
    }
}

void ArmyListWidget::printRecurseSave(QTextStream &str, ModelItem *itm,
                                      int d) const
{
    if (!itm->isChecked())
        return;
    QStringList texts = itm->createTexts();

    for (int i = 0; i < d; ++i)
        str << '\t';

    str << texts.join(';');
    endl(str);

    foreach (ModelItem *i, itm->_belows)
    {
        printRecurseSave(str, i, d+1);
    }
}

QString ArmyListWidget::recurseLoad(QTextStream &str, ModelItem *parnt,
                                    int d)
{
    QStringList dat;
    ModelItem *itm;

    QString line = str.readLine();

    while(!line.isNull() && line.count('\t') > d)
    {
        line.remove('\t');
        dat = line.split(';');

//        itm = parnt->getChild(dat.at(3).toInt());
        if (dat.at(3).toInt() % 100)
            itm = parnt->getChild(dat.at(3).toInt());
        else
        {
            itm = parnt->getChild((dat.at(3).toInt()/100)-1);
//            itm = itm->cloneItem();
        }

        itm->toggleCheck();
//        itm->changeTexts(dat.at(0));
        itm->setValue(dat.at(1).toInt());

        line = recurseLoad(str, itm, d+1);
    }

    return line;

}
