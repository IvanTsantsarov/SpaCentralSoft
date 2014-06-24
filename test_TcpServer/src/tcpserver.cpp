#include "tcpserver.h"

#include <QtNetwork/QNetworkConfigurationManager>
#include <QMessageBox>

#include "tcpserverthread.h"


TcpServer::TcpServer(QString ipV4Address,
                     quint16 port,
                     QMainWindow* mainWindow) :
    QTcpServer(mainWindow)
{
    bool isValidHostAddress = _hostAddress.setAddress(ipV4Address);
    if (!isValidHostAddress) {
        QString errorMessage = "Invalid IPv4 address: " + ipV4Address;
        showErrorMessage(errorMessage);
    }

    _port = port;
    _mainWindow = mainWindow;
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

    isOk = connect(tcpServerThread, SIGNAL(commandRecieved(quint8, qreal)),
                   this, SIGNAL(commandReceived(quint8, qreal)));
    Q_ASSERT(isOk);
    Q_UNUSED(isOk);

    isOk = connect(tcpServerThread, SIGNAL(commandSent(SensorData)),
                   this, SIGNAL(commandSent(SensorData)));
    Q_ASSERT(isOk);
    Q_UNUSED(isOk);

    tcpServerThread->run();
}

void TcpServer::onSensorDataChanged(SensorData &sensorData)
{
    _sensorData = sensorData;
}

void TcpServer::onSocketError(QTcpSocket::SocketError socketError)
{
    QString errorMessage = "SocketError " + QString::number(socketError);

    emit serverError(errorMessage);
}
