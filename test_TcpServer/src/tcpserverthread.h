#ifndef TCPSERVERTHREAD_H
#define TCPSERVERTHREAD_H

#include <QThread>
#include <QtNetwork/QTcpSocket>

#include "structures.h"

class TcpServerThread : public QThread
{
    Q_OBJECT

private:
    qintptr _socketDescriptor;
    SensorData _sensorData;

    QTcpSocket _tcpSocket;

public:
    explicit TcpServerThread(qintptr socketDescriptor,
                             const SensorData& sensorData,
                             QObject* parent);
    void run();
private:
    void sendTemperatureCurrent();
    void setTemperatureDesired(QByteArray& incommingCommandBlock);

signals:
    void socketError(QTcpSocket::SocketError socketError);
    void temperatureDesiredChanged(quint8 sensorId, qreal temperatureDesired);
    void temperatureDesiredChanged(SensorData sensorData);
    void commandSent(SensorData sensorData);

private slots:
    void onReadyRead();
};

#endif // TCPSERVERTHREAD_H
