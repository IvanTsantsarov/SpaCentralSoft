#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "tcpserver.h"

#include <QMainWindow>
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
    SensorData _sensorData;
    TcpServer* _tcpServer;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

signals:
    void sensorDataChanged(SensorData& sensorData);

private slots:
    void onClicked_pushButton_SetData();
    void onServerStarted(QString ipV4Address, quint16 port);
    void onDataSent(SensorData sensorData);
};

#endif // MAINWINDOW_H
