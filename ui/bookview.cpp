#include "bookview.h"

#include "library.h"
#include "settings.h"
#include "shared.h"

#include <QGraphicsWebView>
#include <QGraphicsSceneMouseEvent>
#include <QWebElement>
#include <QWebFrame>

#include <QDebug>


using namespace MeBook;

const float friction = 0.90;
const float bounce_accel = 20.0;
const float max_acceleration = 10.0;

BookView::BookView(QDeclarativeItem *parent) :
    QDeclarativeItem(parent),
    m_currentBook(NULL),
    m_bounce(false),
    m_currentChapter(0),
    m_currentPosition(0.0),
    m_mode(Settings::PageMode)
{

    m_webview = new QGraphicsWebView(this);
    m_webview->setResizesToContents(true);

    //Set default encoding
    m_webview->settings()->setDefaultTextEncoding("utf-8");

    m_webview->installEventFilter(this);



    setCSSSetting("color", "#000000");
    setCSSSetting("border-color", "#000000");
    setCSSSetting("background-color", "#edeadb");
    setCSSSetting("white-space", "normal");
    // setCSSSetting("line-height", "22em");
    setCSSSetting("text-align", "justify");

    
    setFiltersChildEvents(true);

    m_kineticTimer.setInterval(25);
    m_kineticTimer.setSingleShot(true);
    connect(&m_kineticTimer, SIGNAL(timeout()), this, SLOT(doKineticScroll()));

    connect(&m_tapAndHoldTimer, SIGNAL(timeout()), this, SLOT(tapAndHold()));
}
    
bool BookView::loadBookWithId(const QString &id)
{
    if(m_currentBook)
        delete m_currentBook;

    m_currentBook = MLibrary.bookWithId(id);    
    if(!m_currentBook){
        return false;
    }

    m_currentBook->loadBookmarks();

    m_currentChapter = 0;
    m_currentPosition = 0.0;

    m_currentBook->progress(m_currentChapter, m_currentPosition);

    QString s = m_currentBook->getSection(m_currentChapter);
    openHTMLContent(s);
}
        
void BookView::closeCurrentBook()
{

    float multiplier = 0.0;
    if(m_mode == Settings::ScrollingMode)
        multiplier = static_cast<float>(qAbs(m_webview->y())) / static_cast<float>(m_webview->boundingRect().height());
    else
        multiplier = static_cast<float>(qAbs(m_webview->x())) / static_cast<float>(m_webview->boundingRect().width());

    int section = m_currentChapter;

    m_currentBook->setProgress(section, multiplier);

    delete m_currentBook;
    m_currentBook = 0;
}

RoleItemModel* BookView::tableOfContent()
{
    if(!m_currentBook)
        return NULL;

    QHash<int, QByteArray> roleNames;
    roleNames.insert(Qt::UserRole + 1, "itemText");

    RoleItemModel *model = new RoleItemModel(roleNames);
    QStringList list(m_currentBook->getTOC());
    foreach(QString s, list){
        QStandardItem *item = new QStandardItem();
        item->setData(s, Qt::UserRole + 1);
        model->appendRow(item);
    }

    qDebug() << "Model:" << model->rowCount();
    return model;
}


RoleItemModel* BookView::bookmarks()
{
    if(!m_currentBook)
        return NULL;

    QList<Bookmark> bm = m_currentBook->getBookmarks();

    QHash<int, QByteArray> roleNames;
    roleNames.insert(Qt::UserRole + 1, "itemText");
    roleNames.insert(Qt::UserRole + 2, "image");
    
    RoleItemModel *model = new RoleItemModel(roleNames);
    //TODO: Render a square image with color..
    foreach(Bookmark b, bm){
        QStandardItem *item = new QStandardItem();
        item->setData(b.getName(), Qt::UserRole + 1);
        item->setData(b.getColor(), Qt::UserRole + 2);
        model->appendRow(item);
    }
    qDebug() << "Model:" << model->rowCount();
    return model;
}

