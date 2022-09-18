#include "arrange.h"
#include "graphwidget.h"
#include "groupwidget.h"
#include "itemwidget.h"

#include <QDebug>
#include <cmath>

using namespace QNodeGraph;

Arrange::Arrange()
{
}

int Arrange::triggerAutoArrange(QWidget *v)
{
    Mode arrangeMode;
    SortBy sortBy;
    int spacing=0;
    if (getAutoArrange(v,&arrangeMode,&sortBy, &spacing))
    {
        return arrange(v,spacing,arrangeMode,sortBy);
    }
    return -100; // NOT USED.
}

bool Arrange::getAutoArrange(QWidget *v, Mode *mode, SortBy * sortBy, int * spacing)
{
    if (v->objectName().startsWith("GROUP-"))
    {
        *mode = (Mode) ((GroupWidget *)v)->getAutoArrangeAlgorithm();
        *sortBy = (SortBy) ((GroupWidget *)v)->getAutoArrangeAlgorithm();
        *spacing = ((GroupWidget *)v)->getAutoArrangeSpacing();
        return ((GroupWidget *)v)->getAutoArrange();
    }
    else
    {
        *mode = (Mode) ((GraphWidget *)v)->getAutoArrangeAlgorithm();
        *sortBy = (SortBy) ((GraphWidget *)v)->getAutoArrangeAlgorithm();
        *spacing = ((GraphWidget *)v)->getAutoArrangeSpacing();
        return ((GraphWidget *)v)->getAutoArrange();
    }

    return false;
}

void Arrange::resetAllLayerAndSortPositionInfo(QWidget *v)
{
    for (auto i : GraphWidget::allChildrenItems(v))
    {
        i->setLayer(-1);
        i->setSortPosition(-1);
    }
}

ItemWidget *Arrange::getUnarrangedItemWithMaxLinks(QWidget *v)
{
    ItemWidget * element = nullptr;

    for (auto item1 : GraphWidget::allChildrenItems(v))
    {
        ItemWidget * item2 = element;

        if (item1->getLayer()==-1)
        {
            if (element==nullptr)
                element = item1;

            else if ( item1->getLinks().count() > item2->getLinks().count() )
            {
                element = item1;
            }
        }
    }

    return element;
}

void Arrange::assignLayerToItemsRecursively(QList<ItemWidget *> layerZeroItems)
{
    for ( auto r0item :  layerZeroItems )
    {
        r0item->assignLayerRecursively(0);
    }
}

QList<ItemWidget *> Arrange::getLayerZeroItems(QWidget *v)
{
    QList<ItemWidget *> layerZeroItems;

    bool automatic = true;

    for (auto item : GraphWidget::allChildrenItems(v))
    {
        if (item->getBelongsToLayerZero())
        {
            automatic = false;
            layerZeroItems += item;
            item->assignLayerRecursively(0);
        }
    }

    if (automatic)
    {
        // Set arrange layer to -1 for everyone:
        resetAllLayerAndSortPositionInfo(v);

        ItemWidget * item;
        while ((item=getUnarrangedItemWithMaxLinks(v)))
        {
            item->assignLayerRecursively(0);
        }

        for (auto chItem : GraphWidget::allChildrenItems(v))
        {
            if (chItem->getLayer()==0)
                layerZeroItems.append(chItem);
        }
    }

    return layerZeroItems;
}

int Arrange::getLayerCount(QWidget *v)
{
    int layerCount=0;

    for (auto item : GraphWidget::allChildrenItems(v))
    {
        if ((item->getLayer()+1)>layerCount)
            layerCount = (item->getLayer()+1);
    }

    return layerCount;
}

QList<ItemWidget *> Arrange::getItemsFromLayer(QWidget *v, int layer)
{
    QList<ItemWidget *> items;

    for (auto item : GraphWidget::allChildrenItems(v))
    {
        if (item->getLayer()==layer)
            items.append(item);
    }

    return items;
}

