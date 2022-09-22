#include "itemwidget.h"
#include <QIcon>
#include <QDebug>

#include "link.h"
#include "abstractnodewidget.h"
#include "xmlfunctions.h"
#include "graphwidget.h"
#include "arrange.h"

#include <QBuffer>
#include <QPainter>
#include <cmath>

using namespace QNodeGraph;

void ItemWidget::setInternalObjectID()
{
    setObjectName("ITEM-" + id);
}

void ItemWidget::localInit()
{
    setInternalObjectID();

    // Icon:
    QPixmap x(32,32);
    x.fill(Qt::black);
    icon = new QIcon(x);
    setIcon(GRAPH->getDefaultItemIcon());
    setIconSize(GRAPH->getDefaultItemIconSize());

    // Texts:
    description = "";
    subText = "";
    setTextPosition(GRAPH->getDefaultItemTextPosition());

    // Shape:
    setShape(GRAPH->getDefaultItemShape());

    // Selection/Mouse:
    setSelected(false);
    setMouseTracking(true);
    mouseover = false;

    // Layers/Zoom:
    setBelongsToLayerZero(false);
    setZoomOutLevel(0);

    // Filter:
    currentFilterMatch = false;

    ////////////////////////////////////////////////////////////////////////////
    recalculateSize();

    // Setup a random position over the workspace
    moveToRandom();

    // Autosort/arrange items in workspace
    Arrange::triggerAutoArrange((QWidget *)parent());
}

ItemWidget::ItemWidget(QWidget *parent, const QString & xml) : AbstractNodeWidget(parent,xml, "ItemWidget")
{
    localInit();
}

ItemWidget::ItemWidget(const QString &id, QWidget * parent, const QString & text, const QString &subtext) : AbstractNodeWidget(id,parent,text,subtext)
{
    localInit();
}
ItemWidget::ItemWidget(const QString & id, QWidget * parent, const QString &text, const QString &subtext, int zoomOutLevel) : AbstractNodeWidget(id,parent,text,subtext)
{
    localInit();
    setZoomOutLevel(zoomOutLevel);
}

ItemWidget::ItemWidget(const QString &id, QWidget *parent) : AbstractNodeWidget(id,parent)
{
    localInit();
}

ItemWidget::~ItemWidget()
{
    // TODO: orphan links?
    // Destroy links
    while (links.size())
    {
        Link * reldestroy = (Link *) links[0];
        ItemWidget * nextItem =  (ItemWidget *) (reldestroy->getItem1()==this ? reldestroy->getItem2() : reldestroy->getItem1()) ;
        nextItem->removeLink(this, true);
        links.removeAll(reldestroy);
    }

    if ( icon != nullptr )
        delete icon;
}

void ItemWidget::setIcon(const QIcon &icon)
{
    if ( this->icon != nullptr )
        delete this->icon;
    this->icon = new QIcon( icon );
}

void ItemWidget::setIconSize(int w)
{
    setIconSize(w,w);
}

void ItemWidget::setIconSize(int w, int h)
{
    IconSize.setWidth ( w );
    IconSize.setHeight ( h );
    recalculateSize();
}

void ItemWidget::setTextPosition(const ItemWidget::TextPosition & textPosition)
{
    this->textPosition = textPosition;
    recalculateSize();
}

void ItemWidget::setZoomOutLevel(const unsigned int &zoomOutLevel)
{
    this->zoomOutLevel = zoomOutLevel;
    recalculateSize();
}

void ItemWidget::filter(const QString &filterText)
{
    if (filterText==QString())
    {
        currentFilterMatch = false;
    }
    else if (text.toLower().contains(filterText.toLower()) ||
             subText.toLower().contains(filterText.toLower()) ||
             description.toLower().contains(filterText.toLower()) ||
             embeddedData.toLower().contains(filterText.toLower()))
        currentFilterMatch = true;
    else
    {
        currentFilterMatch = false;
        for (const auto & tag: tags)
        {
            if (tag.toLower().contains(filterText.toLower()))
                currentFilterMatch = true;
        }
    }
    update();
}

