#include "connectiontask.h"

#include <QtNetwork/QTcpSocket>

#include "constants.h"

ConnectionTask::ConnectionTask(quintptr socketDescritor,
                               const QList<SensorData>& sensors) :
    _socketDescriptor(socketDescritor),
    _sensors(sensors)
{
    qDebug() << "Task created:" <<_socketDescriptor;
}

void ConnectionTask::readDataFromSensor(quint8 sensorId, QByteArray& dataArray)
{
    for (SensorData sensor: _sensors) {
        if (sensorId == sensor.byte01) {
            dataArray.append(QChar(COMMAND_SET).toUpper().toLatin1());
            dataArray.append(static_cast<quint8>(sensor.byte01));
            dataArray.append(static_cast<qint8>(sensor.byte02));
            dataArray.append(static_cast<quint8>(sensor.byte03));
        }
    }
}

void ConnectionTask::run()
{
    if (!_socketDescriptor) {
        return;
    }

    QTcpSocket socket;
    socket.setSocketDescriptor(_socketDescriptor);

    bool isReading = true;
    while (isReading) {
        QByteArray commandIncomming = socket.readAll();
        qDebug() << "Bytes read" << commandIncomming.size();

        if (4 ==  commandIncomming.size()) {
            isReading = false;

            _sensorData.byte00 = QChar(commandIncomming[0]).toLower().toLatin1();
            _sensorData.byte01 = static_cast<quint8>(commandIncomming[1]);
            _sensorData.byte02 = static_cast<qint8>(commandIncomming[2]);
            _sensorData.byte03 = static_cast<quint8>(commandIncomming[3]);

            QByteArray commandOutgoing;

            // Check incomming command
            switch(_sensorData.byte00) {
            case COMMAND_GET:
                readDataFromSensor(_sensorData.byte01, commandOutgoing);
                if (socket.isValid()) {
                    socket.write(commandOutgoing);
                    emit commandSent(_sensorData);
                    qDebug() << "Valid socket";
                }
                break;
            case COMMAND_SET:
                emit sensorSet(_sensorData);
                qDebug() << "Data set";
                break;
            case COMMAND_ERROR:
                break;
            default:
                qDebug() << "SERVER ERROR: Received unknown command byte!";
            }
        }
    }
}

void ConnectionTask::setSocketDescriptor(quintptr socketDescriptor)
{
    _socketDescriptor = socketDescriptor;
}
