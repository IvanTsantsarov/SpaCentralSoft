#include "tcpserverthread.h"

TcpServerThread::TcpServerThread(qintptr socketDescriptor,
                                 const SensorData& sensorData,
                                 QObject* parent) :
    QThread(parent),
    _socketDescriptor(socketDescriptor),
    _sensorData(sensorData)
{
    bool isOk;
    isOk = connect(&_tcpSocket, SIGNAL(readyRead()),
                   this, SLOT(onReadyRead()));
    Q_ASSERT(isOk);
    Q_UNUSED(isOk);
}

void TcpServerThread::run()
{
    bool isSocketDescriptorSet = _tcpSocket.setSocketDescriptor(_socketDescriptor);
    if (!isSocketDescriptorSet) {
        emit socketError(_tcpSocket.error());

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

    _tcpSocket.write(dataBlock);
    emit commandSent(_sensorData);
    _tcpSocket.disconnectFromHost();
    _tcpSocket.waitForDisconnected();
}

void TcpServerThread::sendTemperatureCurrent()
{
    QByteArray outgoingCommandBlock;

    outgoingCommandBlock.append(static_cast<char>(_sensorData.byte01));
    outgoingCommandBlock.append(static_cast<char>(_sensorData.byte02));
    outgoingCommandBlock.append(static_cast<char>(_sensorData.byte03));
    outgoingCommandBlock.append(static_cast<char>(_sensorData.byte04));

    _tcpSocket.write(outgoingCommandBlock);
    emit commandSent(_sensorData);
    _tcpSocket.disconnectFromHost();
    _tcpSocket.waitForDisconnected();
}

void TcpServerThread::setTemperatureDesired(QByteArray &incommingCommandBlock)
{
    quint8 sensorId = static_cast<quint8>(incommingCommandBlock.at(1));
    qint8 integralPart = static_cast<qint8>(incommingCommandBlock.at(2));
    quint8 fractionalPart = static_cast<quint8>(incommingCommandBlock.at(3));

    qreal temperatureDesired = integralPart + fractionalPart / 100;

    emit commandRecieved(sensorId, temperatureDesired);
}

void TcpServerThread::onReadyRead()
{
    QByteArray incommingCommandBlock = _tcpSocket.readAll();

    if (!(4 == incommingCommandBlock.size())) {
            return;
    }

    char command = incommingCommandBlock.at(0);

    switch (command) {
    case 'g':
        sendTemperatureCurrent();
        break;
    case 's':
        setTemperatureDesired(incommingCommandBlock);
        break;
    default:
        qDebug() << "Invalid command received:" << command;
    }
}

