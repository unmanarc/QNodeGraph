#include "xmlfunctions.h"

using namespace QNodeGraph;

XMLFunctions::XMLFunctions()
{
}

QString XMLFunctions::createSimpleTag(const QString &tag, const QString &text)
{
    return QString("<" + tag +  ">%1</" + tag + ">\n").arg(text);
}

QString XMLFunctions::createSimpleTag(const QString &tag, const uint64_t &text)
{
    return QString("<" + tag +  ">%1</" + tag + ">\n").arg(text);
}

QString XMLFunctions::createSimpleTag(const QString &tag, const int64_t &text)
{
    return QString("<" + tag +  ">%1</" + tag + ">\n").arg(text);
}

QString XMLFunctions::createSimpleTag(const QString &tag, const double &text)
{
    return QString("<" + tag +  ">%1</" + tag + ">\n").arg(text);
}

QString XMLFunctions::createSimpleTag(const QString &tag, const bool &text)
{
    return QString("<" + tag +  ">%1</" + tag + ">\n").arg(text?1:0);
}

QString XMLFunctions::createColorXMLTag(const QString &tag, const QColor &color)
{
    return QString("<" + tag +  " r=\"%1\" g=\"%2\" b=\"%3\" a=\"%4\"></" + tag + ">\n").arg(
                color.red(),color.green(),color.blue()).arg(color.alpha());
}

QColor XMLFunctions::colorFromXML(const QDomNode &child)
{
    int a = child.toElement().attribute("a").toUInt();
    int r = child.toElement().attribute("r").toUInt();
    int g = child.toElement().attribute("g").toUInt();
    int b = child.toElement().attribute("b").toUInt();
    return QColor(r,g,b,a);
}
