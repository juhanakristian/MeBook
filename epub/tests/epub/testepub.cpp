#include <QtTest/QtTest>
#include <QDebug>
#include "epub.h"

class TestEpub : public QObject
{
    Q_OBJECT
private slots:
    void initialize();
    void title();
    void author();
    void publisher();
    void published();
    void spine();
    void manifest();
    void file();
    void ncx();

private:
    Epub *book;
};

void TestEpub::initialize()
{
    book = new Epub("../test1.epub");
    QVERIFY(book->isValid() && book->m_rootpath == "OPS");
}
    
void TestEpub::title()
{ 
    QString s = book->title();
    qDebug() << s;
    QVERIFY(s == "Free As In Freedom");
}

void TestEpub::author()
{ 
    QString s = book->author();
    qDebug() << s;
    QVERIFY(s == "Sam Williams");
}

void TestEpub::publisher()
{ 
    QString s = book->publisher();
    qDebug() << s;
    QVERIFY(s == "Feedbooks (http://www.feedbooks.com)");
}

void TestEpub::published()
{ 
    QString s = book->published();
    qDebug() << s;
    QVERIFY(s == "2002");
}

void TestEpub::spine()
{
    QStringList l = book->spine();
    qDebug() << l;
    QVERIFY(!l.isEmpty());
}
    
void TestEpub::manifest()
{
    QList<Epub::ManifestItem> l = book->manifest();
    QListIterator<Epub::ManifestItem> iter(l);
    while(iter.hasNext()){
        Epub::ManifestItem i = iter.next();
        qDebug() << i.href << i.id;
    }
    QVERIFY(!l.isEmpty());
}

void TestEpub::file()
{
    QString f = QString(book->file("about.xml"));
    qDebug() << f;
    QVERIFY(!f.isEmpty());
}
    
void TestEpub::ncx()
{
    QList<Epub::NCXItem> list = book->ncx();
    QListIterator<Epub::NCXItem> iter(list);
    while(iter.hasNext())
        qDebug() << iter.next().title;
    QVERIFY(!list.isEmpty());
}

QTEST_MAIN(TestEpub)
#include "testepub.moc"
