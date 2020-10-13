#ifndef DETACHMENT_H
#define DETACHMENT_H

#include "organisationrole.h"

/*                  |       |
 * Technically,     |       | <- air
 * the glass is     |~~~~~~~|
 * always full      |       | <- liguid
 *                  |_______|
 */

class QStringList;
template<typename T> class QList;

class DetachmentRoleType;
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

class Detachment : public OrganisationRole
{
    Q_OBJECT
    
public:
    static QPixmap getIcon(int ofRole);

    enum SlotIndex // Printing slots
    {
        Role, Min, Max, Special
    };

    explicit Detachment(const QStringList &args, QWidget *parent = nullptr);
    virtual ~Detachment();
    
    void selectionChange(const QList<int> &mins,
                         const QList<int> &maxs);
    virtual void roleSelected(int role, int amount);

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
    void clone(const QStringList&);

public slots:
    void on_betweenLimits(int from, bool check);
    

    
private:
    QStringList vq_args;
    QList<DetachmentRoleType*> vq_slotList;
    QList<bool> vq_availableList;
    QList<LimitHandler*> vq_handlerList;
    int v_points;

    bool v_isAvailable;
    bool v_isSelected;
    bool v_hasMouse;


};

#endif // DETACHMENT_H
