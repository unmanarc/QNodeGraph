#include "groupwidget.h"
#include "qnamespace.h"
#include "abstractnodewidget.h"
#include "qpainter.h"
#include "xmlfunctions.h"
#include "graphwidget.h"
#include "arrange.h"

#include <QMouseEvent>
#include <QTextStream>
#include <QIODevice>

#define SPACING_VSIDES_GROUP 3
#define SPACING_TEXT_OFFSET 5
using namespace QNodeGraph;


void GroupWidget::localInit()
{
    setInternalObjectID();

    setAutoArrange(true);
    setAutoArrangeSpacing(6);
    setAutoArrangeAlgorithm( Arrange::ARRANGEALG_ROWS );
    setSortBy(Arrange::SORTBY_INSERT_POS);
    setTitleBackgroundColor(QColor(50,50,50,200));
    setTextColor(Qt::white);
    setSubTextColor(Qt::white);

    // TODO: implement text align
    setSubTextAlignFlags(Qt::AlignCenter);
    setTextAlignFlags(Qt::AlignCenter);

    setResizable(true);
    resizingX = false;
    resizingY = false;

    // Recalculate Widget Size/Resize based all available info.
    recalculateSize();
    // Setup a random position over the workspace
    moveToRandom();

    // Autosort/arrange items in workspace
    Arrange::triggerAutoArrange((QWidget *)parent());
}

void GroupWidget::setInternalObjectID()
{
    setObjectName("GROUP-" + id);
}

const QColor &GroupWidget::getTitleBackgroundColor() const
{
    return titleBackgroundColor;
}

void GroupWidget::setTitleBackgroundColor(const QColor &newTitleColor)
{
    titleBackgroundColor = newTitleColor;
}

int GroupWidget::getAutoArrangeSpacing() const
{
    return autoArrangeSpacing;
}

void GroupWidget::setAutoArrangeSpacing(int newAutoArrangeSpacing)
{
    autoArrangeSpacing = newAutoArrangeSpacing;
}

GroupWidget::GroupWidget(QWidget *parent, const QString & xml) : AbstractNodeWidget(parent,xml, "GroupWidget")
{
    localInit();
}

GroupWidget::GroupWidget(const QString &id, QSize size, QWidget * parent, const QString & text, const QString &subtext) : AbstractNodeWidget(id,parent,text,subtext)
{
    resize(size);
    localInit();
}

QList<ItemWidget *> GroupWidget::allRecursiveItems()
{
    return GraphWidget::allChildrenItems(this);
}

void GroupWidget::selectAllItems()
{
    for (auto obj : GraphWidget::allChildrenItems(this))
    {
         obj->setSelected(true);
    }
}

GroupWidget::GroupWidget(const QString &id, QSize size, QWidget *parent) : AbstractNodeWidget(id,parent)
{
    resize(size);
    localInit();
}

GroupWidget::~GroupWidget()
{
    for ( auto i : allRecursiveItems() )
    {
        delete i;
    }
}