RoleItemModel* BookView::annotations() 
{
    if(!m_currentBook)
        return NULL;

    QList<Annotation> an = m_currentBook->getAnnotations();

    QHash<int, QByteArray> roleNames;
    roleNames.insert(Qt::UserRole + 1, "itemText");
    roleNames.insert(Qt::UserRole + 2, "image");
    roleNames.insert(Qt::UserRole + 3, "subtext");

    RoleItemModel *model = new RoleItemModel(roleNames);

    foreach(Annotation a, an){
        QStandardItem *item = new QStandardItem();
        item->setData(a.getText(), Qt::UserRole + 1);
        item->setData(a.getColor(), Qt::UserRole + 2);
        item->setData(a.getAnnotation(), Qt::UserRole + 3);
        model->appendRow(item);
    }

    return model;
}

void BookView::openTOCItem(int index, bool top)
{
    m_currentChapter = index;
    if(top)
        m_currentPosition = 0.0;

    connect(m_webview, SIGNAL(loadFinished(bool)), this, SLOT(finished(bool)));

    QString s = m_currentBook->getSectionOfTOCItem(index);
    openHTMLContent(s);

}

void BookView::nextChapter()
{
    int index = m_currentChapter + 1;
    QString s = m_currentBook->getSectionOfTOCItem(index);

    if(s.isEmpty()){
        s = m_currentBook->getSectionOfTOCItem(0);
        index = 0;
    }

    connect(m_webview, SIGNAL(loadFinished(bool)), this, SLOT(finished(bool)));
    openHTMLContent(s);

    m_currentChapter = index;
    m_currentPosition = 0.0;

}

void BookView::previousChapter()
{
    int index = m_currentChapter - 1;
    QString s = m_currentBook->getSectionOfTOCItem(index);

    if(s.isEmpty()){
        s = m_currentBook->getSectionOfTOCItem(0);
        index = 0;
    }

    connect(m_webview, SIGNAL(loadFinished(bool)), this, SLOT(finished(bool)));
    openHTMLContent(s);

    m_currentChapter = index;
    m_currentPosition = 0.0;

}


void BookView::goToPosition(float position)
{
    int section = m_currentBook->getSectionAtPosition(position * m_currentBook->getBookLength());

    if(section != -1){
        float multiplier = (((position * m_currentBook->getBookLength()) - 
            (float)m_currentBook->getSectionStart(section)) / 
            (float)m_currentBook->getSectionLength(section));
        m_currentPosition = multiplier;
    }

    openTOCItem(section, false);
    emit positionInBookChanged();

}

QString BookView::getParagraphAt(const QPoint &position){
    QPointF p = QPointF(position);
    QWebHitTestResult result = m_webview->page()->mainFrame()->hitTestContent(p.toPoint());
    QWebElement element = result.enclosingBlockElement();
    if(element.tagName() == "P" && !element.isNull())
        return element.toPlainText();
    
    return QString();
}


void BookView::addBookmark(const QString &name, const QColor &color)
{
    float multiplier = static_cast<float>(qAbs(m_webview->y())) / static_cast<float>(m_webview->boundingRect().height());
    Bookmark bookmark(name, color, m_currentChapter, multiplier);
    m_currentBook->addBookmark(bookmark);
}

void BookView::addAnnotation( const QString &annotation, 
                    const QString &text, 
                    const QString &paragraph,
                    const QColor &color)
{

        float multiplier = static_cast<float>(qAbs(m_webview->y())) / static_cast<float>(m_webview->boundingRect().height());
        Annotation ann( m_currentChapter, 
                        multiplier, 
                        paragraph, 
                        text, 
                        annotation, 
                        color );

        m_currentBook->addAnnotation(ann);
}

qreal BookView::positionInBook() const
{
    if(!m_currentBook)
        return 0.0;

    int start = m_currentBook->getSectionStart(m_currentChapter);

    float c = 0.0f;
    if(m_mode == Settings::ScrollingMode)
        c = qAbs(m_webview->y() / m_webview->boundingRect().height());
    else
        c = qAbs(m_webview->x() / m_webview->boundingRect().width());

    int cp = m_currentBook->getSectionLength(m_currentChapter) * c;

    qreal r = static_cast<qreal>(start+cp) / static_cast<qreal>(m_currentBook->getBookLength());

    return r;
}
    
