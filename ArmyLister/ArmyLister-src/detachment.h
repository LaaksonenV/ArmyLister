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
    eBattlefieldRole_HQ
    , eBattlefieldRole_Troop
    , eBattlefieldRole_Elite
    , eBattlefieldRole_Fast
    , eBattlefieldRole_Heavy
    , eBattlefieldRole_Transport
    , eBattlefieldRole_Flyer
    , eBattlefieldRole_Fort
    , eBattlefieldRole_Lord
    , eBattlefieldRole_OverRoles
};
}

namespace SpecialSlot
{
enum
{
    eSpecialSlot_Null
    , eSpecialSlot_LimitByMin
    , eSpecialSlot_LimitByMax
    , eSpecialSlot_OverSlot
};
}

class Detachment : public OrganisationRole
{
    Q_OBJECT
    
public:
    static QPixmap getIcon(int ofRole);

    enum SlotIndex // Printing slots
    {
        eRole, eMin, eMax, eSpecial
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
    QStringList argsList_;
    QList<DetachmentRoleType*> slotList_;
    QList<bool> availableList_;
    QList<LimitHandler*> handlerList_;
    int points_;

    bool bAvailable_;
    bool bSelected_;
    bool bHasMouse_;


};

#endif // DETACHMENT_H
