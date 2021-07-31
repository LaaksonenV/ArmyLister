#ifndef LISTCREATOR_H
#define LISTCREATOR_H

#include <QDialog>

class QTreeWidget;
class QTreeWidgetItem;
class QLineEdit;
class QTextStream;
class QTextEdit;
class QLabel;
class QVBoxLayout;
class QGridLayout;

class ListCreatorWidgetOrg;
class ListCreatorWidgetIncl;
class MCLineEdit;

#define HQ_ROLE "HQ"
#define TROOP_ROLE "Troop"
#define ELITE_ROLE "Elite"
#define FAST_ROLE "Fast Attack"
#define HEAVY_ROLE "Heavy Support"
#define TRANSPOR_ROLE "Dedicated Transport"
#define FLYER_ROLE "Flyer"
#define FORT_ROLE "Fortification"
#define LORD_ROLE "Lord of War"

class ListCreator : public QDialog
{
    Q_OBJECT

public:

    static void CreateArmy(QWidget *parent = nullptr);

    static void CreateList(QWidget *parent = nullptr);

    ListCreator(const QStringList &header,
                QWidget *parent = nullptr);

    virtual ~ListCreator();

private slots:
    void on_orgChange();

    void on_includeAdd(const QString& filename);

    void on_loadFile();

    bool on_save();
    void on_OK();

    QTreeWidgetItem *on_createNext();
    QTreeWidgetItem *on_createUnder();
    QTreeWidgetItem *on_createOver();

    void on_copy();
    void on_delete();
    void on_cut();

    void on_pasteUnder();
    void on_pasteNext();


    void on_labelChange(const QString &text);
    void on_costChange(const QString &text);

    void on_currentChanged(QTreeWidgetItem *now, QTreeWidgetItem*);

private:
    void addOrg();
    void addIncl();

    bool checkFileName();

    const int preMadeItem = 1001;
    QTreeWidgetItem *createPreMadeItem(const QString &text);

    void initialise40k();
    void initialise9A(const QStringList &org);


    void readFile();
    void writeFileRecurse(QTextStream &str,         //
                          QTreeWidgetItem *item,    //
                          int depth);               //

    QString textForFile(const QStringList &str);
    QStringList textFromFile(const QString &str);

    void keyPressEvent(QKeyEvent *e);

private:
    QGridLayout *lay_;

    ListCreatorWidgetOrg *org_;
    ListCreatorWidgetIncl *incl_;

    QTreeWidget *list_;
    QList<QLineEdit *> lineEdits_;
    QTextEdit *info_;
    QStringList included_;

    QString fileName_;

    QTreeWidgetItem *clipboard_;

    MCLineEdit *editor_;
    };

#endif // LISTCREATOR_H
