#ifndef GROUPWIDGET_H
#define GROUPWIDGET_H

#include "abstractnodewidget.h"
#include "itemwidget.h"

namespace QNodeGraph
{

class GroupWidget : public AbstractNodeWidget
{
    Q_OBJECT
public:
    /**
     * @brief GroupWidget Group Node Widget Constructor
     * @param parent parent graph
     * @param xml XML to set
     */
    GroupWidget(QWidget *parent, const QString &xml);
    /**
     * @brief GroupWidget Group Node Widget Constructor
     * @param id node identifier
     * @param size element size
     * @param parent parent graph
     */
    GroupWidget(const QString &id, QSize size, QWidget *parent);
    /**
     * @brief GroupWidget
     * @param id node identifier
     * @param size element size
     * @param parent parent graph
     * @param text title text
     * @param subtext subtitle text
     */
    GroupWidget(const QString &id, QSize size, QWidget *parent, const QString &text, const QString & subtext);

    /**
     * @brief ~GroupWidget virtual Destructor
     */
    virtual ~GroupWidget();

    /**
     * @brief allRecursiveItems Get all recursive items
     * @return all children items (recursively)
     */
    QList<ItemWidget *> allRecursiveItems();
    /**
     * @brief selectAllItems Select all children items.
     */
    void selectAllItems();

    /**
     * @brief setTextAlignFlags Set text align flags from Qt
     * @param newTextAlignFlags align flags (eg. Qt::AlignCenter)
     */
    void setTextAlignFlags(int newTextAlignFlags);
    /**
     * @brief setSubTextAlignFlags Set subtext align flags from Qt
     * @param newSubTextAlignFlags align flags (eg. Qt::AlignCenter)
     */
    void setSubTextAlignFlags(int newSubTextAlignFlags);

    /**
     * @brief getResizable Get if manual resizing is allowed
     * @return true if manual resizing is allowed.
     */
    bool getResizable() const;
    /**
     * @brief setResizable Set for manual resizing allowed
     * @param newResizable true to allow manual resizing, otherwise false
     */
    void setResizable(bool newResizable);

    /**
     * @brief getAutoArrange Get if auto arrange is enabled
     * @return true for auto arrange
     */
    bool getAutoArrange() const;
    /**
     * @brief setAutoArrange Set auto arrange
     * @param newAutoArrange true to enable, false to disable
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
     * @brief getTitleBackgroundColor Get Group Title background Color
     * @return color
     */
    const QColor &getTitleBackgroundColor() const;
    /**
     * @brief setTitleBackgroundColor Set title background color
     * @param newTitleColor color
     */
    void setTitleBackgroundColor(const QColor &newTitleColor);

protected slots:
    void mouseMoveEvent(QMouseEvent * event);
    void mousePressEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
    virtual void paintEvent( QPaintEvent* );

protected:

    void localInit();

    QString getXMLLocal();
    QString getXMLLocalProperties();

    bool setXMLLocalProperties(const QDomNode &child);
    bool setXMLLocal(const QDomNode & child);

    void recalculateSize();
    void setInternalObjectID();

private:
    int textAlignFlags,subTextAlignFlags;

    // Resize Process:
    bool resizable;
    bool resizingX, resizingY;

    bool autoArrange;
    int autoArrangeAlgorithm, sortBy, autoArrangeSpacing;

    QPoint mouseCurrentPos;
    QColor titleBackgroundColor;

};

}

#endif
