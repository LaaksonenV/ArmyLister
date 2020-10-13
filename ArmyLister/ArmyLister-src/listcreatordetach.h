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
    QListWidget *qv_detachmentList;
    int v_currentIndex;

    QLineEdit *qv_cp;
    QList<QSpinBox*> qv_mins;
    QList<QSpinBox*> qv_maxs;
    QList<QComboBox*> qv_spes;

    struct RoleSlotStruct
    {
        int min, max, special;
    };
    struct DetachmentStruct
    {
        int cp;
        QList<RoleSlotStruct> roles;
    };

    QList<DetachmentStruct> qv_structs;

    QString qv_currentFile;
};


#endif // BATTLEFORGEDFILE_H