void BookView::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    if(newGeometry == oldGeometry)
        return;

    m_webview->page()->setPreferredContentsSize(newGeometry.size().toSize());

    QDeclarativeItem::geometryChanged(newGeometry, oldGeometry);
}

bool BookView::sceneEventFilter(QGraphicsItem *watched, QEvent *event)
{
    if(event->type() == QEvent::GraphicsSceneMousePress){
        QGraphicsSceneMouseEvent *mouseEvent = static_cast<QGraphicsSceneMouseEvent*>(event);
        m_last = watched->mapToScene(mouseEvent->pos());

        //Fix for harmattan..
        if(width() < height()) {
            float temp = m_last.x();
            m_last.setX(m_last.y());
            m_last.setY(temp);
        }
        m_acceleration = 0.0;
        m_kineticTimer.stop();
        m_tapAndHoldPoint = mouseEvent->pos();
        m_tapAndHoldTimer.start(1000);
        m_previousPoints.clear();
    } else if(event->type() == QEvent::GraphicsSceneMouseRelease){
        QGraphicsSceneMouseEvent *mouseEvent = static_cast<QGraphicsSceneMouseEvent*>(event);

        QPointF p = watched->mapToScene(mouseEvent->pos());

        //Fix for harmattan..
        if(width() < height()) {
            float temp = p.x();
            p.setX(p.y());
            p.setY(temp);
        }

        QPointF d = p - m_last;
        if(m_tapAndHoldTimer.isActive())
            m_tapAndHoldTimer.stop();

        if(m_mode == Settings::PageMode) {
            int l = qAbs(static_cast<int>(m_webview->x() / m_pageWidth));
            float mx = qAbs(m_webview->x()) - static_cast<float>(m_pageWidth * l);
            if(mx > (m_pageWidth / 2))
                l++;
            m_webview->setX(-m_pageWidth * l);
            emit positionInBookChanged();
            if(currentPage() >= numPages()) {
                nextChapter();
            } else if(currentPage() < 0) {
                previousChapter();
            }
            return true;
        }

        if(m_previousPoints.length() < 1)
            return true;

        float final;
        for(int i = 1; i < m_previousPoints.length(); i++){
            QPointF f = m_previousPoints[i-1];
            QPointF l = m_previousPoints[i];
            final += (l.y()-f.y()) * i;
        }
        final /= m_previousPoints.length();
        qDebug() << final;
        if( qAbs(final) > 3 ){
            m_acceleration = (final < max_acceleration) ? final : max_acceleration;
            m_kineticTimer.start();
        }
        else if(final != 0){
            checkForBounce();
            m_kineticTimer.start();
        }


        m_last = QPoint();

        m_previousPoints.clear();


    } else if(event->type() == QEvent::GraphicsSceneMouseMove){
        QGraphicsSceneMouseEvent *mouseEvent = static_cast<QGraphicsSceneMouseEvent*>(event);

        QPointF p = watched->mapToScene(mouseEvent->pos());


        if(m_last.isNull())
            return true;

        //Fix for harmattan..
        if(width() < height()) {
            float temp = p.x();
            p.setX(p.y());
            p.setY(temp);
        }

        QPointF d = p - m_last;
        m_last = p;

        if(m_mode == Settings::PageMode) {
            m_webview->moveBy(-d.x(), 0);
            return true;
        }
        m_webview->moveBy(0, d.y());

        //FIXME: Calculating position on every move creates a lot of lag on big books
//        emit positionInBookChanged();
        
        QPointF p2 = m_last;
        p2.setX(0);

        if(!m_previousPoints.contains(p2))
            m_previousPoints.append(p2);
        if(m_previousPoints.length() > 5)
            m_previousPoints.pop_front();
            
        m_tapAndHoldTimer.stop();
    } else if(event->type() == QEvent::GraphicsSceneMouseDoubleClick) {
        emit doubleClicked();
    }

    //Prevent further processing by returning true
    return true;
}


