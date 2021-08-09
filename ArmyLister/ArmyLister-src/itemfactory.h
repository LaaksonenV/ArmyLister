#ifndef ITEMFACTORY_H
#define ITEMFACTORY_H

#include <QMap>

#define CONTROL_C "\\:/#~^+*?"

class ModelItemBase;
class ModelItemBasic;
class ItemSatellite;
class ModelItemCategory;
class QTextStream;
struct TempTreeModelItem;
struct PointContainer;
struct UnitContainer;


class ItemFactory
{

public:

    enum ControlCharacters
    {
        eControl_Global
        , eControl_Local
        , eControl_Print
        , eControl_Model
        , eControl_Take
        , eControl_GlobalModelLimit
        , eControl_GlobalCheck
        , eControl_CreateUnitValues
        , eControl_SelectPoints
        , eControl_Comment
        , eControl_Text
        , eControl_Slot
        , eControl_GlobalLimit
        , eControl_Limit
        , eControl_Multiple
        , eControl_Selection
        , eControl_ModelMax
        , eControl_UnitCat
        , eControl_ItemCat
        , eControl_WildCard
        , eControl_ForAll
        , eControl_AutoCheck
        , eControl_Replace
        , eControl_AlwaysCheck
        , eControl_ModelMin
        , eControl_NOT
        , eControl_GroupStart
        , eControl_GroupEnd
        , eControl_SlotStart
        , eControl_SlotEnd
        , eControl_ListStart
        , eControl_ListEnd
        , eControl_TagStart
        , eControl_TagEnd
        , eControl_Retinue
        , eControl_OrgMainSplit
        , eControl_OrgPointSplit
        , eControl_CreateList
        , eControl_LocalLimit
        , eControl_CreateGroup
        , eControl_NULL = -1
    };

    static const QChar& CCharacter(enum ControlCharacters c);
    static const ControlCharacters ControlC(const QChar &c);

    ItemFactory();
    ~ItemFactory();

    /*!
     * \brief addArmyFile Base function to create itemlist
     * \param top TopModelItem to build the list under
     * \param fileName QString name of the main file
     * \param newArmy boolean clear old data
     * \return Success of reading neccessary files and creating the list
     *
     * Base function to call for making a new list under an already
     * built TopModelItem. Empty lines in the file are only allowed
     * between categories.
     */
    bool addArmyFile(ModelItemBase *top, const QString &fileName,
                     bool newArmy = true,
                     const QStringList &tags = QStringList());





private:

    static inline const QList<QChar> s_controlcharacters
    {
        '§','!','"','@','#','£','€','¤','$','%','&','/','=','?','+','\\',
        '^','¨','~','.',';',',',':','_','|','{','}','(',')','[',']',
        '<','>','R','#',';','L','#','G'
        ,'\0' // NULL, leave as last
        // Unused ' ´ ` - ½
    };

    /*!
     * \brief parseMainList Recursively reads through the main file to
     * create the initial tree
     * \param line Current line of file to process
     * \param str Reading stream of main file
     * \param parentBranch Parent branch for new one
     * \param prev A StringList containing all previously processed
     * special information
     * \return Next line of file to process or terminate process
     *
     * This function will create an initial modeltreebranch for a single
     * category just from the information in the main file, to later be
     * compiled with theinformation in the 'include' files. It will work
     * recursively on the file until a new category begins.
     * \a prev will contain all previous special information and entry
     * names in the branch. Control elements and special information are
     * extracted with separate functions.
     *
     * \sa parseControl() and parseSpecial()
     */
    QString parseMainList(const QString &line, QTextStream &str,
                          TempTreeModelItem *parentBranch,
                          const QStringList &supTags);

    void parseGlobalControl(QString &line, QTextStream &str,
                               const QStringList &supTags, bool &retinue);

    /*!
     * \brief parseIncludes Read the stream for included files
     * \param str Main file to read
     * \return Last line of file read, that wasn't indented
     *
     * Include files contain possible tables and lists for item prices,
     * unit model counts and equipment lists. This function will pass
     * filenames read from the mainfile to \sa parseFile()
     */
    QString parseIncludes(QTextStream &str,
                          const QStringList &tags = QStringList());

    /*!
     * \brief parseFile Function for handling included files
     * \param fileName
     *
     *
     */
    void parseFile(const QString &fileName,
                   const QStringList &supTags = QStringList());

    QString parseOrganisation(QTextStream &str);

    /*!
     * \brief parseControl Remove and return control elements from text
     * \param text Reference to text to work on
     * \return List of control elements found
     *
     * Control elements are found at the beginning of a line, and start
     * with an exclamation point '!'. These are removed from \a text and
     * returned as a separate list.
     */
    QStringList parseControl(QString &text, QChar ctrl = QChar());

    /*!
     * \brief parseSpecial Remove and return special information from
     * text
     * \param textReference to text to work on
     * \return List of information entries found
     *
     * Special information is placed after control element or the entry
     * name, and are closed in <>. Many special information entries may
     * be put in a single line. All these are removed from \a text and
     * returned as a separate list.
     *
     * Call parseSpecial() only after parseControl(), as control element
     * may contain names encased in <> also
     */
    QStringList parseTags(QString &text);

