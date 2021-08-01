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

    bool isUSAllowed()const{return allowUnderStrength_;}

    enum Colours
    {
        eColor_Ok
        , eColor_Not
        , eColor_Neutral
    };

    static const QColor& Color(enum Colours c);

    enum Numbers
    {
        eDefaultArmySize_9A
        , eDefaultArmySize_40k
    };

    static const unsigned short DefaultSize(enum Numbers i);

    enum ItemMetas
    {
        eItem_PlainTextWidth
        , eItem_Height
        , eItemHPos
        , eItem_OrgFontSize
        , eItem_FontSize
        , eItem_PrintFontSize
        , eItem_SpinnerFontSize
        , eItem_ExpandButtonSize
        , eItem_CostFieldWidth
        , eItem_MouseHoldTime
    };

    static const unsigned short ItemMeta(enum ItemMetas i);

    enum Fonts
    {
        eFont_ItemFont
        , eFont_OrgFont
        , eFont_SpinnerFont
        , eFont_PrintFont
    };

    static const QFont& Font(enum Fonts i);

    enum Strings
    {
        eDefault_ListFolder
        , eDefault_AppendFolder
    };

    static const QString& Strng(enum Strings s);

private:
    static inline QList<QColor> s_colours_
    {
        QColor("#bbffbb")
        , QColor("#ffaaaa")
        , QColor("#dddddd")
    };

    static inline QList<unsigned short> s_sizes_
    {
        4500
        , 2000
    };

    static inline QList<unsigned short> s_itemmeta_
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
        , 2000 //MouseHoldTime
    };

    static inline QList<QFont> s_fonts_
    {
        QFont("Gabriola",ItemMeta(eItem_FontSize),QFont::Bold)
        , QFont("Gabriola",ItemMeta(eItem_OrgFontSize),QFont::Bold)
        , QFont("Calibri",ItemMeta(eItem_SpinnerFontSize),QFont::Medium)
        , QFont("Consolas",ItemMeta(eItem_PrintFontSize))
    };

    static inline QList<QString> s_strings_
    {
        ".\\"
        , ".\\Appends"
    };

private:
    bool allowUnderStrength_;
};

#endif // SETTINGS_H
