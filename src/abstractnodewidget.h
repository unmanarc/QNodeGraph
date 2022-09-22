#ifndef ABSTRACTNODEWIDGET_H
#define ABSTRACTNODEWIDGET_H

#include <QWidget>
#include <QString>
#include <QPixmap>
#include <QSize>
#include <QPoint>
#include <QList>
#include <QDomDocument>
#include <QDomElement>

#define PI 3.14159265

#define SPACING_BORDER1 1
#define SPACING_VSIDES 3
#define SPACING_HSIDES 3

#define GRAPH ((GraphWidget *)nodeGraphParent)

namespace QNodeGraph
{

class AbstractNodeWidget : public QWidget
{
    Q_OBJECT
public:
    enum ItemBoxFillMode { ITEMBOX_FILL_TRANSPARENT=0, ITEMBOX_FILL_SOLID=1, ITEMBOX_FILL_GRADIENT=2 };

    /////////////////////////////////////////////////////////////////////
    // INIT:
    /**
     * @brief AbstractNodeWidget Constructor
     * @param parent parent container
     * @param xml XML to initialize
     */
    AbstractNodeWidget(QWidget *parent, const QString &xml, const QString & type);
    /**
     * @brief AbstractNodeWidget Constructor
     * @param id node ID
     * @param parent parent container
     */
    AbstractNodeWidget(const QString &id, QWidget *parent);
    /**
     * @brief AbstractNodeWidget Constructor
     * @param id node ID
     * @param parent parent container
     * @param text internal text
     * @param subtext internal sub text
     */
    AbstractNodeWidget(const QString &id, QWidget *parent, const QString &text, const QString & subtext);
    /**
     * @brief ~AbstractNodeWidget Virtual Destructor
     */
    virtual ~AbstractNodeWidget();
    /**
     * @brief init Initialization
     * @param id node id
     * @param parent parent container
     */
    void init(const QString & id, QWidget *parent);

    /////////////////////////////////////////////////////////////////////
    // ID:
    /**
     * @brief setId Get Node ID
     * @param nodeId Node ID
     */
    void setId(const QString &nodeId);
    /**
     * @brief getID Get Node ID
     * @return node ID
     */
    QString getID() const;
    /**
     * @brief compareID compare if the id is equal to the itemId
     * @param itemId item id
     * @return true if equal
     */
    bool compareID(const QString & itemId) const;

    /////////////////////////////////////////////////////////////////////
    // TEXT AND SUBTEXT:
    /**
     * @brief setText Set Node Text
     * @param text Node Text
     */
    void setText(const QString &text);
    /**
     * @brief getText Get Node Text
     * @return Node Text
     */
    QString getText() const;
    /**
     * @brief setSubText Set Node SubText
     * @param subText node subtext
     */
    void setSubText(const QString & subText);
    /**
     * @brief getSubText Get Node SubText
     * @return node subtext
     */
    QString getSubText() const;
    /**
     * @brief setTextColor Set Text Color
     * @param textColor Text Color
     */
    void setTextColor(const QColor & textColor);
    /**
     * @brief setSubTextColor Set SubText Color
     * @param subTextColor SubText Color
     */
    void setSubTextColor(const QColor & subTextColor);
    /**
     * @brief setSubTextFont Set SubText Font
     * @param font font to be used in the text
     */
    void setSubTextFont(const QFont &font);
    /**
     * @brief setTextFont Set Text Font
     * @param font font to be used in the subtext
     */
    void setTextFont(const QFont & font);

    // TODO: description is used?
    /**
     * @brief setDescription Set Description
     * @param description description
     */
    void setDescription(const QString &description);

