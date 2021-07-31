#include "listcreatorwidgets.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

#include <QListWidget>
#include <QListWidgetItem>
#include <QFileDialog>

#include "listcreatordeforg.h"

ListCreatorWidgetOrg::ListCreatorWidgetOrg(QWidget *parent)
    : QWidget(parent)
    , org_(QString())
    , creator_(nullptr)
{
    QHBoxLayout *chLay = new QHBoxLayout();

    text_ = new QLabel("Undefined");

    chLay->addWidget(text_);

    QPushButton *button = new QPushButton("Define organisation", this);
    connect(button, &QPushButton::clicked,
            this, &ListCreatorWidgetOrg::on_create);
    chLay->addWidget(button);

    setLayout(chLay);
}

void ListCreatorWidgetOrg::setOrg(const QString &line)
{
    if (!line.isEmpty())
    {
        text_->setText(line.section('#',0,0));
        org_ = line.section('#',1);
//        emit finished();
    }
}

QString ListCreatorWidgetOrg::getOrg() const
{
    if (org_.isNull())
        return QString();

    QString ret = text_->text();

    ret += "#" + org_;

    return ret;
}

void ListCreatorWidgetOrg::on_create()
{
    if (!creator_)
    {
        creator_ = new ListCreatorDefOrg(this);
        connect(creator_, &ListCreatorDefOrg::finished,
                this, &ListCreatorWidgetOrg::on_finished);

        if (!org_.isNull())
            creator_->init(text_->text(), org_.split('#'));
    }

    creator_->open();
}

void ListCreatorWidgetOrg::on_finished()
{
    if (creator_->result() == QDialog::Accepted)
    {
        text_->setText(creator_->getType());
        org_ = creator_->getOrg().join('#');
        emit finished();
    }
}



ListCreatorWidgetIncl::ListCreatorWidgetIncl(QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout *chLay = new QHBoxLayout();

    files_ = new QListWidget(this);
    files_->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    files_->setSelectionMode(QAbstractItemView::SingleSelection);

    chLay->addWidget(files_);

    QVBoxLayout *butLay = new QVBoxLayout();

    QPushButton *button = new QPushButton("Add file", this);
    connect(button, &QPushButton::clicked,
            this, &ListCreatorWidgetIncl::on_add);
    butLay->addWidget(button);

    button = new QPushButton("Remove file", this);
    connect(button, &QPushButton::clicked,
            this, &ListCreatorWidgetIncl::on_remove);
    butLay->addWidget(button);


    chLay->addLayout(butLay);

    setLayout(chLay);
}

QStringList ListCreatorWidgetIncl::getFiles() const
{
    QStringList ret;
    QListWidgetItem *itm;
    for (int i = 0; i < files_->count(); ++i)
    {
        itm = files_->item(i);
        ret << itm->text();
    }
    return ret;
}

void ListCreatorWidgetIncl::addFile(const QString &file)
{
    bool ok = true;


    for (int i = 0; i < files_->count(); ++i)
        if (file == files_->item(i)->text())
        {
            ok = false;
            break;
        }

    if (ok)
    {
        files_->addItem(file);
        emit fileAdded(file);
    }
}

void ListCreatorWidgetIncl::on_add()
{
    QStringList files = QFileDialog::getOpenFileNames(
                this, "include files", QDir::currentPath(),
                "(*txt)");

    QDir dir(QDir::currentPath());
    for (int i = 0; i < files.count(); ++i)
    {
        addFile(dir.relativeFilePath(files.at(i)));
    }
}

void ListCreatorWidgetIncl::on_remove()
{
    if (!files_->currentItem())
        return;

    QListWidgetItem *itm = files_->takeItem(files_->currentRow());
    emit fileRemoved(itm->text());
}
