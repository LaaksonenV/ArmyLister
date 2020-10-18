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
    , _org(QString())
    , _creator(nullptr)
{
    QHBoxLayout *chLay = new QHBoxLayout();

    _text = new QLabel("Undefined");

    chLay->addWidget(_text);

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
        _text->setText(line.section('#',0,0));
        _org = line.section('#',1);
//        emit finished();
    }
}

QString ListCreatorWidgetOrg::getOrg() const
{
    if (_org.isNull())
        return QString();

    QString ret = _text->text();

    ret += "#" + _org;

    return ret;
}

void ListCreatorWidgetOrg::on_create()
{
    if (!_creator)
    {
        _creator = new ListCreatorDefOrg(this);
        connect(_creator, &ListCreatorDefOrg::finished,
                this, &ListCreatorWidgetOrg::on_finished);

        if (!_org.isNull())
            _creator->init(_text->text(), _org.split('#'));
    }

    _creator->open();
}

void ListCreatorWidgetOrg::on_finished()
{
    if (_creator->result() == QDialog::Accepted)
    {
        _text->setText(_creator->getType());
        _org = _creator->getOrg().join('#');
        emit finished();
    }
}



ListCreatorWidgetIncl::ListCreatorWidgetIncl(QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout *chLay = new QHBoxLayout();

    _files = new QListWidget(this);
    _files->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    _files->setSelectionMode(QAbstractItemView::SingleSelection);

    chLay->addWidget(_files);

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
    for (int i = 0; i < _files->count(); ++i)
    {
        itm = _files->item(i);
        ret << itm->text();
    }
    return ret;
}

void ListCreatorWidgetIncl::addFile(const QString &file)
{
    bool ok = true;


    for (int i = 0; i < _files->count(); ++i)
        if (file == _files->item(i)->text())
        {
            ok = false;
            break;
        }

    if (ok)
    {
        _files->addItem(file);
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
    if (!_files->currentItem())
        return;

    QListWidgetItem *itm = _files->takeItem(_files->currentRow());
    emit fileRemoved(itm->text());
}