    /////////////////////////////////////////////////////////////////////
    // DECORATION:
    /**
     * @brief setSelectedBorderColor Set Selected Border Color
     * @param selectedBorderColor Color when the border is selected or mouse is over
     */
    void setSelectedBorderColor(const QColor & selectedBorderColor);
    /**
     * @brief setBorderColor Set Border Color
     * @param borderColor border color when not selected
     */
    void setBorderColor(const QColor &borderColor);
    /**
     * @brief getFillMode Get Fill Mode (eg. solid, transparent, gradient)
     * @return fill mode.
     */
    AbstractNodeWidget::ItemBoxFillMode getFillMode() const;
    /**
     * @brief setFillMode Set Fill Mode (eg. solid, transparent, gradient)
     * @param newFillMode fill mode.
     */
    void setFillMode(AbstractNodeWidget::ItemBoxFillMode newFillMode);
    /**
     * @brief getFillColor Get main fill color
     * @return main fill color
     */
    const QColor &getFillColor() const;
    /**
     * @brief setFillColor Set Main Fill Color
     * @param newFillColor Main Fill Color
     */
    void setFillColor(const QColor &newFillColor);
    /**
     * @brief getFillColor2 Get second fill color (for gradient 2nd color)
     * @return second fill color (for gradient 2nd color)
     */
    const QColor &getFillColor2() const;
    /**
     * @brief setFillColor2 Set second fill color (for gradient 2nd color)
     * @param newFillColor2 second fill color (for gradient 2nd color)
     */
    void setFillColor2(const QColor &newFillColor2);
    /**
     * @brief getBorderRoundRectPixels Get Border Round pixels
     * @return round pixels from the border
     */
    int getBorderRoundRectPixels() const;
    /**
     * @brief setBorderRoundRectPixels Set Border Round pixels
     * @param newRoundRectPixels round pixels from the border
     */
    void setBorderRoundRectPixels(int newRoundRectPixels);

    /////////////////////////////////////////////////////////////////////
    // POSITION AND SIZE:
    /**
     * @brief getAbsolutePos Get absolute position in the graphic
     * @return point referencing the x,y position in the graphic
     */
    QPoint getAbsolutePos() const;
    /**
     * @brief getAbsoluteRect Get absolute position and size (QRect) in the graphic
     * @return QRect referencing the x,y position in the graphic and the w,h from this object
     */
    QRect getAbsoluteRect() const;
    /**
     * @brief getCenterPoint Get the center point of this element.
     * @return center point from top={0,0}
     */
    QPoint getCenterPoint() const;
    /**
     * @brief getExternalRadius Get the external radius, this is useful for generating lines that will not touch the element.
     * @return external radius in pixels
     */
    double getExternalRadius() const;

    /**
     * @brief overlapsWithSibling Check if overlaps with any sibling in the parent graphic
     * @param diffPos differential position (default=0,0, which means the absolute position of the object in the graphic)
     * @return true if overlaps
     */
    bool overlapsWithSibling( const QPoint &diffPos = QPoint(0,0) );

    /**
     * @brief toPaint Rectangle to paint if the node was overlaped
     */
    QRect toPaint;

    /**
     * @brief getVerticalOffset Get Vertical offset for internal objects (eg. groups have a title, this will create an offset, items will have zero offsets)
     * @return vertical reserved pixels
     */
    int getVerticalOffset() const;

    /**
     * @brief moveToRandom Move this element to a random point of the graphic.
     */
    void moveToRandom();
    /**
     * @brief setAnchor Anchor the node position in the graph
     * @param x true to anchor
     */
    void setAnchor(bool x);
    /**
     * @brief getAnchor Get if the node is anchored in the graph
     * @return true if anchored
     */
    bool getAnchor() const;

    /////////////////////////////////////////////////////////////////////
    // XML:
    /**
     * @brief getXML Get XML from this object
     * @param xmltag include the XML main tag (to save this element alone)
     * @return XML structure with all the node properties.
     */
    QString getXML(const QString &widgetName, bool xmltag=false);
    /**
     * @brief setXML Setup this node from an XML
     * @param xml XML with the node data
     * @return true if succeed, false otherwise
     */
    bool setXML(const QString &widgetName, const QString &xml);
    /**
     * @brief getEmbeddedData Get embedded data
     * @return embedded data
     */
    QString getEmbeddedData() const;
    /**
     * @brief setEmbeddedData Set embedded data
     * @param x embeddedData
     */
    void setEmbeddedData(const QString & embeddedData);

