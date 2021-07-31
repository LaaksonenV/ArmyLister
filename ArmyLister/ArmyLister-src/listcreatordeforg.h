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

    QButtonGroup *orgtype_;

    QLineEdit *detachment_;

    QGridLayout *n9AOrg_;
    QPushButton *add9AOrg_;

    const QStringList coreNames{"Characters","Core","Special"};
    QList<QLineEdit*> names_;
    QList<QComboBox*> types_;
    QList<QSpinBox*> values_;

    const QStringList items{"max","min","-"};
};

#endif // LISTCREATORDEFORG_H
