#ifndef MODELITEMBASE_H
#define MODELITEMBASE_H

#include <QWidget>

class ItemSatellite;

#include <QList>
#include <QTextStream>

/*!
 * \brief The ModelItemBase class is an model item for list
 * entitys, providing base functions for moving events up and down,
 * but no drawing
 */
class ModelItemBase : public QWidget
{
    Q_OBJECT
public:

    /*!
     * \brief ModelItemBase basic constructor
     * \param set Settings file
     * \param parent
     */
    ModelItemBase(QWidget *parent = nullptr);

    /*!
     * \brief ModelItemBase clone constructor
     * \param source item to clone
     * \param root clones parent
     */
    ModelItemBase(ModelItemBase *source, ModelItemBase *parent);

    /*!
     * \brief ~ModelItemBase basic destructor
     */
    virtual ~ModelItemBase();

    virtual void clone(ModelItemBase *toRoot, int i = -1);

    /*!
     * \brief sizeHint Reimplemented from QWidget
     * \return
     */
    virtual QSize sizeHint() const;

    /*!
     * \brief getItem function to get existing item
     * \param text text in the item
     * \return Item or nullptr if no item found
     */
    virtual ModelItemBase* getItem(const QString &text) const;

    /*!
     * \brief addItem function to add an item as a branch
     * \param item to be added
     *
     * Added item will be moved to correct place, and an index will be
     * provided.
     */
    virtual void addItem(ModelItemBase *item);

    virtual void insertItem(ModelItemBase *item, int to);

    /*!
     * \brief moveSteps moves the item down or up in parent
     * \param step itemHeights to move
     *
     * Move will be incremental by defined itemHeight and itemHPos
     */
    virtual void moveSteps(short stepY, short stepX = 0);

    /*!
     * \brief visibleItems counts how many items under it are visible
     * \return count of visible items
     *
     * Base item is not visible, so it returns only how many branches
     * are visible
     */
    virtual int visibleItems(bool = false) const;

    /*!
     * \brief setTrunk similar wo QWidget::setParent()
     *
     * Base has no root
     */
    virtual void setTrunk(ModelItemBase*){}

    /*!
     * \brief setIndex set place in branch list
     * \param i place
     */
    void setIndex(int i);

    /*!
     * \brief setCost simple setter
     * \param i new cost
     */
    virtual void setCost(int i, int = -1);

    virtual void loadSelection(QString &str);

    virtual void saveSelection(QTextStream &str);

    /*!
     * \brief getText simple getter
     * \return text shown to user
     *
     * Base item has no text
     */
    virtual QString getText() const;

    /*!
     * \brief getCost simple getter
     * \return cost shown to user
     */
    virtual int getCost() const;

    /*!
     * \brief getModelCount simple getter
     * \return current model count
     *
     * Base item doesn't include models; this function must be
     * implemented in a derived class that does. ModelItemBase returns -1
     */
    virtual int getCurrentCount() const;

    /*!
     * \brief passSpecial pass function for special elements
     *
     * Pass functions pass information towards the root. As Base item
     * is a root item, all derived classes with roots must implement
     * this.
     *
     * Units use special element information
     */
    virtual void passTagsUp(const QStringList &, bool);

    /*!
     * \brief changeCost changed cost value
     * \param c amount changed
     * \param role special role cost is counted towards
     *
     * If \a role is given > 0, the function will not change the actual
     * cost. Item with a root must take action to pass the information
     * to an item that handles it, and in any case to pass the function
     * to the root
     */
    virtual void passCostUp(int c, bool = false, int role = 0);

    /*!
     * \brief passModelsDown pass function for model count change
     * \param models new model count
     *
     * Items limited by model count need to know if it changes
     */
    virtual void passModelsDown(int models, bool push = false);

    /*!
     * \brief PassSpecialDown pass function for edited special elements
     * \param list new special elements list
     *
     * Special elements are changed based on passSpecialUp, and new
     * list needs to be spread to all items that have limitations
     * base on the special elements
     */
    virtual void passTagsDown(const QStringList &list);

    /*!
     * \brief passCostDown pass function for remaining points on limited
     * branch
     * \param left points left to choose
     */
    virtual void passCostDown(int left);

    virtual void overrideModels(int){}

    /*!
     * \brief branchChecked is called when an item is attempted to check
     * \return true if the root of item allows checking
     *
     * As item without root, Base item return true. Implement on an item
     * with root
     */
    virtual bool branchSelected(int, int, int, int = -1);

    /*!
     * \brief resizeEvent reimplemented
     * \param e
     *
     * Upon receiving a resize event, branches are resized also
     */
    virtual void resizeEvent(QResizeEvent *e);

    /*!
     * \brief The Limited enum is used as a pseudoflag numbers for
     * limits
     *
     * Checkable isn't really a limiter, just an easy way to tell if
     * items checking is limited (_limit > Checkable). Enum order must
     * be: non selection stopping, selection stopping. Checkable must be
     * defined as last non selection stopping limit
     */
    enum Limited : short
    {
        eNotClonable = 1,
        eSelectionLimit = eNotClonable*2,

        eGlobalLimit = eSelectionLimit*2,
        eSpecialLimit = eGlobalLimit*2,
        eCostLimit = eSpecialLimit*2,
        eCountLimit = eCostLimit*2,
        eModelsLimit = eCountLimit*2,

        eCriticalLimit = eModelsLimit*2,


        eCheckable = eSelectionLimit,
        eLastBaseLimit = eCriticalLimit
    };

    virtual void printToStream(QTextStream &str);

    void endCloning();

private:

    /*!
     * \brief expand show items choises
     *
     * Expand is usually called by toggling a button. It will then resize
     * to fit all its children, thus showing them, and informing trunk
     * to move all item below this to fit.
     *
     * As Base item has nothing to show, expand() must be implemented
     * in derived classes that do
     */
    virtual void expand(bool);

public slots:

    /*!
     * \brief branchExpanded this slot is called when a branch expands
     * \param item index of item that expanded
     * \param steps amount of items that were shown
     *
     * When an item in the middle of the tree/list is expanded, the
     * root item must be resized to fit the now visible items, and
     * all items under the expanded item must be moved out of the way.
     * Vice versa for retracting items, noted by negative \a steps
     */
    virtual void branchExpanded(int item, int steps);

    /*!
     * \brief LimitedBy sets the limited flag with pseudo flags
     *
     * When a flag is set, it should be called as normal Limited enum,
     * and when disabled as negative
     */
    virtual void limitedBy(short){}

    /*!
     * \brief toggleCheck is called when the items selection status
     * should be toggled
     */
    virtual void toggleCheck(int = -1){}

    /*!
     * \brief toggleExpand is called when the item should be expanded
     */
    virtual void toggleExpand(){}


signals:

    void modelsChanged(int, bool, bool);
    void itemSelected(int amount,int role);
    void valueChanged(int amount,int role);
    void countsChanged(int amount, int role);

    void releaseCloneSatellite();

protected:

    int cost_;
    int index_;

    QList<ModelItemBase *> branches_;

private:

    int initCost_;

};


#endif // MODELITEMBASE_H
