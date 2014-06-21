#include "tcpclient.h"

TcpClient::TcpClient(QString ipV4Address,
                     quint16 port,
                     QList<quint8> sensorIds,
                     quint32 queryInterval,
                     QMainWindow* mainWindow) :
    QObject(mainWindow)
{
    _ipV4Address.setAddress(ipV4Address);
    _port = port;
    _sensorIds = sensorIds;
    _queryInterval = queryInterval;
    _mainWindow = mainWindow;

    connect(&_timer, SIGNAL(timeout()),
            this, SLOT(requestData()));
}

void TcpClient::start()
{
    _timer.start(_queryInterval);
}

void TcpClient::showErrorMessage(const QString& errorMessage)
{
    QMessageBox messageBox(_mainWindow);
    messageBox.setWindowTitle("Connection error");
    messageBox.setIcon(QMessageBox::Critical);
    messageBox.setText(errorMessage);
    messageBox.setStandardButtons(QMessageBox::Abort);

    messageBox.exec(); // Show modal

    //exit(-1);
}

void TcpClient::requestData()
{
    _socket.abort();
    _socket.connectToHost(_ipV4Address, port);
}

void TcpClient::setData(quint8 sensorId, quint8 temperatureDesired)
{
    // TODO:
//    _socket.abort;
//    _socket.connectToHost(_ipV4Address, port);
}

