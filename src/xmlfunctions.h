#ifndef XMLFUNCTIONS_H
#define XMLFUNCTIONS_H

#include <QString>
#include <QDomNode>
#include <QColor>

namespace QNodeGraph
{
class XMLFunctions
{
public:
    XMLFunctions();

    /**
     * @brief createSimpleTag Create simple XML entity from qstring
     * @param tag tag name
     * @param text qstring
     * @return XML entity
     */
    static QString createSimpleTag(const QString &tag, const QString &text);
    /**
     * @brief createSimpleTag Create simple XML entity from uint64_t
     * @param tag tag name
     * @param text uint64_t
     * @return XML entity
     */
    static QString createSimpleTag(const QString &tag, const uint64_t &text);
    /**
     * @brief createSimpleTag Create simple XML entity from int64_t
     * @param tag tag name
     * @param text int64_t
     * @return XML entity
     */
    static QString createSimpleTag(const QString &tag, const int64_t &text);
    /**
     * @brief createSimpleTag Create simple XML entity from double
     * @param tag tag name
     * @param text double
     * @return XML entity
     */
    static QString createSimpleTag(const QString &tag, const double &text);
    /**
     * @brief createSimpleTag Create simple XML entity from bool
     * @param tag tag name
     * @param text bool
     * @return XML entity
     */
    static QString createSimpleTag(const QString &tag, const bool &text);
    /**
     * @brief createSimpleTag Create simple XML entity from QColor
     * @param tag tag name
     * @param color QColor
     * @return XML entity
     */
    static QString createColorXMLTag(const QString &tag, const QColor &color);

    /**
     * @brief colorFromXML Create color from XML entity
     * @param child XML dom node entity
     * @return QColor type color
     */
    static QColor colorFromXML(const QDomNode &child);

};

}
#endif // XMLFUNCTIONS_H
