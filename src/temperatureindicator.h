#ifndef TEMPERATUREINDICATOR_H
#define TEMPERATUREINDICATOR_H

#include <QObject>
#include <QGraphicsTextItem>
#include <QListWidget>

#include "constants.h"
#include "cgraphicsscene.h"
#include "cgraphicsrectitem.h"

class CGraphicsScene;

class TemperatureIndicator : public QObject
{
    Q_OBJECT
private:
    QString _text;
    QListWidget* _listWidget;
    qreal _temperatureDesired = 17.5;
    qreal _temperatureCurrent = 0.0;
    bool _sensorConnected = false;
    bool _indicatorSelected = false;
    SensorState _sensorState = SensorState::Undefined;

    CGraphicsScene* _graphicsScene; 
#ifdef USE_GRAPHICSRECTITEM_ZONE
    QGraphicsRectItem* _graphicsRectItem_Zone;
#else
    QGraphicsPixmapItem* _graphicsPixmapItem;
#endif
    CGraphicsRectItem* _graphicsRectItem;
    QGraphicsTextItem* _graphicsTextItem;

private:

public:
    explicit TemperatureIndicator(QString text, QString imageFileName, QListWidget* listWidget, QObject* parent = 0);

    CGraphicsRectItem* getGraphicsRectItem() const;
    QString text() const; 

    bool isSensorFunctional();
    qreal getTemperatureCurrent();
    qreal getTemperatureDesired();

    void setConnected(bool connected);
    void setPosition(QPointF& position);
    void setIndicatorSelected(bool indicatorSelected);
    void setSensorState(SensorState sensorState);
    bool setTemperatureDesired(qreal temp);
    bool setTemperatureCurrent(qreal temp);

    void update();

signals:
    void selected();
    void doubleClicked(QGraphicsSceneMouseEvent* event);
public slots:
    void onSelected();
};

#endif // TEMPERATUREINDICATOR_H
