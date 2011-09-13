#include "book.h"

#include <QDebug>
#include <QDomDocument>
#include <QDomNodeList>

#include <QSqlQuery>
#include <QSqlError>

using namespace MeBook;

Book::Book() : 
        filename(QString()),
        generator(0), 
        loaded(false), 
        lengthInWords(0),
        inLibrary(false),
        m_section(0),
        m_sectionProgress(0.0)
{

}

Book::Book(const QString &bookFilename, bool loadLater) : 
        filename(bookFilename),
        generator(0), 
        loaded(false), 
        lengthInWords(0),
        inLibrary(false),
        m_section(0),
        m_sectionProgress(0.0)
{
    if(!loadLater){
        setUpGenerator();
        openBook(filename);
    }

    loadProgress();
}


Book::~Book()
{
    delete generator;

    for(QList<BookSectionData>::iterator iter = sections.begin(); iter != sections.end(); ++iter){
        delete (*iter).content;
    }

    saveProgress();
}

void Book::openBook(const QString &bookFilename)
{
    filename = bookFilename;
    loadProgress();
    if(!generator)
        setUpGenerator();
    generator->readMetaData();
    generator->readContents();
    loaded = true; 
}

void Book::load()
{    
    if(!generator)
        setUpGenerator();
    generator->readMetaData();
    generator->readContents();
    loaded = true; 
}

void Book::setUpGenerator()
{
    generator = new EpubGenerator(filename);

    connect(generator, SIGNAL(setTitle(QString)), this, SLOT(setTitle(QString)));
    connect(generator, SIGNAL(setAuthor(QString)), this, SLOT(setAuthor(QString)));
    connect(generator, SIGNAL(setPublisher(QString)), this, SLOT(setPublisher(QString)));
    connect(generator, SIGNAL(setSubject(QString)), this, SLOT(setSubject(QString)));
    connect(generator, SIGNAL(setDescription(QString)), this, SLOT(setDescription(QString)));
    connect(generator, SIGNAL(setCategory(QString)), this, SLOT(setCategory(QString)));
    connect(generator, SIGNAL(setCopyright(QString)), this, SLOT(setCopyright(QString)));
    connect(generator, SIGNAL(addTOCItem(QString, QString)), this, SLOT(addTOCItem(QString, QString)));
    connect(generator, SIGNAL(addSection(QString*, QString)), this, SLOT(addSection(QString*, QString)));
    connect(generator, SIGNAL(addCoverImage(QPixmap)), this, SLOT(addCoverImage(QPixmap)));
}

void Book::loadMetaData()
{
    if(!generator)
        setUpGenerator();

    generator->readMetaData();
}

bool Book::isLoaded()
{
    return loaded;
}

int Book::getBookLength()
{
    QString bookString = getBook();
    bookString = bookString.remove(QRegExp("<[a-zA-Z\\/][^>]*>"));
    bookString = bookString.replace("\n", " ");
    bookString = bookString.replace("\r", " ");

    return bookString.length();
}

int Book::getPositionInBook(const QString &text)
{
    QString bookString = getBook();
    bookString = bookString.remove(QRegExp("<[a-zA-Z\\/][^>]*>"));
    bookString = bookString.replace("\n", " ");
    int pos = bookString.indexOf(text);
    if(pos > 0)
        return pos;

    return -1;
}

QString Book::getParagraphAtPosition(int pos, int &sectionNumber)
{
    QString bookString = getBook();
    bookString = bookString.remove(QRegExp("<[a-zA-Z\\/][^>]*>"));
    bookString = bookString.replace("\n", " ");
    
    QString temp = bookString;
    if(pos > 100 && (pos + 100) < getBookLength()){
        temp = temp.left(pos - 100); 
        temp = temp.right(100);
        
        for(int i = 0; i < sections.count(); ++i)
        {
            QString section = *sections.at(i).content;
            section = section.replace("\n", " ");
            section = section.remove(QRegExp("<[a-zA-Z\\/][^>]*>"));
            if(section.contains(temp)){
                QDomDocument document;
                document.setContent(*sections.at(i).content);
                sectionNumber = i;

                QDomNodeList list = document.elementsByTagName("P");
                if(list.count() == 0)
                    list = document.elementsByTagName("p");
                for(int a = 0; a < list.count(); ++a)
                {
                    QDomNode node = list.at(a);
                    QString text = node.firstChild().nodeValue();
                    text = text.replace("\n", " ");
                    if(text.contains(temp))
                        return text;
                }
                //If can't find the exact paragraph estimate.
                int ppos = section.indexOf(temp); 
                float point = (float)ppos / (float)section.length();

                int p = (float)list.count() * point;
                QDomNode node =  list.at(p);
                if(!node.isNull()){
                    QString text =  node.firstChild().nodeValue();
                    text = text.replace("\n", " ");
                    return text;
                }
            }
        }

        

    }

    return QString();


}

