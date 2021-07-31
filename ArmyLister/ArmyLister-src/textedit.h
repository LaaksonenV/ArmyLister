#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <QLineEdit>

class QCompleter;
class QStringListModel;

#include <QLineEdit>

/*!
 * \brief The MCLineEdit class is a text completer for creators
 */
class MCLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit MCLineEdit(QWidget *parent = 0);
    void addToCompleter(const QStringList &list);

protected:
    void keyPressEvent(QKeyEvent *e);

private:
    QString cursorWord(const QString& sentence) const;

private slots:
    void insertCompletion(QString);

private:
    QStringListModel *compmod_;
    QCompleter* c_;
};


#endif // TEXTEDIT_H
