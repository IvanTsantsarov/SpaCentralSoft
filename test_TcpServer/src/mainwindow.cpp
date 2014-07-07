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

    // Set validators for the Command Settings/Bytes Box (GUI)
    QRegExp regExp_Byte00("[gseGSE]");
    QRegExpValidator* validator_Byte00 = new QRegExpValidator(regExp_Byte00, this);
    ui->lineEdit_Byte00->setValidator(validator_Byte00);

    QIntValidator* validator_Byte02 = new QIntValidator(-100, 100, this);
    ui->lineEdit_Byte02->setValidator(validator_Byte02);

    QIntValidator* validator_Byte03= new QIntValidator(0, 99, this);
    ui->lineEdit_Byte03->setValidator(validator_Byte03);

    // TcpServer: Intialization
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
    // TcpServer: Connections
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
                   SIGNAL(temperatureDesiredChanged(SensorData)),
                   this,
                   SLOT(onTemperatureDesiredChanged(SensorData)));
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

    // GUI signal connections
    isOk = connect(this->ui->pushButton_SetData,
                   SIGNAL(clicked()),
                   this,
                   SLOT(onClicked_pushButton_SetData()));
    Q_ASSERT(isOk);
    Q_UNUSED(isOk);

    _sensors = _tcpServer->getSensors();
    Q_ASSERT(_sensors);

    // Create a few default test values - "sensors"
    _sensors->append(SensorData('s', CUSTOM_SENSOR01, 18, 3));
    _sensors->append(SensorData('g', CUSTOM_SENSOR02, 20, 5));
    _sensors->append(SensorData('e', CUSTOM_SENSOR03, 24, 7));

    // Add "sensors" to the GUI
    for (int currentSensorIndex = 0; _sensors->size() > currentSensorIndex; currentSensorIndex++ ) {
        const SensorData* sensor = &_sensors->at(currentSensorIndex);
        Q_ASSERT(sensor);
        ui->comboBox_Byte01->addItem(QString::number(sensor->byte01), sensor->byte01);
    }
    // Set the input fields of Command Settings/Bytes Box (GUI) to match the first
    // item in the sensorsId ComboBox
    setLabels(_sensors->at(0));

    // Set the default values from the currently selected item in
    // Command Settings/Bytes Box (GUI)
    onClicked_pushButton_SetData();

    // TcpServer: Start
    _tcpServer->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

//Unused method
//void MainWindow::addSensor(const SensorData& sensorData)
//{
//    ui->lineEdit_Byte00->setText(QString(sensorData.byte00));
//    ui->comboBox_Byte01->addItem(QString::number(sensorData.byte01),
//                                 sensorData.byte01);
//    ui->lineEdit_Byte00->setText(QString::number(sensorData.byte02));
//    ui->lineEdit_Byte00->setText(QString::number(sensorData.byte03));
//}

///
/// \brief MainWindow::setSensor
/// \param sensorData
///
///
void MainWindow::setSensor(const SensorData& sensorData)
{
    // Look for the corresponding sensor in the list with sensors
    for (int currentSensorIndex = 0;
         _sensors->count() > currentSensorIndex;
         currentSensorIndex++) {
        SensorData currentSensor = _sensors->at(currentSensorIndex);

        if (currentSensor.byte01 == sensorData.byte01) {
            currentSensor.byte00 = sensorData.byte00;
            currentSensor.byte02 = sensorData.byte02;
            currentSensor.byte03 = sensorData.byte03;
        }

        _sensors->replace(currentSensorIndex, currentSensor);

        // If this is the current selected sensor in the Command
        // Settings/Bytes Box (GUI) set the corresponding input fields
        if (qvariant_cast<quint8>(ui->comboBox_Byte01->currentData())
                == _sensors->at(currentSensorIndex).byte01 ) {
            setLabels(_sensors->at(currentSensorIndex));
        }
    }
}

void MainWindow::setLabels(const SensorData& sensorData)
{
    ui->lineEdit_Byte00->setText(QString(sensorData.byte00));
    ui->lineEdit_Byte02->setText(QString::number(sensorData.byte02));
    ui->lineEdit_Byte03->setText(QString::number(sensorData.byte03));
}

// SLOTS: Implementation

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

void MainWindow::onTemperatureDesiredChanged(SensorData sensorData)
{
    setSensor(sensorData);
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

///
/// \brief MainWindow::onClicked_pushButton_SetData
///
/// Reads user input from Command Settings/Bytes Box (GUI) and emits
/// sensorDataChanged(SensorData) signal.
///
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

    for (int currentSensorIndex = 0;
         _sensors->size() > currentSensorIndex;
         currentSensorIndex++ ) {
        if (_sensors->at(currentSensorIndex).byte01 == sensorData.byte01)
        {
            _sensors->replace(currentSensorIndex, sensorData);
        }
    }

    emit sensorDataChanged(sensorData);

    QString consoleMessage = QString("New data set: %1 %2 %3 %4")
            .arg(sensorData.byte00, 3)
            .arg(sensorData.byte01, 3)
            .arg(sensorData.byte02, 3)
            .arg(sensorData.byte03, 3);

    ui->textEdit_ConsoleOutput->append(consoleMessage);
}

///
/// \brief MainWindow::on_comboBox_Byte01_currentIndexChanged
/// \param index
///
/// If another sensorId is selected, this slot sets the corresponding fields
/// of Command Settings/Bytes Box (GUI)
void MainWindow::on_comboBox_Byte01_currentIndexChanged(int index)
{
    for (int currentSensorIndex = 0;
         _sensors->count() > currentSensorIndex;
         currentSensorIndex++) {
        if (qvariant_cast<quint8>(ui->comboBox_Byte01->itemData(index))
                == _sensors->at(currentSensorIndex).byte01) {
            setLabels(_sensors->at(currentSensorIndex));
        }
    }
}
