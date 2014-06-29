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
    QMainWindow* _mainWindow;

    QHostAddress _hostAddress;
    quint16 _port;

    SensorData _sensorData;

public:
    explicit TcpServer(QString ipV4Address, quint16 port, QMainWindow* mainWindow);

    void start();

private:
    void showErrorMessage(const QString& errorMessage);

protected:
    void incomingConnection(qintptr socketDescriptor);

signals:
    void serverStarted(QString ipV4Address, quint16 port);
    void serverError(QString errorMessage);
    void temperatureDesiredChanged(quint8 sensorId, qreal temperatureDesired);
    void temperatureDesiredChanged(SensorData sensorData);
    void commandSent(SensorData sensorData);

public slots:
    void onSensorDataChanged(SensorData& sensorData);

private slots:
    void onSocketError(QTcpSocket::SocketError socketError);
};

#endif // TCPSERVER_H
