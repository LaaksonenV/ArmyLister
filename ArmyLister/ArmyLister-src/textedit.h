#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <QLineEdit>
#include <QCompleter>

#include <QLineEdit>

class MCLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit MCLineEdit(QWidget *parent = 0);
    void setMultipleCompleter(QCompleter* completer);

protected:
    void keyPressEvent(QKeyEvent *e);

private:
    QString cursorWord(const QString& sentence) const;

private slots:
    void insertCompletion(QString);

private:
    QCompleter* c;
};


#endif // TEXTEDIT_H
