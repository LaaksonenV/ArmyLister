#include "settings.h"

const QColor& Settings::Color(enum Colours c)
{
    return s_colours_.at(c);
}

const unsigned short Settings::DefaultSize(enum Numbers i)
{
    return s_sizes_.at(i);
}

const unsigned short Settings::ItemMeta(enum ItemMetas i)
{
    return s_itemmeta_.at(i);
}

const QFont& Settings::Font(enum Fonts i)
{
    return s_fonts_.at(i);
}

const QString& Settings::Strng(enum Strings s)
{
    return s_strings_.at(s);
}

Settings::Settings()
    : QSettings("settings.ini", QSettings::IniFormat)
    , allowUnderStrength_(false)
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
