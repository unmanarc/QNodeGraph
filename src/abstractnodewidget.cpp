#include "abstractnodewidget.h"
#include <QPainter>
#include <QBuffer>
#include <QFontMetrics>
#include <QMouseEvent>
#include <algorithm>

#include <cmath>
#include <random>

#include "itemwidget.h"
#include "xmlfunctions.h"
#include "qnamespace.h"
#include "graphwidget.h"

#define RANDOM_ITERS 5

using namespace QNodeGraph;

void AbstractNodeWidget::init(const QString &id, QWidget *parent)
{
    if (parent->objectName().startsWith("GROUP-"))
    {
        // TODO: multiple nested groups...
        nodeGraphParent = parent->parentWidget();
        groupParent = parent;
    }
    else
    {
        nodeGraphParent = parent;
        groupParent = nullptr;
    }

    setObjectName("UNDEF-" + id);

    QPixmap x(32,32);
    x.fill(Qt::black);
    setAnchor(false);

    this->id=id;
    this->text=id;
    this->subText="";
    this->description="";

    this->textFont=(GRAPH->getDefaultNodeTextFont());
    this->subTextFont=(GRAPH->getDefaultNodeSubTextFont());

    setTextColor(GRAPH->getDefaultNodeTextColor());
    setSubTextColor(GRAPH->getDefaultNodeSubTextColor());

    setFillColor2(GRAPH->getDefaultNodeFillColor2());
    setFillColor(GRAPH->getDefaultNodeFillColor());
    setFillMode(GRAPH->getDefaultNodeFillMode());
    setSelectedBorderColor(GRAPH->getDefaultNodeSelectedBorderColor());
    setBorderColor( GRAPH->getDefaultNodeBorderColor() );
    this->borderRoundRectPixels=(GRAPH->getDefaultNodeBorderRoundRectPixels());

    setSelected(false);
    setMouseTracking(true);
    mouseover = false;
    pressed = false;

    verticalOffset = 0;
}

AbstractNodeWidget::AbstractNodeWidget(QWidget *parent, const QString & xml, const QString & type) : QWidget(parent)
{
    init("",parent);
    setXML(type,xml);
}

AbstractNodeWidget::AbstractNodeWidget(const QString &id, QWidget * parent, const QString & text, const QString &subtext) : QWidget(parent)
{
    init(id,parent);
    this->text=(text);
    this->subText=(subtext);
}

AbstractNodeWidget::AbstractNodeWidget(const QString &id, QWidget *parent)
    : QWidget(parent)
{
    init(id,parent);
}

AbstractNodeWidget::~AbstractNodeWidget()
{
}

bool AbstractNodeWidget::setXML(const QString & widgetName, const QString & xml)
{
    QString errorMsg;
    int errorLine;
    int errorColumn;

    QDomDocument doc;
    if (!doc.setContent(xml,false, &errorMsg, &errorLine, &errorColumn))
        return false;

    QDomElement myroot = doc.documentElement();
    // Not a ItemWidget/GroupWidget element
    if (myroot.tagName() != widgetName)
        return false;

    QDomNode child = myroot.firstChild();
    while (!child.isNull())
    {
        if (child.toElement().tagName() == "properties")
        {
            SetNodeXMLLocalProperties(child);
        }
        else if (child.toElement().tagName() == "data")
        {
            // from base64
            QByteArray bArrayElement = QByteArray::fromBase64(QByteArray(child.toElement().text().toUtf8()));
            embeddedData = QString().fromUtf8(bArrayElement.data(),bArrayElement.size());
        }
        else
        {
            setXMLLocal(child);
        }

        child = child.nextSibling();
    }

    show();
    return true;
}

