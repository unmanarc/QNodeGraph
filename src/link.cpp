#include "link.h"
#include "itemwidget.h"
#include <cmath>

#include <QDebug>
using namespace QNodeGraph;

Link::Link()
{
    linkType = TYPE_DIRECTED;
    arcDirection = DIR_BOTH;
}

Link::~Link()
{
}

bool Link::operator ==( const Link & other) const
{
    if ( other.item1 == item1 &&  other.item2 == item2 )
        return true;
    else if ( other.item1 == item2 &&  other.item2 == item1 )
        return true;
    else
        return false;
}

void Link::paint(QPainter &painter, const QColor & backgroundColor)
{
    ItemWidget * item1 = (ItemWidget *) this->getItem1();
    ItemWidget * item2 = (ItemWidget *) this->getItem2();

    QColor linkColor = this->getColor();

    if (linkColor==backgroundColor)
        linkColor = QColor(255-backgroundColor.red(),
                            255-backgroundColor.green(),
                            255-backgroundColor.blue());


    double mytransp = (item1->getZoomOutFactor()<item2->getZoomOutFactor() ? item2->getZoomOutFactor() : item1->getZoomOutFactor());

    if (item1->getIsSelected() || item2->getIsSelected() )
    {
        // Don't reduce...
    }
    else if (item1->getCurrentFilterMatch() || item2->getCurrentFilterMatch())
    {
        // Don't reduce...
    }
    else
        linkColor.setAlphaF(mytransp);

    QPen lpen;
    lpen.setColor(linkColor);
    if (item1->getIsSelected() || item2->getIsSelected() || (item1->getCurrentFilterMatch() || item2->getCurrentFilterMatch()))
    {
        int r = linkColor.red()+80;
        int g = linkColor.green()+80;
        int b = linkColor.blue()+80;
        if (r>255) r = 255;
        if (g>255) g = 255;
        if (b>255) b = 255;
        lpen.setColor(QColor(r,g,b));
        lpen.setWidth(3);
    }

    painter.setPen(lpen);

    QPoint element1Pos = item1->getAbsolutePos() + item1->getCenterPoint();
    QPoint element2Pos = item2->getAbsolutePos() + item2->getCenterPoint();

    // arrow size:
    auto hyp = 10;
    auto beta = PI/7;


    if (linkType == TYPE_DIRECTED)
    {
        if (1)
        {
            int a = element1Pos.y()-element2Pos.y();
            int b = element1Pos.x()-element2Pos.x();

            auto alpha = atan( (double)a/(double)b );
            if (b>=0) alpha+=PI;

            auto R = item1->getExternalRadius();
            auto x = R*cos(alpha);
            auto y = R*sin(alpha);

            element1Pos.setX( element1Pos.x()+x );
            element1Pos.setY( element1Pos.y()+y );

            if (arcDirection == DIR_BOTH || arcDirection == DIR_REV)
            {
                // Draw the arrow.
                QPoint arrowPointBase = element1Pos;
                arrowPointBase.setX( arrowPointBase.x()+(hyp*cos(alpha)) );
                arrowPointBase.setY( arrowPointBase.y()+(hyp*sin(alpha)) );

                auto adj = (double)hyp/cos(beta);

                QPoint arrowPointRight = element1Pos;
                arrowPointRight.setX( arrowPointRight.x()+(adj*cos(alpha-beta)) );
                arrowPointRight.setY( arrowPointRight.y()+(adj*sin(alpha-beta)) );

                // Part 2:
                QPoint arrowPointLeft = element1Pos;
                arrowPointLeft.setX( arrowPointLeft.x()-(adj*cos(PI-alpha-beta)) );
                arrowPointLeft.setY( arrowPointLeft.y()+(adj*sin(PI-alpha-beta)) );

                QPolygon arrow1,arrow2;
                arrow1 << element1Pos << arrowPointBase << arrowPointRight;
                arrow2 << element1Pos << arrowPointBase << arrowPointLeft;
                painter.setBrush(lpen.color());
                painter.drawPolygon(arrow1);
                painter.drawPolygon(arrow2);
                painter.setBrush(Qt::transparent);
            }
        }

        if (1)
        {
            int a = element2Pos.y()-element1Pos.y();
            int b = element2Pos.x()-element1Pos.x();

            auto alpha = atan( (double)a/(double)b )+PI;
            if (b<0) alpha+=PI;

            auto R = item2->getExternalRadius();
            auto x = R*cos(alpha);
            auto y = R*sin(alpha);

            element2Pos.setX( element2Pos.x()+x );
            element2Pos.setY( element2Pos.y()+y );

            if (arcDirection == DIR_BOTH || arcDirection == DIR_FWD)
            {
                // Draw the arrow.

                QPoint arrowPointBase = element2Pos;
                arrowPointBase.setX( arrowPointBase.x()+(hyp*cos(alpha)) );
                arrowPointBase.setY( arrowPointBase.y()+(hyp*sin(alpha)) );

                auto adj = (double)hyp/cos(beta);

                QPoint arrowPointRight = element2Pos;
                arrowPointRight.setX( arrowPointRight.x()+(adj*cos(alpha-beta)) );
                arrowPointRight.setY( arrowPointRight.y()+(adj*sin(alpha-beta)) );

                // Part 2:
                QPoint arrowPointLeft = element2Pos;
                arrowPointLeft.setX( arrowPointLeft.x()-(adj*cos(PI-alpha-beta)) );
                arrowPointLeft.setY( arrowPointLeft.y()+(adj*sin(PI-alpha-beta)) );

                QPolygon arrow1,arrow2;
                arrow1 << element2Pos << arrowPointBase << arrowPointRight;
                arrow2 << element2Pos << arrowPointBase << arrowPointLeft;
                painter.setBrush(lpen.color());
                painter.drawPolygon(arrow1);
                painter.drawPolygon(arrow2);
                painter.setBrush(Qt::transparent);
            }
        }
    }
    else
    {
        element1Pos = item1->getAbsolutePos() + item1->getIconCenterPoint();
        element2Pos = item2->getAbsolutePos() + item2->getIconCenterPoint();
    }

    painter.drawLine(
                element1Pos,
                element2Pos
                );
}

void Link::setItems(void * item1, void * item2)
{
    this->item1=item1;
    this->item2=item2;
}

void Link::setDescription(const QString &description)
{
    this->description = description;
}

void Link::setType(const Type &type)
{
    this->linkType = type;
}

QString Link::getDescription()
{
    return this->description;
}

Link::Type Link::getType()
{
    return this->linkType;
}

void * Link::getItem1()
{
    return this->item1;
}

void * Link::getItem2()
{
    return this->item2;
}

void Link::setColor(const QColor &color )
{
    this->color = color;
}

QColor Link::getColor()
{
    return this->color;
}

Link::Direction Link::getArcDirection() const
{
    return arcDirection;
}

void Link::setArcDirection(Direction newArcDirection)
{
    arcDirection = newArcDirection;
}
