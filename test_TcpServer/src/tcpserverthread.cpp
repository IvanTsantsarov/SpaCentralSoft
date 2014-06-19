#include "tcpserverthread.h"

TcpServerThread::TcpServerThread(qintptr socketDescriptor,
                                 const SensorData& sensorData,
                                 QObject* parent) :
    QThread(parent),
    _socketDescriptor(socketDescriptor),
    _sensorData(sensorData)
{
}

void TcpServerThread::run()
{
    QTcpSocket tcpSocket;

    bool isSocketDescriptorSet = tcpSocket.setSocketDescriptor(_socketDescriptor);
    if (!isSocketDescriptorSet) {
        emit socketError(tcpSocket.error());

        return;
    }

    QByteArray dataBlock;

    dataBlock.append(static_cast<char>(_sensorData.byte01));
    dataBlock.append(static_cast<char>(_sensorData.byte02));
    dataBlock.append(static_cast<char>(_sensorData.byte03));
    dataBlock.append(static_cast<char>(_sensorData.byte04));

    qDebug() << dataBlock.at(0);
    qDebug() << static_cast<quint8>(dataBlock.at(1)) + 1;
    qDebug() << static_cast<qint8>(dataBlock.at(2)) + 1;
    qDebug() << static_cast<quint8>(dataBlock.at(3)) + 1;

    tcpSocket.write(dataBlock);
    emit dataSent(_sensorData);
    tcpSocket.disconnectFromHost();
    tcpSocket.waitForDisconnected();
}
