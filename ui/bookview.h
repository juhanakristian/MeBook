#ifndef BOOKVIEW_H
#define BOOKVIEW_H

#include <QDeclarativeItem>
#include <QTimer>

#include "shared.h"
#include "settings.h"
#include "roleitemmodel.h"
#include "book.h"


class QGraphicsWebView;
class QWebElement;

namespace MeBook{

    class BookView : public QDeclarativeItem{
        Q_OBJECT
        /*! /brief PositionInBook property
         *
         * Represents the current position in the book with a value from 0 to 1
         */
        Q_PROPERTY(qreal positionInBook READ positionInBook NOTIFY positionInBookChanged)
        Q_PROPERTY(QPoint tapAndHoldPoint READ tapAndHoldPoint)
    public:
        BookView(QDeclarativeItem *parent = 0);

        Q_INVOKABLE bool loadBookWithId(const QString &id);
        Q_INVOKABLE void closeCurrentBook();
        
        /*!Functions for getting book data
         *
         */
        Q_INVOKABLE RoleItemModel* tableOfContent();
        Q_INVOKABLE RoleItemModel* bookmarks();
        Q_INVOKABLE RoleItemModel* annotations();

        Q_INVOKABLE void openTOCItem(int index, bool top = true);

        Q_INVOKABLE void nextChapter();
        Q_INVOKABLE void previousChapter();

        Q_INVOKABLE void goToPosition(float position);

        Q_INVOKABLE QString getParagraphAt(const QPoint &position);

        Q_INVOKABLE void addBookmark(const QString &name, const QColor &color);
        Q_INVOKABLE void addAnnotation( const QString &annotation, 
                                        const QString &text, 
                                        const QString &paragraph,
                                        const QColor &color);

        QPoint tapAndHoldPoint(){
            return m_tapAndHoldPoint.toPoint();
        }

        qreal positionInBook() const;

    protected:
        void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);

        bool sceneEventFilter(QGraphicsItem *watched, QEvent *event);
        // void mousePressEvent(QGraphicsSceneMouseEvent*);
        // void mouseReleaseEvent(QGraphicsSceneMouseEvent*);
        // void mouseMoveEvent(QGraphicsSceneMouseEvent*);

        void setCSSSetting(QString setting, QString value);
        void setHTMLAttribute(QString attribute, QString value);
        void makeDOMChangesEffective();
        void setDOMElementSettings(QWebElement &parentElement);

    private:
        /*! \brief QGraphicsWebView is the element that handles the actual rendering.
         *
         */
        QGraphicsWebView *m_webview;


        void openHTMLContent(const QString &html);

        QHash<QString, QString> cssSettings;
        QHash<QString, QString> htmlAttributes;

        Book *m_currentBook;

        //Variables for kinetic scrolling in scroll view
        QTimer m_kineticTimer;
        float m_acceleration;
        QPointF m_last;

        QTimer m_accelerationTimer;
        QList<QPointF> m_previousPoints;

        bool m_bounce;
        float m_bounceAcceleration;
        void checkForBounce();

        //Variables for storing current position in book
        int m_currentChapter;
        float m_currentPosition;

        QTimer m_tapAndHoldTimer;
        QPointF m_tapAndHoldPoint;

        Settings::BookViewMode m_mode;

        int m_pageWidth;

        int numPages() const;
        int currentPage() const;

    private slots:
        void finished(bool ok);

        void doKineticScroll();
        void gatherScrollData();

        void tapAndHold();

    signals:
        void positionInBookChanged();
        void contextMenu(const QPoint &point);
        void doubleClicked();

    };

}

#endif
