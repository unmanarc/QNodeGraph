#ifndef QNODEGRAPHWIDGET_H
#define QNODEGRAPHWIDGET_H

#include <QPixmap>
#include <QSize>
#include <QIcon>
#include <QList>

#include "itemwidget.h"
#include "groupwidget.h"

namespace QNodeGraph
{

struct XY
{
    XY()
    {x=y=0;}

    int x,y;
};

class GraphWidget : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief QGraphWidget Constructor
     * @param parent
     */
    GraphWidget(QWidget *parent = 0);
    ~GraphWidget();
    /**
     * @brief setTitle Set Graph Title
     * @param title Graphic title string..
     */
    void setTitle(const QString &title);
    /**
     * @brief getTitle Get Graph Title
     * @return Graph Title String
     */
    QString getTitle();

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // VIEW PORT:
    /**
     * @brief setBackgroundColor Set Graph Background Color
     * @param bgcolor Background color
     */
    void setBackgroundColor(const QColor & backgroundColor);
    /**
     * @brief getBackgroundColor Get Graph Background Color
     * @return background color
     */
    QColor getBackgroundColor() const;
    /**
     * @brief replaceMinimumSize Set minimum size of a container using the bigest children as reference
     * @param v container (group or graphic)
     */
    static void replaceMinimumSize(QWidget *v);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // NODES/CHILDRENS:
    /**
     * @brief getNodeById Get ItemWidget (as ItemWidget*) by it's ID.
     * @param id ID
     * @return item that match with the id
     */
    ItemWidget * getItemById(const QString &id);
    /**
     * @brief itemAt Get item at certain position
     * @param p position
     * @param includeNestedItems (check for items inside groups)
     * @return item found in place at this position (raised)
     */
    ItemWidget *itemAt(const QPoint & p, bool includeNestedItems = true);
    /**
     * @brief allChildrenItems Get all children items from a container
     * @param v container
     * @return all children items
     */
    static QList<ItemWidget *> allChildrenItems(QWidget * v);
    /**
     * @brief allChildrenGroups Get all children groups from a container
     * @param v container
     * @return all children groups
     */
    static QList<GroupWidget *> allChildrenGroups(QWidget * v);
    /**
     * @brief allChildrenItemsAndGroups Get all children items and groups (nodes) from a container
     * @param v container
     * @return all children items and groups (nodes)
     */
    static QList<AbstractNodeWidget *> allChildrenItemsAndGroups(QWidget * v);
    /**
     * @brief allRecursiveItemsAndGroups Get all children items and groups (nodes) from a container recursively
     * @param v container
     * @return all children items and groups (nodes)
     */
    static QList<AbstractNodeWidget *> allRecursiveItemsAndGroups(QWidget *v);
    /**
     * @brief allRecursiveItems Get all items from the widget and all the sub containers recursively
     * @param v container
     * @return items
     */
    static QList<ItemWidget *> allRecursiveItems(QWidget *v);
    /**
     * @brief getSelectedItemsRecursively Get all selected items from this graph and all the sub containers recursively
     * @return selected items
     */
    static QList<ItemWidget *> getSelectedItemsRecursively(QWidget *v);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // ITEM MANIPULATION:
    /**
     * @brief deselectAll Remove all selections on items
     */
    void deselectAll();
    /**
     * @brief selectAll Select all direct children items on this graphic
     */
    void selectAll();
    /**
     * @brief selectAllRecursiveItems Select all items on this graphic and sub containers recursively
     */
    void selectAllRecursiveItems();
    /**
     * @brief invertSelectionOnAllRecursiveItems Invert all selections in all items (recursively)
     */
    void invertSelectionOnAllRecursiveItems();
    /**
     * @brief deleteAll Delete all children items
     */
    void deleteAll();
    /**
     * @brief deleteAllRecursiveItems Delete all items recursively
     */
    void deleteAllRecursiveItems();

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // MANUAL LINKING:
    /**
     * @brief setManualLinking Set Manual Linking... at the end of the link process, itemLinked signal will be emited
     * @param manualLink true to link the next two selected items, false to behave as usual.
     */
    void setManualNodesLinking(const bool &manualLink = true);
    /**
     * @brief getIsDuringManualLinking Get if we are during manual linking
     * @return true if we are during manual linking
     */
    bool getIsDuringManualLinking() const;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // DEFAULTS:
    /**
     * @brief setDefaultItemIcon Set Default Items Icon
     * @param itemsDefaultIcon icon data
     */
    void setDefaultItemIcon(const QIcon & itemsDefaultIcon);
    /**
     * @brief getDefaultItemIcon Get default icon for new items
     * @return icon data
     */
    QIcon getDefaultItemIcon() const;

