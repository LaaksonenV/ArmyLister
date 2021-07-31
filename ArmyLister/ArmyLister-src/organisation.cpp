#include "organisation.h"

#include <QList>

#include "basicrole.h"

Organisation::Organisation(QWidget *parent)
    : QWidget(parent)
    , roles_(QList<OrganisationRole*>())
{
//    setMinimumSize(100,200);
    setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    setFixedWidth(OrganisationRole::fixedWidth());
}

Organisation::~Organisation(){}

QSize Organisation::sizeHint() const
{
    if (!roles_.count())
        return QSize();
    QSize ret(OrganisationRole::fixedWidth(),
              OrganisationRole::fixedHeight());
    ret.rheight() *= getCount() + 15;
    ret.rwidth() += 15;
    return ret;
}

void Organisation::setLists(const QString &detachments, int limit)
{
    if (detachments.isEmpty())
        return;
    QStringList lst;
    lst << "Total";
    Organisation::addPart(lst, limit);
    lst = detachments.split('#');
    lst.removeFirst();
    foreach (QString s, lst)
    {
        addPart(s.split(';'), limit);
    }
    setMinimumSize(sizeHint());
}

int Organisation::getCount() const
{
    return roles_.count();
}


void Organisation::onRoleSelection(int amount, int role)
{
    if (role+1 >= getCount())
        foreach (OrganisationRole *r, roles_)
        {
            r->roleSelected(role, amount);
        }
    else if (role >= 0)
        roles_.at(role+1)->roleSelected(role, amount);
    else
        roles_.first()->roleSelected(-1, amount);
}

void Organisation::addPart(const QStringList &args, int limit)
{
    BasicRole *role = new BasicRole(args, this);

    role->setGlobalMax(limit);
    roles_ << role;

    insertPart(role);
}

void Organisation::insertPart(OrganisationRole *part, bool first)
{
    if (first)
        emit makeRoom(1);
    else
        part->moveStep(getCount()-1);


    resize(OrganisationRole::fixedWidth(),
           OrganisationRole::fixedHeight()*getCount());

    connect(this, &Organisation::makeRoom,
            part, &OrganisationRole::moveStep);
}