int Arrange::horizontalTree(QWidget *v, QList<ItemWidget *> layerZero)
{
    resetAllLayerAndSortPositionInfo(v); // Set sorting layer to -1 for everyone
    assignLayerToItemsRecursively(layerZero);

    int layerCount = getLayerCount(v);
    int prevLayerItemsCount = -1;

    // Determine horizontal spacing
    int horizontalSpacing = v->size().width()/(layerCount+1);

    for (int layer=0; layer<layerCount; layer++)
    {
        // Setup each layer.
        QList<ItemWidget *> layerItems = getItemsFromLayer(v,layer);

        // Determine how many slots there are in the layer
        int layerSlots = layerItems.count();
        if (prevLayerItemsCount>0 && layerItems.count()<=prevLayerItemsCount)
            layerSlots = prevLayerItemsCount+1;

        // Determine vertical spacing
        int verticalSpacing = v->size().height()/(layerSlots+1);

        // Configure weight for each element
        for ( int i = 0; i < layerItems.size(); i++ )
        {
            ItemWidget * item = layerItems[i];

            if (layer==0)
                // Configure original weightiness for layer 0
                item->setSortPosition( (i+1)*(verticalSpacing) );
            else
                // Determine weightiness for layer >1
                item->calcSortPosition();
        }

        // Sorted items...
        QVector<ItemWidget *> layerSortedSlots(layerSlots);
        // nullptr-Initialize vector
        for ( int i = 0; i < layerSlots; i++ )
        {
            layerSortedSlots[i]=nullptr;
        }

        // Fill slots with items
        for ( int i = 0; i < layerItems.size(); i++ )
        {
            ItemWidget * item = layerItems[i];

            int bestPosition = (item->getSortPosition()/verticalSpacing)-1;

            // bestPosition is an heuristic, therefore, adjust into his borders
            if (bestPosition>=layerSlots)
                bestPosition = layerSlots-1;
            if (bestPosition<0)
                bestPosition = 0;

            // Fit
            int diff_p = 0, diff_m = 0;
            while ( layerSortedSlots[bestPosition+diff_p] && layerSortedSlots[bestPosition-diff_m] )
            {
                if (bestPosition-diff_m>0)
                    diff_m++;
                if (bestPosition+diff_p<(layerSlots-1))
                    diff_p++;
            }
            if (layerSortedSlots[bestPosition-diff_m]==nullptr)
                bestPosition = bestPosition-diff_m;
            else if (layerSortedSlots[bestPosition+diff_p]==nullptr)
                bestPosition = bestPosition+diff_p;

            // Final best position
            layerSortedSlots[bestPosition] = item;
        }

        // The final move
        for ( int i = 0; i < layerSlots; i++ )
        {
            ItemWidget * item = layerSortedSlots[i];
            if (item)
            {
                item->setSortPosition(verticalSpacing*(i+1));
                if (!item->getAnchor())
                    item->move(horizontalSpacing*(layer+1) - (item->size().width()/2) , verticalSpacing*(i+1));
            }
        }

        prevLayerItemsCount = layerItems.count();
    }
    return 0;
}