    /////////////////////////////////////////////////////////////////////
    // Selection:
    /**
     * @brief setSelected Set that the node is selected
     * @param x true for selected, false otherwise
     */
    void setSelected(bool x);
    /**
     * @brief getIsSelected Get if the node is selected
     * @return true for selected, false otherwise
     */
    bool getIsSelected() const;
    /**
     * @brief setMouseOver Set if the mouse pointer is over the element
     * @param x true if it's over the element
     */
    void setMouseOver(bool x);
    /**
     * @brief setSelectionMark Internal method to mark already processed nodes (when selecting in recursion)
     * @param x true to mark as already processed
     */
    void setSelectionMark(bool x);
    /**
     * @brief getSelectionMark Get the selection Mark (to check if the node was already processed by the recursion)
     * @return true if already processed
     */
    bool getSelectionMark() const;

protected:
    // XML SET/GET
    bool setNodeXMLLocalProperties(const QDomNode &master);

    virtual QString getXMLLocal()=0;
    virtual QString getXMLLocalProperties()=0;

    virtual bool setXMLLocalProperties(const QDomNode & child)=0;
    virtual bool setXMLLocal(const QDomNode & child)=0;

    virtual void recalculateSize()=0;
    virtual void setInternalObjectID()=0;

    // Parent
    QWidget * nodeGraphParent, *groupParent;

    // Proprieties
    bool anchored;

    int borderRoundRectPixels;

    QString id, text, subText, description, embeddedData;
    QFont textFont, subTextFont;
    AbstractNodeWidget::ItemBoxFillMode fillMode;

    QColor borderColor, selectedBorderColor;
    QColor textColor, subTextColor;
    QColor fillColor, fillColor2;

    // Temp vars
    bool mouseover,selected, pressed;
    bool selectionMark;


    // Permitted Y Vertical offset for internal objects.
    int verticalOffset;

private:
    /**
     * @brief grabPositionOffset Grab the position offset (internal function when moving objects)
     * @param offset initial point
     */
    void grabPositionOffset(const QPoint &offset);
    /**
     * @brief moveRelative Move relative to differential position
     * @param diffPos differencial position
     * @param force force to move even if overlaps
     * @return true if moved, false if not
     */
    bool moveRelative(const QPoint &diffPos, bool force = false);

    QPoint mouseOffset, absOffset;

protected slots:
    virtual void mouseMoveEvent(QMouseEvent * event);
    virtual void mouseReleaseEvent(QMouseEvent * event);
    virtual void mouseDoubleClickEvent(QMouseEvent * e);
    virtual void mousePressEvent( QMouseEvent*  );
    virtual void leaveEvent ( QEvent * );
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    virtual void enterEvent ( QEnterEvent * event );
#else
    virtual void enterEvent ( QEvent * event );
#endif
    virtual void keyPressEvent ( QKeyEvent * event );
    virtual void keyReleaseEvent ( QKeyEvent * event );
    virtual void focusOutEvent ( QFocusEvent * ) ;

signals:
    // Double click over element (check if are node item or group)
    void itemDoubleClick(AbstractNodeWidget * emisor);
    // Delete Key Pressed (check if are node item or group)
    void itemDeletePressed(AbstractNodeWidget * emisor);
    // Escape Key Pressed (check if are node item or group)
    void itemEscapePressed(AbstractNodeWidget * emisor);
    // Key Pressed (check if are node item or group)
    void itemKeyPressEvent (AbstractNodeWidget * emisor, QKeyEvent * event );
    // Simple click over element (check if are node item or group)
    void itemClickEvent(AbstractNodeWidget * emisor, QMouseEvent * e);
};

}

#endif
