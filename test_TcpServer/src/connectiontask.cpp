#include "connectiontask.h"

#include <QtNetwork/QTcpSocket>

#include "constants.h"

ConnectionTask::ConnectionTask(quintptr socketDescritor,
                               const QList<SensorData>& sensors) :
    _socketDescriptor(socketDescritor),
    _sensors(sensors)
{
    qDebug() << "Socket descriptor:" <<_socketDescriptor << ":" << socketDescritor;
}

void ConnectionTask::run()
{
    if(!_socketDescriptor) {
        return;
    }

    QTcpSocket socket;
    socket.setSocketDescriptor(_socketDescriptor);

    bool isReading = true;
    while (!isReading) {
        QByteArray commandIncomming = socket.readAll();
        if (4 ==  commandIncomming.size()) {
            isReading = false;

            _sensorData.byte00 = QChar(commandIncomming[0]).toLower().toLatin1();
            _sensorData.byte01 = static_cast<quint8>(commandIncomming[1]);
            _sensorData.byte02 = static_cast<qint8>(commandIncomming[2]);
            _sensorData.byte03 = static_cast<quint8>(commandIncomming[3]);

            // Check incomming command
            switch(_sensorData.byte00) {
            case COMMAND_GET:
                emit commandSent(_sensorData);
                break;
            case COMMAND_SET:
                emit setSensor(_sensorData);
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
