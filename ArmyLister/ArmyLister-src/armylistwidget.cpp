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
    , topItem_(new ModelItemBase(this))
    , name_(QString())
    , points_(0)
{
    setWidget(topItem_);
    setWidgetResizable(true);
    setMinimumWidth(500);
    setSizePolicy(QSizePolicy::Expanding,
                  QSizePolicy::MinimumExpanding);

    connect(topItem_, &ModelItemBase::itemSelected,
            this, &ArmyListWidget::roleSelected);
    connect(topItem_, &ModelItemBase::valueChanged,
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
    if (!fctr.addArmyFile(topItem_, fileName))
        return false;
    QFileInfo forName;
    forName.setFile(fileName);
    name_ = forName.completeBaseName();
    adjustSize();
    return true;
}

void ArmyListWidget::printList() const
{
    if (!topItem_)
        return;
    QString filename = name_;
    filename.append("-");
    filename.append(QString::number(points_));
    filename.append(".txt");
    QFile file(filename);
    if (!file.open(QFile::WriteOnly | QFile::Text))
        return;

    QTextStream str(&file);

    int count = name_.size()+5;
    str.setFieldWidth(int(Settings::ItemMeta(Settings::eItem_PlainTextWidth)-count)
                      /2);
    str << name_;
    str.setFieldWidth(0);
    str << " " << points_;
    endl(str);

    topItem_->printToStream(str);

    file.close();
}

void ArmyListWidget::saveList() const
{
    QString filename = name_;
    filename.append("-");
    filename.append(QString::number(points_));
    filename.append(".alst");
    saveListAs(filename);
}

void ArmyListWidget::saveListAs(QString filename) const
{
    if (!topItem_)
        return;

    QFile file(filename);
    if (!file.open(QFile::WriteOnly | QFile::Text))
        return;
    QTextStream str(&file);

    str << name_;
    endl(str);
    str << QFileInfo(file).lastModified().toString();
    endl(str);

    topItem_->saveSelection(str);

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
    if (!fctr.addArmyFile(topItem_, listfile))
        return;

    adjustSize();

    name_ = line;

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
        topItem_->loadSelection(line);

    file.close();
}

void ArmyListWidget::on_valueChange(int i, int r)
{
    if (r<0)
        points_ += i;

    emit valueChanged(i,r);
}
