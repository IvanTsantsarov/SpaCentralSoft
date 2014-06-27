#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "tcpclient.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    struct Sensor
    {
        SensorData sensorData;
        qreal tempetaruteDesired;

        Sensor(SensorData sensorData, qreal temperatureDesired)
        {
            this->sensorData = sensorData;
            this->tempetaruteDesired = temperatureDesired;
        }
    };

private:
    TcpClient* _tcpClient;
    QList<quint8> _sensorIds;
    QList<Sensor> _sensors;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void setSensor(SensorData sensorData);

private:
    Ui::MainWindow *ui;

private slots:
    void setTemperatureDesired();
    void onDataReceived(quint8 sensorId, qreal temperatureCurrent);
    void onDataReceived(SensorData sensorData);
    void on_comboBox_SensorId_currentIndexChanged(int index);
};

#endif // MAINWINDOW_H
