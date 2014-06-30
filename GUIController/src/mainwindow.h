#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
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

    ProgramSettings _programSettings;
    QSettings* _programSettingsPersistant;
    QList<IndicatorProperties> _listIndicatorProperties;

    CGraphicsScene* _scene;
    QList<TemperatureIndicator*> _temperatureIndicators;
    TemperatureIndicator* _currentTemperatureIndicator;

    QTcpSocket* _socket = nullptr;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

// GUI
private:
    void showDialogChangeTemperature();

public:
    void selectTemperatureIndicator(QPointF point);
    
private slots:
    void onActionAboutTriggered();
    void onListWidgetItemClicked(QListWidgetItem* item);
    void onListWidgetItemDoubleClicked(QListWidgetItem* item);
    void onTemperatureIndicatorDoubleClicked(QGraphicsSceneMouseEvent* event);

// Socket connection
private:
    void connectSocket();
    void sendTemperatureTarget(TemperatureIndicator* temperatureIndicator);
    void setTemperatureIndicator(quint8 sensorId, qreal temperature);

private slots:
    void onDataRecieved();
    void onDisconnected();
    void onConnected();
    void onErrorSocket(QAbstractSocket::SocketError socketError);
};

#endif // MAINWINDOW_H