void GroupWidget::paintEvent(QPaintEvent * e)
{
    QPainter painter(this);


    /*qreal inverseDPR = 1.0 / ((QWidget *)parent())->devicePixelRatio();
    painter.scale(inverseDPR, inverseDPR);*/

    QFontMetrics textFontMetrics(textFont);
    QFontMetrics subTextFontMetrics(subTextFont);

    QColor borderColorNow = borderColor;
    QColor selectedBorderColorNow = selectedBorderColor;

    if (GRAPH->getBackgroundColor() == selectedBorderColor)
        selectedBorderColorNow = QColor(
                    255-GRAPH->getBackgroundColor().red(),
                    255-GRAPH->getBackgroundColor().green(),
                    255-GRAPH->getBackgroundColor().blue()
                    );


    QPoint endp = QPoint( size().width()-(SPACING_BORDER1*2), SPACING_BORDER1+textFontMetrics.height()+SPACING_BORDER1+subTextFontMetrics.height()+SPACING_VSIDES_GROUP);

    painter.setPen(Qt::transparent);
    auto titleC = getTitleBackgroundColor();
    if (selected || mouseover)
        titleC.setAlpha(255);
    painter.setBrush(titleC);
    painter.drawRect( QRect( QPoint( SPACING_BORDER1,SPACING_BORDER1), QPoint(endp.x(),endp.y()) ) );

    // Rectangle color
    if (selected || mouseover)
        painter.setPen( selectedBorderColorNow);
    else
        painter.setPen( borderColorNow );

    // Fill brush:
    switch (this->fillMode)
    {
    case AbstractNodeWidget::ITEMBOX_FILL_GRADIENT:
    {
        QLinearGradient linearGrad(QPointF(size().width()/2, 0), QPointF(size().width()/2, size().height() ));
        linearGrad.setColorAt(0, fillColor);
        linearGrad.setColorAt(1, fillColor2);
        painter.setBrush( QBrush(linearGrad) );
    }break;
    case AbstractNodeWidget::ITEMBOX_FILL_SOLID:
    {
        painter.setBrush( QBrush(fillColor) );
    }break;
    default:
    case AbstractNodeWidget::ITEMBOX_FILL_TRANSPARENT:
    {
        painter.setBrush( QBrush(Qt::transparent) );
    }break;
    }

    // External Rectangle:
    QRect rectangle(SPACING_BORDER1, SPACING_BORDER1, size().width()-(SPACING_BORDER1*2), size().height()-(SPACING_BORDER1*2));

    if (!borderRoundRectPixels)
        painter.drawRect(rectangle);
    else
        painter.drawRoundedRect(rectangle, borderRoundRectPixels, borderRoundRectPixels);

    // Line separator...
    painter.drawLine( QLine( QPoint( SPACING_BORDER1,                    SPACING_BORDER1+textFontMetrics.height()+SPACING_BORDER1+subTextFontMetrics.height()+SPACING_VSIDES_GROUP ), // X,Y
                             endp ) // X,Y
                      );

    // Draw texts:
    painter.setFont( textFont );
    painter.setPen( textColor );
    painter.drawText( QRect(SPACING_BORDER1*3+SPACING_TEXT_OFFSET,
                            SPACING_BORDER1*3,
                            size().width()-(SPACING_BORDER1*3*2)-SPACING_TEXT_OFFSET,
                            textFontMetrics.height()+SPACING_BORDER1), text );
    painter.setFont( subTextFont );
    painter.setPen( subTextColor );
    painter.drawText( QRect(SPACING_BORDER1*3+SPACING_TEXT_OFFSET,
                            SPACING_BORDER1*3+textFontMetrics.height(),
                            size().width()-(SPACING_BORDER1*3)-SPACING_TEXT_OFFSET,
                            subTextFontMetrics.height()+SPACING_BORDER1
                            ), subText );

    QWidget::paintEvent(e);
}

QString GroupWidget::getXMLLocal()
{
    QString exportedXML;
    exportedXML.append("<Items>");

    for (auto item : GraphWidget::allChildrenItems(this))
    {
        exportedXML.append(item->getXML("ItemWidget",false));
    }

    exportedXML.append("</Items>");
    return exportedXML;
}

QString GroupWidget::getXMLLocalProperties()
{
    QString exportedXML;  

    exportedXML.append( XMLFunctions::createSimpleTag("textAlignFlags", (uint64_t)textAlignFlags));
    exportedXML.append( XMLFunctions::createSimpleTag("subTextAlignFlags", (uint64_t)subTextAlignFlags));

    exportedXML.append(XMLFunctions::createColorXMLTag("titleBackgroundColor",titleBackgroundColor));

    exportedXML.append( XMLFunctions::createSimpleTag("width", (uint64_t)size().width()));
    exportedXML.append( XMLFunctions::createSimpleTag("height", (uint64_t)size().height()));

    return exportedXML;
}

