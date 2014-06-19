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

public:
    explicit TcpServerThread(qintptr socketDescriptor,
                             const SensorData& sensorData,
                             QObject* parent);
    void run();

signals:
    void socketError(QTcpSocket::SocketError socketError);
    void dataSent(SensorData sensorData);

public slots:

};

#endif // TCPSERVERTHREAD_H