void ItemWidget::paintEvent(QPaintEvent * e)
{
    // TODO: pixmaps...
    QPixmap pAnchor;
    QPixmap pLayerZero;

    recalculateSize();
    // Draw icon and propietries...
    QPainter painter(this);


    /*qreal inverseDPR = 1.0 / ((QWidget *)parent())->devicePixelRatio();
    painter.scale(inverseDPR, inverseDPR);*/

    QColor rectBorderColor = borderColor;

    if (currentFilterMatch)
        rectBorderColor.setRgb(255,0,0);

    // Set the relative size according to the selection + layer.
    double layerSizeMultiplier = ( mouseover || selected ? 1.0 : 1.0-( zoomOutLevel>7? 0.6 : zoomOutLevel/10.0 ) );

    QColor colorSelectedNow = selectedBorderColor;
    if (GRAPH->getBackgroundColor() == selectedBorderColor)
        colorSelectedNow = QColor(
                    255-GRAPH->getBackgroundColor().red(),
                    255-GRAPH->getBackgroundColor().green(),
                    255-GRAPH->getBackgroundColor().blue()
                    );
    painter.setPen(rectBorderColor);

    QColor localFillColor = fillColor;
    QColor localFillColor2 = fillColor2;

    if (selected || mouseover)
        localFillColor.setAlphaF( 1.0 );
    if (selected || mouseover)
        localFillColor2.setAlphaF( 1.0 );

    switch (this->fillMode)
    {
    case ITEMBOX_FILL_GRADIENT:
    {
        QLinearGradient linearGrad(QPointF(size().width()/2, 0), QPointF(size().width()/2, size().height() ));
        linearGrad.setColorAt(0, localFillColor );
        linearGrad.setColorAt(1, localFillColor2 );
        painter.setBrush( QBrush(linearGrad) );
    }break;
    case ITEMBOX_FILL_SOLID:
    {
        painter.setBrush( localFillColor );
    }break;
    default:
    case ITEMBOX_FILL_TRANSPARENT:
    {
        painter.setBrush( (mouseover || selected) ? Qt::black: Qt::transparent );
    }break;
    }

    switch ( this->shape )
    {
    case ITEMBOX_SHAPE_BOX:
    {
        // Draw solo rectangle
        QRect rectangle(SPACING_BORDER1, SPACING_BORDER1, size().width()-(SPACING_BORDER1*2), size().height()-(SPACING_BORDER1*2));

        if ( selected || mouseover )
            painter.setPen(colorSelectedNow);

        if (!borderRoundRectPixels)
            painter.drawRect(rectangle);
        else
            painter.drawRoundedRect(rectangle, borderRoundRectPixels, borderRoundRectPixels);
    } break;
    case ITEMBOX_SHAPE_CIRCLE:
    {
        QRect rectangle(0.0, 0.0, size().width(), size().height());

        // Draw circle/ellipse...
        if ( selected || mouseover )
            painter.setPen(colorSelectedNow);

        painter.drawEllipse(rectangle.center(),(rectangle.width()/2)-1, (rectangle.height()/2)-1 );
    } break;
    default:
    case ITEMBOX_SHAPE_NONE:
    {
    } break;
    }

    unsigned int iconHeight = IconSize.height() * layerSizeMultiplier;
    unsigned int iconWidth = IconSize.width() * layerSizeMultiplier;

    // Icon Pixmap
    QPixmap iconPixmap = icon->pixmap(iconWidth,iconHeight, (mouseover || selected) ? QIcon::Selected : QIcon::Normal );

    switch (textPosition)
    {
    case TEXTPOS_RIGHT:
    {
        // Draw Icon pixmap
        painter.drawPixmap(SPACING_BORDER1+SPACING_HSIDES,
                           SPACING_BORDER1+SPACING_VSIDES, iconPixmap);

        if (anchored)
            painter.drawPixmap(3,3+iconPixmap.height()-8, pAnchor);
        if (belongsToLayerZero)
            painter.drawPixmap(3,3, pLayerZero);

        // Draw Text
        QFont fontTextSB = textFont;
        fontTextSB.setPointSize(fontTextSB.pointSize()*layerSizeMultiplier);
        QFontMetrics metrics(fontTextSB);
        painter.setPen(textColor);
        painter.setFont(fontTextSB);
        painter.drawText( SPACING_BORDER1+SPACING_HSIDES+iconWidth+SPACING_HSIDES,
                          (size().height()/2)-(SPACING_HSIDES/2)-metrics.height(),
                          size().width()-(SPACING_BORDER1+SPACING_HSIDES+iconWidth+SPACING_HSIDES+SPACING_HSIDES+SPACING_BORDER1),
                          metrics.height(),
                          Qt::AlignCenter, text);

        // Draw SubText
        QFont fontSubTextSB = subTextFont;
        fontSubTextSB.setPointSize(fontSubTextSB.pointSize()*layerSizeMultiplier);
        QFontMetrics submetrics(fontSubTextSB);
        painter.setPen( (selected || mouseover)? selectedBorderColor : subTextColor);
        painter.setFont(fontSubTextSB);
        painter.drawText( SPACING_BORDER1+SPACING_HSIDES+iconWidth+SPACING_HSIDES,
                          (size().height()/2)+(SPACING_HSIDES/2),
                          size().width()-(SPACING_BORDER1+SPACING_HSIDES+iconWidth+SPACING_HSIDES+SPACING_HSIDES+SPACING_BORDER1),
                          submetrics.height(),
                          Qt::AlignCenter, subText);
    }
        break;
    case TEXTPOS_LEFT:
    {
        // Draw pixmap
        painter.drawPixmap( size().width()-(iconWidth+SPACING_BORDER1+SPACING_HSIDES),
                            SPACING_BORDER1+SPACING_HSIDES, iconPixmap);

        if (anchored)
            painter.drawPixmap(size().width()-iconWidth, 3+iconPixmap.height()-8, pAnchor);
        if (belongsToLayerZero)
            painter.drawPixmap(size().width()-iconWidth, 3+iconPixmap.height()-8, pLayerZero);

        // Draw Text:
        QFont fontTextSB = textFont;
        fontTextSB.setPointSize(fontTextSB.pointSize()*layerSizeMultiplier);
        QFontMetrics metrics(fontTextSB);
        painter.setPen(textColor);
        painter.setFont(fontTextSB);
        painter.drawText( SPACING_BORDER1+SPACING_HSIDES,
                          (size().height()/2)-(SPACING_VSIDES/2)-metrics.height(),
                          size().width()-(SPACING_BORDER1+SPACING_HSIDES+iconWidth+SPACING_HSIDES+SPACING_HSIDES+SPACING_BORDER1),
                          metrics.height(),
                          Qt::AlignCenter, text);

        // Draw SubText
        QFont fontSubTextSB = subTextFont;
        fontSubTextSB.setPointSize(fontSubTextSB.pointSize()*layerSizeMultiplier);
        QFontMetrics submetrics(fontSubTextSB);
        painter.setPen( (selected || mouseover)? selectedBorderColor : subTextColor);
        painter.setFont(fontSubTextSB);
        painter.drawText( SPACING_BORDER1+SPACING_HSIDES,
                          (size().height()/2)+(SPACING_VSIDES/2),
                          size().width()-(SPACING_BORDER1+SPACING_HSIDES+iconWidth+SPACING_HSIDES+SPACING_HSIDES+SPACING_BORDER1),
                          submetrics.height(),
                          Qt::AlignCenter, subText);
    }
        break;
    case TEXTPOS_BOTTOM:
    {
        // Draw pixmap
        painter.drawPixmap( (size().width()/2) - (iconWidth/2) ,  (size().height()/2)-iconHeight , iconPixmap);

        if (anchored)
            painter.drawPixmap(size().width()/2 - iconWidth/2 +  3, 3+iconPixmap.height()-8, pAnchor);
        if (belongsToLayerZero)
            painter.drawPixmap(size().width()/2 - iconWidth/2 +  3,3, pLayerZero);

        // Draw Text:
        QFont fontTextSB = textFont;
        fontTextSB.setPointSize(fontTextSB.pointSize()*layerSizeMultiplier);
        QFontMetrics metrics(fontTextSB);
        painter.setPen(textColor);
        painter.setFont(fontTextSB);
        painter.drawText(  0,  (size().height()/2) + (SPACING_VSIDES/2) , size().width(), metrics.height(), Qt::AlignCenter , text);

        // Draw SubText
        QFont fontSubTextSB = subTextFont;
        fontSubTextSB.setPointSize(fontSubTextSB.pointSize()*layerSizeMultiplier);
        QFontMetrics submetrics(fontSubTextSB);
        painter.setPen( (selected || mouseover)? selectedBorderColor : subTextColor);
        painter.setFont(fontSubTextSB);
        painter.drawText(  0,  (size().height()/2) + (SPACING_VSIDES/2) + metrics.height() + (SPACING_VSIDES/2) , size().width(), submetrics.height(), Qt::AlignCenter , subText);
    }
        break;
    default:
        break;
    }

    QWidget::paintEvent(e);
}

