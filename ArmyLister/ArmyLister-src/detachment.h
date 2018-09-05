#ifndef DETACHMENT_H
#define DETACHMENT_H

#include <QWidget>

/*                  |       |
 * Technically,     |       | <- air
 * the glass is     |~~~~~~~|
 * always full      |       | <- liguid
 *                  |_______|
 */

class QStringList;
template<typename T> class QList;

class RoleSlot;
class LimitHandler;


namespace BattlefieldRole
{
enum
{
    HQ, Troop, Elite, Fast, Heavy,
    Transport, Flyer, Fort, Lord, OverRoles
};
}

namespace SpecialSlot
{
enum
{
    Null, LimitByMin, LimitByMax, OverSlot
};
}

class Detachment : public QWidget
{
    Q_OBJECT
    
public:
    static QPixmap getIcon(int ofRole);

    enum SlotIndex // Printing slots
    {
        Role, Min, Max, Special
    };

    explicit Detachment(const QStringList &args, QWidget *parent = nullptr);
    
    virtual QSize sizeHint() const;

    void selectionChange(const QList<int> &mins,
                         const QList<int> &maxs);
    void roleSelected(int role, int amount);

    QList<int> getMinimums();
    QList<int> getMaximums();
    int getPoints();

protected:
    virtual void enterEvent(QEvent *);
    virtual void leaveEvent(QEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseDoubleClickEvent(QMouseEvent *);
    virtual void paintEvent(QPaintEvent *);

signals:
    void toggled(Detachment *,bool);
    void clone(Detachment *, QStringList);

public slots:
    void on_betweenLimits(int from, bool check);
    

private: //functions
    void printFaultLabel();
    
private:
    QStringList vq_args;
    QList<RoleSlot*> vq_slotList;
    QList<bool> vq_availableList;
    QList<LimitHandler*> vq_handlerList;
    QString vq_name;
    int v_points;

    bool v_isAvailable;
    bool v_isSelected;
    bool v_hasMouse;

    const int c_fontsize = 12;
    const int c_frameWidth = 6;
    const int c_iconSideLength = 40;
    const int c_labelHeight = 15;
};

#endif // DETACHMENT_H
