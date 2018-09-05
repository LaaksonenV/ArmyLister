#ifndef LISTCREATOR_H
#define LISTCREATOR_H

#include <QDialog>

class QTreeWidget;
class QTreeWidgetItem;
class QLineEdit;
class QTextStream;
class QTextEdit;
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
    ListCreator(const QString &file,
                const QStringList &header,
                QWidget *parent = nullptr);

    virtual ~ListCreator();

    void initialiseArmy();
    void initialiseWargear();
    void initialiseList();

    QStringList include(bool print = false);

private slots:
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
    const int preMadeItem = 1001;
    QTreeWidgetItem *createPreMadeItem(const QString &text);


    void readFile();
    void writeFileRecurse(QTextStream &str,         //
                          QTreeWidgetItem *item,    //
                          int depth);               //

    QString textForFile(const QStringList &str);
    QStringList textFromFile(const QString &str);

    void keyPressEvent(QKeyEvent *e);

private:
    QTreeWidget *_list;
    QList<QLineEdit *> _lines;
    QTextEdit *_info;
    QStringList _included;

    QString _fileName;

    QTreeWidgetItem *_clipboard;

    MCLineEdit *_editor;
    };

#endif // LISTCREATOR_H
