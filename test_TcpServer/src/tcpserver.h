#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QMainWindow>
#include <QThreadPool>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QTcpSocket>

#include "structures.h"

class TcpServer : public QTcpServer
{
    Q_OBJECT
private:
    QHostAddress _hostAddress;
    quint16 _port;

    QList<SensorData> _sensors;

    QMainWindow* _mainWindow;

    SensorData _sensorData;

public:
    explicit TcpServer(QString ipV4Address,
                       quint16 port,
                       QMainWindow* mainWindow);

    void start();

private:
    void showErrorMessage(const QString& errorMessage);
    void setSensor(const SensorData& sensorData);

protected:
    void incomingConnection(qintptr socketDescriptor);

public:
    QList<SensorData>* getSensors();

signals:
    void commandSent(SensorData sensorData);
    void sensorDataChanged();
    void serverError(QString errorMessage);
    void serverStarted(QString ipV4Address, quint16 port);
    void sensorSet(SensorData sensorData);

public slots:
    void onSensorSet(SensorData sensorData);

private slots:
    void onSocketError(QTcpSocket::SocketError socketError);
};

#endif // TCPSERVER_H
