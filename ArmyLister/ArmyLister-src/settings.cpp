#include "settings.h"

const QColor& Settings::Color(enum Colours c)
{
    return _colours.at(c);
}

const unsigned short Settings::DefaultSize(enum Numbers i)
{
    return _sizes.at(i);
}

const unsigned short Settings::ItemMeta(enum ItemMetas i)
{
    return _itemmeta.at(i);
}

const QFont& Settings::Font(enum Fonts i)
{
    return _fonts.at(i);
}

const QString& Settings::Strng(enum Strings s)
{
    return _strings.at(s);
}

Settings::Settings()
    : QSettings("settings.ini", QSettings::IniFormat)
    , _allowUnderStrength(false)
{
}

void Settings::addMultiValue(const QString &newValue,
                   const QString &group)
{
    QStringList values;
    QString val;

    int size = beginReadArray(group);
    for (int i = 0; i < size; ++i)
    {
        setArrayIndex(i);
        val = value("value").toString();
        if (val == newValue)
        {
            endArray();
            return;
        }
        values << val;
    }
    endArray();

    if (values.contains(newValue))
        return;

    values << newValue;

    beginWriteArray(group, size+1);
    for (int i = 0; i < size+1; ++i)
    {
        setArrayIndex(i);
        setValue("value", values.at(i));
    }
    endArray();
}

QStringList Settings::multiValue(const QString &group)
{
    QStringList ret;
    int size = beginReadArray(group);
    for (int i = 0; i < size; ++i)
    {
        setArrayIndex(i);
        ret << value("value").toString();
    }
    endArray();
    return ret;
}