void BookView::setCSSSetting(QString setting, QString value){
    cssSettings.insert(setting, value);
}

void BookView::setHTMLAttribute(QString attribute, QString value){
    htmlAttributes.insert(attribute, value);
}

void BookView::makeDOMChangesEffective(){
    QWebElement documentElement = m_webview->page()->mainFrame()->documentElement();
    setDOMElementSettings(documentElement);
}

void BookView::setDOMElementSettings(QWebElement &parentElement){
    QWebElement element = parentElement.firstChild(); 
    while(!element.isNull())
    {
        //CSS
        for(QHash<QString, QString>::iterator iter = cssSettings.begin(); iter != cssSettings.end(); ++iter){
            if(!element.hasClass("MebookAnnotation"))
                element.setStyleProperty(iter.key(), (*iter));

            //TODO: Handle image scaling better.
            // if(element.tagName() == "IMG"){
            //     if(renderSettings){
            //         int maxWidth = renderSettings->getMaxImageSize().width();
            //         int maxHeight = renderSettings->getMaxImageSize().height();
            //         element.setStyleProperty("max-width", QString::number(maxWidth) + "px");
            //         element.setStyleProperty("max-height", QString::number(maxHeight) + "px");
            //     }
            //     element.setStyleProperty("margin", "0px");
            //     element.setStyleProperty("float", "center");
            // }
        }

        //HTML attributes
        for(QHash<QString, QString>::iterator iter = htmlAttributes.begin(); iter != htmlAttributes.end(); ++iter){
            if(element.hasAttribute(iter.key()))
                element.setAttribute(iter.key(), *iter);
        }

        //Draw annotation backgrounds

        if(element.tagName() == "P"){
            QList<Annotation> list = m_currentBook->getAnnotations();
            for(QList<Annotation>::iterator ai = list.begin(); ai != list.end(); ++ai){
                Annotation an = *ai;
                if(element.toPlainText().contains(an.getParagraph())){
                    QString text = element.toPlainText();
                    QColor color = an.getColor();
                    // if(renderSettings){
                    //     if(renderSettings->getBackgroundColor().lightness() > color.lightness())
                    //         color = color.lighter();
                    //     else
                    //         color = color.darker();
                    // }
                    text.replace(an.getText(), "<span class=\"MebookAnnotation\" style=\"background-color:" + color.name() + "\">" + an.getText() + "</span>");
                    element.setInnerXml(text);
                }
            }
        }

        setDOMElementSettings(element);
        element = element.nextSibling();
    }
}

void BookView::openHTMLContent(const QString &html)
{
    QWebPage *p = new QWebPage();


    p->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
    p->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);

    p->setNetworkAccessManager(m_currentBook->getResourceManager());

    QWebSettings *settings = m_webview->page()->settings()->globalSettings();
    settings->clearMemoryCaches();

    //Set font
    QString font = MSettings.fontFamily();
    settings->setFontFamily(QWebSettings::SansSerifFont, font);
    settings->setFontFamily(QWebSettings::StandardFont, font);
    settings->setFontFamily(QWebSettings::SerifFont, font);
    settings->setFontFamily(QWebSettings::FixedFont, font);
    settings->setFontFamily(QWebSettings::FantasyFont, font);
    settings->setFontSize(QWebSettings::MinimumFontSize, MSettings.fontPixelSize());

    m_mode = static_cast<Settings::BookViewMode>(MSettings.viewMode());

    connect(m_webview, SIGNAL(loadFinished(bool)), this, SLOT(finished(bool)));
    p->currentFrame()->setHtml(html, QUrl(dummyURL));

    p->setPreferredContentsSize(boundingRect().size().toSize());

    delete m_webview->page();
    m_webview->setPage(p);

}

