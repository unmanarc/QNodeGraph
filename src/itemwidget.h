#ifndef ITEMWIDGET_H
#define ITEMWIDGET_H

#include "abstractnodewidget.h"
#include "link.h"

#include <QSet>

namespace QNodeGraph
{

class ItemWidget : public AbstractNodeWidget
{
    Q_OBJECT
public:
    enum TextPosition { TEXTPOS_BOTTOM = 0, TEXTPOS_RIGHT = 1, TEXTPOS_LEFT = 2 };
    enum ItemBoxShape { ITEMBOX_SHAPE_NONE =0, ITEMBOX_SHAPE_CIRCLE=1, ITEMBOX_SHAPE_BOX = 2 };

    /**
     * @brief ItemWidget Constructor
     * @param parent items container (group or graph)
     * @param xml XML to initialize this node.
     */
    ItemWidget(QWidget *parent, const QString &xml);
    /**
     * @brief ItemWidget Constructor
     * @param id new item id
     * @param parent items container (group or graph)
     */
    ItemWidget(const QString &id, QWidget *parent);
    /**
     * @brief ItemWidget Constructor
     * @param id new item id
     * @param parent items container (group or graph)
     * @param text text to be printed on the item
     * @param subtext 2nd text to be printed
     */
    ItemWidget(const QString &id, QWidget *parent, const QString &text, const QString & subtext);
    /**
     * @brief ItemWidget Constructor
     * @param id new item id
     * @param parent items container (group or graph)
     * @param text text to be printed on the item
     * @param subtext 2nd text to be printed
     * @param zoomOutLevel zoom out level (from 0 to 10), 0 means no change.
     */
    ItemWidget(const QString &id, QWidget *parent, const QString & text, const QString & subtext, int zoomOutLevel);

    /**
     * @brief ~ItemWidget Virtual Destructor (will destroy related inter-elements links)
     */
    virtual ~ItemWidget();

    /**
     * @brief filter filter based in the filter text, if match, item will be marked.
     * @param filterText filter text
     */
    void filter(const QString & filterText);

    /* Linked Item */
    /**
     * @brief linkItem Link item to this item.
     * @param itemToLink Item to link
     * @param linkName Link Name
     * @param linkColor Link Color
     * @param linkType Link Type (directed/undirected)
     * @param arcDirection Arc Direction
     */
    void linkItem(ItemWidget * itemToLink, const QString & linkName, const QColor & linkColor, const Link::Type & linkType = Link::TYPE_UNDIRECTED , const Link::Direction &arcDirection = Link::DIR_BOTH );
    /**
     * @brief addLink Add Link to this element
     * @param linkPtr Link pointer
     */
    void addLink(void * linkPtr);
    /**
     * @brief removeLink Remove Link
     * @param linkedItem Link to be removed
     * @param destroyLinkObject delete the link object
     */
    void removeLink(ItemWidget * linkedItem, bool destroyLinkObject = false);
    /**
     * @brief isLinkedTo check if it's linked to another item
     * @param linkedItem
     * @return true if a link exist
     */
    bool isLinkedTo(ItemWidget * item);
    /**
     * @brief getLinks Get Links
     * @return Get links pointer addresss (of type Link*)
     */
    QList<void *> getLinks();

    /* Item Configuration Scheme */

    // Icon:
    /**
     * @brief setIcon Set Icon
     * @param icon icon data
     */
    void setIcon(const QIcon & icon);
    /**
     * @brief setIconSize Set Icon Size (W=H)
     * @param w width and height
     */
    void setIconSize(int w);
    /**
     * @brief setIconSize Set Icon Size
     * @param w width
     * @param h height
     */
    void setIconSize(int w, int h);
    /**
     * @brief getIconCenterPoint Get Icon Center Point
     * @return item position of the icon center
     */
    QPoint getIconCenterPoint() const;

