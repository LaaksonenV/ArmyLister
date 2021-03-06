#ifndef LISTCREATORWIDGETS_H
#define LISTCREATORWIDGETS_H

#include <QWidget>

class ListCreatorDefOrg;
class QLabel;

class ListCreatorWidgetOrg : public QWidget
{
    Q_OBJECT
public:
    explicit ListCreatorWidgetOrg(QWidget *parent);

    void setOrg(const QString &line);
    QString getOrg() const;

signals:
    void finished();

private slots:
    void on_create();

    void on_finished();

private:

    QLabel *_text;
    QString _org;

    ListCreatorDefOrg *_creator;
};

class QListWidget;

class ListCreatorWidgetIncl : public QWidget
{
    Q_OBJECT
public:
    explicit ListCreatorWidgetIncl(QWidget *parent);

    QStringList getFiles() const;
    void addFile(const QString &file);

signals:
    void fileAdded(const QString&);
    void fileRemoved(const QString&);

private slots:
    void on_add();
    void on_remove();

private:
    QListWidget *_files;
};

#endif // LISTCREATORWIDGETS_H
