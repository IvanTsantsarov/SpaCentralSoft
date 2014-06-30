#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <QString>
#include <QPointF>
#include <QFont>

struct ProgramSettings
{
    struct Server
    {
        QString ipV4Address;
        int port;
    } server;

    QString language;
    QString assetsPath;
    QString backgroundImageFileName;
    qreal highlightOpacity;

    struct IndicatorProperties
    {
        qreal opacity;
        int borderwidth;
        QString bordercolor;
        QString fontFamily;
        int     fontSize;
        QString fontWeight;
        QString fontColor;
        QFont   font;
        qreal rectPadding;
        int alphaSelected;
        int alphaDisselected;
    } indicator;
};

struct IndicatorProperties
{
    quint8 sensorId;
    QString text;
    QString highlightImageFileName;
    QPointF position;
    qreal temperatureTarget;
};

#endif // STRUCTURES_H
