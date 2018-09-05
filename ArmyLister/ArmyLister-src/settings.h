#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

#define SETTING_ARMY_GROUP "Armies"
#define SETTING_LIST_GROUP "Lists"

class Settings : protected QSettings
{
public:
    Settings();

    void addMultiValue(const QString &newValue,
                       const QString &group);
    QStringList multiValue(const QString &group);

    bool isUSAllowed()const{return _allowUnderStrength;}

    const unsigned short itemHeight = 24;
    const unsigned short itemHPos = 18;
    const unsigned short textFontSize = 15;
    const unsigned short expandButtonSize = 20;
    const unsigned short costFieldWidth = 50;
    const char * font = "Gabriola";

private:
    bool _allowUnderStrength;
};

#endif // SETTINGS_H
