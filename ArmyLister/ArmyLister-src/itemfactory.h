#ifndef ITEMFACTORY_H
#define ITEMFACTORY_H

#include <QMap>

#define CONTROL_C "\\:/#~^+*?"

class ModelItemBase;
class ModelItemBasic;
class ModelSatelliteLimiter;
class ModelItemCategory;
class Settings;
class QTextStream;
struct TempTreeModelItem;
struct PointContainer;


class ItemFactory
{

public:
    ItemFactory(Settings *set);
    ~ItemFactory();

    /*!
     * \brief addArmyFile Base function to create itemlist
     * \param top TopModelItem to build the list under
     * \param fileName QString name of the main file
     * \return Success of reading neccessary files and creating the list
     *
     * Base function to call for making a new list under an already
     * built TopModelItem. Empty lines in the file are only allowed
     * between categories.
     */
    bool addArmyFile(ModelItemBase *top, const QString &fileName);

private:

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
                          QStringList prev = QStringList());

    /*!
     * \brief parseIncludes Read the stream for included files
     * \param str Main file to read
     * \return Last line of file read, that wasn't indented
     *
     * Include files contain possible tables and lists for item prices,
     * unit model counts and equipment lists. This function will pass
     * filenames read from the mainfile to \sa parseFile()
     */
    QString parseIncludes(QTextStream &str);

    QString mapOrganisation(QTextStream &str);

    /*!
     * \brief parseControl Remove and return control elements from text
     * \param text Reference to text to work on
     * \return List of control elements found
     *
     * Control elements are found at the beginning of a line, and start
     * with exclamation point '!'. These are removed from \a text and
     * returned as a separate list.
     */
    QStringList parseControl(QString &text);

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
    QStringList parseSpecial(QString &text);

    void compileCategories(TempTreeModelItem *temproot,
                           ModelItemBase *root);
    void compileUnit(TempTreeModelItem *tempknot,
                     ModelItemCategory *trunk);
    void compileItems(TempTreeModelItem *tempknot,
                      ModelItemBase *trunk,
                      const QMap<QString, int> &slotmap,
                      ModelSatelliteLimiter *sharedSat = nullptr);
    void compileList(TempTreeModelItem *tempknot,
                     ModelItemBase *trunk,
                     const QMap<QString, int> &slotmap,
                     ModelSatelliteLimiter *sharedSat = nullptr);
    void compileSelection(TempTreeModelItem *tempknot,
                          ModelItemBase *trunk);
    ModelSatelliteLimiter *checkControls(TempTreeModelItem *tempknot,
                                         ModelItemBasic *knot);

    /*!
     * \brief parseFile Function for handling included files
     * \param fileName
     *
     *
     */
    void parseFile(const QString &fileName);

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
    QString parseList(QTextStream &str, QString line);

    /*!
     * \brief parseTableEntry Creates price table entry for a given string
     * \param line Given string
     * \return Given string with price table information removed
     *
     * This function will create a price table entry for a line, if
     * the line contains price information. It will return the line
     * without price information and special information used by the
     * table
     */
    QString parseTableEntry(const QString &line);

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
     * same denominator.
     */
    int countItems(QString text,
                    const QStringList &special);

    /*!
     * \brief findTableEntry finds the best possible table entry
     * \param text Name of the item to look for
     * \param special special entries for the item
     * \return Pointcontainer whose name, specials and multipliers match,
     * or null pointer if none found
     */
    PointContainer *findTableEntry(QString &text,
                            const QStringList &special);

    void clear();
private:
    Settings *_settings;

    QList<PointContainer*> _pointList;
    QMap<QString, QStringList> _listList;
    QMap<QString, ModelSatelliteLimiter*> _globalLimiters;
    QMap<QString, int> _nameMap;
};

struct TempTreeModelItem
{
    TempTreeModelItem(const QString &text = QString(),
                      const QStringList &ctrl = QStringList(),
                      const QStringList &spec = QStringList(),
                      TempTreeModelItem *parent = nullptr);

    void addChild(TempTreeModelItem *item);



    QString _text;
    QStringList _control;
    QStringList _spec;
    QList<TempTreeModelItem*> _unders;

};

struct PointContainer
{
    QString text;
    QStringList special;
    int points;
    int min;
    int max;
    int specialPoints;
    int multiplier;

    PointContainer()
        : text(QString())
        , special(QStringList())
        , points(0)
        , min(0)
        , max(0)
        , specialPoints(0)
        , multiplier(1)
    {
    }

};

#endif // ITEMFACTORY_H