    /**
     * @brief setDefaultItemIconSize Set Default Item Icon Size
     * @param itemsDefaultIconSize Items default icon size (W=H)
     */
    void setDefaultItemIconSize(const int & itemsDefaultIconSize);
    /**
     * @brief getDefaultItemIconSize Get default icon for new items
     * @return icon size (W=H)
     */
    int getDefaultItemIconSize() const;

    /**
     * @brief setDefaultNodeTextFont Set Default Text Font for new nodes
     * @param itemsDefaultTextFont font
     * @param override change current visual values on existing elements
     */
    void setDefaultNodeTextFont(const QFont & itemsDefaultTextFont, bool override = false);
    /**
     * @brief getDefaultNodeTextFont Get Default Node Text Font
     * @return default text font for new nodes.
     */
    QFont getDefaultNodeTextFont() const;

    /**
     * @brief setDefaultNodeSubTextFont Set default subtext font for new nodes
     * @param itemsDefaultSubTextFont font
     * @param override change current visual values on existing elements
     */
    void setDefaultNodeSubTextFont(const QFont &itemsDefaultSubTextFont, bool override = false);
    /**
     * @brief getDefaultNodeSubTextFont Get default subtext font for new nodes
     * @return font
     */
    QFont getDefaultNodeSubTextFont() const;

    /**
     * @brief setDefaultNodeTextColor Set default text color for new nodes
     * @param itemsDefaultTextColor text color
     * @param override change current visual values on existing elements
     */
    void setDefaultNodeTextColor(const QColor & itemsDefaultTextColor, bool override = false);
    /**
     * @brief getDefaultNodeTextColor Get default text color for new nodes
     * @return text color
     */
    QColor getDefaultNodeTextColor() const;

    /**
     * @brief setDefaultItemTextPosition Set default text position for new items
     * @param itemsDefaultTextPosition default text position
     * @param override change current visual values on existing elements
     */
    void setDefaultItemTextPosition(ItemWidget::TextPosition itemsDefaultTextPosition, bool override = false);
    /**
     * @brief getDefaultItemTextPosition Get default text position for new items
     * @return default text position
     */
    ItemWidget::TextPosition getDefaultItemTextPosition() const;

    /**
     * @brief setDefaultNodeSubTextColor Set default subtext color for new nodes
     * @param itemsDefaultSubTextColor subtext color
     * @param override change current visual values on existing elements
     */
    void setDefaultNodeSubTextColor(const QColor &itemsDefaultSubTextColor, bool override = false);
    /**
     * @brief getDefaultNodeSubTextColor Get default subtext color for new nodes
     * @return subtext color
     */
    QColor getDefaultNodeSubTextColor() const;

    /**
     * @brief setDefaultNodeSelectedBorderColor Set default selected border color on new nodes
     * @param itemsDefaultSelectedColor selected border color
     * @param override change current visual values on existing elements
     */
    void setDefaultNodeSelectedBorderColor(const QColor & itemsDefaultSelectedColor, bool override = false);
    /**
     * @brief getDefaultNodeSelectedBorderColor Get default selected border color on new nodes
     * @return selected border color
     */
    QColor getDefaultNodeSelectedBorderColor() const;

    /**
     * @brief setDefaultNodeBorderColor Set default border color for new nodes
     * @param itemsDefaultBorderColor border color
     * @param override change current visual values on existing elements
     */
    void setDefaultNodeBorderColor(const QColor &itemsDefaultBorderColor, bool override = false);
    /**
     * @brief getDefaultNodeBorderColor Get default border color for new nodes
     * @return color
     */
    QColor getDefaultNodeBorderColor() const;

    /**
     * @brief setDefaultItemShape Set default shape for new items
     * @param newItemsDefaultDisplayBoxMode item shape
     * @param override change current visual values on existing elements
     */
    void setDefaultItemShape(ItemWidget::ItemBoxShape newItemsDefaultDisplayBoxMode, bool override = false);
    /**
     * @brief getDefaultItemShape Get default shape for new items
     * @return shape
     */
    ItemWidget::ItemBoxShape getDefaultItemShape() const;

