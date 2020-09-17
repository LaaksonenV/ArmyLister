#include "organisation.h"

#include <QList>


Organisation::Organisation(QWidget *parent)
    : QWidget(parent)
    , qv_partsList()
{
    setMinimumSize(100,200);
    setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}

Organisation::~Organisation(){}

QSize Organisation::sizeHint() const
{
    if (!qv_partsList.count())
        return QSize();
    QSize ret(qv_partsList.at(0)->sizeHint());
    ret.rheight() *= qv_partsList.count();
    ret.rwidth() += 15;
    return ret;
}

void Organisation::setLists(const QString &detachments)
{

    if (detachments.isEmpty())
        return;
    QStringList lst = detachments.split('#');
    lst.removeFirst();
    foreach (QString s, lst)
    {
        addPart(s.split(';'));
    }
    setMinimumSize(sizeHint());
}
