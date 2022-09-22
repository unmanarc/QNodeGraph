#include "graphwidget.h"
#include <QPainter>
#include <QString>
#include <QMouseEvent>
#include <QDomDocument>
#include <QDomElement>
#include <QFontMetrics>
#include <QTextStream>
#include <QDebug>
#include <QBuffer>

#include "qnamespace.h"
#include "groupwidget.h"
#include "link.h"
#include "itemwidget.h"

#include <math.h>

#include <QMessageBox>

#include "abstractnodewidget.h"
#include "xmlfunctions.h"

#include "arrange.h"

using namespace QNodeGraph;

GraphWidget::GraphWidget(QWidget *parent) : QWidget(parent)
{
    setAllowOverlap(false);
    setMouseTracking(true);
    setMinimumSize(250,250);
    resize(1200,800);
    mouseCurrentPos.setX(0);
    mouseCurrentPos.setY(0);
    mouseRect.setRect(-1,-1,-1,-1);
    keyCtrlActivated = false;

    addKeyAction(KEYACT_ESCAPE_KEY_DESELECT);

    setManualNodesLinking(false);
    setResizable(false);
    resizingX = false;
    resizingY = false;

    // Item Defaults:
    setDefaultNodeBorderRoundRectPixels(4);
    setDefaultItemShape(ItemWidget::ITEMBOX_SHAPE_BOX);
    setDefaultNodeSelectedBorderColor(QColor(130,160,255));
    setDefaultNodeBorderColor(Qt::gray);
    setDefaultNodeFillColor(QColor(80,80,80));
    setDefaultNodeFillColor2(QColor(0,0,0,127));
    setDefaultNodeFillMode(ItemWidget::ITEMBOX_FILL_GRADIENT);

    // Icon Defaults:
    setDefaultItemIcon(QIcon(":/computer.png"));
    setDefaultItemIconSize(32);

    // Text defaults:
    setDefaultNodeTextFont(QFont( "Monospace", 8, QFont::DemiBold ));
    setDefaultNodeSubTextFont(QFont( "Monospace", 7, QFont::Normal ));
    setDefaultNodeTextColor(Qt::white);
    setDefaultNodeSubTextColor(QColor(200,200,200));
    setDefaultItemTextPosition(ItemWidget::TEXTPOS_RIGHT);

    // Filter default:
    setFilterText(QString());

    // Arrange:
    setAutoArrange(false);
    setAutoArrangeAlgorithm(Arrange::ARRANGEALG_ROWS);
    setAutoArrangeSpacing(6);

    // Accept keyboard focus.
    setFocusPolicy(Qt::StrongFocus);
}

GraphWidget::~GraphWidget()
{
}

