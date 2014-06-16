#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QMainWindow>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QNetworkSession>

class TcpServer : public QObject
{
    Q_OBJECT
private:
    QMainWindow* _mainWindow;
    QTcpServer* _tcpServer;
    QNetworkSession* _networkSession;

public:
    explicit TcpServer(QMainWindow* mainWindow = 0);

    void start();

signals:
    void sessionOpened(QString message);

private slots:
    void sessionOpened();
    void sendData();
public slots:

};

#endif // TCPSERVER_H