/* Export into XML */
QString AbstractNodeWidget::getXML(const QString & widgetName, bool xmltag)
{
    QString exportedXML;
    if (xmltag) exportedXML = "<?xml version=\"1.0\" encoding='UTF-8'?>";

    exportedXML.append(QString::asprintf("<%s>",widgetName.toStdString().c_str()));
    exportedXML.append("<properties>");

    if (1)
    {
        exportedXML.append( XMLFunctions::createSimpleTag("id",(QString)id.toUtf8().toBase64()) );

        exportedXML.append(QString("<pos x=\"%1\" y=\"%2\"></pos>\n").arg(
                               QString().setNum(pos().x()),
                               QString().setNum(pos().y()))
                           );

        exportedXML.append( XMLFunctions::createSimpleTag("text",(QString)text.toUtf8().toBase64()) );
        exportedXML.append( XMLFunctions::createSimpleTag("subText",(QString)subText.toUtf8().toBase64()) );
        exportedXML.append( XMLFunctions::createSimpleTag("description",(QString)description.toUtf8().toBase64()) );

        exportedXML.append( XMLFunctions::createSimpleTag("textFont",(QString)textFont.toString().toUtf8().toBase64()) );
        exportedXML.append( XMLFunctions::createSimpleTag("subTextFont",(QString)subTextFont.toString().toUtf8().toBase64()) );

        exportedXML.append(XMLFunctions::createColorXMLTag("borderColor",borderColor));
        exportedXML.append(XMLFunctions::createColorXMLTag("selectedBorderColor",selectedBorderColor));
        exportedXML.append(XMLFunctions::createColorXMLTag("textColor",textColor));
        exportedXML.append(XMLFunctions::createColorXMLTag("subTextColor",subTextColor));
        exportedXML.append(XMLFunctions::createColorXMLTag("fillColor",fillColor));
        exportedXML.append(XMLFunctions::createColorXMLTag("fillColor2",fillColor2));

        exportedXML.append( XMLFunctions::createSimpleTag("fillMode", (uint64_t)fillMode));
        exportedXML.append( XMLFunctions::createSimpleTag("borderRoundRectPixels", (uint64_t)borderRoundRectPixels));

        exportedXML.append( XMLFunctions::createSimpleTag("anchored", anchored));

        exportedXML+=getXMLLocalProperties();
    }

    exportedXML.append("</properties>");

    exportedXML+=getXMLLocal();

    exportedXML.append("<data>");
    exportedXML.append(QString(embeddedData.toUtf8().toBase64()));
    exportedXML.append("</data>");
    exportedXML.append(QString::asprintf("</%s>",widgetName.toStdString().c_str()));

    return exportedXML;
}

void AbstractNodeWidget::setSelectionMark(bool x)
{
    selectionMark = x;
}

bool AbstractNodeWidget::getSelectionMark() const
{
    return selectionMark;
}


void AbstractNodeWidget::setDescription(const QString & description)
{
    this->description = description;
}

void AbstractNodeWidget::setId(const QString & nodeId)
{
    this->id = nodeId;
    setInternalObjectID();
}

QString AbstractNodeWidget::getID() const
{
    return id;
}

QString AbstractNodeWidget::getText() const
{
    return text;
}

QString AbstractNodeWidget::getSubText() const
{
    return subText;
}

bool AbstractNodeWidget::compareID(const QString &groupId) const
{
    return (this->id == groupId);
}

QPoint AbstractNodeWidget::getAbsolutePos() const
{
    if (parent()->objectName().startsWith("GROUP-"))
    {
        return pos()+((QWidget *)parent())->pos();
    }
    return pos();
}

QRect AbstractNodeWidget::getAbsoluteRect() const
{
    QRect r = rect();
    auto apos = getAbsolutePos();
    r.setRect(apos.x(),apos.y(), r.width(), r.height() );
    return r;
}

void AbstractNodeWidget::setSubText(const QString &subText)
{
    this->subText = subText;
    recalculateSize();
}

void AbstractNodeWidget::setText(const QString & text)
{
    this->text = text;
    recalculateSize();
}

void AbstractNodeWidget::setTextFont(const QFont &font)
{
    textFont = font;
    recalculateSize();
}

void AbstractNodeWidget::setAnchor(bool x)
{
    anchored = x;
}

bool AbstractNodeWidget::getAnchor() const
{
    return anchored;
}

void AbstractNodeWidget::setSubTextFont(const QFont & font)
{
    subTextFont = font;
    recalculateSize();
}

void AbstractNodeWidget::setTextColor(const QColor &textColor)
{
    this->textColor = textColor;
}

void AbstractNodeWidget::setSubTextColor(const QColor &subTextColor)
{
    this->subTextColor = subTextColor;
}