bool GraphWidget::setXML(const QString &xml)
{
    deleteAll();

    keyActions.clear();

    QString errorMsg;
    int errorLine;
    int errorColumn;

    QDomDocument doc;
    if (!doc.setContent(xml,false, &errorMsg, &errorLine, &errorColumn))
        return false;

    QDomElement myroot = doc.documentElement();
    // Not a QGraphWidget element
    if (myroot.tagName() != "QGraphWidget")
        return false;

    QDomNode child = myroot.firstChild();
    while (!child.isNull())
    {
        if (child.toElement().tagName() == "workSize")
        {
            int x = child.toElement().attribute("x").toUInt();
            int y = child.toElement().attribute("y").toUInt();
            resize(x,y);
        }
        else if (child.toElement().tagName() == "defaultIcon")
        {
            int x = child.toElement().attribute("x").toUInt();
            setDefaultItemIconSize(x);

            QByteArray bArrayElement = QByteArray::fromBase64(QByteArray(child.toElement().text().toUtf8()));
            QPixmap p;
            p.loadFromData(bArrayElement);
            itemsDefaultIcon.addPixmap( p );
        }
        else if (child.toElement().tagName() == "title")
        {
            QByteArray bArrayElement = QByteArray::fromBase64(QByteArray(child.toElement().text().toUtf8()));
            title = QString().fromUtf8(bArrayElement.data(),bArrayElement.size());
        }
        else if (child.toElement().tagName() == "autoArrange")
        {
            autoArrange = child.toElement().text().toUInt();
        }
        else if (child.toElement().tagName() == "resizable")
        {
            setResizable(child.toElement().text().toUInt()==1?true:false);
        }
        else if (child.toElement().tagName() == "deleteOnDeleteKey")
        {
            if (child.toElement().text().toUInt())
                addKeyAction(KEYACT_DELETE_KEY_DELETES);
        }
        else if (child.toElement().tagName() == "deselectOnEscapeKey")
        {
            if (child.toElement().text().toUInt())
                addKeyAction(KEYACT_ESCAPE_KEY_DESELECT);
        }
        else if (child.toElement().tagName() == "defaultTextPosition")
        {
            setDefaultItemTextPosition((ItemWidget::TextPosition)child.toElement().text().toUInt());
        }
        else if (child.toElement().tagName() == "backgroundColor")
        {
            setBackgroundColor(XMLFunctions::colorFromXML(child));
        }
        else if (child.toElement().tagName() == "defaultTextColor")
        {
            setDefaultNodeTextColor(XMLFunctions::colorFromXML(child));
        }
        else if (child.toElement().tagName() == "defaultSubTextColor")
        {
            setDefaultNodeSubTextColor(XMLFunctions::colorFromXML(child));
        }
        else if (child.toElement().tagName() == "defaultSelectedBorderColor")
        {
            setDefaultNodeSelectedBorderColor( XMLFunctions::colorFromXML(child) );
        }
        else if (child.toElement().tagName() == "defaultBorderColor")
        {
            setDefaultNodeBorderColor( XMLFunctions::colorFromXML(child) );
        }
        else if (child.toElement().tagName() == "defaultFillColor")
        {
            setDefaultNodeFillColor( XMLFunctions::colorFromXML(child) );
        }
        else if (child.toElement().tagName() == "defaultFillColor2")
        {
            setDefaultNodeFillColor2( XMLFunctions::colorFromXML(child) );
        }
        else if (child.toElement().tagName() == "defaultBorderRoundRectPixels")
        {
            setDefaultNodeBorderRoundRectPixels(child.toElement().text().toUInt());
        }
        else if (child.toElement().tagName() == "defaultShape")
        {
            setDefaultItemShape((ItemWidget::ItemBoxShape)child.toElement().text().toUInt());
        }
        else if (child.toElement().tagName() == "defaultFillMode")
        {
            setDefaultNodeFillMode((AbstractNodeWidget::ItemBoxFillMode)child.toElement().text().toUInt());
        }
        else if (child.toElement().tagName() == "defaultTextFont")
        {
            QByteArray bArrayElement = QByteArray::fromBase64(QByteArray(child.toElement().text().toUtf8()));
            itemsDefaultTextFont.fromString( QString().fromUtf8(bArrayElement.data(),bArrayElement.size()) );
        }
        else if (child.toElement().tagName() == "defaultSubTextFont")
        {
            QByteArray bArrayElement = QByteArray::fromBase64(QByteArray(child.toElement().text().toUtf8()));
            itemsDefaultSubTextFont.fromString( QString().fromUtf8(bArrayElement.data(),bArrayElement.size()) );
        }
        else if (child.toElement().tagName() == "Groups")
        {
            QDomNode itemChilds = child.toElement().firstChild();
            while (!itemChilds.isNull())
            {
                if (itemChilds.toElement().tagName()=="GroupWidget")
                {
                    // Create new ItemWidget element here.
                    QString parseData;
                    QTextStream textStream(&parseData, QIODevice::WriteOnly);
                    itemChilds.save(textStream,1);
                    new GroupWidget(this,parseData);
                }
                itemChilds = itemChilds.nextSibling();
            }
        }
        else if (child.toElement().tagName() == "Items")
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

        child = child.nextSibling();
    }
    repaint();
    update();
    return true;
}

void GraphWidget::setFilterText(const QString & filterText, bool includeLinkedElements)
{
    this->filterText = filterText;
    this->includeLinkedElementsOnFilter = includeLinkedElements;

    // Filter+Mark every element...
    for (auto item : allRecursiveItems(this))
    {
        item->filter(this->filterText);
    }

    // Discovery linked elements and mark them:
    if (includeLinkedElements)
    {
        for (auto item : allRecursiveItems(this))
        {
            if (item->getCurrentFilterMatch())
            {
                for (void * _link : item->getLinks())
                {
                    Link * link = (Link *)_link;
                    if (link->getItem1()!=item)
                        ((ItemWidget *)link->getItem1())->setCurrentFilterMatch(true);
                    else if (link->getItem2()!=item)
                        ((ItemWidget *)link->getItem2())->setCurrentFilterMatch(true);
                }
            }
        }
    }
    update();
}

void GraphWidget::removeSelectedItemsFromLayerZero()
{
    for ( auto selectedItem : getSelectedItemsRecursively(this))
    {
        selectedItem->setBelongsToLayerZero(false);
    }
    update();
}

void GraphWidget::addSelectedItemsToLayerZero()
{
    for ( auto selectedItem : getSelectedItemsRecursively(this))
    {
        selectedItem->setBelongsToLayerZero(true);
    }
    update();
}

void GraphWidget::addKeyAction(const KeyActions &action)
{
    keyActions.insert(action);
}

void GraphWidget::removeKeyAction(const KeyActions &action)
{
    keyActions.remove(action);
}

bool GraphWidget::getKeyAction(const KeyActions &action)
{
    return keyActions.contains(action);
}

void GraphWidget::removeSelectionMarkOnAllItemsRecursively()
{
    for (auto item : allRecursiveItems(this))
        item->setSelectionMark(false);
}

