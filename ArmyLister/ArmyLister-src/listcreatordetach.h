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

const QString c_fileName = "BattleForged.txt";

class ListCreatorDetach : public QDialog
{
    Q_OBJECT
    
public:
    
    ListCreatorDetach(QWidget *parent);

    void writeFile();
    void readFile();
    QString getDetachmentList();

public slots:
    int addItem();
    void removeItem();
    void specialSelected(int index);

    void itemSelected(int index);

    void on_OK();
    void on_AddItem();

private: //functions
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
};


#endif // BATTLEFORGEDFILE_H