    /**
     * @brief setDefaultNodeBorderRoundRectPixels Set default border round pixels for new nodes
     * @param newItemsDefaultBorderRoundRectPixels round pixels
     * @param override change current visual values on existing elements
     */
    void setDefaultNodeBorderRoundRectPixels(int newItemsDefaultBorderRoundRectPixels, bool override=false);
    /**
     * @brief getDefaultNodeBorderRoundRectPixels Get default border round pixels for new nodes
     * @return round pixels
     */
    int getDefaultNodeBorderRoundRectPixels() const;

    /**
     * @brief setDefaultNodeFillColor Set default fill color for new nodes
     * @param newItemsDefaultFillColor fill color
     * @param override change current visual values on existing elements
     */
    void setDefaultNodeFillColor(const QColor &newItemsDefaultFillColor, bool override=false);
    /**
     * @brief getDefaultNodeFillColor Get default fill color for new nodes
     * @return fill color
     */
    const QColor &getDefaultNodeFillColor() const;

    /**
     * @brief setDefaultNodeFillMode Set default fill mode for new nodes
     * @param newItemsDefaultFillMode fill mode (eg. transparent, solid, gradient)
     * @param override change current visual values on existing elements
     */
    void setDefaultNodeFillMode(AbstractNodeWidget::ItemBoxFillMode newItemsDefaultFillMode, bool override=false);
    /**
     * @brief getDefaultNodeFillMode Get default fill mode for new nodes
     * @return fill mode (eg. transparent, solid, gradient)
     */
    AbstractNodeWidget::ItemBoxFillMode getDefaultNodeFillMode() const;

    /**
     * @brief getDefaultNodeFillColor2 Get default 2nd fill color for gradients on new items
     * @return color
     */
    const QColor &getDefaultNodeFillColor2() const;
    /**
     * @brief setDefaultNodeFillColor2 Set default 2nd fill color for gradients on new items
     * @param newDefaultItemFillColor2 color
     */
    void setDefaultNodeFillColor2(const QColor &newDefaultItemFillColor2);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // FILTER:
    /**
     * @brief setFilterText Set filter text (to every item)
     * @param _filterText
     */
    void setFilterText(const QString &filterText);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // GRAPHIC PROPERTIES:
    /**
     * @brief getAllowOverlap Get if it's allowed to overlap Nodes
     * @return true if overlap is allowed
     */
    bool getAllowOverlap() const;
    /**
     * @brief setAllowOverlap Set to allow overlap nodes
     * @param newAllowOverlap true to overlap
     */
    void setAllowOverlap(bool newAllowOverlap);
    /**
     * @brief getAutoArrange Get if the graphic will autoarrange on new items/groups
     * @return true for auto arrange
     */
    bool getAutoArrange() const;
    /**
     * @brief setAutoArrange Set to auto arrange new items/groups
     * @param newAutoArrange true for auto arrange new items
     */
    void setAutoArrange(bool newAutoArrange);

    /**
     * @brief getAutoArrangeAlgorithm Get default auto arrange algorithm to use
     * @return algorithm to use (cast as Arrange::Mode)
     */
    int getAutoArrangeAlgorithm() const;
    /**
     * @brief setAutoArrangeAlgorithm Set default auto arrange algorithm to use
     * @param newAutoArrangeAlgorithm algorithm to use (cast to int from Arrange::Mode)
     */
    void setAutoArrangeAlgorithm(int newAutoArrangeAlgorithm);

    /**
     * @brief getSortBy Get default sort option for arrangements
     * @return sort option from (cast as Arrange::SortBy)
     */
    int getSortBy() const;
    /**
     * @brief setSortBy Set default sort option for arrangements
     * @param newSortBy sort option (cast to int from Arrange::SortyBy)
     */
    void setSortBy(int newSortBy);

    /**
     * @brief getAutoArrangeSpacing Get Auto Arrange space between nodes
     * @return pixel space between nodes
     */
    int getAutoArrangeSpacing() const;
    /**
     * @brief setAutoArrangeSpacing Set Auto Arrange space between nodes
     * @param newAutoArrangeSpacing pixel space between nodes
     */
    void setAutoArrangeSpacing(int newAutoArrangeSpacing);