void AbstractNodeWidget::setMouseOver(bool x)
{
    mouseover = x;
}

void AbstractNodeWidget::setBorderColor(const QColor & borderColor)
{
    this->borderColor = borderColor;
}

void AbstractNodeWidget::setSelected(bool x)
{
    selected = x;
    // [De]Select visual changes...:
    //    repaint(); ?
}


int AbstractNodeWidget::getBorderRoundRectPixels() const
{
    return borderRoundRectPixels;
}

void AbstractNodeWidget::setBorderRoundRectPixels(int newRoundRectPixels)
{
    borderRoundRectPixels = newRoundRectPixels;
}


const QColor &AbstractNodeWidget::getFillColor() const
{
    return fillColor;
}

void AbstractNodeWidget::setFillColor(const QColor &newFillColor)
{
    fillColor = newFillColor;
}

AbstractNodeWidget::ItemBoxFillMode AbstractNodeWidget::getFillMode() const
{
    return fillMode;
}

void AbstractNodeWidget::setFillMode(AbstractNodeWidget::ItemBoxFillMode newFillMode)
{
    fillMode = newFillMode;
}

QPoint AbstractNodeWidget::getCenterPoint() const
{
    return QPoint(size().width()/2,size().height()/2);
}

double AbstractNodeWidget::getExternalRadius() const
{
    if (objectName().startsWith("ITEM-"))
    {
        if (    ((ItemWidget *)this)->getShape() == ItemWidget::ITEMBOX_SHAPE_CIRCLE
                || ((ItemWidget *)this)->getShape() == ItemWidget::ITEMBOX_SHAPE_NONE

                )
        {
            return (size().height() / 2.0)+2;
        }
    }
    return sqrt( pow(size().height()/2.0,2) + pow(size().width()/2.0,2) );
}

void AbstractNodeWidget::setSelectedBorderColor(const QColor &selectedBorderColor)
{
    this->selectedBorderColor = selectedBorderColor;
}

bool AbstractNodeWidget::getIsSelected() const
{
    return selected;
}

/* Embedded XML: Useful to extend the capabilities of the item */
QString AbstractNodeWidget::getEmbeddedData() const
{
    return embeddedData;
}

void AbstractNodeWidget::setEmbeddedData(const QString &embeddedData)
{
    this->embeddedData=embeddedData;
}

void AbstractNodeWidget::moveToRandom()
{
    auto parentSize = ((QWidget *)parent())->size();
    auto lsize = size();
    // TODO: avoid overlap groups and intems...

    int pVerticalOffset = parent()->objectName().startsWith("GROUP-")? ((AbstractNodeWidget *)parent())->getVerticalOffset() : 0;

    std::mt19937 rg{std::random_device{}()};
    std::uniform_int_distribution<int> pickX(1, parentSize.width()-lsize.width());
    std::uniform_int_distribution<int> pickY(1, parentSize.height()-lsize.height()-pVerticalOffset);

    for (int i=0;i<RANDOM_ITERS;i++)
    {
        QPoint nextRelativePos = QPoint(pickX(rg),pickY(rg));

        // TODO: Not enough space? extend the graphic...

        if (moveRelative(nextRelativePos, i==RANDOM_ITERS-1))
            break;

        // If the nodes overflow the graphic, put in the place...
        //  GraphWidget::checkAndFixChildrenPositions((QWidget *)parent(),pVerticalOffset);
    }

    toPaint = QRect(0,0,0,0);
}