int Book::getSectionStart(int section)
{ 
    if(section >= 0 && section < sections.count()){
        int num = 0;
        for(int i = 0; i < section; ++i)
        {
            QString s = *sections.at(i).content;

            s = s.remove(QRegExp("<[a-zA-Z\\/][^>]*>"));
            s = s.replace("\n", " ");
            s = s.replace("\r", " ");

            num += s.length();
        }

        return num;
    }
    return -1;
}

int Book::getSectionLength(int section)
{

    if(section >= 0 && section < sections.count()){
        QString s = *sections.at(section).content;

        s = s.remove(QRegExp("<[a-zA-Z\\/][^>]*>"));
        s = s.replace("\n", " ");
        s = s.replace("\r", " ");

        return s.length();
    }
    return -1;
}

int Book::getSectionEnd(int section)
{
    if(section >= 0 && section < sections.count()){
        int num = getSectionStart(section);
        num += getSectionLength(section);
        return num;
    }
    return -1;
}

int Book::getSectionAtPosition(int pos)
{
    if(pos >= 0 && pos <= getBookLength()){ 
        for(int i = 0; i < sections.count(); ++i)
        {
            if(getSectionStart(i) <= pos && getSectionEnd(i) >= pos)
                return i;

            if(i == sections.count()-1)
                return i;
        }
    }

    return -1;
}



QString Book::getCover()
{
    if(!sections.isEmpty())
        return getSection(0);
    return QString();

}

const QString Book::getSection(int number)
{
    if(number < sections.count() && number >= 0)
        return *sections.at(number).content;

    return QString();
}


const QString Book::getSectionName(int number)
{
    if(number >= 0 && number < sections.count()){
        QString title = sections.at(number).label;
        return title;
    }


    return QString();
}

QString Book::getBook()
{
    QString ret;
    int i = 0;
    for(QList<BookSectionData>::iterator iter = sections.begin(); iter != sections.end(); ++iter)
    {
        ret += "<div id=section"+QString::number(i)+">";
        ret += *(*iter).content;
        ret += "</div>";
        i++;
    }
    return ret;
}

QList<QString> Book::getTOC()
{
    if(toc.count() < 2 && sections.count() >= 2){
        QList<QString> fakeTOC;
        for(int i = 0; i < sections.count(); ++i)
        {
            fakeTOC.push_back(tr("Part") + " " + QString::number(i+1));
        }
        return fakeTOC;
    } 

    return toc;    


}

const QString Book::getSectionOfTOCItem(int index)
{
    for(QList<BookSectionData>::iterator iter = sections.begin(); iter != sections.end(); ++iter)
    {
        BookSectionData section = *iter;
        if(section.TOCIndex == index)
            return *section.content;
    }

    return QString();
}

int Book::getSectionNumberOfTOCIndex(int index)
{
    for(int i = 0; i < sections.count(); i++)
    {
        BookSectionData section = sections.at(i);
        qDebug() << "Searching index" <<index <<" current " << section.TOCIndex;
        if(section.TOCIndex == index)
            return i;
    }

    return -1;
}

void Book::setFilename(QString f)
{
    filename = f;
}

QString Book::getFilename()
{
    return filename;
}

const QString Book::getTitle() const
{
    return title;
}


const QString Book::getAuthor() const
{
    return author;
}


