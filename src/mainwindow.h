#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "cgraphicsscene.h"
#include "temperatureindicator.h"

namespace Ui {
    class MainWindow;
}

class CGraphicsScene;
class TemperatureIndicator;

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow* ui;
    CGraphicsScene* _scene;
    QList<TemperatureIndicator*> _temperatureIndicators;
    TemperatureIndicator* _currentTemperatureIndicator;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void showDialogChangeTemperature();

public:
    void selectTemperatureIndicator(QPointF point);
    
private slots:
    void onActionAboutTriggered();
    void onListWidgetItemClicked(QListWidgetItem* item);
    void onListWidgetItemDoubleClicked(QListWidgetItem* item);
    void onTemperatureIndicatorDoubleClicked(QGraphicsSceneMouseEvent* event);

};

#endif // MAINWINDOW_H
