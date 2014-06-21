#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QValidator>
#include <QRegExpValidator>
#include <QIntValidator>

#include <QMessageBox>

#include "constants.h"

extern "C"
{
    #include "utilities.h"
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QRegExp regExp_Byte01("[gsGS]");
    QRegExpValidator* validator_Byte01 = new QRegExpValidator(regExp_Byte01, this);
    ui->lineEdit_Byte01->setValidator(validator_Byte01);

    QIntValidator* validator_Byte02 = new QIntValidator(0, 255, this);
    ui->lineEdit_Byte02->setValidator(validator_Byte02);

    QIntValidator* validator_Byte03 = new QIntValidator(-100, 100, this);
    ui->lineEdit_Byte03->setValidator(validator_Byte03);

    QIntValidator* validator_Byte04 = new QIntValidator(0, 99, this);
    ui->lineEdit_Byte04->setValidator(validator_Byte04);

    _tcpServer = new TcpServer(CUSTOM_SERVERADDRESS, CUSTOM_PORT, this);
    if (nullptr == _tcpServer) {
        QMessageBox(QMessageBox::Critical,
                    "Server error",
                    "Failed to start server!!!",
                    QMessageBox::Close,
                    this,
                    Qt::Popup);
    }
    bool isOk;
    isOk = connect(_tcpServer,
                   SIGNAL(serverStarted(QString, quint16)),
                   this,
                   SLOT(onServerStarted(QString, quint16)));
    Q_ASSERT(isOk);
    Q_UNUSED(isOk);

    isOk = connect(_tcpServer,
                   SIGNAL(dataSent(SensorData)),
                   this,
                   SLOT(onDataSent(SensorData)));
    Q_ASSERT(isOk);
    Q_UNUSED(isOk);

    isOk = connect(this,
                   SIGNAL(sensorDataChanged(SensorData&)),
                   _tcpServer,
                   SLOT(onSensorDataChanged(SensorData&)));
    Q_ASSERT(isOk);
    Q_UNUSED(isOk);

    isOk = connect(this->ui->pushButton_SetData,
                   SIGNAL(clicked()),
                   this,
                   SLOT(onClicked_pushButton_SetData()));
    Q_ASSERT(isOk);
    Q_UNUSED(isOk);

    onClicked_pushButton_SetData();
    _tcpServer->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onClicked_pushButton_SetData()
{
    QString stringByte01 = ui->lineEdit_Byte01->text();
    QString stringByte02 = ui->lineEdit_Byte02->text();
    QString stringByte03 = ui->lineEdit_Byte03->text();
    QString stringByte04 = ui->lineEdit_Byte04->text();

    _sensorData.byte01 = stringByte01.at(0).toLatin1();
    _sensorData.byte02 = (quint8) stringByte02.toUShort();
    _sensorData.byte03 = (qint8) stringByte03.toShort();
    _sensorData.byte04 = (quint8) stringByte04.toShort();

    emit sensorDataChanged(_sensorData);

    QString consoleMessage = QString("New data set: %1 %2 %3 %4")
            .arg(_sensorData.byte01, 3)
            .arg(_sensorData.byte02, 3)
            .arg(_sensorData.byte03, 3)
            .arg(_sensorData.byte04, 3);

    ui->textEdit_ConsoleOutput->append(consoleMessage);
}

void MainWindow::onServerStarted(QString ipV4Address, quint16 port)
{
    QString message("Server listening at IP: " + ipV4Address + ":" + QString::number(port));
    ui->textEdit_ConsoleOutput->append(message);
}

void MainWindow::onDataSent(SensorData sensorData)
{
    QString consoleMessage = QString("Data was sent: %1 %2 %3 %4")
            .arg(sensorData.byte01, 3)
            .arg(sensorData.byte02, 3)
            .arg(sensorData.byte03, 3)
            .arg(sensorData.byte04, 3);

    ui->textEdit_ConsoleOutput->append(consoleMessage);
}


