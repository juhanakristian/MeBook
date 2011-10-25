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
        Q_PROPERTY(int contentY READ contentY WRITE setContentY)
        Q_PROPERTY(bool textSelectionEnabled READ textSelectionEnabled WRITE setTextSelectionEnabled)
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
        Q_INVOKABLE void previousChapter(bool end = false);

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

        int contentY() const;
        void setContentY(int y);

        bool textSelectionEnabled() const;
        Q_INVOKABLE void setTextSelectionEnabled(bool enabled);

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

        bool m_textSelectionEnabled;
        void openHTMLContent(const QString &html);
        void handlePaging();
        void recalculatePosition();
        QHash<QString, QString> cssSettings;
        QHash<QString, QString> htmlAttributes;

        Book *m_currentBook;

        //Variables for kinetic scrolling in scroll view
        QTimer m_kineticTimer;
        float m_acceleration;
        QPointF m_last;

        QTimer m_accelerationTimer;
        QList<QPointF> m_previousPoints;

        QTimer m_animationTimer;
        int m_animationStep;
        int m_step;

        QTimer m_widthFixTimer;

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
        int m_pageCount;

        int numPages() const;
        int currentPage() const;

    private slots:
        void finished(bool ok);

        void doKineticScroll();
        void gatherScrollData();

        void tapAndHold();

        void doAnimation();
        void resetPosition();

    signals:
        void positionInBookChanged();
        void contextMenu(const QPoint &point);
        void doubleClicked();

    };

}

#endif