void ItemWidget::recalculateSize()
{
    int x=0;
    int y=0;
    double zoomOutFactor = getZoomOutFactor();
    unsigned int iconHeight = (IconSize.height() * zoomOutFactor);
    unsigned int iconWidth = (IconSize.width() * zoomOutFactor);

    QFont fontTextSB = textFont;
    fontTextSB.setPointSize(fontTextSB.pointSize()*zoomOutFactor);
    QFontMetrics metrics(fontTextSB);
    unsigned int textWidth     = (unsigned int)metrics.horizontalAdvance(text);
    unsigned int textHeight    = (unsigned int)metrics.height();

    QFont fontSubTextSB = subTextFont;
    fontSubTextSB.setPointSize(fontSubTextSB.pointSize()*zoomOutFactor);
    QFontMetrics subfmetrics(fontSubTextSB);
    unsigned int subtextWidth  = (unsigned int)subfmetrics.horizontalAdvance(subText);
    unsigned int subtextHeight = (unsigned int)subfmetrics.height();

    textWidth = (textWidth>subtextWidth? textWidth : subtextWidth);
    if (textPosition==TEXTPOS_RIGHT || textPosition==TEXTPOS_LEFT)
    {
        int verticalMinSize = std::max(iconHeight,textHeight+2+subtextHeight);

        x= SPACING_BORDER1+SPACING_HSIDES+iconWidth+SPACING_HSIDES+textWidth+SPACING_HSIDES+SPACING_BORDER1;
        y= SPACING_BORDER1+SPACING_VSIDES+verticalMinSize+SPACING_VSIDES+SPACING_BORDER1;
    }
    else if ( textPosition==TEXTPOS_BOTTOM )
    {
        int horizontalMinSize = std::max( iconWidth, textWidth );

        x=SPACING_BORDER1+SPACING_HSIDES+horizontalMinSize+SPACING_HSIDES+SPACING_BORDER1;
        y=SPACING_BORDER1+SPACING_VSIDES+iconHeight+SPACING_VSIDES+textHeight+SPACING_VSIDES+subtextHeight+SPACING_VSIDES+SPACING_BORDER1;

        if ( shape == ITEMBOX_SHAPE_CIRCLE || shape== ITEMBOX_SHAPE_NONE )
        {
            // R2 is the radius for the text.
            auto R2 = std::max(
                        sqrt( pow(textHeight+subtextHeight+2,2) + pow((subtextWidth/2) ,2)  ),
                        sqrt( pow(textHeight+1,2) + pow((textWidth/2) ,2) )
                        );
            // R1 is the radius for the icon..
            auto R1 = sqrt( pow(iconWidth/2,2) + pow(iconHeight,2)  );

            y=x=std::max(R1*2,R2*2)+5;
        }
    }

    resize(x,y);

    if (groupParent)
    {
        GraphWidget::replaceMinimumSize(groupParent);
    }
    else
    {
        GraphWidget::replaceMinimumSize(GRAPH);
    }
}

