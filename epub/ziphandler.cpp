#include "ziphandler.h"

#include <QtEndian>
#include <QDebug>
#include <QBuffer>
#include <QTextCodec>


ZipHandler::ZipHandler(const QString &filename, QObject *parent) : 
    QObject(parent),
    m_filename(filename),
    m_file(filename),
    m_valid(false)
{
    m_unzfile = unzOpen(m_filename.toUtf8().constData());
    if(m_unzfile)
        m_valid = read_zipinfo();
}

ZipHandler::~ZipHandler()
{
    if(m_unzfile)
        unzClose(m_unzfile);
}

QStringList ZipHandler::files() const
{
    QStringList l;
    QListIterator<FileInZip> iter(m_files);
    while(iter.hasNext()){
        l << iter.next().filename;
    }

    return l;
}

QByteArray ZipHandler::fileAsData(const QString &name)
{
    FileInZip fiz;
    if(!fileByName(name, fiz))
        return QByteArray();

    int error;

    // error = unzLocateFile(m_unzfile, qPrintable(name), 2);
    // if(error != UNZ_OK){
    //     qDebug() << "Error: Can't find file!";
    //     return QByteArray(); 
    // }
 
    error = unzGoToFilePos(m_unzfile, &fiz.filepos);
    if(error != UNZ_OK){
        qDebug() << "Error moving to file pos!";
        return QByteArray(); 
    }
    error = unzOpenCurrentFile(m_unzfile);
    if(error != UNZ_OK){
        qDebug() << "Error opening file in zip:" << fiz.filename;
        return QByteArray(); 
    }

    
    void *buffer;
    unsigned int bufferSize = 4096;
    buffer = (void*)malloc(bufferSize);

    QByteArray ba;
    int result = 1;
    while(result){
        result = unzReadCurrentFile(m_unzfile, buffer, bufferSize);
        if(result <= 0)
            break;
        ba.append((char*)buffer, result);
    }
    error = unzCloseCurrentFile(m_unzfile);
 
    free(buffer);
    
    qDebug() << "File loaded:" << name << ba.size();

    return ba;
}

QString ZipHandler::fileAsString(const QString &name)
{
    FileInZip fiz;
    if(!fileByName(name, fiz))
        return QString();

    int error;


    QByteArray ba = fileAsData(name);
    QTextCodec *codec = QTextCodec::codecForHtml(ba, QTextCodec::codecForName("UTF-8"));

    return codec->toUnicode(ba);
 
}


bool ZipHandler::isValid() const
{
    return m_valid;
}

bool ZipHandler::read_zipinfo()
{
    int error;

    error = unzGetGlobalInfo(m_unzfile, &m_fileinfo);
    if(error != UNZ_OK){
        qDebug() << "Error:" << error;
        return false;
    }

    for(int i = 0; i < m_fileinfo.number_entry; i++){
        if(!readCurrentFileInfo()){
            qDebug() << "Error reading fileinfo for entry:"<<i;
        }

        //Break so we don't try to move past the last file.
        if(i == m_fileinfo.number_entry-1)
            break;

        error = unzGoToNextFile(m_unzfile);
        if(error != UNZ_OK){
            qDebug() << "Error:" << error;
            return false;
        }
    }

    return true;
}


bool ZipHandler::readCurrentFileInfo()
{
    char filename[256];
    int error;
    unz_file_info fileInfo;

    error = unzGetCurrentFileInfo(m_unzfile, &fileInfo, filename, sizeof(filename), NULL, 0, NULL, 0);

    if(error != UNZ_OK){
        qDebug() << "Error reading file info: " << error;
        return false;
    }

    QString fn(filename);
    if(fn.endsWith("/")){
        qDebug() << "File is a directory!" << fn;
        return false;
    }

    FileInZip fiz;
    fiz.filename = fn;
    fiz.fileinfo = fileInfo;

    QStringList l = fn.split("/");
    fiz.filenameWithoutPath = l.last();

    error = unzGetFilePos(m_unzfile, &fiz.filepos);
    if(error != UNZ_OK){
        qDebug() << "Error reading file position:" << error;
        return false;
    }

    m_files << fiz;
    
    return true;
}

bool ZipHandler::fileByName(const QString &name, FileInZip &f)
{ 
    QListIterator<FileInZip> iter(m_files);
    while(iter.hasNext()){
        FileInZip fiz = iter.next();
        if(fiz.filename == name){
            f = fiz;
            return true;
        }
    }

    return false;
}
