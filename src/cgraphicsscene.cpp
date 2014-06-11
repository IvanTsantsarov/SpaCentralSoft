#include "cgraphicsscene.h"

#include <QGraphicsSceneMouseEvent>

#include <QDebug>

CGraphicsScene::CGraphicsScene(MainWindow* mainWindow)
{
    m_mainWindow = mainWindow;
}

void CGraphicsScene::mousePressEvent (QGraphicsSceneMouseEvent* mouseEvent)
{
    QPointF point = mouseEvent->scenePos();
    m_mainWindow->selectTemperatureIndicator(point);
    QGraphicsScene::mousePressEvent(mouseEvent);
}

void CGraphicsScene::mouseReleaseEvent (QGraphicsSceneMouseEvent* mouseEvent)
{
    QPointF point = mouseEvent->scenePos();
    qDebug() << "Mouse release event at x: " << point.x() << " y: " << point.y();
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}
