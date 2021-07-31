#ifndef SELECTCOUNTER_H
#define SELECTCOUNTER_H

#include <QWidget>

class QString;
class QSize;
class QPixmap;

class DetachmentRoleType : public QWidget
{
    Q_OBJECT
public:
    explicit DetachmentRoleType(QPixmap icon, QWidget *parent = nullptr);

    void setLimits(int min, int max);
    void setMax(int max);
    void setMin(int min);

    int getMin();
    int getMax();

    bool isBetweenLimits();

protected:
    void paintEvent(QPaintEvent *);

signals:
    void betweenLimits(bool);

public slots:
    void selected(int amount);
    void on_Selection(int incrMin, int incrMax);

private:
    QPixmap icon_;

    int min_, max_, current_;
    int effectiveMin_, effectiveMax_;
};

#endif // SELECTCOUNTER_H
