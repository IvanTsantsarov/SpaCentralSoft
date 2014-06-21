#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "constants.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _sensorIds.push_back(CUSTOM_SENSOR01);
    _sensorIds.push_back(CUSTOM_SENSOR02);

    _tcpClient = new TcpClient(CUSTOM_IPV4ADDRESS,
                               CUSTOM_PORT,
                               _sensorIds,
                               CUSTOM_QUERYINTERVAL,
                               this);
    _tcpClient->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}
