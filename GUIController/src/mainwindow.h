#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork/QTcpSocket>

#include "configloader.h"
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

    QTcpSocket _socket;

    ProgramSettings _programSettings;
    QList<IndicatorProperties> _listIndicatorProperties;

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

    void onDataRecieved();
    void onDisconnected();
};

#endif // MAINWINDOW_H
