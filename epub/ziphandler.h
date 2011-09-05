#ifndef ZIPHANDLER_H
#define ZIPHANDLER_H

#include <QObject>
#include <QFile>
#include <QStringList>
#include "minizip/unzip.h"

struct FileInZip{
    unz_file_info fileinfo;
    unz_file_pos filepos;
    QString filename;
    QString filenameWithoutPath;
};


class ZipHandler : public QObject{
    Q_OBJECT
public:
    explicit ZipHandler(const QString &filename, QObject *parent = 0);
    ~ZipHandler();

    bool isValid() const;
    QStringList files() const;
    QByteArray fileAsData(const QString &name);
    QString fileAsString(const QString &name);

private:
    bool read_zipinfo();
    bool readCurrentFileInfo();

    bool fileByName(const QString &name, FileInZip &f);

    QList<FileInZip> m_files;

    QString m_filename;
    QFile m_file;
    unzFile m_unzfile;
    unz_global_info m_fileinfo;

    bool m_valid;


};

#endif
