#include <QtGui/QApplication>
#include <QtDeclarative>

#include <QVariant>

#include "library.h"
#include "settings.h"
#include "shared.h"
#include "feedbooks.h"
#include "bookview.h"
#include "roleitemmodel.h"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QDeclarativeView view;
 
    qmlRegisterType<MeBook::BookView>("com.mebook.bookview", 1, 0, "BookView");
    qmlRegisterType<RoleItemModel>("com.mebook.bookview", 1, 0, "RoleItemModel");


    MeBook::MLibrary.loadLibrary();
    QDeclarativeContext *context = new QDeclarativeContext(view.rootContext());
    if(context->parentContext())
        context = context->parentContext();
    QDeclarativeEngine *engine = context->engine();

    qmlRegisterType<MeBook::Settings>("com.mebook.settings", 1, 0, "MeBookSettings");

    MeBook::FeedBooks *feedBooksCatalog = new MeBook::FeedBooks(engine);

    
    engine->addImageProvider("covers", new MeBook::CoverImageProvider);
    context->setContextProperty("books", MeBook::MLibrary.booksModel());
    context->setContextProperty("library", &MeBook::MLibrary);
    context->setContextProperty("settings", &MeBook::MSettings);
    context->setContextProperty("feedbooksModel", feedBooksCatalog->searchResultsModel());
    context->setContextProperty("feedbooks", feedBooksCatalog);

    view.setSource(QUrl("qrc:/qml/main.qml"));
    view.showFullScreen();
    return app.exec();
}