QString GraphWidget::getXML()
{

    QByteArray iconData;
    QBuffer iconDataBuffer(&iconData);
    iconDataBuffer.open(QIODevice::WriteOnly);
    itemsDefaultIcon.pixmap(getDefaultItemIconSize(), getDefaultItemIconSize()).save(&iconDataBuffer, "PNG");

    QString exportedXML;
    exportedXML = "<?xml version=\"1.0\" encoding='UTF-8'?>";

    exportedXML.append("<QGraphWidget version=\"1.0\">");

    exportedXML.append(XMLFunctions::createSimpleTag("title", (QString) title.toUtf8().toBase64()));
    exportedXML.append(QString("<workSize x=\"%1\" y=\"%2\"></workSize>").arg(size().width()).arg(size().height()));
    exportedXML.append(QString("<defaultIcon x=\"%1\" y=\"%2\">%3</defaultIcon>").arg(
                           QString().setNum(getDefaultItemIconSize()),
                           QString().setNum(getDefaultItemIconSize()),
                           QString(iconData.toBase64()))
                       );

    exportedXML.append(XMLFunctions::createSimpleTag("autoArrange", (uint64_t) autoArrange));
    exportedXML.append(XMLFunctions::createSimpleTag("resizable", resizable));

    exportedXML.append(XMLFunctions::createSimpleTag("deleteOnDeleteKey", getKeyAction(KEYACT_DELETE_KEY_DELETES)));
    exportedXML.append(XMLFunctions::createSimpleTag("deselectOnEscapeKey", getKeyAction(KEYACT_ESCAPE_KEY_DESELECT)));

    exportedXML.append(XMLFunctions::createSimpleTag("defaultTextPosition",(uint64_t)itemsDefaultTextPosition));

    exportedXML.append(XMLFunctions::createColorXMLTag("backgroundColor",backgroundColor));
    exportedXML.append(XMLFunctions::createColorXMLTag("defaultTextColor",defaultItemTextColor));
    exportedXML.append(XMLFunctions::createColorXMLTag("defaultSubTextColor",defaultItemSubTextColor));
    exportedXML.append(XMLFunctions::createColorXMLTag("defaultSelectedBorderColor",defaultItemSelectedBorderColor));
    exportedXML.append(XMLFunctions::createColorXMLTag("defaultBorderColor",defaultItemBorderColor));
    exportedXML.append(XMLFunctions::createColorXMLTag("defaultFillColor",defaultItemFillColor));
    exportedXML.append(XMLFunctions::createColorXMLTag("defaultFillColor2",defaultItemFillColor2));


    exportedXML.append(XMLFunctions::createSimpleTag("defaultBorderRoundRectPixels",(uint64_t)itemsDefaultBorderRoundRectPixels));
    exportedXML.append(XMLFunctions::createSimpleTag("defaultShape",(uint64_t)itemsDefaultShape));
    exportedXML.append(XMLFunctions::createSimpleTag("defaultFillMode",(uint64_t)itemsDefaultFillMode));

    exportedXML.append(XMLFunctions::createSimpleTag("defaultTextFont",(QString)itemsDefaultTextFont.toString().toUtf8().toBase64()));
    exportedXML.append(XMLFunctions::createSimpleTag("defaultSubTextFont",(QString)itemsDefaultSubTextFont.toString().toUtf8().toBase64()));

    exportedXML.append("<Groups>");
    for (auto group : GraphWidget::allChildrenGroups(this))
    {
        exportedXML.append(group->getXML("GroupWidget",false));
    }
    exportedXML.append("</Groups>");

    exportedXML.append("<Items>");

    for (auto item : GraphWidget::allChildrenItems(this))
    {
        exportedXML.append(item->getXML("ItemWidget",false));
    }

    exportedXML.append("</Items>");

    exportedXML.append("</QGraphWidget>");
    return exportedXML;
}

QList<ItemWidget *> GraphWidget::getSelectedItemsRecursively(QWidget *v)
{
    QList<ItemWidget *> selectedItems;

    for (auto i : allRecursiveItems(v))
    {
        if (i->getIsSelected())
            selectedItems.append(i);
    }

    return selectedItems;
}

void GraphWidget::keyReleaseEvent ( QKeyEvent *  )
{
    setKeyCtrlActivated(false);
}

void GraphWidget::keyPressEvent ( QKeyEvent * event )
{
    switch (event->key())
    {
    case Qt::Key_Control:
    {
        keyCtrlActivated = true;
    } break;
    case Qt::Key_Escape:
    {
        if (getKeyAction(KEYACT_ESCAPE_KEY_DESELECT))
            deselectAll();
    } break;
    case Qt::Key_Delete:
    {
        if (getKeyAction(KEYACT_DELETE_KEY_DELETES))
        {
            for ( auto selectedItem : getSelectedItemsRecursively(this))
            {
                delete selectedItem;
            }
            repaint();
        }
    } break;
    default:
    {
    }
    }
}

