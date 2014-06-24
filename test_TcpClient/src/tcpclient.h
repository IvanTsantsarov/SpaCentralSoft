#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QHostAddress>
#include <QMainWindow>
#include <QTimer>

#include "structures.h"

class TcpClient : public QObject
{
    Q_OBJECT

private:
    QHostAddress _ipV4Address;
    quint16 _port;
    int _currentSensorIdIndex = 0;
    QList<quint8> _sensorIds;
    quint32 _queryInterval;
    QTcpSocket _socket;
    QMainWindow* _mainWindow;
    QTimer _timer;

public:
    explicit TcpClient(QString ipV4Address,
                       quint16 port,
                       QList<quint8> sensorIds,
                       quint32 queryInterval,
                       QMainWindow* mainWindow);

public:
    void start();

private:
    void showErrorMessage(const QString& errorMessage);

signals:
    void dataRecieved(quint8 sensorId, qreal temperatureCurrent);

public slots:
    void requestData();
    void setData(quint8 sensorId, qreal temperatureDesired);
    void readData();
    void displaySocketError(QAbstractSocket::SocketError socketError);
};

#endif // TCPCLIENT_H