int Arrange::verticalTree(QWidget *v, QList<ItemWidget *> layerZero)
{
    resetAllLayerAndSortPositionInfo(v); // Set sorting layer to -1 for everyone
    assignLayerToItemsRecursively(layerZero);

    int layerCount = getLayerCount(v);
    int prevLayerItemsCount = -1;

    // Determine horizontal spacing
    int verticalSpacing = v->size().height()/(layerCount+1);

    for (int layer=0; layer<layerCount; layer++)
    {
        // Setup each layer.
        QList<ItemWidget *> layerItems = getItemsFromLayer(v,layer);

        // Determine how many slots there are in the layer
        int layerSlots = layerItems.count();
        if (prevLayerItemsCount>0 && layerItems.count()<=prevLayerItemsCount)
            layerSlots = prevLayerItemsCount+1;

        // Determine horiz spacing
        int horizontalSpacing = v->size().width()/(layerSlots+1);

        // Configure weightiness
        for ( int i = 0; i < layerItems.size(); i++ )
        {
            ItemWidget * item = layerItems[i];

            if (layer==0)
                // Configure original weightiness for layer 0
                item->setSortPosition( (i+1)*(horizontalSpacing) );
            else
                // Determine weightiness for layer >1
                item->calcSortPosition();
        }

        // Sorted items...
        QVector<ItemWidget *> layerSortedSlots(layerSlots);
        // Initialize vector with nullptr
        for ( int i = 0; i < layerSlots; i++ )
        {
            layerSortedSlots[i]=nullptr;
        }

        // Fill slots with items
        for ( int i = 0; i < layerItems.size(); i++ )
        {
            ItemWidget * item = layerItems[i];

            int bestPosition = (item->getSortPosition()/horizontalSpacing)-1;

            // bestPosition is an heuristic, therefore, adjust into his borders
            if (bestPosition>=layerSlots)
                bestPosition = layerSlots-1;
            if (bestPosition<0)
                bestPosition = 0;

            // Fit
            int diff_p = 0, diff_m = 0;
            while ( layerSortedSlots[bestPosition+diff_p] && layerSortedSlots[bestPosition-diff_m] )
            {
                if (bestPosition-diff_m>0)
                    diff_m++;
                if (bestPosition+diff_p<(layerSlots-1))
                    diff_p++;
            }
            if (layerSortedSlots[bestPosition-diff_m]==nullptr)
                bestPosition = bestPosition-diff_m;
            else if (layerSortedSlots[bestPosition+diff_p]==nullptr)
                bestPosition = bestPosition+diff_p;

            // Final best position
            layerSortedSlots[bestPosition] = item;
        }

        // The final move
        for ( int i = 0; i < layerSlots; i++ )
        {
            ItemWidget * item = layerSortedSlots[i];
            if (item)
            {
                item->setSortPosition(horizontalSpacing*(i+1));
                if (!item->getAnchor())
                    item->move(   horizontalSpacing*(i+1), verticalSpacing*(layer+1) - (item->size().height()/2) );
            }
        }

        prevLayerItemsCount = layerItems.count();
    }
    return 0;
}

int Arrange::star(QWidget *v, QList<ItemWidget *> layerZero)
{
    resetAllLayerAndSortPositionInfo(v); // Set sorting layer to -1 for everyone
    assignLayerToItemsRecursively(layerZero);

    int prevLayerItemsCount = -1;
    int layerCount = getLayerCount(v);

    if (!layerCount)
        return -1;

    // Determine horizontal spacing
    int horizontalSpacing = (v->size().width()>v->size().height() ? v->size().height()-100 : v->size().width()-100 ) / (layerCount);

    for (int layer=0; layer<layerCount; layer++)
    {
        // Setup each layer.
        QList<ItemWidget *> layerItems = getItemsFromLayer(v,layer);

        // Determine how many slots there are in the layer
        int layerSlots = layerItems.count();
        if (prevLayerItemsCount>0 && layerItems.count()<=prevLayerItemsCount) layerSlots = prevLayerItemsCount+1;

        // Determine vertical spacing
        double verticalSpacing = 360/((double)layerSlots);

        // Configure weightiness
        for ( int i = 0; i < layerItems.size(); i++ )
        {
            ItemWidget * item = layerItems[i];
            if (layer==0)
                // Configure original weightiness for layer 0
                item->setSortPosition( i*(verticalSpacing) );
            else
                // Determine weightiness for layer >1
                item->calcSortPosition();
        }

        // Sorted items...
        QVector<ItemWidget *> layerSortedSlots(layerSlots);
        // nullptr-Initialize vector
        for ( int i = 0; i < layerSlots; i++ ) {   layerSortedSlots[i]=nullptr;    }

        // Fill slots with items
        for ( int i = 0; i < layerItems.size(); i++ )
        {
            ItemWidget * item = layerItems[i];

            int bestPosition = (item->getSortPosition()/verticalSpacing)-1;

            // bestPosition is an heuristic, therefore, adjust into his borders
            if (bestPosition>=layerSlots)
                bestPosition = layerSlots-1;
            if (bestPosition<0)
                bestPosition = 0;

            // Fit
            int diff_p = 0, diff_m = 0;
            while ( layerSortedSlots[bestPosition+diff_p] && layerSortedSlots[bestPosition-diff_m] )
            {
                if (bestPosition-diff_m>0)
                    diff_m++;
                if (bestPosition+diff_p<(layerSlots-1))
                    diff_p++;
            }
            if (layerSortedSlots[bestPosition-diff_m]==nullptr)
                bestPosition = bestPosition-diff_m;
            else if (layerSortedSlots[bestPosition+diff_p]==nullptr)
                bestPosition = bestPosition+diff_p;

            // Final optimus position
            layerSortedSlots[bestPosition] = item;
        }

        // The final move
        for ( double i = 0; i < layerSlots; i=i+1.0 )
        {
            ItemWidget * item = layerSortedSlots[i];
            if (item)
            {
                item->setSortPosition(verticalSpacing*i);

                int r = (horizontalSpacing*(layer+1))/2;
                if (layerSlots==1)
                    r = 0;
                double degrees = verticalSpacing*i;
                double radians = (((double)degrees)/180.0)*PI;

                int x = (v->size().width()/2);
                int xplus = ((double)r)*(cos(radians));
                x = x + xplus;

                int y = (v->size().height()/2) - ((double)r)*(sin(radians)) ;

                if (!item->getAnchor())
                    item->move(x - (item->size().width()/2) ,y);
            }
        }
        prevLayerItemsCount = layerItems.count();
    }
    return 0;
}

