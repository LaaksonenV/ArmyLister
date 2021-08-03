#ifndef MODELITEMBASIC_H
#define MODELITEMBASIC_H

#include "modelitembase.h"

class QPushButton;
class QLabel;

/*!
 * \brief The ModelItemBasic class provides a basic checkable item
 */
class ModelItemBasic : public ModelItemBase
{
    Q_OBJECT
    Q_PROPERTY(int itemHeight MEMBER p_ItemHeight WRITE setP_ItemHeight)

    void init();

public:

    /*!
     * \brief ModelItemBasic basic constructor
     * \param set Settings file
     * \param parent root of the item
     * \param group optional argument used by slot items
     *
     * This constructor will automatically call addItem on \a parent
     */
    ModelItemBasic(ModelItemBase *parent);


    ModelItemBasic(ModelItemBasic *source, ModelItemBase *parent);

    virtual ~ModelItemBasic();

    virtual void clone(ModelItemBase *toRoot);

    void cloning(ModelItemBasic *clone);

    /*!
     * \brief addItem function to add an item as a branch
     * \param item to be added
     *
     * Reimplemented from Base.
     *
     * Creates an expansion button
     */
    virtual void addItem(ModelItemBase *item);

//    virtual void insertItem(ModelItemBase *item, int to);

    virtual int visibleItems(bool underCover = false) const;

    /*!
     * \brief setTrunk similar wo QWidget::setParent()
     * \param item new parent
     *
     * Reimplemented from ModelItemBase
     */
    virtual void setTrunk(ModelItemBase *item);

    /*!
     * \brief setText setter for the items title
     * \param text title text
     */
    void setText(const QString &text, int = -1);

    /*!
     * \brief setSpecial setter for special elements
     * \param list special element list
     *
     * Special elements either change the units elements (start with +/-)
     * or deny selection if elements aren't included in units elements
     */
    void setSpecial(const QStringList &list);

    /*!
     * \brief setModelLimiter setter for model limits
     * \param min -1 disables limit
     * \param max -1 disables limit
     *
     * If units models are over max or under min, sets the item
     * uncheckable
     */
    void setModelLimiter(int min, int max);

    /*!
     * \brief setAlwaysChecked simple setter
     * \param b if always checked
     *
     * Always checked items are automatically checked, and can't be
     * manually unchecked, unless limits so degree
     */
    void setAlwaysChecked(bool b = true);

    /*!
     * \brief setForAll simple setter
     * \param b if cost is for all
     *
     * For all stands for item being bought for all models.
     * This information will be passed with changeCost()
     */
    void setForAll(bool b = true);

    void setModelOverride(int models);

    /*!
     * \brief setCostLimit simple setter
     * \param limit max cost of item
     *
     * Limits branches based on how much they cost. Items that exceed
     * the limit are set uncheckable
     */
    void setCostLimit(int limit);

    /*!
     * \brief setCountsAs simple setter
     * \param role counts also as
     *
     * If \a role is set >= 0, when the item is checked its cost will be
     * passed up towards the set role also, for limiting purposes
     */
    void setCountsAs(int role);

    virtual void setUnitCountsAs(int){}

    virtual void setRange(int, int = 0){}

    virtual void setMultiCost(int, int){}

    virtual void loadSelection(QString &str);

    virtual void saveSelection(QTextStream &str);


    /*!
     * \brief getModelCount simple getter
     * \return current model count
     */
    virtual int getCurrentCount() const;

    /*!
     * \brief getText simple getter
     * \return text shown to user
     */
    virtual QString getText() const;

    virtual QString getPrintText() const;

    virtual void passSpecialUp(const QStringList &list, bool check);

    virtual void passCostUp(int c, bool b = false, int role = 0);

    virtual void passModelsDown(int models, bool push = false);

    virtual void passSpecialDown(const QStringList &list);

    virtual void passCostDown(int left);

    virtual void overrideModels(int models);

    virtual void branchExpanded(int item, int steps);

    virtual bool branchSelected(int check, int role, int, int slot = 0);

    bool checkLimit(int limit);

    virtual void resizeEvent(QResizeEvent *e);

    virtual void printToStream(QTextStream &str);

    virtual void toggleCheck(int slot = -1);

    virtual void limitedBy(short flag);

protected:

    virtual bool toggleSelected(int change, int slot);

    void print(QTextStream &str, int pre,
               const QStringList &override = QStringList());

    virtual void paintEvent(QPaintEvent*);
    virtual void enterEvent(QEvent*);
    virtual void leaveEvent(QEvent*);
    virtual void mousePressEvent(QMouseEvent*e);

    virtual QPushButton* createPlus();

    virtual void toggleExpand();

    virtual void dealWithSpecials(const QStringList &list, bool check);

    virtual int endOfText() const;

    void fitButton(QPushButton *but);


private:

    virtual void expand(bool expanse);

    void updateSpecials(const QStringList &list, QStringList &to, bool check);

signals:

    void pingSatellite(bool status);

    void passConnection(ItemSatellite * sat, bool res);

    void cloneSatellite();

    void releaseCloneSatellite();

public slots:

    virtual void forceCheck(bool check);

    virtual void currentLimitChanged(int current, bool);

    virtual void connectToSatellite(ItemSatellite * sat,
                                         bool responsible = false);

    virtual void setHardLimit(short limit);


protected:

    ModelItemBase *trunk_;
    bool checked_;
    int current_;

    QLabel *title_;

private:



    QStringList specials_;
    QStringList specialLimiters_;

    QStringList initialSpecials_;

    int forAllModels_;
    int modelOverride_;
    int overriddenModels_;
    int costLimit_;
    int modelLimitMin_;
    bool bHasModelLimitMin_;
    int modelLimitMax_;
    bool bHasModelLimitMax_;
    int countsAs_;

    QPushButton *expandButton_;
    bool bExpanded_;

    bool bAlwaysChecked_;
    bool bMouseIn_;

    int limit_;

    int p_ItemHeight;
    void setP_ItemHeight(int i){p_ItemHeight = i;}
};

#endif // MODELITEMBASIC_H