void GraphWidget::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);

    /*qreal inverseDPR = 1.0 / ((QWidget *)parent())->devicePixelRatio();
    painter.scale(inverseDPR, inverseDPR);*/

    QPixmap drawingArea (size().width(),size().height());
    drawingArea.fill(backgroundColor);

    painter.drawPixmap(0,0, drawingArea);

    // Draw links for all items...
    for (ItemWidget * item : allRecursiveItems(this))
    {
        QList< void * > linkingList = item->getLinks();
        for (int j=0;j<linkingList.count();j++)
        {
            Link * currentLink = (Link *) linkingList[j];
            if (currentLink->getItem1()==item)
            {
                currentLink->paint(painter,backgroundColor);
            }
        }
    }

    // Draw manual-linking
    if (manualLinkedItem[0])
    {
        QColor colorRectx = Qt::blue;
        painter.setPen(colorRectx);

        ItemWidget * start_item = (ItemWidget *)manualLinkedItem[0];

        painter.drawLine(
                    start_item->pos() + start_item->getIconCenterPoint(),
                    QPoint(mouseCurrentPos.x(), mouseCurrentPos.y())
                    );
    }


    // Draw selection..
    if (isUnderSelection())
    {
        int x = mouseRect.x();
        int y = mouseRect.y();
        int w = -(mouseRect.x()-mouseRect.width());
        int h = -(mouseRect.y()-mouseRect.height());

        QRectF rectangle( (w<0? x-abs(w) : x),(h<0? y-abs(h) : y),abs(w),abs(h));

        QColor penColor = defaultItemSelectedBorderColor, brushColor = defaultItemSelectedBorderColor;

        penColor.setAlphaF(0.7);
        painter.setPen(penColor);

        brushColor.setAlphaF(0.2);
        painter.setBrush(brushColor);

        painter.drawRect(rectangle);
        painter.setBrush(Qt::transparent);
    }

    painter.setPen(defaultItemTextColor);


    for ( auto i : allRecursiveItems(this))
    {
        if (i->toPaint.x())
        {
            painter.setBrush(Qt::red);
            painter.drawRect( i->toPaint );
        }
    }




    QFont xfont;
    QFontMetrics mymetrics(xfont);

    // Draw title...
    painter.drawText(0,mymetrics.height(),title);

    QWidget::paintEvent(e);
}

void GraphWidget::focusOutEvent ( QFocusEvent * )
{
    repaint();
    update();
}


void GraphWidget::resizeOnMouseMove(QWidget * v, QMouseEvent *e,
                                    bool resizingX, bool resizingY,
                                    const QPoint & mouseCurrentPos)
{
    // 5px X 5px in the end, resize area.
    if (  (mouseCurrentPos.y()<(v->size().height()) && mouseCurrentPos.y()>=(v->size().height()-5))&&
          (mouseCurrentPos.x()<=(v->size().width()) && mouseCurrentPos.x()>=(v->size().width()-5))  )
    {
        v->setCursor(Qt::SizeFDiagCursor);
    }
    // width resize area
    else if (mouseCurrentPos.x()<=(v->size().width()) && mouseCurrentPos.x()>=(v->size().width()-5))
    {
        v->setCursor(Qt::SizeHorCursor);
    }
    // height resize area
    else if (mouseCurrentPos.y()<=(v->size().height()) && mouseCurrentPos.y()>=(v->size().height()-5))
    {
        v->setCursor(Qt::SizeVerCursor);
    }
    else
    {
        v->setCursor(Qt::ArrowCursor);
    }

    if (resizingX || resizingY)
    {
        if (resizingX && resizingY)
        {
            v->resize(e->pos().x(), e->pos().y());
        }
        else if (resizingX)
        {
            v->resize(e->pos().x(), v->size().height());
        }
        else if (resizingY)
        {
            v->resize(v->size().width(), e->pos().y());
        }
    }
}

bool GraphWidget::resizeStartOnClick(QWidget *v, const QPoint & mouseCurrentPos, bool resizable, bool *resizingX, bool *resizingY)
{
    if (    resizable &&
            ((mouseCurrentPos.x()<=(v->size().width()) && mouseCurrentPos.x()>=(v->size().width()-5)) ||
             (mouseCurrentPos.y()<=(v->size().height()) && mouseCurrentPos.y()>=(v->size().height()-5)) )
            )
    {
        // Begin to resize.
        if (mouseCurrentPos.x()<=(v->size().width()) && mouseCurrentPos.x()>=(v->size().width()-5))
            *resizingX = true;
        if (mouseCurrentPos.y()<=(v->size().height()) && mouseCurrentPos.y()>=(v->size().height()-5))
            *resizingY = true;
        return true;
    }
    return false;
}

