#ifndef SHARED_H
#define SHARED_H

#include <QObject>

//dummyURL is used with QNetworkManager 
namespace MeBook{
    const QString dummyURL = QString("ebook://get.com");
    const QString OSSO_NAME = QString("mebook_osso");

    // class SharedEnums : public QObject{
    //     Q_OBJECT
    //     Q_ENUMS(BookViewMode)
    //     Q_ENUMS(Sorting)
    //     Q_ENUMS(Orientation)
    //     public:
    //         SharedEnums(QObject *parent = 0) : QObject(parent){}

    //     enum BookViewMode{
    //         ScrollingMode, 
    //         PageMode
    //     };

    //     enum Sorting{
    //         byAuthor,
    //         byTitle,
    //         None
    //     };

    //     enum Orientation{
    //         Landscape, 
    //         Portrait
    //     };

    // };

}
#endif