int Arrange::random(QWidget *v)
{
    for (auto item : GraphWidget::allChildrenItemsAndGroups(v))
    {
        item->moveToRandom();
    }

    return 0;
}

int Arrange::rowsMover(QWidget *v, int spacing, SortBy sortBy, bool group, XY * accumulated)
{
    QList<AbstractNodeWidget *> nodes;
    if (group)
    {
        for (auto i : GraphWidget::allChildrenGroups(v))
            nodes.push_back(i);
    }
    else
    {
        for (auto i : GraphWidget::allChildrenItems(v))
            nodes.push_back(i);
    }

    switch (sortBy)
    {
    case SORTBY_INSERT_POS:
        break;
    case SORTBY_OBJECT_ID:
        std::sort(nodes.begin(), nodes.end(), [](const AbstractNodeWidget* a, const AbstractNodeWidget* b) -> bool { return a->getID() < b->getID(); });
        break;
    case SORTBY_TEXT:
        std::sort(nodes.begin(), nodes.end(), [](const AbstractNodeWidget* a, const AbstractNodeWidget* b) -> bool { return a->getText() < b->getText(); });
        break;
    case SORTBY_SUBTEXT:
        std::sort(nodes.begin(), nodes.end(), [](const AbstractNodeWidget* a, const AbstractNodeWidget* b) -> bool { return a->getSubText() < b->getSubText(); });
        break;
    default:
        return -1;
    }

    // First draw groups...
    int currentRow = 0;
    int nodesAtRow = 0;
    int lastMaxY=0;

    accumulated->x = 0;

    for ( auto node : nodes )
    {
rback_sort_rows:
        auto gsize = node->size();

        XY currentPos = *accumulated;
        currentPos.x+=spacing;

        // X:
        if (accumulated->x+gsize.width()+(spacing*2)>(v->size().width()) )
        {
            if (nodesAtRow == 0)
            {
                // not enough room for x, expand x...
                v->resize(accumulated->x+gsize.width()+(spacing*2),
                          v->size().height());
            }
            else
            {
                // reset and move to the next row:
                nodesAtRow = 0;
                currentRow++;
                accumulated->x=0;
                accumulated->y+=lastMaxY+spacing;

                // reset and size...
                v->resize(  v->size().width(),
                            accumulated->y );

                goto rback_sort_rows; // now we are in the next row... try with the same element...
            }
        }

        accumulated->x+=spacing+gsize.width();
        lastMaxY = std::max(lastMaxY,gsize.height());
        nodesAtRow++;

        node->move({currentPos.x,currentPos.y});
    }

    accumulated->y+=lastMaxY+spacing;

    v->resize(v->size().width(),
              accumulated->y);

    return 0;
}
int Arrange::rows(QWidget *v, int spacing, SortBy sortBy)
{
    for (auto i : GraphWidget::allChildrenItemsAndGroups(v))
    {
        // Clean the space from elements (avoid unintented intersections):
        i->move({-10000,-10000});
    }

    XY accumulated;
    accumulated.y = v->objectName().startsWith("GROUP-")? ((AbstractNodeWidget *)v)->getVerticalOffset() : 0;

    rowsMover(v,spacing,sortBy,true,&accumulated);
    rowsMover(v,spacing,sortBy,false,&accumulated);

    return 0;
}