void BookView::checkForBounce()
{
    float webview_bottom = m_webview->geometry().bottom();
    float webview_top = m_webview->geometry().top();


    if(webview_bottom < height()){
        m_acceleration += bounce_accel + (bounce_accel * ((height() - webview_bottom) / height())) * 2.0f;
        qDebug()<<(bounce_accel + (height() - webview_bottom));
        m_bounce = true;

        if((webview_bottom + m_acceleration) > height()){
            m_acceleration = 0.0;
            m_webview->moveBy(0, height() - webview_bottom);
            m_kineticTimer.stop();
        }
            
    } else if(webview_top > 0){
        m_acceleration -= bounce_accel;


        if((webview_top + m_acceleration) < 0){
            m_acceleration = 0.0;
            m_webview->moveBy(0, -webview_top);
            m_kineticTimer.stop();
        }
    }
    
    
}

void BookView::finished(bool ok)
{
    disconnect(m_webview, SIGNAL(loadFinished(bool)), this, SLOT(finished(bool)));


    makeDOMChangesEffective();

    if(m_mode == Settings::PageMode) {
        int pageHeight = boundingRect().size().height();
        int pageWidth = boundingRect().size().width() - 15;
        QWebPage *page = m_webview->page();
        QWebElement bodyElement = page->mainFrame()->findFirstElement("body");
        int pageCount = (bodyElement.geometry().height() / pageHeight) + 1;
        QString paginationScript = "var d = document.getElementsByTagName('body')[0];" \
                                   "var totalHeight = d.offsetHeight;" \
                                   "var pageCount = Math.floor(totalHeight / " + QString::number(pageHeight) + ") + 1;" \
                                   "d.style.marginLeft= \"20px\";" \ 
                                   "d.style.WebkitColumnCount = pageCount;" \
                                   "d.style.width= ((" + QString::number(pageWidth) + " * pageCount) - 20) + \"px\";" \ 
                                   "d.style.height= \"" + QString::number(pageHeight) + "px\";";
        page->mainFrame()->evaluateJavaScript(paginationScript);
        int fullWidth = (pageCount * pageWidth) - 20;
        // emit numberOfPages(pageCount);
        page->setPreferredContentsSize(QSize(fullWidth, pageHeight));
        // page->setViewportSize(QSize(fullWidth, pageHeight));

        m_pageWidth = pageWidth;
    }

    m_webview->setMinimumHeight(m_webview->page()->mainFrame()->contentsSize().height());
    m_webview->setMaximumHeight(m_webview->page()->mainFrame()->contentsSize().height());

    if(m_mode == Settings::ScrollingMode) {
        m_webview->setY(-m_currentPosition * m_webview->boundingRect().height());
        m_webview->setX(0);
    } else {
        m_webview->setX(-m_currentPosition * m_webview->boundingRect().width());
        m_webview->setY(0);
    }

    emit positionInBookChanged();
}
        
void BookView::doKineticScroll()
{
    if(!m_currentBook || m_mode == Settings::PageMode){
        m_kineticTimer.stop();
        return;
    }
    m_webview->moveBy(0, m_acceleration);

    m_acceleration *= friction;

    if(qAbs(m_acceleration) > 0.2)
        m_kineticTimer.start();
    else 
        emit positionInBookChanged();

    checkForBounce();
}


void BookView::gatherScrollData()
{
    QPointF p = m_last;
    p.setX(0);

    if(m_previousPoints.isEmpty()){
        m_previousPoints.append(p);
        return;
    }

    if(!m_previousPoints.contains(p))
        m_previousPoints.append(p);
    if(m_previousPoints.length() > 5)
        m_previousPoints.pop_front();
}
        
void BookView::tapAndHold()
{
    emit contextMenu(m_webview->mapToScene(m_tapAndHoldPoint).toPoint());
}

int MeBook::BookView::numPages() const
{
    return m_webview->boundingRect().width() / m_pageWidth;
}

int MeBook::BookView::currentPage() const
{
    return (-m_webview->x() / m_webview->boundingRect().width()) * numPages();
}
        
// void BookView::contextMenu(const QPoint &point)
// {
//     qDebug() << "ContextMenu";
// 
//     QMenu *menu = new QMenu();
//     QAction *bookmarkAction = menu->addAction(tr("Add bookmark"));
//     if(menu->exec(view->mapToGlobal(pos)) == bookmarkAction){
//         
//     }
// }