void GraphWidget::mouseMoveEvent(QMouseEvent *e)
{
    mouseCurrentPos = e->pos();

    if ( mouseRect.x()!=-1 )
    {
        // Selection starts here...
        mouseRect.setWidth(mouseCurrentPos.x());
        mouseRect.setHeight(mouseCurrentPos.y());
    }

    if (manualLinkedItem[0])
    {
        ItemWidget * linkedObj = itemAt(e->pos());
        manualLinkedItem[1] = linkedObj;
        if (manualLinkedItem[1])
            manualLinkedItem[1]->activateWindow();
    }

    if (resizable)
    {
        resizeOnMouseMove(this,e,resizingX,resizingY,mouseCurrentPos);
        if (resizingX || resizingY)
        {
            checkAndFixChildrenPositions(this);
            // Autosort/arrange items in workspace
            Arrange::triggerAutoArrange(this);
        }
    }

    repaint (  );
}
void GraphWidget::mousePressEvent(QMouseEvent *e)
{
    if (duringManualLinking)
    {
        // ITS UNDER MANUAL LINKING...

        // Setup first linked item...

        if (e->button()==Qt::LeftButton && itemAt ( e->pos() ))
        {
            manualLinkedItem[0] = itemAt ( e->pos() );
            manualLinkedItem[0]->activateWindow();
        }
        else if (e->button()==Qt::LeftButton)
        {
            emit itemLinked(nullptr,nullptr);
            manualLinkedItem[0]=0;
            manualLinkedItem[1]=0;
            duringManualLinking=false;
            setCursor(Qt::ArrowCursor);
        }
    }
    else
    {
        // Normal behaviour:

        if (e->button()==Qt::LeftButton)
        {
            if (!itemAt( e->pos() ))
            {
                deselectAll();
                // Not selecting any item... starting to resize or create a rectangle...
                if ( resizeStartOnClick(this,mouseCurrentPos,resizable,&resizingX,&resizingY ))
                {
                }
                // rectangle selection.
                else if (mouseRect.x()==-1 && mouseRect.width()==-1)
                {
                    // selection begins...
                    mouseRect.setRect( e->pos().x(),
                                       e->pos().y(),
                                       e->pos().x(),
                                       e->pos().y()
                                       );
                }
            }
        }
        else if (e->button()==Qt::RightButton)
        {
            if (keyCtrlActivated)
            {
                auto selectedItems = getSelectedItemsRecursively(this);
                if (!selectedItems.empty())
                {
                    emit itemsRightClickEvent(selectedItems);
                }
            }
        }
    }

}
void GraphWidget::mouseDoubleClickEvent(QMouseEvent *)
{
    if (keyCtrlActivated)
    {
        auto selectedItems = getSelectedItemsRecursively(this);
        if (!selectedItems.empty())
        {
            // Multi-double click...

            emit itemsDoubleClick(selectedItems);
        }
    }
}

void GraphWidget::mouseReleaseEvent(QMouseEvent * event)
{
    // End resizing.
    resizingX = false;
    resizingY = false;

    // SELECT...
    if (isUnderSelection())
    {
        orderMouseRectCoordinates();

        for (auto i : allRecursiveItems(this))
        {
            QRect itemRect;
            auto absPos = i->getAbsolutePos();
            itemRect.setRect( absPos.x(), absPos.y(), i->width(), i->height() );
            if (mouseRect.intersects( itemRect ))
            {
                i->activateWindow();
                i->raise();
                i->setSelected(true);
            }
        }
    }

    if (manualLinkedItem[0])
    {
        manualLinkedItem[1] = itemAt(event->pos());

        if (manualLinkedItem[1])
            manualLinkedItem[1]->activateWindow();

        emit itemLinked(manualLinkedItem[0],manualLinkedItem[1]);
        manualLinkedItem[0]=0;
        manualLinkedItem[1]=0;
        duringManualLinking=false;
        setCursor(Qt::ArrowCursor);
    }

    mouseRect.setRect(-1,-1,-1,-1);
    update();
    repaint();
}

void GraphWidget::setBackgroundColor(const QColor &backgroundColor)
{
    this->backgroundColor = backgroundColor;
}

void GraphWidget::replaceMinimumSize(QWidget * v)
{
    QSize curMinSize = {50,50};

    for (auto obj : GraphWidget::allChildrenItemsAndGroups(v))
    {
        if (obj->size().width() > curMinSize.width())
            curMinSize.setWidth(obj->size().width());
        if (obj->size().height() > curMinSize.height())
            curMinSize.setHeight(obj->size().height());
    }

    if (v->objectName().startsWith("GROUP-"))
        // If is a group, calculate the size as 10% extra + vertical offset
        curMinSize.setHeight(curMinSize.height()*1.1 + ((GroupWidget *)v)->getVerticalOffset());
    else
        // If not, only the 10% extra
        curMinSize.setHeight(curMinSize.height()*1.1);

    curMinSize.setWidth(curMinSize.width()*1.1);

    v->setMinimumSize( curMinSize );
}

