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

    dataBlock.append(static_cast<char>(_sensorData.byte00));
    dataBlock.append(static_cast<char>(_sensorData.byte01));
    dataBlock.append(static_cast<char>(_sensorData.byte02));
    dataBlock.append(static_cast<char>(_sensorData.byte03));

    _tcpSocket.write(dataBlock);
    emit commandSent(_sensorData);
}

void TcpServerThread::sendTemperatureCurrent()
{
    QByteArray outgoingCommandBlock;

    outgoingCommandBlock.append(static_cast<char>(_sensorData.byte00));
    outgoingCommandBlock.append(static_cast<char>(_sensorData.byte01));
    outgoingCommandBlock.append(static_cast<char>(_sensorData.byte02));
    outgoingCommandBlock.append(static_cast<char>(_sensorData.byte03));

    _tcpSocket.write(outgoingCommandBlock);
    emit commandSent(_sensorData);
}

void TcpServerThread::setTemperatureDesired(QByteArray &incommingCommandBlock)
{
    char command = incommingCommandBlock.at(0);
    quint8 sensorId = static_cast<quint8>(incommingCommandBlock.at(1));
    qreal integralPart = static_cast<qreal>(incommingCommandBlock.at(2));
    qreal fractionalPart = static_cast<qreal>(incommingCommandBlock.at(3));

    qreal temperatureDesired;
    if (0 > integralPart) {
        temperatureDesired = integralPart - fractionalPart / 100;
    }
    else {
        temperatureDesired = integralPart + fractionalPart / 100;
    }

    SensorData sensorData(command, sensorId, integralPart, fractionalPart);

    emit temperatureDesiredChanged(sensorId, temperatureDesired);
    emit temperatureDesiredChanged(sensorData);
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