ItemWidget::ItemBoxShape ItemWidget::getShape() const
{
    return shape;
}

void ItemWidget::setShape(ItemWidget::ItemBoxShape newShape)
{
    this->shape = newShape;
}

void ItemWidget::addTag(const QString &tag)
{
    tags.insert(tag);
}

void ItemWidget::removeTag(const QString &tag)
{
    tags.remove(tag);
}

QSet<QString> ItemWidget::getTags()
{
    return tags;
}

bool ItemWidget::containsTag(const QString &tag)
{
    return tags.contains(tag);
}

bool ItemWidget::isLinkedTo(ItemWidget * item)
{
    if (!item || item==this)
        return false;

    Link * newLink = new Link;
    newLink->setItems(this, item);

    // buscar.
    for (int i=0;i<links.size();i++)
    {
        if ((*(Link *)links[i])==(* newLink))
        {
            delete newLink;
            return true;
        }
    }
    delete newLink;
    return false;
}

void ItemWidget::linkItem(ItemWidget * itemToLink, const QString & linkName, const QColor & linkColor, const Link::Type & linkType, const Link::Direction & arcDirection )
{
    if (!itemToLink || itemToLink==this)
        return;

    Link * link = new Link;

    link->setItems(this, itemToLink);
    link->setDescription( linkName );
    link->setType(linkType);
    link->setColor(linkColor);
    link->setArcDirection(arcDirection);

    // search for duplicate previous links and remove them
    for (int i=0;i<links.size();i++)
    {
        if ((*(Link *)links[i])==(* link))
        {
            links.removeAll(links[i]);
            i=-1;
        }
    }

    // Insert...
    links.insert(0,link);

    // Insert in the oppposite/linked item.
    itemToLink->addLink(link);

    // Autosort items in workspace
    Arrange::triggerAutoArrange(GRAPH);
}

