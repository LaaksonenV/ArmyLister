#include "organisationrole.h"

OrganisationRole::OrganisationRole(const QStringList &args
                                   , QWidget *parent)
    : QWidget(parent)
    , name_("")
{
    setFixedWidth(fixedWidth());


    if (!args.count())
        printFaultLabel();

    else
    {
        name_ = args.at(0);
        setFixedHeight(fixedHeight());
    }

    setSizePolicy(QSizePolicy());
}

OrganisationRole::~OrganisationRole()
{

}

QSize OrganisationRole::sizeHint() const
{
    return QSize(fixedWidth(),fixedHeight());
}

int OrganisationRole::fixedHeight()
{
    return s_iconSideLength_ + s_labelHeight_ + 2*s_frameWidth_;
}

int OrganisationRole::fixedWidth()
{
    return s_iconSideLength_ * 10 + 2*s_frameWidth_;
}

void OrganisationRole::moveStep(int count)
{
    move(0,pos().y()+count*fixedHeight());
}

void OrganisationRole::printFaultLabel()
{
    name_ = "Faulty detachment line";
}
