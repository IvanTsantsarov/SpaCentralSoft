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
    Ui::MainWindow *ui;

    TcpServer* _tcpServer;
    QList<SensorData>* _sensors;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void setDataFromInput(SensorData& sensorData);
    void setSensorInput(const SensorData& sensorData);
    void updateSensorInput();

signals:
    void dataSet(SensorData sensorData);

private slots:
    void onCommandSent(SensorData sensorData);
    void onSensorDataChanged();
    void onSensorSet(SensorData sensorData);
    void onServerStarted(QString ipV4Address, quint16 port);

    void onClicked_pushButton_SetData();
    void on_comboBox_Byte01_currentIndexChanged(int index);
};

#endif // MAINWINDOW_H