    /**
     * @brief setResizable set if the graphic is resizeable or not
     * @param resizable true for allows manual resizing
     */
    void setResizable(bool resizable = true);
    /**
     * @brief getResizable Get if this graphic is resizable
     * @return true if allow manual resizing
     */
    bool getResizable();

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // EXPORT/IMPORT:
    /**
     * @brief getXML Get XML with all the graphic
     * @return XML data
     */
    QString getXML();
    /**
     * @brief setXML Set XML to set all the graphic
     * @param xml XML data
     * @return true if no error ocurred
     */
    bool setXML(const QString & xml);


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // SORT/ARRANGE/SELECTION:
    /**
     * @brief removeSelectionMarkOnAllItemsRecursively Remove selection mark
     */
    void removeSelectionMarkOnAllItemsRecursively();
    void removeSelectedItemsFromLayerZero();
    void addSelectedItemsToLayerZero();


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // KEY ACTIONS:
    enum KeyActions{
        KEYACT_DELETE_KEY_DELETES=0,
        KEYACT_ESCAPE_KEY_DESELECT=1
    };

    /**
     * @brief addKeyAction Add default Key Action
     * @param action Action
     */
    void addKeyAction(const KeyActions & action);
    /**
     * @brief removeKeyAction Remove default key action
     * @param action Action
     */
    void removeKeyAction(const KeyActions & action);
    /**
     * @brief getKeyAction Get if some key action is activated
     * @param action action to check
     * @return true if activated
     */
    bool getKeyAction(const KeyActions & action);
    /**
     * @brief getKeyCtrlActivated Get if the ctrl key is pressed
     * @return true if pressed, false otherwise
     */
    bool getKeyCtrlActivated() const;
    /**
     * @brief setKeyCtrlActivated Set that ctrl key was pressed
     * @param newKeyCtrlActivated true if pressed, false otherwise
     */
    void setKeyCtrlActivated(bool newKeyCtrlActivated);


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Resizing internal static functions for graph/groups:
    static void resizeOnMouseMove(QWidget *v, QMouseEvent *e,
                                  bool resizingX, bool resizingY,
                                  const QPoint &mouseCurrentPos);

    static bool resizeStartOnClick(QWidget *v, const QPoint &mouseCurrentPos,
                                   bool resizable,
                                   bool * resizingX, bool * resizingY
                                   );
    static void checkAndFixChildrenPositions(QWidget *v, int vOffset = 0 );

private:
    bool autoArrange;
    int autoArrangeAlgorithm, autoArrangeSpacing;
    int sortBy;

    bool isUnderSelection();
    void orderMouseRectCoordinates();

    // Event pre-configured actions
    QSet<KeyActions> keyActions;

    // Filter:
    QString filterText;

    // Mouse:
    QRect mouseRect;
    QPoint mouseCurrentPos;

    // Items Default Vars:
    int itemsDefaultBorderRoundRectPixels;
    QColor defaultItemTextColor, defaultItemSubTextColor, defaultItemSelectedBorderColor, defaultItemBorderColor, defaultItemFillColor, defaultItemFillColor2;
    QIcon itemsDefaultIcon;
    ItemWidget::TextPosition itemsDefaultTextPosition;
    ItemWidget::ItemBoxShape itemsDefaultShape;
    AbstractNodeWidget::ItemBoxFillMode itemsDefaultFillMode;
    int itemsDefaultIconSize;
    QFont itemsDefaultTextFont, itemsDefaultSubTextFont;

    // Workspace Properties:
    QString title;
    QColor backgroundColor;

    // State variable
    bool keyCtrlActivated;

    // Resize Process:
    bool resizable;
    bool allowOverlap;
    bool resizingX, resizingY;

    // auto-linking state variables
    bool duringManualLinking;
    ItemWidget * manualLinkedItem[2];

protected slots:
    virtual void paintEvent( QPaintEvent* );
    virtual void mouseMoveEvent ( QMouseEvent * );
    virtual void mousePressEvent( QMouseEvent*  );
    virtual void mouseReleaseEvent( QMouseEvent*  );
    virtual void mouseDoubleClickEvent ( QMouseEvent * e );
    virtual void keyPressEvent ( QKeyEvent * event );
    virtual void keyReleaseEvent ( QKeyEvent * event );
    virtual void focusOutEvent ( QFocusEvent * event ) ;

signals:
    // Double click with control over selected items (check if are node item or group)
    void itemsDoubleClick(QList<ItemWidget *> emisor);
    // Right click with control over selected items (check if are node item or group)
    void itemsRightClickEvent(QList<ItemWidget *> emisor);
    // Link 2 items
    void itemLinked(ItemWidget * first, ItemWidget *second);
};
}
#endif
