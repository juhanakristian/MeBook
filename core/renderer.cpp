#include "renderer.h"
#include <QPainter>
#include <QDebug>
#include <QTime>
#include <QApplication>
#include <QCoreApplication>

#include <math.h>

using namespace MeBook;


Renderer::Renderer(QObject *parent) : QObject(parent), 
    paginate(true), 
    pixmapRendering(true)
{
    QWebSettings *settings = QWebSettings::globalSettings();
    settings->setAttribute(QWebSettings::DeveloperExtrasEnabled, false);
    page = new QWebPage(this);
    setCSSSetting("white-space", "normal");
}

Renderer::~Renderer()
{

}

void Renderer::setResourceManager(QNetworkAccessManager *manager)
{
    //Using custom QNetworkAccessManager so css and images load properly.
    if(page->networkAccessManager() != manager){ 
        if(manager){
            page->setNetworkAccessManager(manager);
        }
    }

}

void Renderer::addToQueue(QString content, int section){
    queue.enqueue(QPair<int, QString>(section, content));
    handleQueue();    
}

void Renderer::setAnnotations(QList<Annotation> annotations)
{
    annotationList = annotations;
    makeDOMChangesEffective();
}


void Renderer::renderTile(RenderTile *tile){
    // int pageHeight = page->preferredContentsSize().height();
    int pageWidth = page->preferredContentsSize().width();
    int pageHeight = pageSize.height();
    QWebElement bodyElement = page->mainFrame()->findFirstElement("body");
    int fullWidth = bodyElement.geometry().width();
    //int fullWidth = page->mainFrame()->contentsSize().width();
    qDebug() << "renderTile() fullWidth:" << fullWidth;
    qDebug() << "renderTile() id:" << tile->getID();
    page->setPreferredContentsSize(QSize(fullWidth, pageHeight));
    page->setViewportSize(QSize(fullWidth, pageHeight));

    QPixmap pixmap(tile->getSize().toSize());
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.translate(-tile->getPosition().x(), 0);
    qDebug() << "Rendering at:" << tile->getPosition().x();
    painter.setClipRegion(QRegion(tile->getPosition().x(), 0, pixmap.width(), pixmap.height()));
    bodyElement.render(&painter);
    tile->updatePixmap(pixmap);

    page->setViewportSize(QSize(pageWidth, pageHeight));
    page->setPreferredContentsSize(QSize(pageWidth, pageHeight));
}

void Renderer::setPageSize(QSizeF s)
{
    pageSize = s;
    page->setPreferredContentsSize(pageSize.toSize());
    page->setViewportSize(pageSize.toSize());
    page->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
    page->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
}


void Renderer::handleQueue(){
    if(!queue.isEmpty()){    
        QPair<int, QString> item = queue.dequeue();

        QWebSettings *settings = page->settings()->globalSettings();
        settings->clearMemoryCaches();

        if(renderSettings){
            setCSSSetting("line-height", QString::number(renderSettings->getLineHeight()) + "em");
            settings->setFontFamily(QWebSettings::SansSerifFont, renderSettings->getFont().family());
            settings->setFontFamily(QWebSettings::StandardFont, renderSettings->getFont().family());
            settings->setFontFamily(QWebSettings::SerifFont, renderSettings->getFont().family());
            settings->setFontFamily(QWebSettings::FixedFont, renderSettings->getFont().family());
            settings->setFontFamily(QWebSettings::FantasyFont, renderSettings->getFont().family());
            settings->setFontSize(QWebSettings::MinimumFontSize, renderSettings->getFont().pointSize());

            setCSSSetting("color", renderSettings->getTextColor().name());
            setCSSSetting("border-color", renderSettings->getTextColor().name());
            setCSSSetting("background-color", renderSettings->getBackgroundColor().name());

        }

        current = item;
        //parseLinks(item.second);
        page->mainFrame()->setHtml(item.second, QUrl(dummyURL));
        makeDOMChangesEffective();
        connect(page, SIGNAL(loadFinished(bool)), this, SLOT(render()));

    }
}

void Renderer::setRenderSettings(RenderSettings *settings)
{
    renderSettings = settings;
}

void Renderer::enablePagination(bool toggle)
{
    paginate = toggle;
}

void Renderer::enablePixmapRendering(bool toggle)
{
    pixmapRendering = toggle;
}

QString Renderer::getParagraphAt(const QPoint &point, RenderTile *tile)
{
    QPoint testPoint = point;
    if(tile)
        testPoint = QPoint(point.x(), tile->getPosition().y() + point.y());

    QWebHitTestResult result = page->mainFrame()->hitTestContent(testPoint);
    QWebElement element = result.enclosingBlockElement();
    if(element.tagName() == "P" && !element.isNull())
        return element.toPlainText();
    
    return QString();
}

bool Renderer::hitAnnotation(const QPoint &point, QString &annotationText, RenderTile *tile)
{
    QPoint testPoint = point;
    if(tile)
        testPoint = QPoint(point.x(), tile->getPosition().y() + point.y());

    QWebHitTestResult result = page->mainFrame()->hitTestContent(testPoint);
    QWebElement element = result.enclosingBlockElement();

    QWebElementCollection annotationElements = element.findAll("span.MebookAnnotation");
    QList<QWebElement> elementList = annotationElements.toList();
    if(!elementList.isEmpty()){
        for(QList<QWebElement>::iterator iter = elementList.begin(); iter != elementList.end(); ++iter){
            QWebElement tempElement = *iter;
            if(tempElement.geometry().contains(testPoint)){
                annotationText = element.toPlainText();
                return true;
            }
        }
    }

    return false;
}

