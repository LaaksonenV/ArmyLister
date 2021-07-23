#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

#define SETTING_ARMY_GROUP "Armies"
#define SETTING_LIST_GROUP "Lists"

#include <QColor>
#include <QFont>

class Settings : protected QSettings
{
public:
    Settings();

    void addMultiValue(const QString &newValue,
                       const QString &group);
    QStringList multiValue(const QString &group);

    bool isUSAllowed()const{return _allowUnderStrength;}

    enum Colours
    {
        ColorOk
        , ColorNot
        , ColorNeutral
    };

    static const QColor& Color(enum Colours c);

    enum Numbers
    {
        DefaultArmySize9A
        , DefaultArmySize40k
    };

    static const unsigned short DefaultSize(enum Numbers i);

    enum ItemMetas
    {
        PlainTextWidth
        , ItemHeight
        , ItemHPos
        , OrgFontSize
        , ItemFontSize
        , PrintFontSize
        , SpinnerFontSize
        , ExpandButtonSize
        , CostFieldWidth
    };

    static const unsigned short ItemMeta(enum ItemMetas i);

    enum Fonts
    {
        ItemFont
        , OrgFont
        , SpinnerFont
        , PrintFont
    };

    static const QFont& Font(enum Fonts i);

    enum Strings
    {
        DefaultListFolder
        , DefaultAppendFolder
    };

    static const QString& Strng(enum Strings s);

private:
    static inline QList<QColor> _colours
    {
        QColor("#bbffbb")
        , QColor("#ffaaaa")
        , QColor("#dddddd")
    };

    static inline QList<unsigned short> _sizes
    {
        4500
        , 2000
    };

    static inline QList<unsigned short> _itemmeta
    {
        80 //PlainTextWidth
        , 24 //ItemHeight
        , 18 //ItemHPos
        , 20 //OrgFontSize
        , 15 //ItemFontSize
        , 11 //PrintFontSize
        , 12 //SpinnerFontSize
        , 20 //ExpandButtonSize
        , 50 //CostFieldWidth
    };

    static inline QList<QFont> _fonts
    {
        QFont("Gabriola",ItemMeta(ItemFontSize),QFont::Bold)
        , QFont("Gabriola",ItemMeta(OrgFontSize),QFont::Bold)
        , QFont("Calibri",ItemMeta(SpinnerFontSize),QFont::Medium)
        , QFont("Consolas",ItemMeta(PrintFontSize))
    };

    static inline QList<QString> _strings
    {
        ".\\"
        , ".\\Appends"
    };

private:
    bool _allowUnderStrength;
};

#endif // SETTINGS_H