bool AbstractNodeWidget::SetNodeXMLLocalProperties(const QDomNode & master)
{
    QDomNode child = master.firstChild();
    while (!child.isNull())
    {
        if (child.toElement().tagName() == "id")
        {
            QByteArray bArrayElement = QByteArray::fromBase64(QByteArray(child.toElement().text().toUtf8()));
            setId(QString().fromUtf8(bArrayElement.data(),bArrayElement.size()));
        }
        else if (child.toElement().tagName() == "pos")
        {
            int x = child.toElement().attribute("x").toUInt();
            int y = child.toElement().attribute("y").toUInt();
            move(x,y);
        }
        else if (child.toElement().tagName() == "text")
        {
            QByteArray bArrayElement = QByteArray::fromBase64(QByteArray(child.toElement().text().toUtf8()));
            text = QString().fromUtf8(bArrayElement.data(),bArrayElement.size());
        }
        else if (child.toElement().tagName() == "subText")
        {
            QByteArray bArrayElement = QByteArray::fromBase64(QByteArray(child.toElement().text().toUtf8()));
            subText = QString().fromUtf8(bArrayElement.data(),bArrayElement.size());
        }
        else if (child.toElement().tagName() == "description")
        {
            QByteArray bArrayElement = QByteArray::fromBase64(QByteArray(child.toElement().text().toUtf8()));
            description = QString().fromUtf8(bArrayElement.data(),bArrayElement.size());
        }
        else if (child.toElement().tagName() == "textFont")
        {
            QByteArray bArrayElement = QByteArray::fromBase64(QByteArray(child.toElement().text().toUtf8()));
            textFont.fromString( QString().fromUtf8(bArrayElement.data(),bArrayElement.size()) );
        }
        else if (child.toElement().tagName() == "subTextFont")
        {
            QByteArray bArrayElement = QByteArray::fromBase64(QByteArray(child.toElement().text().toUtf8()));
            subTextFont.fromString( QString().fromUtf8(bArrayElement.data(),bArrayElement.size()) );
        }
        else if (child.toElement().tagName() == "borderColor")
        {
            borderColor = XMLFunctions::colorFromXML(child);
        }
        else if (child.toElement().tagName() == "selectedBorderColor")
        {
            selectedBorderColor = XMLFunctions::colorFromXML(child);
        }
        else if (child.toElement().tagName() == "textColor")
        {
            textColor = XMLFunctions::colorFromXML(child);
        }
        else if (child.toElement().tagName() == "subTextColor")
        {
            subTextColor = XMLFunctions::colorFromXML(child);
        }
        else if (child.toElement().tagName() == "fillColor")
        {
            fillColor = XMLFunctions::colorFromXML(child);
        }
        else if (child.toElement().tagName() == "fillColor2")
        {
            fillColor2 = XMLFunctions::colorFromXML(child);
        }
        else if (child.toElement().tagName() == "fillMode")
        {
            fillMode = ((AbstractNodeWidget::ItemBoxFillMode)child.toElement().text().toUInt());
        }
        else if (child.toElement().tagName() == "borderRoundRectPixels")
        {
            borderRoundRectPixels = child.toElement().text().toUInt();
        }
        else if (child.toElement().tagName() == "anchored")
        {
            anchored = child.toElement().text().toUInt()==1?true:false;
        }
        else
        {
            setXMLLocalProperties(child);
        }

        child = child.nextSibling();
    }

    return true;
}

const QColor &AbstractNodeWidget::getFillColor2() const
{
    return fillColor2;
}

void AbstractNodeWidget::setFillColor2(const QColor &newFillColor2)
{
    fillColor2 = newFillColor2;
}

int AbstractNodeWidget::getVerticalOffset() const
{
    return verticalOffset;
}


