#include <QtTest/QtTest>
#include <QDebug>
#include "ziphandler.h"

class TestZipHandler: public QObject
{
    Q_OBJECT
private slots:
    void initialize();
    void filenames();
    void getFile();
private:
    ZipHandler *handler;
};

void TestZipHandler::initialize()
{
    handler = new ZipHandler("../test.epub");
    QVERIFY(handler->isValid());
}

void TestZipHandler::filenames(){
    QStringList list = handler->files();
    qDebug() << list;
    qDebug() << list.count();
    QVERIFY(!list.isEmpty());
}

void TestZipHandler::getFile()
{
    QString f = handler->fileAsString("OPS/fb.opf");
    qDebug() << f;
    QVERIFY(!f.isEmpty());
}

QTEST_MAIN(TestZipHandler)
#include "testziphandler.moc"