bool GroupWidget::setXMLLocalProperties(const QDomNode &child)
{
    if (child.toElement().tagName() == "textAlignFlags")
    {
        textAlignFlags = child.toElement().text().toInt();
    }
    else if (child.toElement().tagName() == "subTextAlignFlags")
    {
        subTextAlignFlags = child.toElement().text().toInt();
    }
    else if (child.toElement().tagName() == "titleBackgroundColor")
    {
        titleBackgroundColor = XMLFunctions::colorFromXML(child);
    }
    if (child.toElement().tagName() == "width")
    {
        resize( child.toElement().text().toInt(), height());
    }
    else if (child.toElement().tagName() == "height")
    {
        resize( width(), child.toElement().text().toInt());
    }

    else
        return false;
    return true;
}

bool GroupWidget::setXMLLocal(const QDomNode &child)
{

     if (child.toElement().tagName() == "Items")
    {
        QDomNode itemChilds = child.toElement().firstChild();
        while (!itemChilds.isNull())
        {
            if (itemChilds.toElement().tagName()=="ItemWidget")
            {
                // Create new ItemWidget element here.
                QString parseData;
                QTextStream textStream(&parseData, QIODevice::WriteOnly);
                itemChilds.save(textStream,1);
                new ItemWidget(this,parseData);
            }
            itemChilds = itemChilds.nextSibling();
        }
    }

     return true;
}

void GroupWidget::recalculateSize()
{
    QFontMetrics textFontMetrics(textFont);
    QFontMetrics subTextFontMetrics(subTextFont);

    verticalOffset = SPACING_BORDER1+textFontMetrics.height()+SPACING_VSIDES_GROUP+subTextFontMetrics.height()+3+SPACING_VSIDES_GROUP;
    GraphWidget::replaceMinimumSize(GRAPH);
}

int GroupWidget::getSortBy() const
{
    return sortBy;
}

void GroupWidget::setSortBy(int newSortBy)
{
    sortBy = newSortBy;
}

int GroupWidget::getAutoArrangeAlgorithm() const
{
    return autoArrangeAlgorithm;
}

void GroupWidget::setAutoArrangeAlgorithm(int newAutoArrangeAlgorithm)
{
    autoArrangeAlgorithm = newAutoArrangeAlgorithm;
}

bool GroupWidget::getAutoArrange() const
{
    return autoArrange;
}

void GroupWidget::setAutoArrange(bool newAutoArrange)
{
    autoArrange = newAutoArrange;
}

bool GroupWidget::getResizable() const
{
    return resizable;
}

void GroupWidget::setResizable(bool newResizable)
{
    resizable = newResizable;
}

void GroupWidget::mouseMoveEvent(QMouseEvent *e)
{
    mouseCurrentPos = e->pos();

    if (resizable)
    {
       GraphWidget::resizeOnMouseMove(this,e,resizingX,resizingY,mouseCurrentPos);
       if (resizingX || resizingY)
       {
           GraphWidget::checkAndFixChildrenPositions(this);

           GraphWidget::replaceMinimumSize(GRAPH);

           // Autosort/arrange items in workspace
           Arrange::triggerAutoArrange(this);


           GRAPH->repaint();
       }
    }
    // IF not resizing ... give control to the parent.
    if (!resizingX && !resizingY)
        AbstractNodeWidget::mouseMoveEvent(e);
}

void GroupWidget::mousePressEvent(QMouseEvent *e)
{
    if (!GRAPH->getIsDuringManualLinking() && e->button()==Qt::LeftButton)
    {
        if (!childAt( e->pos() ))
        {
            // Not selecting any item... starting to resize or create a rectangle...
            if ( GraphWidget::resizeStartOnClick(this,mouseCurrentPos,resizable,&resizingX,&resizingY ))
            {
            }
        }
    }

    // IF not resizing here ... give control to the parent.
    if (!resizingX && !resizingY)
        AbstractNodeWidget::mousePressEvent(e);
}

void GroupWidget::mouseReleaseEvent(QMouseEvent *event)
{
    // End resizing.
    resizingX = false;
    resizingY = false;
    AbstractNodeWidget::mouseReleaseEvent(event);
}

void GroupWidget::setSubTextAlignFlags(int newSubTextAlignFlags)
{
    subTextAlignFlags = newSubTextAlignFlags;
}

void GroupWidget::setTextAlignFlags(int newTextAlignFlags)
{
    textAlignFlags = newTextAlignFlags;
}
