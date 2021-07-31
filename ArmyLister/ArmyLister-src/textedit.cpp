#include "textedit.h"

#include <QCompleter>
#include <QTextCursor>
#include <QAbstractItemView>
#include <QScrollBar>
#include <QKeyEvent>
#include <QStringListModel>

MCLineEdit::MCLineEdit(QWidget *parent)
    : QLineEdit(parent)
    , compmod_(nullptr)
    , c_(nullptr)
{
    connect(this, &MCLineEdit::editingFinished,
            [this](){addToCompleter(QStringList(text()));});
}
void MCLineEdit::keyPressEvent(QKeyEvent *e)
{

    if (c_ && c_->popup()->isVisible()) {
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
    if (!c_ || !isShortcut)
        // do not process the shortcut when we have a completer
        QLineEdit::keyPressEvent(e);

    const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier
                                               | Qt::ShiftModifier);
    if (!c_ || (ctrlOrShift && e->text().isEmpty()))
        return;

    static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-="); // end of word
    bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
    QString completionPrefix = cursorWord(text());

    if (!isShortcut && (hasModifier || e->text().isEmpty()
                        || completionPrefix.length() < 3
                        || eow.contains(e->text().right(1)))) {
        c_->popup()->hide();
        return;
    }

    if (completionPrefix != c_->completionPrefix()) {
        c_->setCompletionPrefix(completionPrefix);
        c_->popup()->setCurrentIndex(c_->completionModel()->index(0, 0));
    }
    QRect cr = cursorRect();
    cr.setWidth(c_->popup()->sizeHintForColumn(0)
                + c_->popup()->verticalScrollBar()->sizeHint().width());
    c_->complete(cr); // popup it up!


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

    if (!c_)
    {
        compmod_ = new QStringListModel(list);
        compmod_->sort(0);
        c_ = new QCompleter(compmod_);
        c_->setWidget(this);
        connect(c_, SIGNAL(activated(QString)),
                this, SLOT(insertCompletion(QString)));
    }
    else
    {
        int cnt = compmod_->rowCount();
        QModelIndex index;
        if(compmod_->insertRows(cnt, list.count()))
        {
            for (int i = cnt; i < compmod_->rowCount(); ++i)
            {
                index = compmod_->index(i, 0);
                compmod_->setData(index, list.at(i-cnt));
            }
        }
    }

}
