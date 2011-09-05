#ifndef RENDERER_H
#define RENDERER_H

#include <QPixmap>
#include <QThread>
#include <QQueue>
#include <QImage>
#include <QNetworkAccessManager>
#include <QWebElement>
#include <QWebPage>
#include <QWebFrame>
#include <QPixmapCache>

#include "shared.h"
#include "rendersettings.h"

#include "annotation.h"

namespace MeBook{

    class RenderTile{
    public:
        RenderTile(){}
        RenderTile(int s, int i, QPoint p, QSizeF si){
            section = s;
            id = i;
            pos = p;
            size = si;

            cacheKey = "mebook::" + QString::number(section) + "::" + QString::number(id);
        }

        QPixmap& getPixmap(){
            QPixmap *pm;
            if(QPixmapCache::find(cacheKey, pm))
                return *pm;
        }
        const QSizeF getSize(){return size;}
        bool isRendered(){
            QPixmap *pm;
            return QPixmapCache::find(cacheKey, pm);
        }
        int getID(){return id;}
        int getSection(){return section;}
        const QPoint& getPosition(){return pos;}
        void updatePixmap(QPixmap p){
            // pixmap = p;
            QPixmapCache::insert(cacheKey, p);
        }

    private:
        int section;
        int id;
        QPoint pos;
        QSizeF size;
        // QPixmap pixmap;
        QString cacheKey;
    };

    struct RenderQueueItem
    {
        int section;
        QString html;
        int startPage;
        int endPage;
    };

    //Renderer is resposible for loading html to a QWebPage, applying rendering settings, making annotations visible and pagination.
    //Rendering settings and annotations are applied using DOM manipulation.
    class Renderer : public QObject{
        Q_OBJECT
    public:
        Renderer(QObject *parent = 0);
        ~Renderer();
        //setResourceManager set's the QWebPage-objects QNetworkAccessManager so QtWebkit can access stylesheets and images inside ebook file.
        void setResourceManager(QNetworkAccessManager*);
        //renderTile is used for re-rendering pages for ex. when an annotation has been added.
        void renderTile(RenderTile*);
        void setPageSize(QSizeF);
        void setRenderSettings(RenderSettings*);
        void setAnnotations(QList<Annotation>);
        void enablePagination(bool);
        void enablePixmapRendering(bool);
        //Returns the paragraph in given QPoint and possible RenderTile as QString.
        //Used when making an annotation.
        QString getParagraphAt(const QPoint&, RenderTile* = 0);
        //Test whether given QPoint hit's an annotation.
        bool hitAnnotation(const QPoint&, QString&, RenderTile* = 0);
        //Returns the Y-coordinate of the given text in the QWebPage.
        int getTextPosition(const QString&);

    private:
        void handleQueue();

        //TODO: Improve DOM manipulation system. Make it possible to target attribute/setting changes to specific html-elements.
        
        //Adds the given value for the given css-property in the list of css-settings (cssSettings).
        void setCSSSetting(QString, QString);
        //Adds the given value for the given html-attribute in the list of html-attributes (htmlAttributes).
        void setHTMLAttribute(QString, QString);
        //Starts traversing the DOM and setting css-, and html-properties.
        void makeDOMChangesEffective();
        //Recursive function for traversing DOM.
        void setDOMElementSettings(QWebElement&);
        //Remove URLs from html to prevent clipping. URLs are replaced with links.
        void parseLinks(QString&);

        //Experimental DOM pagination.
        void paginateDocument(QWebElement&);

        QHash<QString, QString> cssSettings;
        QHash<QString, QString> htmlAttributes;
        QList<Annotation> annotationList;
        QWebPage *page;
        QQueue< QPair<int, QString> > queue;
        QPair<int, QString> current;
        QSizeF pageSize;

        RenderSettings *renderSettings;

        QThread *renderThread;

        bool paginate;
        bool pixmapRendering;

        int testHeight;

    signals:
        void tileRendered(RenderTile*);
        void contentHeight(int);
        void numberOfPages(int);
        void webPageLoaded(QWebPage*);

    public slots:
        void render();
        void addToQueue(QString, int);
    };

}

#endif