void GraphWidget::selectAll()
{
    for (auto obj : GraphWidget::allChildrenItemsAndGroups(this))
    {
        obj->setSelected(true);
    }
}

void GraphWidget::selectAllRecursiveItems()
{
    for (auto i: allRecursiveItems(this))
    {
        i->setSelected(true);
    }
}
bool GraphWidget::getResizable()
{
    return resizable;
}

void GraphWidget::deleteAll()
{
    for (auto obj : GraphWidget::allChildrenItemsAndGroups(this))
    {
        delete obj;
    }
}

void GraphWidget::deleteAllRecursiveItems()
{
    for (ItemWidget * item : allRecursiveItems(this))
    {
        delete item;
    }
}

QColor GraphWidget::getBackgroundColor() const
{
    return backgroundColor;
}

void GraphWidget::deselectAll()
{
    for (auto i: allRecursiveItems(this))
        i->setSelected(false);

    for (auto i : allChildrenGroups(this))
    {
        i->setSelected(false);
    }
    repaint();
}

void GraphWidget::invertSelectionOnAllRecursiveItems()
{
    for (ItemWidget * item : allRecursiveItems(this))
    {
        item->setSelected(!item->getIsSelected());
    }
}

ItemWidget * GraphWidget::getItemById(const QString & id)
{
    for (ItemWidget * item : allRecursiveItems(this))
    {
        if (item->compareID(id))
            return item;
    }
    return nullptr;
}

ItemWidget * GraphWidget::itemAt(const QPoint &p, bool includeNestedItems)
{
    QWidget * child = childAt(p);

    // check for nested childrens...
    if (child && includeNestedItems && child->objectName().startsWith("GROUP-"))
        child = child->childAt(p-(child->pos()));

    if (child && child->objectName().startsWith("ITEM-"))
        return (ItemWidget *)child;

    return nullptr;
}

QList<ItemWidget *> GraphWidget::allChildrenItems(QWidget *v)
{
    QList<ItemWidget *> r;

    QObjectList ch = v->children();
    for ( int i = 0; i < ch.size(); i++ )
    {
        QObject *obj = ch[i];
        if (obj)
        {
            if      (obj->objectName().startsWith("ITEM-"))
                r+=(ItemWidget *)obj;
        }
    }

    return r;
}

QList<GroupWidget *> GraphWidget::allChildrenGroups(QWidget *v)
{
    QList<GroupWidget *> r;

    QObjectList ch = v->children();
    for ( int i = 0; i < ch.size(); i++ )
    {
        QObject *obj = ch[i];
        if (obj)
        {
            if (obj->objectName().startsWith("GROUP-"))
                r+=(GroupWidget *)obj;
        }
    }

    return r;
}

QList<AbstractNodeWidget *> GraphWidget::allChildrenItemsAndGroups(QWidget *v)
{
    QList<AbstractNodeWidget *> r;

    QObjectList ch = v->children();
    for ( int i = 0; i < ch.size(); i++ )
    {
        QObject *obj = ch[i];
        if (obj)
        {
            if      (obj->objectName().startsWith("ITEM-"))
                r+=(ItemWidget *)obj;
            else if (obj->objectName().startsWith("GROUP-"))
                r+=(GroupWidget *)obj;
        }
    }

    return r;
}

QList<AbstractNodeWidget *> GraphWidget::allRecursiveItemsAndGroups(QWidget *v)
{
    QList<AbstractNodeWidget *> r;

    QObjectList ch = v->children();
    for ( int i = 0; i < ch.size(); i++ )
    {
        QObject *obj = ch[i];
        if (obj)
        {
            if (obj->objectName().startsWith("ITEM-"))
                r+=(AbstractNodeWidget *)obj;
            else if (obj->objectName().startsWith("GROUP-"))
            {
                auto x = ((GroupWidget *)obj)->allRecursiveItems();
                for (auto i : x)
                {
                    r+=(AbstractNodeWidget *)i;
                }
                r+=(AbstractNodeWidget *)obj;
            }
        }
    }
    return r;
}

QList<ItemWidget *> GraphWidget::allRecursiveItems(QWidget *v)
{
    QList<ItemWidget *> r;

    QObjectList ch = v->children();
    for ( int i = 0; i < ch.size(); i++ )
    {
        QObject *obj = ch[i];
        if (obj)
        {
            if (obj->objectName().startsWith("ITEM-"))
                r+=(ItemWidget *)obj;
            else if (obj->objectName().startsWith("GROUP-"))
                r+=((GroupWidget *)obj)->allRecursiveItems();
        }
    }

    return r;
}

bool GraphWidget::isUnderSelection()
{
    if (mouseRect.x()==-1)
        return false;
    if (        abs(mouseRect.x()-mouseRect.width())<4
                && abs(mouseRect.y()-mouseRect.height())<4)
        return false;
    return true;
}

