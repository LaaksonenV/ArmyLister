#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

#define SETTING_ARMY_GROUP "Armies"
#define SETTING_LIST_GROUP "Lists"

#include "QColor"

class Settings : protected QSettings
{
public:
    enum Colours
    {
        ColorOk,
        ColorNot,
        ColorNeutral
    };

    static const QColor& Color(Colours c);

    enum Numbers
    {
        DefaultArmySize9A,
        DefaultArmySize40k,
        PlainTextWidth
    };

    static const unsigned short Number(Numbers i);

    Settings();

    void addMultiValue(const QString &newValue,
                       const QString &group);
    QStringList multiValue(const QString &group);

    bool isUSAllowed()const{return _allowUnderStrength;}

    static inline unsigned short itemHeight = 24;
    static inline unsigned short itemHPos = 18;
    static inline unsigned short textFontSize = 15;
    static inline unsigned short expandButtonSize = 20;
    static inline unsigned short costFieldWidth = 50;
    static inline QString font = "Gabriola";

private:
    static inline QList<QColor> _colours
    {
        QColor("#bbffbb"),
                QColor("#ffaaaa"),
                QColor("#dddddd")
    };

    static inline QList<unsigned short> _numbers
    {
        4500,
        2000,
        80
    };

private:
    bool _allowUnderStrength;
};

#endif // SETTINGS_H