const QString Book::getCategory() const
{
    return category;
}

const QPixmap Book::getCoverImage() const
{
    return coverImage;
}

const QString Book::getSubject() const
{
    return subject;
}

const QString Book::getDescription() const
{
    return description;
}

const QString Book::getPublishDate() const
{
    return publishDate;
}

int Book::getNumberOfSections()
{
    return sections.count();
}

unsigned int Book::getLengthInWords()
{
    return lengthInWords;
}

void Book::setLengthInWords(unsigned int length)
{
    lengthInWords = length;
}

void Book::progress(int &section, float &sectionProgress) const
{
    section = m_section;
    sectionProgress = m_sectionProgress;
}

void Book::setProgress(int section, float sectionProgress)
{
    m_section = section;
    m_sectionProgress = sectionProgress;
    qDebug() << m_section << ":"<<m_sectionProgress;
    saveProgress();
}

void Book::loadProgress()
{
    QSqlQuery query("SELECT * FROM lastspot WHERE filename=:filename");
    query.bindValue(":filename", filename);
    if(query.exec()){
        if(query.next()){
            m_section = query.value(1).toInt();
            m_sectionProgress = query.value(2).toFloat();
        }
    }
    else{
        qDebug() << "Database error: " << query.lastError();
    }
}

void Book::saveProgress() const
{
    QSqlQuery selectQuery("SELECT * FROM lastspot WHERE filename=:filename");
    selectQuery.bindValue(":filename", filename);
    selectQuery.exec();
    if(!selectQuery.next()){
        QSqlQuery insertQuery("INSERT INTO lastspot VALUES(:filename, :section, :percentage)");
        insertQuery.bindValue(":filename", filename);
        insertQuery.bindValue(":section", m_section);
        insertQuery.bindValue(":percentage", m_sectionProgress);

        if(!insertQuery.exec())
            qDebug() << "Database error: " << insertQuery.lastError();
    }else{
        QSqlQuery updateQuery("UPDATE lastspot SET section=:section, percentage=:percentage WHERE filename=:filename");
        updateQuery.bindValue(":section", m_section);
        updateQuery.bindValue(":percentage", m_sectionProgress);
        updateQuery.bindValue(":filename", filename);

        if(!updateQuery.exec())
            qDebug() << "Database error: " << updateQuery.lastError();
    }
}

void Book::loadBookmarks()
{

    QSqlQuery query("SELECT * FROM bookmarks WHERE filename=:filename");
    query.bindValue(":filename", filename);
    if(query.exec()){
        while(query.next()){
            Bookmark bookmark(query.value(1).toString(), QColor(query.value(2).toString()), query.value(3).toInt(), query.value(4).toFloat());

            bookmarks.push_back(bookmark);
        }
    } else{
        qDebug() << "Database error: " << query.lastError();
    }
}

QList<Bookmark> Book::getBookmarks()
{
    return bookmarks;
}

void Book::addBookmark(Bookmark bookmark)
{
    bookmarks.push_back(bookmark);
    // emit bookmarkAdded(b, this);

    //Save bookmark to database
    QSqlQuery insertQuery("INSERT INTO bookmarks VALUES(:filename, :name, :color, :section, :percentage)");
    insertQuery.bindValue(":filename", filename);
    insertQuery.bindValue(":name", bookmark.getName());
    insertQuery.bindValue(":color", bookmark.getColor().name());
    insertQuery.bindValue(":section", bookmark.getSection());
    insertQuery.bindValue(":percentage", bookmark.getMultiplier());

    if(!insertQuery.exec())
        qDebug() << "Database error: " << insertQuery.lastError();
}

void Book::removeBookmark(Bookmark &bookmark)
{
    for(QList<Bookmark>::iterator iter = bookmarks.begin(); iter != bookmarks.end(); ++iter){
        if(bookmark == (*iter)){
            // emit bookmarkDeleted(*iter);
            bookmarks.removeOne(*iter);

            QSqlQuery query("DELETE * FROM bookmarks WHERE filename=:filename AND name=:name");
            query.bindValue("filename", filename);
            query.bindValue("name", (*iter).getName());
            if(!query.exec())
                qDebug() << "Error deleting bookmark" << query.lastError();

            break;
        }
        
    }
}