void GraphWidget::setDefaultNodeTextColor(const QColor &itemsDefaultTextColor, bool override)
{
    this->defaultItemTextColor = itemsDefaultTextColor;
    if (override)
    {
        for (auto i: allRecursiveItemsAndGroups(this))
        {
            i->setTextColor(this->defaultItemTextColor);
        }
    }
}

void GraphWidget::setDefaultNodeFillMode(AbstractNodeWidget::ItemBoxFillMode newItemsDefaultFillMode, bool override)
{
    itemsDefaultFillMode = newItemsDefaultFillMode;
    if (override)
    {
        for (auto i: allRecursiveItemsAndGroups(this))
        {
            i->setFillMode(this->itemsDefaultFillMode);
        }
    }
}
void GraphWidget::setDefaultNodeFillColor(const QColor &newItemsDefaultFillColor, bool override)
{
    this->defaultItemFillColor = newItemsDefaultFillColor;

    if (override)
    {
        for (auto i: allRecursiveItemsAndGroups(this))
        {
            i->setFillColor(this->defaultItemFillColor);
        }
    }
}
void GraphWidget::setDefaultNodeBorderRoundRectPixels(int newItemsDefaultBorderRoundRectPixels, bool override)
{
    itemsDefaultBorderRoundRectPixels = newItemsDefaultBorderRoundRectPixels;

    if (override)
    {
        for ( auto obj : GraphWidget::allRecursiveItemsAndGroups(this))
        {
            obj->setBorderRoundRectPixels(this->itemsDefaultBorderRoundRectPixels);
        }
    }
}

void GraphWidget::setDefaultNodeSubTextColor(const QColor & itemsDefaultSubTextColor, bool override)
{
    this->defaultItemSubTextColor = itemsDefaultSubTextColor;

    if (override)
    {
        for ( auto obj : GraphWidget::allRecursiveItemsAndGroups(this))
        {
            obj->setSubTextColor(this->defaultItemSubTextColor);
        }
    }
}

void GraphWidget::setDefaultNodeSelectedBorderColor(const QColor &itemsDefaultSelectedColor, bool override)
{
    this->defaultItemSelectedBorderColor = itemsDefaultSelectedColor;

    if (override)
    {
        for ( auto obj : GraphWidget::allRecursiveItemsAndGroups(this))
        {
            obj->setSelectedBorderColor(this->defaultItemSelectedBorderColor);
        }
    }

}

void GraphWidget::setDefaultNodeBorderColor(const QColor & itemsDefaultBorderColor, bool override)
{
    this->defaultItemBorderColor = itemsDefaultBorderColor;

    if (override)
    {
        for ( auto obj : GraphWidget::allRecursiveItemsAndGroups(this))
        {
            obj->setBorderColor(this->defaultItemBorderColor);
        }
    }

}

void GraphWidget::setDefaultItemIcon(const QIcon &itemsDefaultIcon)
{
    this->itemsDefaultIcon = itemsDefaultIcon;
}

void GraphWidget::setDefaultItemTextPosition(ItemWidget::TextPosition itemsDefaultTextPosition, bool override)
{
    this->itemsDefaultTextPosition = itemsDefaultTextPosition;

    if (override)
    {
        for ( auto obj : GraphWidget::allRecursiveItems(this))
        {
            obj->setTextPosition(this->itemsDefaultTextPosition);
        }
    }
}

void GraphWidget::setDefaultNodeTextFont(const QFont &itemsDefaultTextFont, bool override)
{
    this->itemsDefaultTextFont = itemsDefaultTextFont;

    if (override)
    {
        for ( auto obj : GraphWidget::allRecursiveItemsAndGroups(this))
        {
            obj->setTextFont(this->itemsDefaultTextFont);
        }
    }
}

void GraphWidget::setDefaultNodeSubTextFont(const QFont & itemsDefaultSubTextFont, bool override)
{
    this->itemsDefaultSubTextFont = itemsDefaultSubTextFont;

    if (override)
    {
        for ( auto obj : GraphWidget::allRecursiveItemsAndGroups(this))
        {
            obj->setSubTextFont(this->itemsDefaultSubTextFont);
        }
    }
}

QColor GraphWidget::getDefaultNodeTextColor() const
{
    return defaultItemTextColor;
}

QColor GraphWidget::getDefaultNodeSubTextColor() const
{
    return defaultItemSubTextColor;
}

QColor GraphWidget::getDefaultNodeSelectedBorderColor() const
{
    return defaultItemSelectedBorderColor;
}

QColor GraphWidget::getDefaultNodeBorderColor() const
{
    return defaultItemBorderColor;
}

QIcon GraphWidget::getDefaultItemIcon() const
{
    return itemsDefaultIcon;
}

ItemWidget::TextPosition GraphWidget::getDefaultItemTextPosition() const
{
    return itemsDefaultTextPosition;
}

