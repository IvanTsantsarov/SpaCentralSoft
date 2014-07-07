#include "tcpserver.h"

#include <QtNetwork/QNetworkConfigurationManager>
#include <QMessageBox>
#include <QMutex>

#include <QThreadPool>

#include "tcpserverthread.h"
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

void TcpServer::incomingConnection(qintptr socketDescriptor)
{

    ConnectionTask* connectionTask = new ConnectionTask(socketDescriptor, _sensors);
    QThreadPool::globalInstance()->start(connectionTask);

    TcpServerThread*  tcpServerThread;
    tcpServerThread = new TcpServerThread(socketDescriptor,
                                          _sensorData,
                                          this);
    bool isOk;
    isOk = connect(tcpServerThread, SIGNAL(finished()),
                   tcpServerThread, SLOT(deleteLater()));
    Q_ASSERT(isOk);
    Q_UNUSED(isOk);

    isOk = connect(tcpServerThread, SIGNAL(socketError(QTcpSocket::SocketError)),
                   this, SLOT(onSocketError(QTcpSocket::SocketError)));
    Q_ASSERT(isOk);
    Q_UNUSED(isOk);

    isOk = connect(tcpServerThread, SIGNAL(temperatureDesiredChanged(quint8, qreal)),
                   this, SIGNAL(temperatureDesiredChanged(quint8, qreal)));
    Q_ASSERT(isOk);
    Q_UNUSED(isOk);

    isOk = connect(tcpServerThread, SIGNAL(temperatureDesiredChanged(SensorData)),
                   this, SIGNAL(temperatureDesiredChanged(SensorData)));
    Q_ASSERT(isOk);
    Q_UNUSED(isOk);

    isOk = connect(tcpServerThread, SIGNAL(commandSent(SensorData)),
                   this, SIGNAL(commandSent(SensorData)));
    Q_ASSERT(isOk);
    Q_UNUSED(isOk);

    tcpServerThread->run();
}

QList<SensorData>* TcpServer::getSensors()
{
    return &_sensors;
}

void TcpServer::onSensorDataChanged(SensorData& sensorData)
{
    for (SensorData sensor: _sensors) {
        if (sensorData.byte01 == sensor.byte01) {
            QMutex mutex;
            mutex.lock();
            sensor.byte00 = sensorData.byte00;
            sensor.byte02 = sensorData.byte02;
            sensor.byte03 = sensorData.byte03;
            mutex.unlock();
        }
    }
}

void TcpServer::onSocketError(QTcpSocket::SocketError socketError)
{
    QString errorMessage = "SocketError " + QString::number(socketError);

    emit serverError(errorMessage);
}
