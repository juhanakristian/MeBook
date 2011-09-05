#ifndef EPUBGENERATOR_H
#define EPUBGENERATOR_H

#include <QHash>

#include "documentgenerator.h"
#include "epub.h"

namespace MeBook{

    class EpubResourceReply : public QNetworkReply
    {
        Q_OBJECT
    public:
        EpubResourceReply(QObject *parent, Epub *epub, const QNetworkRequest &req);
        qint64 bytesAvailable() const;
        void abort();
        bool isSequential() const { return true; }
    protected:
        qint64 readData(char *data, qint64 maxSize);
    private:
        Epub *m_epub;
        QUrl requestUrl;
        int contentSize;
        qint64 offset;
        char *buffer;
    };


    class EpubGenerator : public DocumentGenerator
    {
        Q_OBJECT
    public:
        EpubGenerator(const QString &filename, QObject *parent = 0);
        ~EpubGenerator();

        // QString getMetadata(enum epub_metadata type);
        void readMetaData();
        void readContents();
        char *readFile(const QString&, int&);

    protected:
        QNetworkReply* createRequest(Operation op, const QNetworkRequest &req, QIODevice* outgoingData = 0);

    private:
        QHash<QString, QString> epubDocument;
        Epub *m_epub;

    };


}
#endif