int Arrange::columnsMover(QWidget *v, int spacing, SortBy sortBy, bool group,XY * accumulated)
{
    QList<AbstractNodeWidget *> nodes;
    if (group)
    {
        for (auto i : GraphWidget::allChildrenGroups(v))
            nodes.push_back(i);
    }
    else
    {
        for (auto i : GraphWidget::allChildrenItems(v))
            nodes.push_back(i);
    }

    switch (sortBy)
    {
    case SORTBY_INSERT_POS:
        break;
    case SORTBY_OBJECT_ID:
        std::sort(nodes.begin(), nodes.end(), [](const AbstractNodeWidget* a, const AbstractNodeWidget* b) -> bool { return a->getID() < b->getID(); });
        break;
    case SORTBY_TEXT:
        std::sort(nodes.begin(), nodes.end(), [](const AbstractNodeWidget* a, const AbstractNodeWidget* b) -> bool { return a->getText() < b->getText(); });
        break;
    case SORTBY_SUBTEXT:
        std::sort(nodes.begin(), nodes.end(), [](const AbstractNodeWidget* a, const AbstractNodeWidget* b) -> bool { return a->getSubText() < b->getSubText(); });
        break;
    default:
        return -1;
    }
    int pVerticalOffset = v->objectName().startsWith("GROUP-")? ((AbstractNodeWidget *)v)->getVerticalOffset() : 0;

    // First draw groups...
    int currentColumn = 0;
    int nodesAtColumn = 0;
    int lastMaxX=0;

    accumulated->y = pVerticalOffset;

    for ( auto node : nodes )
    {
rback_sort_columns:
        auto gsize = node->size();

        XY currentPos = *accumulated;
        currentPos.y+=spacing;

        // Y:
        if (accumulated->y+gsize.height()+(spacing*2)>(v->size().height()) )
        {
            if (nodesAtColumn == 0)
            {
                // expand y...
                v->resize(
                          v->size().width(),
                          accumulated->y+gsize.height()+(spacing*2)
                          );
            }
            else
            {
                // reset:
                nodesAtColumn = 0;
                currentColumn++;
                accumulated->y=pVerticalOffset;
                accumulated->x+=spacing+lastMaxX;

                // expand x...
                v->resize(
                          accumulated->x,
                          v->size().height()
                          );

                goto rback_sort_columns; // now we are in the next row... try with the same element...
            }
        }

        accumulated->y+=spacing+gsize.height();
        lastMaxX = std::max(lastMaxX,gsize.width());
        nodesAtColumn++;

        node->move({currentPos.x,currentPos.y});
    }

    accumulated->x+=lastMaxX+spacing;

    v->resize(accumulated->x,
              v->size().height() );

    return 0;
}

int Arrange::columns(QWidget *v, int spacing, SortBy sortBy)
{
    for (auto i : GraphWidget::allChildrenItemsAndGroups(v))
    {
        // Clean the space from elements (avoid unintented intersections):
        i->move({-10000,-10000});
    }

    XY accumulated;
    accumulated.y = v->objectName().startsWith("GROUP-")? ((AbstractNodeWidget *)v)->getVerticalOffset() : 0;

    columnsMover(v,spacing,sortBy,true,&accumulated);
    columnsMover(v,spacing,sortBy,false,&accumulated);
    return 0;
}