void ItemWidget::addLink(void * linkPtr)
{
    Link * link = (Link *) linkPtr;

    // Search for duplicated and remove them...
    for (int i=0;i<links.size();i++)
    {
        if ((*(Link *)links[i])==(* link))
        {
            links.removeAll(links[i]);
            i=-1;
        }
    }

    // Insert...
    links.insert(0,linkPtr);
}

void ItemWidget::removeLink(ItemWidget * linkedNode, bool destroyLinkObject)
{
    Link x;
    x.setItems(this,linkedNode);

    // Search for this item and duplicates and remove them...
    for (int i=0;i<links.size();i++)
    {
        if ((*(Link *)links[i])==( x))
        {
            Link * linkToRemove = ( Link *) links[i];
            if (destroyLinkObject)
                delete linkToRemove;
            links.removeAll(linkToRemove);
            i=-1;
        }
    }
}

QList< void * > ItemWidget::getLinks()
{
    return links;
}

QPoint ItemWidget::getIconCenterPoint() const
{
    double zoomOutFactor = getZoomOutFactor();
    unsigned int iconHeight = (IconSize.height() * zoomOutFactor)/2;
    unsigned int iconWidth = (IconSize.width() * zoomOutFactor)/2;
    QPoint mp;

    if (fillMode == ITEMBOX_FILL_SOLID)
    {
        // Solid originates from the element center.
        mp.setX(size().width()/2);
        mp.setY(size().height()/2);
    }
    else
    {
        // Transparent fill mode will originate from icon.
        switch (textPosition)
        {
        case TEXTPOS_RIGHT:
        {
            mp.setX(iconWidth+3);
            mp.setY(iconHeight+3);
        }
            break;
        case TEXTPOS_LEFT:
        {
            mp.setX(size().width()-iconWidth);
            mp.setY(iconHeight+3);
        }
            break;
        case TEXTPOS_BOTTOM:
        {
            mp.setX(size().width()/2);
            mp.setY( ((size().height()/2.0)-(iconHeight/2.0)) );
        }
            break;
        default:
            break;
        }
    }
    return mp;
}

double ItemWidget::getZoomOutFactor() const
{
    return ( mouseover || selected ? 1.0 : 1.0-( zoomOutLevel>7? 0.6 : zoomOutLevel/10.0 ) );
}

void ItemWidget::setLayer(const int &currentLayer)
{
    this->currentLayer=currentLayer;
}

int ItemWidget::getLayer() const
{
    return currentLayer;
}

void ItemWidget::setSortPosition(const int & sortPosition)
{
    this->sortPosition=sortPosition;
}

int ItemWidget::getSortPosition() const
{
    return sortPosition;
}

bool ItemWidget::assignLayerRecursively(const int &currentLayer)
{
    if (this->currentLayer==-1 )
    {
        if ( belongsToLayerZero && currentLayer!=0 )
            return false;

        this->currentLayer=currentLayer;

        for (int i=0;i<links.size();i++)
        {
            ItemWidget * nextItem1 = (ItemWidget *)(((Link *)links[i])->getItem1());
            ItemWidget * nextItem2 = (ItemWidget *)(((Link *)links[i])->getItem2());

            // Recursive marking...
            nextItem1->assignLayerRecursively(currentLayer+1);
            nextItem2->assignLayerRecursively(currentLayer+1);
        }
    }
    else
        return false;

    return true;
}

bool ItemWidget::getBelongsToLayerZero() const
{
    return belongsToLayerZero;
}