void AbstractNodeWidget::leaveEvent(QEvent * )
{
    setMouseOver(false);
    repaint();
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void AbstractNodeWidget::enterEvent(QEnterEvent *)
#else
void AbstractNodeWidget::enterEvent(QEvent *)
#endif
{
    setMouseOver(true);
    raise();
    repaint();
}

void AbstractNodeWidget::keyPressEvent(QKeyEvent *event)
{
    //qDebug() << "pressing " << event->key() << " on " << objectName();
    switch (event->key())
    {
    case Qt::Key_Control:
    {
        GRAPH->setKeyCtrlActivated(true);
    }break;
    case Qt::Key_Escape:
    {
        if (GRAPH->getKeyAction( GraphWidget::KEYACT_ESCAPE_KEY_DESELECT ))
            GRAPH->deselectAll();
        else
        {
            emit itemEscapePressed(this);
        }
    }break;
    case Qt::Key_Delete:
    {
        if (GRAPH->getKeyAction(GraphWidget::KEYACT_DELETE_KEY_DELETES))
        {
            for ( auto selectedItem : GraphWidget::getSelectedItemsRecursively(this))
            {
                delete selectedItem;
            }
            repaint();
            GRAPH->repaint();
        }
    } break;
    default:
    {
        emit itemKeyPressEvent (this, event );
    }break;
    }
}

void AbstractNodeWidget::keyReleaseEvent(QKeyEvent *)
{
    GRAPH->setKeyCtrlActivated(false);
}

void AbstractNodeWidget::focusOutEvent(QFocusEvent *)
{
    repaint();
    update();
}

void AbstractNodeWidget::grabPositionOffset(const QPoint & offset)
{
    absOffset = getAbsolutePos();
    mouseOffset = offset;
}

bool AbstractNodeWidget::moveRelative(const QPoint &diffPos, bool force)
{
    QPoint nextRelativePos = mapToParent(diffPos);

    auto parentWidth = ((QWidget *)parent())->size().width();
    auto parentHeight = ((QWidget *)parent())->size().height();

    int pVerticalOffset = parent()->objectName().startsWith("GROUP-")? ((AbstractNodeWidget *)parent())->getVerticalOffset() : 0;

    // check for parent lower boundaries
    if (nextRelativePos.x()<0)
        nextRelativePos.setX(0);

    if (nextRelativePos.y()<pVerticalOffset)
        nextRelativePos.setY(pVerticalOffset);

    // check for parent upper boundaries
    if ((nextRelativePos.x()+size().width())> parentWidth )
        nextRelativePos.setX(parentWidth - size().width());

    if ((nextRelativePos.y()+size().height())> parentHeight )
        nextRelativePos.setY(parentHeight - size().height());

    // check if overlaps any silbling...
    if (GRAPH->getAllowOverlap() || force || !overlapsWithSibling(diffPos) )
    {
        this->move(nextRelativePos);
        return true;
    }

    return false;
}

bool AbstractNodeWidget::overlapsWithSibling(const QPoint &diffPos)
{
    QPoint nextAbsolutePos = (getAbsolutePos() + diffPos);
    QRect nextAbsoluteRect = rect();
    nextAbsoluteRect.setRect( nextAbsolutePos.x(), nextAbsolutePos.y(), nextAbsoluteRect.width(), nextAbsoluteRect.height() );

    toPaint.setRect(0,0,0,0);

    bool overlaps = false;

    for (auto sibling : GraphWidget::allChildrenItemsAndGroups((QWidget *)parent()))
    {
        auto siblingAbsRect = sibling->getAbsoluteRect();
        if ( sibling!=this && siblingAbsRect.intersects( nextAbsoluteRect ) )
        {
            toPaint = nextAbsoluteRect;
            overlaps = true;
        }
    }

    return overlaps;
}

void AbstractNodeWidget::mousePressEvent(QMouseEvent *e)
{
    if (!GRAPH->getIsDuringManualLinking())
    {
        if (e->button()==Qt::LeftButton)
        {
            // Multi-Selection:
            if (!GRAPH->getKeyCtrlActivated())
                GRAPH->deselectAll();

            // Select...
            activateWindow();
            raise();
            setSelected(true);

            grabPositionOffset(e->pos());
        }
    }
    ((QWidget *)parent())->repaint();
}

void AbstractNodeWidget::mouseDoubleClickEvent(QMouseEvent *)
{
    if (!GRAPH->getKeyCtrlActivated())
    {
        emit itemDoubleClick(this);
    }
}

void AbstractNodeWidget::mouseReleaseEvent(QMouseEvent *event)
{
    pressed = false;
    if (!GRAPH->getIsDuringManualLinking())
    {
        emit itemClickEvent(this,event);
    }
    ((QWidget *)parent())->repaint();
    GRAPH->repaint();
}

void AbstractNodeWidget::mouseMoveEvent(QMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton) && !getAnchor())
    {
        // move...
        moveRelative(event->pos() - mouseOffset);

        if (GRAPH->getKeyCtrlActivated())
        {
            for ( auto *item : GraphWidget::getSelectedItemsRecursively(GRAPH)  )
            {
                if (item!=this)
                {
                    item->moveRelative( event->pos() - mouseOffset );
                }
            }
        }

        // Repaint everything (relations, group, etc)...
        GRAPH->repaint();
    }
    toPaint = QRect(0,0,0,0);

}

