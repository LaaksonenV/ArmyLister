#include "textedit.h"

#include <QCompleter>
#include <QTextCursor>
#include <QAbstractItemView>
#include <QScrollBar>
#include <QKeyEvent>
#include <QStringListModel>

MCLineEdit::MCLineEdit(QWidget *parent)
    : QLineEdit(parent)
    , _compmod(nullptr)
    , c(nullptr)
{
    connect(this, &MCLineEdit::editingFinished,
            [this](){addToCompleter(QStringList(text()));});
}
void MCLineEdit::keyPressEvent(QKeyEvent *e)
{

    if (c && c->popup()->isVisible()) {
        // The following keys are forwarded by the completer to the widget
        switch (e->key()) {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Escape:
        case Qt::Key_Tab:
        case Qt::Key_Backtab:
            e->ignore();
            return; // let the completer do default behavior
        default:
            break;
        }
    }

    bool isShortcut = ((e->modifiers() & Qt::ControlModifier) &&
                       e->key() == Qt::Key_E); // CTRL+E
    if (!c || !isShortcut)
        // do not process the shortcut when we have a completer
        QLineEdit::keyPressEvent(e);

    const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier
                                               | Qt::ShiftModifier);
    if (!c || (ctrlOrShift && e->text().isEmpty()))
        return;

    static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-="); // end of word
    bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
    QString completionPrefix = cursorWord(text());

    if (!isShortcut && (hasModifier || e->text().isEmpty()
                        || completionPrefix.length() < 3
                        || eow.contains(e->text().right(1)))) {
        c->popup()->hide();
        return;
    }

    if (completionPrefix != c->completionPrefix()) {
        c->setCompletionPrefix(completionPrefix);
        c->popup()->setCurrentIndex(c->completionModel()->index(0, 0));
    }
    QRect cr = cursorRect();
    cr.setWidth(c->popup()->sizeHintForColumn(0)
                + c->popup()->verticalScrollBar()->sizeHint().width());
    c->complete(cr); // popup it up!


}

QString MCLineEdit::cursorWord(const QString &sentence) const
{
    QRegExp sep("[ ,<([{]");
    QString ret = sentence.left(cursorPosition());

    int i = ret.lastIndexOf(sep);

    int j = cursorPosition() - i;
    ret = sentence.mid(i+1, j-1);
    return ret;
}

void MCLineEdit::insertCompletion(QString arg)
{
    QRegExp sep("[ ,<([{]");
    QString ret = text().left(cursorPosition());

    int i = ret.lastIndexOf(sep);
    int j = cursorPosition() - i;

    setText(text().replace(i+1,j-1,arg));
    emit QLineEdit::textEdited(text());
}

void MCLineEdit::addToCompleter(const QStringList &list)
{

    if (!c)
    {
        _compmod = new QStringListModel(list);
        _compmod->sort(0);
        c = new QCompleter(_compmod);
        c->setWidget(this);
        connect(c, SIGNAL(activated(QString)),
                this, SLOT(insertCompletion(QString)));
    }
    else
    {
        int cnt = _compmod->rowCount();
        QModelIndex index;
        if(_compmod->insertRows(cnt, list.count()))
        {
            for (int i = cnt; i < _compmod->rowCount(); ++i)
            {
                index = _compmod->index(i, 0);
                _compmod->setData(index, list.at(i-cnt));
            }
        }
    }

}
