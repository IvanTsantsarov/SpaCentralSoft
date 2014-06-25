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

private:
    TcpClient* _tcpClient;
    QList<quint8> _sensorIds;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

private slots:
    void setTemperatureDesired();
    void onDataReceived(quint8 sensorId, qreal temperatureCurrent);
};

#endif // MAINWINDOW_H