QFont GraphWidget::getDefaultNodeTextFont() const
{
    return itemsDefaultTextFont;
}

QFont GraphWidget::getDefaultNodeSubTextFont() const
{
    return itemsDefaultSubTextFont;
}

int GraphWidget::getDefaultItemIconSize() const
{
    return itemsDefaultIconSize;
}

void GraphWidget::setDefaultItemIconSize(const int &itemsDefaultIconSize)
{
    this->itemsDefaultIconSize = itemsDefaultIconSize;
}

void GraphWidget::orderMouseRectCoordinates()
{
    if (mouseRect.x()>mouseRect.width())
    {
        auto flip = mouseRect.width();
        mouseRect.setWidth(mouseRect.x());
        mouseRect.setX(flip);
    }
    if (mouseRect.y()>mouseRect.height())
    {
        auto flip = mouseRect.height();
        mouseRect.setHeight(mouseRect.y());
        mouseRect.setY(flip);
    }
}

const QColor &GraphWidget::getDefaultNodeFillColor2() const
{
    return defaultItemFillColor2;
}

void GraphWidget::setDefaultNodeFillColor2(const QColor &newDefaultItemFillColor2)
{
    defaultItemFillColor2 = newDefaultItemFillColor2;
}

bool GraphWidget::getAllowOverlap() const
{
    return allowOverlap;
}

void GraphWidget::setAllowOverlap(bool newAllowOverlap)
{
    allowOverlap = newAllowOverlap;
}

void GraphWidget::setManualNodesLinking(const bool & manualLink)
{
    manualLinkedItem[0]=nullptr;
    manualLinkedItem[1]=nullptr;

    duringManualLinking = manualLink;
    if (duringManualLinking)
    {
        deselectAll();
        setCursor(Qt::CrossCursor);
    }
    else
        setCursor(Qt::ArrowCursor);
}

void GraphWidget::setResizable(bool resizable)
{
    this->resizable = resizable;
}

void GraphWidget::checkAndFixChildrenPositions(QWidget *v,int vOffset)
{
    // TODO: avoid overlaping here on resize...
    // TODO: cuando se mueve sobre el limite, overlapea sin problema.

    for ( auto node : GraphWidget::allChildrenItemsAndGroups(v))
    {
        if (node->pos().y()+node->size().height() >
                v->size().height())
        {
            node->move( node->pos().x(), v->size().height()-node->size().height()  );
        }

        if (node->pos().x()+node->size().width() >
                v->size().width())
        {
            node->move( v->size().width()-node->size().width(), node->pos().y() );
        }
    }
}

void GraphWidget::setKeyCtrlActivated(bool newKeyCtrlActivated)
{
    keyCtrlActivated = newKeyCtrlActivated;
}

bool GraphWidget::getKeyCtrlActivated() const
{
    return keyCtrlActivated;
}

bool GraphWidget::getIsDuringManualLinking() const
{
    return duringManualLinking;
}

AbstractNodeWidget::ItemBoxFillMode GraphWidget::getDefaultNodeFillMode() const
{
    return itemsDefaultFillMode;
}


const QColor &GraphWidget::getDefaultNodeFillColor() const
{
    return defaultItemFillColor;
}


int GraphWidget::getDefaultNodeBorderRoundRectPixels() const
{
    return itemsDefaultBorderRoundRectPixels;
}

ItemWidget::ItemBoxShape GraphWidget::getDefaultItemShape() const
{
    return itemsDefaultShape;
}

void GraphWidget::setDefaultItemShape(ItemWidget::ItemBoxShape newItemsDefaultDisplayBoxMode, bool override)
{
    itemsDefaultShape = newItemsDefaultDisplayBoxMode;
    if (override)
    {
        for ( auto obj : GraphWidget::allRecursiveItems(this))
        {
            obj->setShape(this->itemsDefaultShape);
        }
    }
}

int GraphWidget::getAutoArrangeAlgorithm() const
{
    return autoArrangeAlgorithm;
}

void GraphWidget::setAutoArrangeAlgorithm(int newAutoArrangeAlgorithm)
{
    autoArrangeAlgorithm = newAutoArrangeAlgorithm;
}

int GraphWidget::getSortBy() const
{
    return sortBy;
}

void GraphWidget::setSortBy(int newSortBy)
{
    sortBy = newSortBy;
}

int GraphWidget::getAutoArrangeSpacing() const
{
    return autoArrangeSpacing;
}

void GraphWidget::setAutoArrangeSpacing(int newAutoArrangeSpacing)
{
    autoArrangeSpacing = newAutoArrangeSpacing;
}

bool GraphWidget::getAutoArrange() const
{
    return autoArrange;
}

void GraphWidget::setAutoArrange(bool newAutoArrange)
{
    autoArrange = newAutoArrange;
}


void GraphWidget::setTitle(const QString & title)
{
    this->title = title;
    repaint();
}

QString GraphWidget::getTitle()
{
    return title;
}
