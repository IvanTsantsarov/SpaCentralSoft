#include "tcpserver.h"

#include <QtNetwork/QNetworkConfigurationManager>
#include <QMessageBox>
#include <QTcpSocket>

TcpServer::TcpServer(QMainWindow* mainWindow) :
    QObject(mainWindow)
{
    _mainWindow = mainWindow;
}

void TcpServer::start()
{
    QNetworkConfigurationManager networkConfigurationManager;

    QNetworkConfiguration networkConfiguration =
            networkConfigurationManager.defaultConfiguration();

    _networkSession = new QNetworkSession(networkConfiguration, this);

    connect(_networkSession,
            SIGNAL(opened()),
            this,
            SLOT(sessionOpened()));

    qDebug() << "Opening network session";
    _networkSession->open();

    connect(_tcpServer,
            SIGNAL(newConnection()),
            this,
            SLOT(sendData()));
}

void TcpServer::sessionOpened()
{
    if (_networkSession) {
        QNetworkConfiguration networkConfiguration =
                _networkSession->configuration();

        QString id;

        if (QNetworkConfiguration::UserChoice == networkConfiguration.type()) {
            id = _networkSession->sessionProperty(QLatin1String("UserChoiceConfiguration"))
                    .toString();
        }
        else {
            id  = networkConfiguration.identifier();
        }
    }

    _tcpServer = new QTcpServer(this);
    if (!_tcpServer->listen()) {
//        QMessageBox::critical(this,
//                              "Test Server",
//                              "Unable to start the server: %1.").arg(_tcpServer->errorString()));
        qDebug() << "Unable to start the server: " + _tcpServer->errorString();

        exit(-1);
        return;
    }

    QString ipAddress;

    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // use the first non-localhost IPv4 address
    for (int i = 0; ipAddressesList.size() > i; i++) {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost
                && ipAddressesList.at(i).toIPv4Address()) {
            ipAddress = ipAddressesList.at(i).toString();
            break;
        }
    }

    if (ipAddress.isEmpty()) {
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
    }

    QString message;
    message = "Server is running on IP:" + ipAddress + ":" + QString::number(_tcpServer->serverPort());

    emit sessionOpened(message);
    qDebug() << message;
}

void TcpServer::sendData() {
    QByteArray dataBlock;
    QDataStream out(&dataBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << "12345";

    QTcpSocket* clientConnection = _tcpServer->nextPendingConnection();
    connect(clientConnection,
            SIGNAL(disconnected()),
            clientConnection,
            SLOT(deleteLater()));

    qDebug() << "Data sent";

    clientConnection->write(dataBlock);
    clientConnection->disconnectFromHost();
}
