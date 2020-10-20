#include "armylistwidget.h"

#include <QFile>
#include <QTextStream>
#include <QRegExp>
#include <QtPrintSupport/QPrinter>
#include <QFileInfo>

#include "settings.h"
#include "itemfactory.h"
#include "modelitembase.h"

ArmyListWidget::ArmyListWidget(QWidget *parent)
    : QScrollArea(parent)
    , _topItem(new ModelItemBase(this))
    , _name(QString())
    , _points(0)
{
    setWidget(_topItem);
    setWidgetResizable(true);
    setMinimumWidth(500);
    setSizePolicy(QSizePolicy::Expanding,
                  QSizePolicy::MinimumExpanding);

    connect(_topItem, &ModelItemBase::itemSelected,
            this, &ArmyListWidget::roleSelected);
    connect(_topItem, &ModelItemBase::valueChanged,
            this, &ArmyListWidget::on_valueChange);

}

ArmyListWidget::~ArmyListWidget()
{
//    foreach (PointContainer *p, _pointList)
  //      delete p;
}

bool ArmyListWidget::addArmyFile(const QString &fileName)
{
      // Reusable class, move to members
    ItemFactory fctr;
    if (!fctr.addArmyFile(_topItem, fileName))
        return false;
    QFileInfo forName;
    forName.setFile(fileName);
    _name = forName.completeBaseName();
    adjustSize();
    return true;
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

    int count = _name.size()+5;
    str.setFieldWidth(int(Settings::ItemMeta(Settings::PlainTextWidth)-count)
                      /2);
    str << _name;
    str.setFieldWidth(0);
    str << " " << _points;
    endl(str);

    _topItem->printToStream(str);

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

/*    foreach (ModelItem *i, _topItem->_branches)
    {
        printRecurseSave(str, i, 0);
    }
    file.close();*/
}

void ArmyListWidget::loadList(const QString &filename)
{

    // Need a bit of work, remove getchild 41
/*    QFile file(filename);
    if (!file.open(QFile::ReadOnly | QFile::Text))
        return;
    QTextStream str(&file);

    QString line = str.readLine();

    if (str.atEnd())
        return;

    ItemFactory fctr();
    if (!fctr.addArmyFile(_topItem, line + ".txt"))
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

    file.close();*/
}

void ArmyListWidget::on_valueChange(int i, int r)
{
    if (r<0)
        _points += i;

    emit valueChanged(i,r);
}

void ArmyListWidget::printRecurseSave(QTextStream &str, ModelItemBase *itm,
                                      int d) const
{
/*    if (!itm->isChecked())
        return;
    QStringList texts = itm->createTexts();

    for (int i = 0; i < d; ++i)
        str << '\t';

    str << texts.join(';');
    endl(str);

    foreach (ModelItem *i, itm->_branches)
    {
        printRecurseSave(str, i, d+1);
    }*/
}

QString ArmyListWidget::recurseLoad(QTextStream &str, ModelItemBase *parnt,
                                    int d)
{

/*    QStringList dat;
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

    return line;*/
return QString();
}
