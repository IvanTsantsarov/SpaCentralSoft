#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QValidator>
#include <QRegExpValidator>
#include <QIntValidator>

#include <QMessageBox>
#include <QTime>

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

    QRegExp regExp_Byte00("[gsGS]");
    QRegExpValidator* validator_Byte00 = new QRegExpValidator(regExp_Byte00, this);
    ui->lineEdit_Byte00->setValidator(validator_Byte00);

    QIntValidator* validator_Byte01 = new QIntValidator(0, 255, this);
    ui->lineEdit_Byte01->setValidator(validator_Byte01);

    QIntValidator* validator_Byte02 = new QIntValidator(-100, 100, this);
    ui->lineEdit_Byte02->setValidator(validator_Byte02);

    QIntValidator* validator_Byte03= new QIntValidator(0, 99, this);
    ui->lineEdit_Byte03->setValidator(validator_Byte03);

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
                   SIGNAL(temperatureDesiredChanged(quint8,qreal)),
                   this,
                   SLOT(onTemperatureDesiredChanged(quint8,qreal)));
    Q_ASSERT(isOk);
    Q_UNUSED(isOk);

    isOk = connect(_tcpServer,
                   SIGNAL(commandSent(SensorData)),
                   this,
                   SLOT(onCommandSent(SensorData)));
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
    QString stringByte01 = ui->lineEdit_Byte00->text();
    QString stringByte02 = ui->lineEdit_Byte01->text();
    QString stringByte03 = ui->lineEdit_Byte02->text();
    QString stringByte04 = ui->lineEdit_Byte03->text();

    _sensorData.byte00 = stringByte01.at(0).toLatin1();
    _sensorData.byte01 = static_cast<quint8>(stringByte02.toUShort());
    _sensorData.byte02 = static_cast<qint8>(stringByte03.toShort());
    _sensorData.byte03 = static_cast<quint8>(stringByte04.toShort());

    emit sensorDataChanged(_sensorData);

    QString consoleMessage = QString("New data set: %1 %2 %3 %4")
            .arg(_sensorData.byte00, 3)
            .arg(_sensorData.byte01, 3)
            .arg(_sensorData.byte02, 3)
            .arg(_sensorData.byte03, 3);

    ui->textEdit_ConsoleOutput->append(consoleMessage);
}

void MainWindow::onServerStarted(QString ipV4Address, quint16 port)
{
    QString message("Server listening at IP: " + ipV4Address + ":" + QString::number(port));
    ui->textEdit_ConsoleOutput->append(message);
}

void MainWindow::onTemperatureDesiredChanged(quint8 sensorId, qreal temperatureDesired)
{
    QString consoleMessage = QString("Set: sensorId: %1, desired temperature: %2")
            .arg(sensorId, 3)
            .arg(temperatureDesired, 6);

    ui->textEdit_ConsoleOutput->append(consoleMessage);
}

void MainWindow::onCommandSent(SensorData sensorData)
{
    QString consoleMessage = QString("%1: Data was sent: %2 %3 %4 %5")
            .arg(QTime::currentTime().toString())
            .arg(sensorData.byte00, 3)
            .arg(sensorData.byte01, 3)
            .arg(sensorData.byte02, 3)
            .arg(sensorData.byte03, 3);

    ui->textEdit_ConsoleOutput->append(consoleMessage);
}


