#ifndef LISTCREATORDEFORG_H
#define LISTCREATORDEFORG_H

#include <QDialog>

#include <QList>
#include <QStringList>

class QLineEdit;
class QComboBox;
class QSpinBox;
class QGridLayout;
class QButtonGroup;

class ListCreatorDefOrg : public QDialog
{
public:
    ListCreatorDefOrg(QWidget *parent = nullptr);

    QString getType() const;
    QStringList getOrg() const;

    void init(const QString &orgtype,
              const QStringList &org);

private slots:

    void on_selectFile();

    void on_add9AAS();

private:

    QButtonGroup *_orgtype;

    QLineEdit *_detachment;

    QGridLayout *_9AOrg;
    QPushButton *_add9AOrg;

    const QStringList _coreNames{"Characters","Core","Special"};
    QList<QLineEdit*> _names;
    QList<QComboBox*> _types;
    QList<QSpinBox*> _values;

    const QStringList _items{"max","min","-"};
};

#endif // LISTCREATORDEFORG_H
