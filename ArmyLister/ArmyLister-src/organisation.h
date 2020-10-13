#ifndef ORGANISATION_H
#define ORGANISATION_H

#include <QWidget>

#include <QList>

class OrganisationRole;

template<typename T> class QList;

class Organisation : public QWidget
{
    Q_OBJECT
public:
    explicit Organisation(QWidget *parent = nullptr);
    virtual ~Organisation();

    virtual QSize sizeHint() const;

    virtual void setLists(const QString &detachments, int limit);

    int getCount() const;

signals:
    void roleSelected(int, int);

    void makeRoom(int);

public slots:
    virtual void onRoleSelection(int amount, int role);


protected:
    virtual void addPart(const QStringList &args, int limit);
    virtual void insertPart(OrganisationRole *part,
                            bool first = false);

private:

    QList<OrganisationRole*> _roles;
};

#endif // ORGANISATION_H
