#ifndef NINTHAGEROLE_H
#define NINTHAGEROLE_H

#include "organisationrole.h"

#include <QString>

class BasicRole : public OrganisationRole
{
    Q_OBJECT
public:
    explicit BasicRole(const QStringList &args, QWidget *parent = nullptr);
    virtual ~BasicRole();

    void setGlobalMax(int n);

    virtual void roleSelected(int , int amount);

protected:
    virtual void paintEvent(QPaintEvent *);

signals:

private:
    QString _printText;

    int _max;
    int _current;
    int _globalMax;
//    int _currentMax;

};

#endif // NINTHAGEROLE_H