void ItemWidget::setBelongsToLayerZero(const bool & belongsToLayerZero)
{
    this->belongsToLayerZero=belongsToLayerZero;
}

void ItemWidget::calcSortPosition()
{
    int parentCount = 0;
    int weightSum = 0;

    for (int i=0;i<links.size();i++)
    {
        ItemWidget * nextItem = (ItemWidget *)(((Link *)links[i])->getItem1());

        if (nextItem==this)
            nextItem = (ItemWidget *)(((Link *)links[i])->getItem2());

        if (nextItem->getSortPosition()>=0 && nextItem->getLayer()!=getLayer() )
        {
            // PARENT, already determined and not in the same layer
            parentCount++;
            weightSum+=nextItem->getSortPosition();
        }
    }

    // Avoid division by zero.
    if (parentCount>0)
        setSortPosition(weightSum/parentCount);
}

void ItemWidget::zoomOutLevelUp()
{
    if (zoomOutLevel<9)
        zoomOutLevel++;
}

void ItemWidget::zoomOutLevelDown()
{
    if (zoomOutLevel)
        zoomOutLevel--;
}

bool ItemWidget::setXMLLocalProperties(const QDomNode & child)
{
    if (child.toElement().tagName() == "tags")
    {
        tags.clear();
        for (QDomNode tagChild = child.firstChild();!tagChild.isNull(); tagChild = child.nextSibling())
        {
            QDomElement element = tagChild.toElement();
            if (element.tagName() == "tag")
            {
                addTag(QByteArray::fromBase64(QByteArray(element.text().toUtf8())));
            }
        }
    }
    else if (child.toElement().tagName() == "zoomOutLevel")
    {
        zoomOutLevel = child.toElement().text().toUInt();
    }
    else if (child.toElement().tagName() == "icon")
    {
        if (icon) delete icon;

        int x = child.toElement().attribute("x").toUInt();
        int y = child.toElement().attribute("y").toUInt();

        IconSize.setHeight(y);
        IconSize.setWidth(x);

        QByteArray bArrayElement = QByteArray::fromBase64(QByteArray(child.toElement().text().toUtf8()));
        QPixmap pArrayElement;
        pArrayElement.loadFromData(bArrayElement);
        icon = new QIcon(pArrayElement);
    }
    else if (child.toElement().tagName() == "textPosition")
    {
        textPosition = ((ItemWidget::TextPosition)child.toElement().text().toUInt());
    }
    else if (child.toElement().tagName() == "shape")
    {
        shape = ((ItemWidget::ItemBoxShape)child.toElement().text().toUInt());
    }
    else if (child.toElement().tagName() == "belongsToLayerZero")
    {
        belongsToLayerZero = child.toElement().text().toUInt()==1?true:false;
    }
    return true;
}

bool ItemWidget::SetXMLLinks(const QDomNode &master)
{
    QString id1;
    QString id2;
    QString link_name;
    QColor link_color;
    Link::Type linkType = Link::TYPE_UNDIRECTED;
    Link::Direction linkDirection = Link::DIR_BOTH;

    QDomNode child = master.firstChild();
    while (!child.isNull())
    {

        if (child.toElement().tagName() == "id1")
        {
            QByteArray bArrayElement = QByteArray::fromBase64(QByteArray(child.toElement().text().toUtf8()));
            id1 = QString().fromUtf8(bArrayElement.data(),bArrayElement.size());
        }
        if (child.toElement().tagName() == "id2")
        {
            QByteArray bArrayElement = QByteArray::fromBase64(QByteArray(child.toElement().text().toUtf8()));
            id2 = QString().fromUtf8(bArrayElement.data(),bArrayElement.size());
        }
        if (child.toElement().tagName() == "description")
        {
            QByteArray bArrayElement = QByteArray::fromBase64(QByteArray(child.toElement().text().toUtf8()));
            link_name = QString().fromUtf8(bArrayElement.data(),bArrayElement.size());
        }
        else if (child.toElement().tagName() == "color")
        {
            int r = child.toElement().attribute("r").toUInt();
            int g = child.toElement().attribute("g").toUInt();
            int b = child.toElement().attribute("b").toUInt();
            link_color.setRgb(r,g,b);
        }
        else if (child.toElement().tagName() == "linkType")
        {
            linkType = (Link::Type)child.toElement().text().toUInt();
        }
        else if (child.toElement().tagName() == "linkDirection")
        {
            linkDirection = (Link::Direction)child.toElement().text().toUInt();
        }
        child = child.nextSibling();
    }

    if (id1==id) // Link with the id2
        linkItem(GRAPH->getItemById(id2), link_name, link_color, linkType, linkDirection );
    else if (id2==id) //Link with the id1
        linkItem(GRAPH->getItemById(id1), link_name, link_color, linkType, linkDirection );

    return true;
}

