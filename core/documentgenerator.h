#ifndef DOCUMENTGENERATOR
#define DOCUMENTGENERATOR

#include <QHash>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkProxy>
#include <QPixmap>

namespace MeBook{

    //DocumentGenerator is the base class for filetype support.
    //When adding support for a filetype one subclasses DocumentGenerator and provides implementations for the pure virtual functions.
    //DocumentGenerator emits the data with signals which are connected to Book-class's corresponding slots.
    class DocumentGenerator : public QNetworkAccessManager{
        Q_OBJECT
    public:
        DocumentGenerator(const QString &filename, QObject *parent = 0);
        ~DocumentGenerator();

        virtual void readMetaData() = 0;
        virtual void readContents() = 0;

    protected:
        virtual QNetworkReply* createRequest(Operation op, const QNetworkRequest &req, QIODevice* outgoingData = 0) = 0;

        const QString bookFilename;


    signals:
        void setTitle(QString);
        void setAuthor(QString);
        void setPublisher(QString);
        void setDescription(QString);
        void setSubject(QString);
        void setPublishDate(QString);
        void setCategory(QString);
        void setCopyright(QString);

        void addChapter(QString, QString*);
        void addTOCItem(QString, QString);
        void addSection(QString*, QString);
        void addCoverImage(QPixmap);
    };

}
#endif
