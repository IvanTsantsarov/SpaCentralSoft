#include "tcpserver.h"

#include <QtNetwork/QNetworkConfigurationManager>
#include <QMessageBox>
#include <QMutex>

#include <QThreadPool>

#include "connectiontask.h"


TcpServer::TcpServer(QString ipV4Address,
                     quint16 port,
                     QMainWindow* mainWindow) :
    QTcpServer(mainWindow),
    _port(port),
    _mainWindow(mainWindow)
{
    bool isValidHostAddress = _hostAddress.setAddress(ipV4Address);
    if (!isValidHostAddress) {
        QString errorMessage = "Invalid IPv4 address: " + ipV4Address;
        showErrorMessage(errorMessage);
    }
}

void TcpServer::start()
{
    bool isListening = this->listen(_hostAddress, _port);
    if (!isListening) {
        QString errorMessage("Unable to start server at "
                             + _hostAddress.toString()
                             + ":"
                             + QString::number(_port));
        showErrorMessage(errorMessage);

        return;
    }
    else {
        emit serverStarted(_hostAddress.toString(), _port);
    }
}

void TcpServer::showErrorMessage(const QString& errorMessage)
{
    QMessageBox messageBox(_mainWindow);
    messageBox.setWindowTitle("Server error");
    messageBox.setIcon(QMessageBox::Critical);
    messageBox.setText(errorMessage);
    messageBox.setStandardButtons(QMessageBox::Abort);

    messageBox.exec(); // Show modal

    exit(-1);
}

void TcpServer::setSensor(const SensorData& sensorData)
{
    for (int currentSensorIndex = 0;
         _sensors.count() > currentSensorIndex;
         currentSensorIndex++) {
        SensorData currentSensor = _sensors.at(currentSensorIndex);

        if (currentSensor.byte01 == sensorData.byte01) {
            currentSensor.byte00 = sensorData.byte00;
            currentSensor.byte02 = sensorData.byte02;
            currentSensor.byte03 = sensorData.byte03;
        }

        _sensors.replace(currentSensorIndex, currentSensor);
    }

    emit sensorDataChanged();
}

void TcpServer::incomingConnection(qintptr socketDescriptor)
{
    ConnectionTask* connectionTask = new ConnectionTask(socketDescriptor, _sensors);
    connectionTask->setAutoDelete(true);

    bool isOk;
    isOk = connect(connectionTask,
                   SIGNAL(commandSent(SensorData)),
                   this,
                   SIGNAL(commandSent(SensorData)));
    Q_ASSERT(isOk);
    Q_UNUSED(isOk);

    isOk = connect(connectionTask,
                   SIGNAL(sensorSet(SensorData)),
                   this,
                   SLOT(onSensorSet(SensorData)));
    Q_ASSERT(isOk);
    Q_UNUSED(isOk);

    QThreadPool::globalInstance()->start(connectionTask);

}

QList<SensorData>* TcpServer::getSensors()
{
    return &_sensors;
}

void TcpServer::onSensorSet(SensorData sensorData)
{
    setSensor(sensorData);

    emit sensorSet(sensorData);
}

void TcpServer::onSocketError(QTcpSocket::SocketError socketError)
{
    QString errorMessage = "SocketError " + QString::number(socketError);

    emit serverError(errorMessage);
}