void Book::removeBookmark(int index)
{
    if(!bookmarks.isEmpty() && index >= 0 && index < bookmarks.count())
        removeBookmark(bookmarks[index]);

}

QList<Annotation> Book::getAnnotations()
{
    return annotations;
}

bool Book::isInLibrary()
{
    return inLibrary;
}

void Book::changeLibraryStatus(bool s)
{
    inLibrary = s;
}

Annotation* Book::getAnnotation(const QString &text)
{

    for(QList<Annotation>::iterator iter = annotations.begin(); iter != annotations.end(); ++iter){
        Annotation a = *iter;
        if(a.getParagraph() == text)
            return &(*iter);
    }

    return 0;
}

void Book::addAnnotation(Annotation a)
{
    annotations.push_back(a);
    emit annotationAdded(a, this);
}

void Book::removeAnnotation(Annotation &annotation)
{

    for(QList<Annotation>::iterator iter = annotations.begin(); iter != annotations.end(); ++iter){
        if(annotation == (*iter)){
            emit annotationDeleted(*iter);
            annotations.removeOne(*iter);
            break;
        }
        
    }
}

void Book::removeAnnotation(int index)
{
    if(!annotations.isEmpty() && index >= 0 && index < annotations.count())
        removeAnnotation(annotations[index]);
}

bool Book::hasTOC()
{
    return !toc.isEmpty();
}

QNetworkAccessManager *Book::getResourceManager()
{
    if(generator)
        return static_cast<QNetworkAccessManager*>(generator);

    return NULL;
}

void Book::setTitle(QString s)
{
    title = s;
    emit titleChanged();
}

void Book::setAuthor(QString s)
{
    //TODO: A system for cleaning book data.
    if(s.contains("aut: "))s = s.remove("aut: ");
    if(s.contains("Author: "))s = s.remove("Author: ");
    s = s.remove(QRegExp("\\([^)]*\\)"));
    author = s;
    emit authorChanged();
}


void Book::setPublisher(QString s)
{
    publisher = s;
}
void Book::setDescription(QString s)
{
    description = s;
}
void Book::setSubject(QString s)
{
    subject = s;
}
void Book::setPublishDate(QString s)
{
    publishDate = s;
}
void Book::setCategory(QString s)
{
    category = s;
}
void Book::setCopyright(QString s)
{
    copyright = s;
}

void Book::addTOCItem(QString s, QString link){
    toc.push_back(s);
    for(QList<BookSectionData>::iterator iter = sections.begin(); iter != sections.end(); ++iter)
    {
        BookSectionData &section = *iter;
        if(section.link == link){
            section.label = s;
            section.TOCIndex = toc.length() - 1;
        }
    }
}

void Book::addSection(QString *s, QString link)
{
    //If this is the first section try to catch the cover picture
    //TODO: Use HTMLParser instead of regexp
    if(sections.count() == 0 && coverImage.isNull()){
//        QString str("<img src=\"images/cover.png\" alt=\"Cover\" style=\"height: 100%; max-width: 100%;\" />");
        QRegExp rx("<img[^>]+>");
        int pos = rx.indexIn(*s); 
        if(pos > -1){
            QString img = rx.cap(0);
            QRegExp rx2("src=['\"a-z0-9_./-]+");
            pos = rx2.indexIn(img);
            if(pos > -1){
                img = rx2.cap(0);
                img = img.remove("src=");
                img = img.remove("\"");
                img = img.remove("'");
                img = img.remove("../");
                QPixmap cover;
                int size = 0;
                char *data = generator->readFile(img, size);
                if(data && size != -1){
                    cover.loadFromData(reinterpret_cast<unsigned char*>(data), size);
                    addCoverImage(cover);
                }
            }
        }
    }
    BookSectionData section;
    section.content = s;
    section.link = link;
    section.TOCIndex = sections.count();
    section.label = QString();
    sections.push_back(section);    
}

void Book::addCoverImage(QPixmap cover)
{
    coverImage = cover;
    emit coverImageAdded();
}
