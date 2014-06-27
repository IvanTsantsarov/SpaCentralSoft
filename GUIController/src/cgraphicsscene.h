#ifndef CGRAPHICSSCENE_H
#define CGRAPHICSSCENE_H

#include <QGraphicsScene>

#include "mainwindow.h"

class MainWindow;

class CGraphicsScene : public QGraphicsScene
{
private:
    MainWindow* m_mainWindow;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent);

public:
    CGraphicsScene(MainWindow* mainWindow);
};

#endif // CGRAPHICSSCENE_H
