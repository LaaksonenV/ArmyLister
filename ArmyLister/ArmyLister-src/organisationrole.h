#ifndef ROLESLOT_H
#define ROLESLOT_H

#include <QWidget>

class OrganisationRole : public QWidget
{
    Q_OBJECT
public:
    explicit OrganisationRole(const QStringList &args, QWidget *parent = nullptr);
    virtual ~OrganisationRole();

    virtual QSize sizeHint() const;

    static int fixedHeight();
    static int fixedWidth();

    virtual void roleSelected(int role, int amount) = 0;

public slots:
    virtual void moveStep(int count);

protected:
    virtual void paintEvent(QPaintEvent *) = 0;

    virtual void printFaultLabel();

    QString name_;

    static const int s_frameWidth_ = 6;
    static const int s_iconSideLength_ = 40;
    static const int s_labelHeight_ = 15;
};

#endif // ROLESLOT_H
