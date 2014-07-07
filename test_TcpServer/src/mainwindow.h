#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "tcpserver.h"

#include <QMainWindow>
#include <QList>
#include <QString>
#include <QDebug>

#include "structures.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    TcpServer* _tcpServer;
    QList<SensorData>* _sensors;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    void setSensor(const SensorData& sensorData);
    void setLabels(const SensorData& sensorData);

signals:
    void sensorDataChanged(SensorData& sensorData);

private slots:
    void onServerStarted(QString ipV4Address, quint16 port);
    void onTemperatureDesiredChanged(quint8 sensorId, qreal temperatureDesired);
    void onTemperatureDesiredChanged(SensorData sensorData);
    void onCommandSent(SensorData sensorData);

    void onClicked_pushButton_SetData();
    void on_comboBox_Byte01_currentIndexChanged(int index);
};

#endif // MAINWINDOW_H
