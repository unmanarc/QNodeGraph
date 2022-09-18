#ifndef QNODELINKWIDGET_H
#define QNODELINKWIDGET_H

#include <QColor>
#include <QString>
#include <QPainter>

// TODO: link weight

namespace QNodeGraph
{

class Link
{
public:
    Link();
    virtual ~Link();

    enum Type { TYPE_UNDIRECTED=0, TYPE_DIRECTED=1 };
    enum Direction { DIR_FWD=1, DIR_REV=2, DIR_BOTH=3 };

    bool operator==( const Link &other) const;

    /**
     * @brief paint paint this link into the painter object
     * @param painter painter object
     * @param backgroundColor background color (to avoid non-visible links)
     */
    void paint(QPainter & painter, const QColor &backgroundColor);

    // Items:
    /**
     * @brief setItems Set Pair of Items
     * @param item1 first item
     * @param item2 second item
     */
    void setItems(void * item1, void *item2);
    /**
     * @brief getItem1 Get First Item
     * @return first item
     */
    void * getItem1();
    /**
     * @brief getItem2 Get Second Item
     * @return second item
     */
    void * getItem2();

    // Description
    /**
     * @brief setDescription Set link description
     * @param description link description
     */
    void setDescription(const QString & description);
    /**
     * @brief getDescription Get link description
     * @return link description
     */
    QString getDescription();

    // Type:
    /**
     * @brief setType Set Link type (eg. directed, undirected)
     * @param type type
     */
    void setType(const Type & type);
    /**
     * @brief getType Get link type
     * @return directed/undirected
     */
    Type getType();

    // Color:
    /**
     * @brief setColor Set link color
     * @param color color
     */
    void setColor(const QColor & color);
    /**
     * @brief getColor Get link color
     * @return link color
     */
    QColor getColor();

    // Arc Direction
    /**
     * @brief setArcDirection Set arc direction
     * @param newArcDirection arc direction (fwd 1->2,rev 1<-2, both 1<->2)
     */
    void setArcDirection(Direction newArcDirection);
    /**
     * @brief getArcDirection Get Arc Direction
     * @return arc direction
     */
    Direction getArcDirection() const;

private:
    void * item1;
    void * item2;

    QString description;
    Type linkType;
    Direction arcDirection;
    QColor color;
};

}

#endif
