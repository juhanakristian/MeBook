#ifndef LIBRARY_H
#define LIBRARY_H

#include <QSqlDatabase>
#include <QSqlRelationalTableModel>
#include <QSqlRecord>
#include <QSqlRelation>
#include <QSqlQuery>
#include <QVariant>

#include <QDeclarativeEngine>
#include <QDeclarativeImageProvider>
#include <QDeclarativeExtensionPlugin>
#include <QList>

#include <QDebug>

#include "book.h"
#include "shared.h"
#include "settings.h"

class QObject;
class QSqlTableModel;
class QNetworkAccessManager;

namespace MeBook{

    #define MLibrary Library::getInstance()


    class CoverImageProvider : public QDeclarativeImageProvider{
    public:
        CoverImageProvider() : QDeclarativeImageProvider(QDeclarativeImageProvider::Pixmap){}

        QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize){
            int width = 100;
            QSqlQuery selectQuery("SELECT * FROM books WHERE id=:id");
            selectQuery.bindValue(":id", id);
            selectQuery.exec();
            if(selectQuery.next()){
                QByteArray bat = selectQuery.value(5).toByteArray();
                QPixmap covert;
                covert.loadFromData(bat, "PNG");
                if(!covert.isNull()){
                    covert.scaledToWidth(requestedSize.width());
                    *size = covert.size();
                    return covert;
                }
                
            }

            QPixmap defaultCover(width, 150);
            defaultCover.fill();
            *size = defaultCover.size();

            return defaultCover;

        }
    };

    class RoleSqlTableModel : public QSqlTableModel{
        Q_OBJECT
    public:
        RoleSqlTableModel(QObject *parent = 0) : QSqlTableModel(parent), sortColumn(-1){}
        ~RoleSqlTableModel(){}
        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const{
            if(!cache.contains(index.row()))
                return QVariant();

            QHash<int, QVariant> t = cache[index.row()];

            if(t.contains(role))
                return t[role];

            return QVariant();

            if(!index.isValid())
                return QVariant();
        }

        //Model data is cached so we don't have to load it all the time
        void reload() {


            QSqlTableModel m;
            m.setTable(this->tableName());
            if(sortColumn != -1)
                m.sort(sortColumn, Qt::AscendingOrder);
            else
                m.select();

            for(int i = 0; i < m.rowCount(); ++i) {
                QSqlRecord r = m.record(i);
                QHash<int, QVariant> h;
                for(QHash<int, QByteArray>::iterator iter = roles.begin();
                    iter != roles.end(); ++iter) {
                    QVariant v = r.value(QString(iter.value()));
                    h[iter.key()] = v;
                }

                cache[i] = h;
            }

            select();

        }

        void generateRoleNames(){
            roles.clear();
            for(int i = 0; i < this->columnCount(); i++){
                roles[Qt::UserRole + i] = this->headerData(i, Qt::Horizontal).toByteArray();
            }
            setRoleNames(roles);
        }

        Q_INVOKABLE void sortByColumn(int column) {
            sortColumn = column;
            reload();
        }
    private:
        QHash<int, QByteArray> roles;
        QHash<int, QHash<int, QVariant> > cache;
        int sortColumn;
    };



    //Library handles saving books, bookmarks and annotations to a database.
    class Library : public QObject{
        Q_OBJECT
    public:
        static Library &getInstance();    
        //Library is initialized with a path to a folder and filepath to a database.
    //    Library(const QString &lD, const QString &databasename, QObject *parent = 0);

        //Loads library by calling loadBooksfromdb and scanDirectory.
        void loadLibrary();

        //Recursively scans a directory tree for epub-files.
        void scanDirectory(const QString&);

        //Loads books from database and creates Book-objects.
        void loadBooksfromdb();
        void saveBooktodb(Book*);

        Book *bookWithId(const QString &id);

        //Returns a QList of Book-pointers.
        //This is not used anymore.
        Q_INVOKABLE QList<Book*> getBooks(Settings::Sorting sorting = Settings::None);

        RoleSqlTableModel *booksModel(){
            return m_booksModel;
        }

    private:
        Library();
        ~Library();
        Library(const Library&){}
        Library &operator=(const Library&){}
        static Library instance;    

        QStringList filenames;
        void connectdb(const QString&);
        void saveBookProgress(Book*);
        void loadBookProgress(Book*);
        void loadBookmarks(Book*);
        void loadAnnotations(Book*);
        //Check if books in database still exists and if a there's any new files in book directory.
        bool checkForChanges();

        class BookDownload
        {
        public:
            QString filename;
            QString url;
            bool open;
            QNetworkReply *reply;
            inline bool operator==(const BookDownload &d) const
            {
                return (d.url == url);
            }
        };

        QList<BookDownload> m_downloadQueue;
        QNetworkAccessManager *m_manager;

        QSqlDatabase database;
        QString libraryDirectory;

        RoleSqlTableModel *m_booksModel;

        QList<Book*> books;

    public slots: 
        //Books can be added to library either with a pointer to Book-object (usually done when loading a book from a catalog) or with a filename.
        void addToLibrary(Book*, bool = true);
        Q_INVOKABLE void addToLibrary(const QString&);
        void removeFromLibrary(Book*);

        void deleteAnnotation(Book*, Annotation);
        void deleteBookmark(Book*, Bookmark);
        void saveBookmark(const Bookmark&, Book*);
        void saveAnnotation(const Annotation&, Book*);

        void bookDownloaded(QNetworkReply *reply);

    signals:
        //Emitted when a book has been added to library so the UI can be updated.
        void bookAdded(Book*);
        void bookRemoved(Book*);
    };

}

#endif
