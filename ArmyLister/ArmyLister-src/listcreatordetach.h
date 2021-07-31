#ifndef BATTLEFORGEDFILE_H
#define BATTLEFORGEDFILE_H

#include <QDialog>

class QStringList;
template<typename T> class QList;
class QSpinBox;
class QListWidget;
class QComboBox;
class QListWidgetItem;
class QLineEdit;

class ListCreatorDetach : public QDialog
{
    Q_OBJECT
    
public:
    
    ListCreatorDetach(QWidget *parent, const QString &filename);

    static QString getOrganisationList(const QString &filename);

public slots:
    int addItem();
    void removeItem();
    void specialSelected(int index);

    void itemSelected(int index);

    void on_OK();
    void on_AddItem();

private: //functions

    void writeFile();
    void readFile();

    void saveCurrent();
    
private:
    QListWidget *detachmentList_;
    int currentIndex_;

    QLineEdit *cp_;
    QList<QSpinBox*> mins_;
    QList<QSpinBox*> maxs_;
    QList<QComboBox*> specials_;

    struct RoleSlotStruct
    {
        int min_, max_, special_;
    };
    struct DetachmentStruct
    {
        int cp_;
        QList<RoleSlotStruct> roles_;
    };

    QList<DetachmentStruct> structs_;

    QString currentFile_;
};


#endif // BATTLEFORGEDFILE_H