int Renderer::getTextPosition(const QString &text)
{
    QWebElementCollection elements = page->mainFrame()->findAllElements("P");
    for(int i = 0; i < elements.count(); ++i)
    {
        QWebElement element = elements.at(i);
        if(element.toPlainText().contains(text))
            return element.geometry().y();
    }

    return -1;
}



void Renderer::render()
{

    disconnect(page, SIGNAL(loadFinished(bool)), this, SLOT(render()));
    int fullHeight = page->mainFrame()->contentsSize().height();
    emit contentHeight(fullHeight);

    // int pageHeight = page->preferredContentsSize().height();
    // int pageWidth = page->preferredContentsSize().width();
    int pageHeight = pageSize.height(); 
    int pageWidth = pageSize.width(); 

    if(!pixmapRendering){
        emit webPageLoaded(page);        
        return;
    }


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
    emit numberOfPages(pageCount);
    page->setPreferredContentsSize(QSize(fullWidth, pageHeight));
    page->setViewportSize(QSize(fullWidth, pageHeight));

    int pageX = 0;
    int i = 0;
    const unsigned int padding = 10;
    while(i < pageCount){
        pageX = (i * pageWidth) - padding;

        RenderTile *tile = new RenderTile(current.first, i, QPoint(pageX, 0), QSize(pageWidth, pageHeight));
        emit tileRendered(tile);
        i++;

    }

    emit numberOfPages(i);

    //Set the viewport and preferred size back to page size so webkit renders scaling elements correctly.
    page->setViewportSize(QSize(pageWidth, pageHeight));
    page->setPreferredContentsSize(QSize(pageWidth, pageHeight));

    handleQueue();


}

void Renderer::setCSSSetting(QString setting, QString value){
    cssSettings.insert(setting, value);
}

void Renderer::setHTMLAttribute(QString attribute, QString value){
    htmlAttributes.insert(attribute, value);
}

void Renderer::makeDOMChangesEffective(){
    QWebElement documentElement = page->mainFrame()->documentElement();
    setDOMElementSettings(documentElement);
}

void Renderer::setDOMElementSettings(QWebElement &parentElement){
    QWebElement element = parentElement.firstChild(); 
    while(!element.isNull())
    {
        //CSS
        for(QHash<QString, QString>::iterator iter = cssSettings.begin(); iter != cssSettings.end(); ++iter){
            if(!element.hasClass("MebookAnnotation"))
                element.setStyleProperty(iter.key(), (*iter));

            //TODO: Handle image scaling better.
            if(element.tagName() == "IMG"){
                if(renderSettings){
                    int maxWidth = renderSettings->getMaxImageSize().width();
                    int maxHeight = renderSettings->getMaxImageSize().height();
                    element.setStyleProperty("max-width", QString::number(maxWidth) + "px");
                    element.setStyleProperty("max-height", QString::number(maxHeight) + "px");
                }
                element.setStyleProperty("margin", "0px");
                element.setStyleProperty("float", "center");
            }
        }

        //HTML attributes
        for(QHash<QString, QString>::iterator iter = htmlAttributes.begin(); iter != htmlAttributes.end(); ++iter){
            if(element.hasAttribute(iter.key()))
                element.setAttribute(iter.key(), *iter);
        }

        //Draw annotation backgrounds
        if(element.tagName() == "P"){
            QList<Annotation> list = annotationList;
            for(QList<Annotation>::iterator ai = list.begin(); ai != list.end(); ++ai){
                Annotation an = *ai;
                if(element.toPlainText().contains(an.getParagraph())){
                    QString text = element.toPlainText();
                    QColor color = an.getColor();
                    if(renderSettings){
                        if(renderSettings->getBackgroundColor().lightness() > color.lightness())
                            color = color.lighter();
                        else
                            color = color.darker();
                    }
                    text.replace(an.getText(), "<span class=\"MebookAnnotation\" style=\"background-color:" + color.name() + "\">" + an.getText() + "</span>");
                    element.setInnerXml(text);
                }
            }
        }

        setDOMElementSettings(element);
        element = element.nextSibling();
    }
}

//Urls in text cause trouble because they can't be reflowed
//TODO: Replace removed url with a link
void Renderer::parseLinks(QString &content){
    content = content.replace(QRegExp("[\\[\\s:]\\http://[\\]/#.a-z0-9]*"), "<a href=#>(link)</a>");
}

void Renderer::paginateDocument(QWebElement &parent)
{
    int pageHeight = page->preferredContentsSize().height();
    QWebElement element = parent.firstChild(); 
    while(!element.isNull())
    {
        if(element.tagName() != "BODY"){
            if(element.tagName() == "DIV"){
                if((element.geometry().height() + testHeight) < pageHeight){
                    testHeight += element.geometry().height();
                    break;
                }
            }
            else if(element.tagName() == "P"){
                testHeight += element.geometry().height();
                if(testHeight > pageHeight){
                    QString s = "</p><br><p class=\"testClass\">";
                    int over = testHeight - pageHeight;
                    float percentage = 1 - ((float)over / (float)element.geometry().height());
                    QStringList words = element.toInnerXml().split(" ");

                    int insertPoint = words.count() * percentage;

                    words.insert(insertPoint - 10, s);

                    element.setInnerXml(words.join(" "));
                    testHeight = over;
                }
            }else{
                testHeight += element.geometry().height();
            }
        }
        paginateDocument(element);
        element = element.nextSibling();
    }
}