    /*!
     * \brief parseList Simple function to read file for lists
     * \param str File to read
     * \param line Previously read line
     * \return  Last line read that is not indented
     *
     * Lists start with a name in [], and entries are indented after
     * that. Entries are passed through parseTableEntry()
     * in case they contain extra price information.
     * This function will also create global limiters of items when
     * neccessary
     */
    QString parseList(QTextStream &str, QString line, const QStringList &suptag);
    QString parseGroup(QTextStream &str, QString line);


    QString parseTable(QTextStream &str, QString line);

    void compileCategories(const TempTreeModelItem *temproot,
                           ModelItemBase *root);
    ModelItemBasic *compileUnit(const TempTreeModelItem *tempknot,
                     ModelItemBase *trunk);
    void compileItems(const TempTreeModelItem *tempknot,
                      ModelItemBase *trunk,
                      const UnitContainer *ucont,
                      QMap<QString, ItemSatellite *> &localGroupLimiters,
                      ItemSatellite *takeLimiter = nullptr);
    void compileList(const TempTreeModelItem *tempknot,
                     ModelItemBase *trunk,
                     const UnitContainer *ucont,
                     QMap<QString, ItemSatellite *> &localGroupLimiters,
                     ItemSatellite *takeLimiter = nullptr);
    void compileSelection(const TempTreeModelItem *tempknot,
                          ModelItemBase *trunk,
                          const UnitContainer *ucont,
                          QMap<QString, ItemSatellite *> &localGroupLimiters,
                          ItemSatellite *takeLimiter = nullptr);
    void compileSlots(const TempTreeModelItem *tempknot,
                      ModelItemBase *trunk,
                      const QMap<QString, int> &slotmap,
                      const QStringList &defaults,
                      const UnitContainer *ucont,
                      QMap<QString, ItemSatellite *> &localGroupLimiters,
                      ItemSatellite *takeLimiter = nullptr);
    bool checkTagLimiter(const QStringList &limitingTags,
                         const QStringList &tags);
    ItemSatellite *checkControls(const TempTreeModelItem *tempknot,
                                 ModelItemBasic *knot,
                                 QMap<QString, ItemSatellite *>
                                 &localGroupLimiters);
    const UnitContainer *checkCost(ModelItemBasic *knot, const QString &text,
                                   int &models,
                                   const UnitContainer *ucont = nullptr,
                                   int slot = 0);

    /*!
     * \brief countItems searches the cost of the given text item
     * \param text Name of the item(s)
     * \param special Special entries of the text
     * \return Summed cost of all items in text
     *
     * Entries in 40k lists have multiple items in one entry. This
     * function will split the text by commas and ampersands, and
     * look through the tables for each, noting down their prices and
     * returning them. If an item is preceded by number, the price
     * will be multiplied by that, unless the tables contain an item with
     * common denominator.
     */
    int countItems(QString text,
                   const UnitContainer *ucont);

    void clear();
private:
    QList<PointContainer*> pointList_;
    QMap<QString, UnitContainer *> unitMap_;
    QMap<QString, QList<TempTreeModelItem*> > listMap_;
    QMap<QString, int> localLimitGroupMap_;
    QMap<QString, ItemSatellite*> globalLimitMap_;
    QMap<QString, int> nameMap_;
    QMap<QString, TempTreeModelItem*> retinueMap_;

};

/*!
 * \brief The TempTreeModelItem struct holds each entrie's data in text
 * in a tree form
 *
 * \internal
 */
struct TempTreeModelItem
{
    TempTreeModelItem(const QString &text = QString(),
                      const QStringList &ctrl = QStringList(),
                      const QStringList &spec = QStringList(),
                      TempTreeModelItem *parent = nullptr);

    void addChild(TempTreeModelItem *item);



    QString text_;
    QStringList control_;
    QStringList tags_;
    QList<TempTreeModelItem*> unders_;

};

/*!
 * \brief The PointContainer struct holds specific point data from a list
 *
 * TO BE REVISED
 *
 * \internal
 */
struct PointContainer
{
    QString text_;
    QStringList special_;
    int points_;
    int min_;
    int max_;
    int specialPoints_;
    int multiplier_;

    PointContainer()
        : text_(QString())
        , special_(QStringList())
        , points_(0)
        , min_(0)
        , max_(-1)
        , specialPoints_(0)
        , multiplier_(1)
    {
    }

};

struct ItemContainer
{
    int points_;
};

struct UnitContainer
{
    int points_;
    int min_;
    int max_;
    int specialPoints_;
    QMap<QString,ItemContainer> items_;

    UnitContainer()
        : points_(0)
        , min_(0)
        , max_(-1)
        , specialPoints_(0)
        , items_(QMap<QString,ItemContainer>())
    {
    }
};

#endif // ITEMFACTORY_H
