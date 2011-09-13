#ifndef BOOK_H
#define BOOK_H

#include <QList>

#include <QSqlQuery>
#include <QSqlError>

#include "epubgenerator.h"
#include "bookmark.h"
#include "annotation.h"

class QObject;

namespace MeBook{

    //Struct for Book-class's internal book data handling.
    struct BookSectionData{
    //Pointer to sections html-content.
    QString *content;
    //Sections title for Table of Contents
    QString label;
    //Sections link or filename inside epub-file
    QString link;
    //Sections index number in Table of Contents
    //If no TOC is found TOCIndex is same as section number and position in sections(QList)
    int TOCIndex;
    };


    //Book-class holds all the data related to a book.
    //It has a pointer to a DocumentGenerator which does the file handling.
    //Book-object can be initialized by giving a filename.
    //Metadata can be set from downloaded info or epub-file.
    class Book : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(QString title READ getTitle NOTIFY titleChanged)
        Q_PROPERTY(QString author READ getAuthor NOTIFY authorChanged)
    public:
    Book();
    //If loadLater is true, book-content or metadata is not loaded when book is initialized.
    //Loading will have to be done later. This is useful when loading books and their metadata from library or a catalog.
    Book(const QString &bookFilename, bool loadLater = false);
    ~Book();

    void openBook(const QString &bookFilename);
    //Reads metadata from the epub file but not the actual content.
    //This is only used when opening a book that was not in the library and therefor no metadata was available.
    void loadMetaData();
    //Reads all the data from the epub file.
    void load();
    //Returns true if content has been read.
    bool isLoaded();

    //Returns length of book as number of characters.
    int getBookLength();

    /* Returns position of text fragment as int
     * If text is not found returns -1 
     */
    int getPositionInBook(const QString&);

    /* Returns paragraph at the given position
     * Position is a number between 0 and book length
     * If paragraph is not found returns an empty QString
     */
    QString getParagraphAtPosition(int, int&);

    /* These functions return the start position, lenght and
     * end position of the section in characters.
     */
    int getSectionStart(int);
    int getSectionLength(int);
    int getSectionEnd(int);

    /* Returns the section number of the section at given character position
     */
    int getSectionAtPosition(int);

    //Returns first section of the book.
    QString getCover();

    //Returns the html of the section with the given section number as const QString,
    //otherwise returns an empty QString.
    const QString getSection(int);

    const QString getSectionName(int);

    //Returns the whole book content as QString.
    //Sections are wrapped inside div with id Section + section number.
    QString getBook();
    //Returns Table of Contents as a QList of QStrings.
    QList<QString> getTOC();
    const QString getSectionOfTOCItem(int);
    //Returns section number of given TOC index.
    int getSectionNumberOfTOCIndex(int);

    //Set's the filename of the book.
    //This is only useful when downloading and saving the book.
    void setFilename(QString);
    //Returns the absolute filename of the book as QString.
    QString getFilename();

    //Returns true if book has a valid TOC.
    bool hasTOC();

    //Functions for accessing book metadata.
    //It is not guaranteed that all this info is contained in a epub-file.
    //For books that are in library metadata is set from database instead of the actual epub-file.
    //These functions return an empty QString if no data is available.
    const QString getTitle() const;
    const QString getAuthor() const;
    const QString getCategory() const;
    const QString getSubject() const;
    const QString getDescription() const;
    const QString getPublishDate() const;

    //Returns a QPixmap object if a cover image is available, otherwise
    //an empty QPixmap is returned.
    const QPixmap getCoverImage() const;
    int getNumberOfSections();

    //Get or set book's length in words.
    //Used when loading book info in catalogs. epub-data doesn't have this info.
    unsigned int getLengthInWords();
    void setLengthInWords(unsigned int);

    void progress(int &section, float &sectionProgress) const;
    void setProgress(int section, float sectionProgress);

    void loadBookmarks();
    QList<Bookmark> getBookmarks();
    void addBookmark(Bookmark);
    void removeBookmark(Bookmark&);

    QList<Annotation> getAnnotations();
    void addAnnotation(Annotation);
    Annotation* getAnnotation(const QString&);
    void removeAnnotation(Annotation&);

    //Returns true if book is already in library.
    bool isInLibrary();
    void changeLibraryStatus(bool);

    //Returns a QNetworkAccessManager object which a QWebPage object can use to access books files.
    //Without this QWebPage can't load css-stylesheets or images found in book's html.
    QNetworkAccessManager *getResourceManager();

private:
    QString title;
    QString author;
    QString publisher;
    QString description;
    QString subject;
    //Publish date is stored as QString because the format which it's in epub-files varies.
    QString publishDate;
    QString category;
    QString copyright;
    QString filename;


    QList<QString> toc;
    QList<BookSectionData> sections;

    QPixmap coverImage;

    QList<Bookmark> bookmarks;
    QList<Annotation> annotations;

    EpubGenerator *generator;


    int m_section;
    float m_sectionProgress;
    void loadProgress();
    void saveProgress() const;

    bool loaded;
    unsigned int lengthInWords;
    bool inLibrary;

    void setUpGenerator();


    public slots:
    //These slots are called from EpubGenerator which does the actual file handling.
    void setTitle(QString);
    void setAuthor(QString);
    void setPublisher(QString);
    void setDescription(QString);
    void setSubject(QString);
    void setPublishDate(QString);
    void setCategory(QString);
    void setCopyright(QString);

    void addTOCItem(QString, QString);
    void addSection(QString*, QString);

    void addCoverImage(QPixmap);

    void removeBookmark(int);
    void removeAnnotation(int);

    signals:
    //Emitted when a cover image is found. This allows UI elements to add cover image later.
    void coverImageAdded();

    void bookmarkAdded(const Bookmark&, Book*);
    void annotationAdded(const Annotation&, Book*);

    void bookmarkDeleted(const Bookmark&);
    void annotationDeleted(const Annotation&);

    void titleChanged();
    void authorChanged();
    };

}

#endif
