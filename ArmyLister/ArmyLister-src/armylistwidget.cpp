#include "armylistwidget.h"

#include <QFile>
#include <QTextStream>
#include <QRegExp>
#include <QtPrintSupport/QPrinter>
#include <QFileInfo>
#include <QDateTime>
#include <QMessageBox>

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
    str << QFileInfo(file).lastModified().toString();
    endl(str);

    _topItem->saveSelection(str);

    file.close();
}

void ArmyListWidget::loadList(const QString &filename)
{

    QFile file(filename);
    if (!file.open(QFile::ReadOnly | QFile::Text))
        return;
    QTextStream str(&file);

    QString line = str.readLine();

    if (line.isNull())
        return;

    QString listfile = line + ".txt";

    ItemFactory fctr;
    if (!fctr.addArmyFile(_topItem, listfile))
        return;

    adjustSize();

    _name = line;

    line = str.readLine();

    QDateTime now = QFileInfo(listfile).lastModified();
    if (now != QDateTime::fromString(line))
    {
        int ret = QMessageBox::warning(this, tr("Army book modified"),
                                       tr("The army book has been modified "
                                          "since the list was saved.\n"
                                          "The list may not load as "
                                          "intented.\n"),
                                      QMessageBox::Ignore|QMessageBox::Abort,
                                       QMessageBox::Ignore);
        if (ret == QMessageBox::Abort)
        {
            file.close();
            return;
        }
    }

    line = str.readLine();

    if (!line.isEmpty())
        _topItem->loadSelection(line);

    file.close();
}

void ArmyListWidget::on_valueChange(int i, int r)
{
    if (r<0)
        _points += i;

    emit valueChanged(i,r);
}
