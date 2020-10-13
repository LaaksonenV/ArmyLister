#include "organisationrole.h"

OrganisationRole::OrganisationRole(const QStringList &args
                                   , QWidget *parent)
    : QWidget(parent)
    , vq_name("")
{
    setFixedWidth(fixedWidth());


    if (!args.count())
        printFaultLabel();

    else
    {
        vq_name = args.at(0);
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
    return c_iconSideLength + c_labelHeight + 2*c_frameWidth;
}

int OrganisationRole::fixedWidth()
{
    return c_iconSideLength * 10 + 2*c_frameWidth;
}

void OrganisationRole::moveStep(int count)
{
    move(0,pos().y()+count*fixedHeight());
}

void OrganisationRole::printFaultLabel()
{
    vq_name = "Faulty detachment line";
}
