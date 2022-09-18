#ifndef Arrange_H
#define Arrange_H

#include <QWidget>
#include "graphwidget.h"

namespace QNodeGraph
{
class Arrange
{
public:
    Arrange();

    enum Mode { ARRANGEALG_RANDOM=0,
                ARRANGEALG_ROWS=1,
                ARRANGEALG_COLUMNS=2,
                ARRANGEALG_HTREE=3,
                ARRANGEALG_VTREE=4,
                ARRANGEALG_STAR=5 };

    enum SortBy {
        SORTBY_INSERT_POS=0,
        SORTBY_OBJECT_ID=1,
        SORTBY_TEXT=2,
        SORTBY_SUBTEXT=3
    };

    /**
     * @brief triggerAutoArrange Execute autoarrange in group and graphs when autoarrange is activated
     * @param v group or graph
     * @return zero for no errors.
     */
    static int triggerAutoArrange(QWidget * v);

    /**
     * @brief arrange Arrange some widget childrens items using a selected Mode/Sort
     * @param v group or graph
     * @param spacing spacing between items (for col/row)
     * @param mode algoritm
     * @param sortBy sort by (data to be sorted)
     * @return zero for no errors.
     */
    static int arrange( QWidget * v,int spacing, Mode mode, SortBy sortBy )
    {
        switch ( mode )
        {
        case ARRANGEALG_ROWS:
            return rows(v,spacing, sortBy);
        case ARRANGEALG_COLUMNS:
            return columns(v,spacing, sortBy);
        case ARRANGEALG_HTREE:
            return horizontalTree(v,getLayerZeroItems(v));
        case ARRANGEALG_VTREE:
            return verticalTree(v,getLayerZeroItems(v));
        case ARRANGEALG_STAR:
            return star(v,getLayerZeroItems(v));
        default:
        case ARRANGEALG_RANDOM:
            return random(v);
        }
    }

private:
    /**
     * @brief getAutoArrange Get if the widget is configured for auto-arrange his items..
     * @param v widget
     * @param mode output mode for auto arrange
     * @param sortBy sort by mode.
     * @return true for auto arrange
     */
    static bool getAutoArrange(QWidget * v , Mode *mode, SortBy *sortBy, int *spacing);
    /**
     * @brief resetAllLayerAndSortPositionInfo Initialize every item with no layer and sort position
     * @param v items container
     */
    static void resetAllLayerAndSortPositionInfo(QWidget * v);
    /**
     * @brief getUnarrangedItemWithMaxLinks Get Unarranged item with max links
     * @param v item container
     * @return item with max links or nullptr if there is not item left
     */
    static ItemWidget * getUnarrangedItemWithMaxLinks(QWidget * v);
    /**
     * @brief assignLayerToItemsRecursively Assign layer to items recursively starting with layer zero in r0items
     * @param layerZeroItems items to start the recursion, each item from this list will start with layer zero
     */
    static void assignLayerToItemsRecursively(QList<ItemWidget *> layerZeroItems);
    /**
     * @brief getLayerZeroItems Get items marked with layer zero.
     * @param v container
     * @return list of items.
     */
    static QList<ItemWidget *> getLayerZeroItems(QWidget * v);
    /**
     * @brief getLayerCount Get the layer count from the container
     * @param v container
     * @return layer count
     */
    static int getLayerCount(QWidget * v);
    /**
     * @brief getItemsFromLayer Get Items from an specific Layer
     * @param v container
     * @param layer layer number
     * @return list of items that belongs to this layer
     */
    static QList<ItemWidget *> getItemsFromLayer(QWidget * v, int layer);

    /* Arrange Algorithms */
    /**
     * @brief horizontalTree Arrange using Horizonal Tree Topology
     * @param v items container
     * @param layerZero first items to arrange
     * @return 0 if succeed
     */
    static int horizontalTree(QWidget * v, QList<ItemWidget *> layerZero);
    /**
     * @brief verticalTree Arrange using Vertical Tree Topology
     * @param v items container
     * @param layerZero first items to arrange
     * @return 0 if succeed
     */
    static int verticalTree(QWidget * v, QList<ItemWidget *> layerZero);
    /**
     * @brief star Arrange using star topology
     * @param v items container
     * @param layerZero first items to arrange
     * @return 0 if succeed
     */
    static int star(QWidget * v, QList<ItemWidget *> layerZero);
    /**
     * @brief random Arrange randomly in the space
     * @param v container
     * @return 0 if succeed
     */
    static int random(QWidget * v);

    // helper function:
    static int rowsMover(QWidget *v, int spacing, SortBy sortBy, bool group, XY *accumulated);
    /**
     * @brief rows Arrange by rows
     * @param v nodes container (will be resized)
     * @param spacing spacing between items
     * @param sortBy sort by policy
     * @return 0 if succeed
     */
    static int rows(QWidget * v, int spacing, SortBy sortBy);

    // helper function:
    static int columnsMover(QWidget *v, int spacing, SortBy sortBy, bool group, XY *accumulated);
    /**
     * @brief columns Arrange by columns
     * @param v nodes container (will be resized)
     * @param spacing spacing between items
     * @param sortBy sort by policy
     * @return 0 if succeed
     */
    static int columns(QWidget * v, int spacing, SortBy sortBy);
};

}

#endif // Arrange_H