bool ItemWidget::SetNodeXMLLinks(const QDomNode &master)
{
    QDomNode child = master.firstChild();
    while (!child.isNull())
    {
        if (child.toElement().tagName() == "link")
        {
            SetXMLLinks(child);
        }
        child = child.nextSibling();
    }
    return true;
}


bool ItemWidget::setXMLLocal(const QDomNode & child)
{
    if (child.toElement().tagName() == "links")
    {
        SetNodeXMLLinks(child);
    }
    return true;
}

QString ItemWidget::getXMLLocal()
{
    QString exportedXML;
    exportedXML.append("<links>");
    for (int i=0;i<links.count();i++)
    {
        exportedXML.append("<link>");
        Link * link = (Link *) links[i];

        ItemWidget *xitem1=(ItemWidget *)link->getItem1();
        ItemWidget *xitem2=(ItemWidget *)link->getItem2();

        exportedXML.append(QString("<id1>%1</id1>").arg( QString( xitem1->getID().toUtf8().toBase64()) ));
        exportedXML.append(QString("<id2>%1</id2>").arg( QString( xitem2->getID().toUtf8().toBase64()) ));
        exportedXML.append(QString("<description>%1</description>").arg( QString(link->getDescription().toUtf8().toBase64()) ));
        exportedXML.append(QString("<color r=\"%1\" g=\"%2\" b=\"%3\"></color>").arg(
                               QString().setNum(link->getColor().red()),
                               QString().setNum(link->getColor().green()),
                               QString().setNum(link->getColor().blue()))
                           );
        exportedXML.append(QString("<linkType>%1</linkType>").arg( QString().setNum(link->getType()) ));
        exportedXML.append(QString("<linkDirection>%1</linkDirection>").arg( QString().setNum(link->getArcDirection()) ));

        exportedXML.append("</link>");
    }
    exportedXML.append("</links>");
    return exportedXML;
}

QString ItemWidget::getXMLLocalProperties()
{
    QString exportedXML;

    QByteArray iconData;
    QBuffer iconDataBuffer(&iconData);
    iconDataBuffer.open(QIODevice::WriteOnly);
    icon->pixmap(IconSize.width(), IconSize.height()).save(&iconDataBuffer, "PNG");

    exportedXML.append( getXMLTags() );
    exportedXML.append( XMLFunctions::createSimpleTag("zoomOutLevel", (uint64_t)zoomOutLevel));
    exportedXML.append(QString("<icon x=\"%1\" y=\"%2\">%3</icon>").arg(
                           QString().setNum(IconSize.width()),
                           QString().setNum(IconSize.height()),
                           QString(iconData.toBase64())
                           ));

    exportedXML.append( XMLFunctions::createSimpleTag("textPosition", (uint64_t)textPosition));
    exportedXML.append( XMLFunctions::createSimpleTag("shape", (uint64_t)shape));
    exportedXML.append( XMLFunctions::createSimpleTag("belongsToLayerZero", belongsToLayerZero));
    return exportedXML;
}

QString ItemWidget::getXMLTags()
{
    QString r;
    r+="<tags>\n";
    for (const auto &tag: qAsConst(tags))
        r+=QString::asprintf("<tag>%s</tag>\n", tag.toUtf8().toBase64().toStdString().c_str());
    r+="</tags>\n";

    return r;
}

void ItemWidget::selectRecursivelyAllLinkedItems()
{
    if (getSelectionMark())
        return;

    setSelectionMark(true);
    setSelected(true);

    for (int i=0;i<links.count();i++)
    {
        Link * link = (Link *) links[i];
        ((ItemWidget *)link->getItem1())->selectRecursivelyAllLinkedItems();
        ((ItemWidget *)link->getItem2())->selectRecursivelyAllLinkedItems();
    }
}
