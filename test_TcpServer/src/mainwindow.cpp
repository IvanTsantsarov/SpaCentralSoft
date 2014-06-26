#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QValidator>
#include <QRegExpValidator>
#include <QIntValidator>

#include <QMessageBox>
#include <QTime>

#include "constants.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Create a few default test values
    _sensors.append(SensorData('s', CUSTOM_SENSOR01, 18, 2));
    _sensors.append(SensorData('g', CUSTOM_SENSOR02, 20, 5));
    _sensors.append(SensorData('s', CUSTOM_SENSOR03, 25, 7));

    for (auto sensor: _sensors) {
        ui->comboBox_Byte01->addItem(QString::number(sensor.byte01), sensor.byte01);
    }
    setLabels(_sensors.at(0));

    // Set validators for the data input box
    QRegExp regExp_Byte00("[gsGS]");
    QRegExpValidator* validator_Byte00 = new QRegExpValidator(regExp_Byte00, this);
    ui->lineEdit_Byte00->setValidator(validator_Byte00);

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

void MainWindow::addSensor(const SensorData& sensorData)
{
    ui->lineEdit_Byte00->setText(QString(sensorData.byte00));
    ui->comboBox_Byte01->addItem(QString::number(sensorData.byte01),
                                 sensorData.byte01);
    ui->lineEdit_Byte00->setText(QString::number(sensorData.byte02));
    ui->lineEdit_Byte00->setText(QString::number(sensorData.byte03));
}

void MainWindow::setSensor(const SensorData& sensorData)
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

        if (qvariant_cast<quint8>(ui->comboBox_Byte01->currentData())
                == currentSensor.byte01 ) {
            setLabels(currentSensor);
        }
    }
}

void MainWindow::setLabels(const SensorData& sensorData)
{
    ui->lineEdit_Byte00->setText(QString(sensorData.byte00));
    ui->lineEdit_Byte02->setText(QString::number(sensorData.byte02));
    ui->lineEdit_Byte03->setText(QString::number(sensorData.byte03));
}

void MainWindow::onClicked_pushButton_SetData()
{
    QString stringByte01 = ui->lineEdit_Byte00->text();
    QString stringByte02 = ui->comboBox_Byte01->currentText();
    QString stringByte03 = ui->lineEdit_Byte02->text();
    QString stringByte04 = ui->lineEdit_Byte03->text();

    SensorData sensorData(stringByte01.at(0).toLatin1(),
                          static_cast<quint8>(stringByte02.toUShort()),
                          static_cast<qint8>(stringByte03.toShort()),
                          static_cast<quint8>(stringByte04.toShort()));

    emit sensorDataChanged(sensorData);

    QString consoleMessage = QString("New data set: %1 %2 %3 %4")
            .arg(sensorData.byte00, 3)
            .arg(sensorData.byte01, 3)
            .arg(sensorData.byte02, 3)
            .arg(sensorData.byte03, 3);

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

void MainWindow::on_comboBox_Byte01_currentIndexChanged(int index)
{
    for (int currentSensorIndex = 0;
         _sensors.count() > currentSensorIndex;
         currentSensorIndex++) {
        if (qvariant_cast<quint8>(ui->comboBox_Byte01->itemData(index))
                == _sensors.at(currentSensorIndex).byte01) {
            setLabels(_sensors.at(currentSensorIndex));
        }
    }
}
