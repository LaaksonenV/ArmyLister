#ifndef ORGANISATION_H
#define ORGANISATION_H

#include <QWidget>

template<typename T> class QList;

class Organisation : public QWidget
{
    Q_OBJECT
public:
    explicit Organisation(QWidget *parent = nullptr);
    virtual ~Organisation();

    virtual QSize sizeHint() const;

    virtual void setLists(const QString &detachments);

signals:

public slots:
    virtual void roleSelected(int role, int amount) = 0;


protected:

    virtual void addPart(const QStringList &args) = 0;

    QList<Detachment*> qv_partsList;
};

#endif // ORGANISATION_H
