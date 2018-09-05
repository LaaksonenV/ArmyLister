#ifndef FILEIO_H
#define FILEIO_H

#include <QObject>

class FileIO
{
public:
    FileIO();

private:
    QString _filename;
    QStringList _buffer;
};

#endif // FILEIO_H