    /**
     * @brief getShape Get Item shape
     * @return item shape (box, circle, none)
     */
    ItemWidget::ItemBoxShape getShape() const;
    /**
     * @brief setShape Set Item Shape
     * @param newShape shape (box, circle, none)
     */
    void setShape(ItemWidget::ItemBoxShape newShape);
    /**
     * @brief addTag Add Tag (eg. computer, router, etc)
     * @param tag tag name
     */
    void addTag(const QString & tag);
    /**
     * @brief removeTag Remove Tag
     * @param tag tag name
     */
    void removeTag(const QString & tag);
    /**
     * @brief getTags Get Item Tags
     * @return item assigned tags
     */
    QSet<QString> getTags();
    /**
     * @brief checkTag check if tag exist
     * @param tag tag name
     * @return true if exist in this item
     */
    bool containsTag(const QString & tag);


    // Text:
    void setTextPosition(const ItemWidget::TextPosition &textPosition);

    // Zoom Level:
    /**
     * @brief setZoomOutLevel Zoom Out Level
     * @param zoomOutLevel level from 0 to 10
     */
    void setZoomOutLevel(const unsigned int & zoomOutLevel);
    /**
     * @brief zoomOutLevelUp Increase the zoom out level by 1
     */
    void zoomOutLevelUp();
    /**
     * @brief zoomOutLevelDown Decrease the zoom out level by 1
     */
    void zoomOutLevelDown();
    /**
     * @brief getZoomOutFactor Get zoom out factor
     * @return double between >0 and 1, where 1 is no zoom, this number can be multiplied for font/size purporses.
     */
    double getZoomOutFactor() const;


    // Layer Ordering:
    /**
     * @brief selectRecursivelyAllLinkedItems Select Recursively All Linked Items
     */
    void selectRecursivelyAllLinkedItems();
    /**
     * @brief setSortPosition Set sort position (useful for sort heuristic calculations)
     * @param x integer referencing the position
     */
    void setSortPosition(const int &x);
    /**
     * @brief getSortPosition Get sort position
     * @return integer referencing the position
     */
    int getSortPosition() const;
    /**
     * @brief setLayer Set layer in the arrange for this item
     * @param x layer number
     */
    void setLayer(const int & currentLayer);
    /**
     * @brief getLayer Get layer in the arrange for this item
     * @return
     */
    int getLayer() const;
    /**
     * @brief calcSortPosition Calculate sort position based on parent positions
     */
    void calcSortPosition();
    /**
     * @brief assignLayerRecursively Calculate the layer based on a recursive function
     * @param value current layer
     * @return true if done, false if already done
     */
    bool assignLayerRecursively(const int & currentLayer);
    /**
     * @brief getBelongsToLayerZero Get if this item belongs to layer zero by selection
     * @return true if belongs
     */
    bool getBelongsToLayerZero() const;
    /**
     * @brief setBelongsToLayerZero Set this item to belong to layer zero by selection,
     *                              if there is one element selected this way, the layer zero will be from this and not from the selection
     * @param belongsToLayerZero true for belong, false otherwise (default)
     */
    void setBelongsToLayerZero(const bool &belongsToLayerZero);

protected:
    virtual void paintEvent( QPaintEvent* );
    void localInit();

    QString getXMLLocal();
    QString getXMLLocalProperties();

    QString getXMLTags();

    bool setXMLLocalProperties(const QDomNode &child);
    bool setXMLLocal(const QDomNode & child);

    void setInternalObjectID();

private:
    // Zoom out level:
    unsigned int zoomOutLevel;

    // Icon
    QIcon * icon;
    QSize IconSize;

    // Proprieties
    bool belongsToLayerZero;
    bool currentFilterMatch;

    QSet<QString> tags;
    ItemWidget::TextPosition textPosition;
    ItemWidget::ItemBoxShape shape;

    // Linked Nodes
    QList<void *> links;

    // Temp vars
    int currentLayer, sortPosition;

    // Private methods
    void recalculateSize();

    // XML
    bool SetNodeXMLLinks(const QDomNode & master);
    bool SetXMLLinks(const QDomNode & master);
};

}

#endif
