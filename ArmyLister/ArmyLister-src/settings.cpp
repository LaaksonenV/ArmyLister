#include "settings.h"

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
