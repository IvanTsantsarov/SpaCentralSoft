#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "constants.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _sensors.push_back(Sensor(SensorData('s', CUSTOM_SENSOR01, 0, 0), 25.5));
    _sensors.push_back(Sensor(SensorData('s', CUSTOM_SENSOR02, 0, 0), 27.5));
    _sensors.push_back(Sensor(SensorData('s', CUSTOM_SENSOR03, 0, 0), 28.5));

    for (Sensor sensor: _sensors) {
        ui->comboBox_SensorId->addItem(QString::number(sensor.sensorData.byte01),
                                       sensor.sensorData.byte01);
        _sensorIds.push_back(sensor.sensorData.byte01);
    }

    _tcpClient = new TcpClient(CUSTOM_IPV4ADDRESS,
                               CUSTOM_PORT,
                               _sensorIds,
                               CUSTOM_QUERYINTERVAL,
                               this);

    bool isOk;
    // TcpClient: Connections
    isOk = connect(_tcpClient, SIGNAL(dataReceived(quint8, qreal)),
                   this, SLOT(onDataReceived(quint8, qreal)));
    Q_ASSERT(isOk);
    Q_UNUSED(isOk);

    isOk = connect(_tcpClient, SIGNAL(dataReceived(SensorData)),
                   this, SLOT(onDataReceived(SensorData)));
    Q_ASSERT(isOk);
    Q_UNUSED(isOk);

    // GUI: Connections
    isOk = connect(ui->pushButton_TemperatureSet, SIGNAL(clicked()),
                   this, SLOT(setTemperatureDesired()));
    Q_ASSERT(isOk);
    Q_UNUSED(isOk);

    isOk = connect(ui->comboBox_SensorId, SIGNAL(currentIndexChanged(int)),
                   this, SLOT(on_comboBox_SensorId_currentIndexChanged(int)));
    Q_ASSERT(isOk);
    Q_UNUSED(isOk);

    _tcpClient->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setSensor(SensorData sensorData)
{
    for (Sensor currentSensor: _sensors) {
        if (currentSensor.sensorData.byte01 == sensorData.byte01) {
            currentSensor.sensorData.byte00 = sensorData.byte00;
            currentSensor.sensorData.byte02 = sensorData.byte02;
            currentSensor.sensorData.byte03 = sensorData.byte03;
        }
    }

    if (ui->comboBox_SensorId->currentData().toInt()
            == sensorData.byte01) {

        qint8 integralPart = sensorData.byte02;
        quint8 fractionalPart = sensorData.byte03;

        qreal temperatureCurrent;

        if (0 > integralPart) {
            temperatureCurrent = integralPart - fractionalPart / 100;
        }
        else {
            temperatureCurrent = integralPart + fractionalPart / 100;
        }

        ui->lineEdit_TemperatureCurrent->setText(QString::number(temperatureCurrent));
    }
}

void MainWindow::setTemperatureDesired()
{
    _tcpClient->setData(ui->comboBox_SensorId->currentData().toDouble(),
                        static_cast<qreal>(ui->doubleSpinBox_TemperatureSet->value()));
}

void MainWindow::onDataReceived(quint8 sensorId, qreal temperatureCurrent)
{
    QString consoleMessage = QString("Current sensorId: %1, current temperature: %2")
            .arg(sensorId)
            .arg(temperatureCurrent);

    ui->textEdit_OutputConsole->append(consoleMessage);
}

void MainWindow::onDataReceived(SensorData sensorData)
{
    setSensor(sensorData);
}

void MainWindow::on_comboBox_SensorId_currentIndexChanged(int index)
{
    for (int currentSensorIndex = 0;
         _sensors.count() > currentSensorIndex;
         currentSensorIndex++) {
        if (qvariant_cast<quint8>(ui->comboBox_SensorId->itemData(index))
                == _sensors.at(currentSensorIndex).sensorData.byte01) {
            qint8 integralPart = _sensors.at(currentSensorIndex).sensorData.byte02;
            quint8 fractionalPart = _sensors.at(currentSensorIndex).sensorData.byte03;

            qreal temperatureCurrent;

            if (0 > integralPart) {
                temperatureCurrent = integralPart - fractionalPart / 100;
            }
            else {
                temperatureCurrent = integralPart + fractionalPart / 100;
            }

            ui->lineEdit_TemperatureCurrent->setText(QString::number(temperatureCurrent));
            ui->doubleSpinBox_TemperatureSet->setValue(_sensors.at(currentSensorIndex).tempetaruteDesired);
        }
    }
}
