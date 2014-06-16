#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _tcpServer = new TcpServer(this);
    bool isOk;
    isOk = connect(_tcpServer,
                   SIGNAL(sessionOpened(QString)),
                   this,
                   SLOT(sessionOpened(QString)));
    qDebug() << isOk;
    Q_ASSERT(isOk);
    Q_UNUSED(isOk);

    _tcpServer->start();
}





MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::sessionOpened(QString message)
{
    qDebug() << "Session...";
    ui->textEdit_ConsoleOutput->setText(message);
}
