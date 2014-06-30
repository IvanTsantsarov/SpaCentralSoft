#ifndef TEMPERATUREINDICATOR_H
#define TEMPERATUREINDICATOR_H

#include <QObject>
#include <QGraphicsTextItem>
#include <QListWidget>

#include "custom/constants.h"
#include "custom/structures.h"
#include "custom/types.h"
#include "cgraphicsscene.h"
#include "cgraphicsrectitem.h"

class CGraphicsScene;

class TemperatureIndicator : public QObject
{
    Q_OBJECT
private:
#ifndef USE_OBSOLETE
    ProgramSettings _programSettings;
    IndicatorProperties _indicatorProperties;
#endif // USE_OBSOLETE

    QString _text;
    QListWidget* _listWidget;
    qreal _temperatureTarget = 17.5;
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

    void updateState();
public:
    explicit TemperatureIndicator(IndicatorProperties indicatorProperties,
                                  ProgramSettings programSettings,
                                  QListWidget* listWidget,
                                  QObject* parent = 0);

    CGraphicsRectItem* getGraphicsRectItem() const;
    QString text() const; 

    quint8 getSensorId();
    bool isSensorFunctional();
    qreal getTemperatureCurrent();
    qreal getTemperatureTarget();

    void setConnected(bool connected);
    void setPosition(QPointF& position);
    void setIndicatorSelected(bool indicatorSelected);
    void setSensorState(SensorState sensorState);
    void setTemperatureTarget(qreal temperature);
    void setTemperatureCurrent(qreal temperature);

    void update();

signals:
    void selected();
    void doubleClicked(QGraphicsSceneMouseEvent* event);

public slots:
    void onSelected();
};

#endif // TEMPERATUREINDICATOR_H
