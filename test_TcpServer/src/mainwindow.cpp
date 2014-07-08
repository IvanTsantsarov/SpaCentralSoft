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
    // MainWindow: Connections
    isOk = connect(this,
                   SIGNAL(dataSet(SensorData)),
                   _tcpServer,
                   SLOT(onSensorSet(SensorData)));
    Q_ASSERT(isOk);
    Q_UNUSED(isOk);

    // TcpServer: Connections
    isOk = connect(_tcpServer,
                   SIGNAL(commandSent(SensorData)),
                   this,
                   SLOT(onCommandSent(SensorData)));
    Q_ASSERT(isOk);
    Q_UNUSED(isOk);

    isOk = connect(_tcpServer,
                   SIGNAL(sensorDataChanged()),
                   this,
                   SLOT(onSensorDataChanged()));
    Q_ASSERT(isOk);
    Q_UNUSED(isOk);

    isOk = connect(_tcpServer,
                   SIGNAL(sensorSet(SensorData)),
                   this,
                   SLOT(onSensorSet(SensorData)));
    Q_ASSERT(isOk);
    Q_UNUSED(isOk);

    isOk = connect(_tcpServer,
                   SIGNAL(serverStarted(QString, quint16)),
                   this,
                   SLOT(onServerStarted(QString, quint16)));
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
    setSensorInput(_sensors->at(0));

    // TcpServer: Start
    _tcpServer->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setDataFromInput(SensorData& sensorData)
{
    QString stringByte01 = ui->lineEdit_Byte00->text();
    QString stringByte02 = ui->comboBox_Byte01->currentText();
    QString stringByte03 = ui->lineEdit_Byte02->text();
    QString stringByte04 = ui->lineEdit_Byte03->text();

    sensorData.byte00 = stringByte01.at(0).toLatin1();
    sensorData.byte01 = static_cast<quint8>(stringByte02.toUShort());
    sensorData.byte02 = static_cast<qint8>(stringByte03.toShort());
    sensorData.byte03 = static_cast<quint8>(stringByte04.toShort());
}

///
/// \brief MainWindow::setSensor
/// \param sensorData
///
///
void MainWindow::updateSensorInput()
{
    SensorData currentSensor;

    // Look for the corresponding sensor in the list with sensors
    for (int currentSensorIndex = 0;
         _sensors->count() > currentSensorIndex;
         currentSensorIndex++) {
        currentSensor = _sensors->at(currentSensorIndex);

        // If this is the current selected sensor in the Command
        // Settings/Bytes Box (GUI) set the corresponding input fields
        if (qvariant_cast<quint8>(ui->comboBox_Byte01->currentData())
                == _sensors->at(currentSensorIndex).byte01 ) {
            setSensorInput(_sensors->at(currentSensorIndex));
        }
    }
}

void MainWindow::setSensorInput(const SensorData& sensorData)
{
    ui->lineEdit_Byte00->setText(QString(sensorData.byte00));
    ui->lineEdit_Byte02->setText(QString::number(sensorData.byte02));
    ui->lineEdit_Byte03->setText(QString::number(sensorData.byte03));
}

// SLOTS: Implementation

void MainWindow::onCommandSent(SensorData sensorData)
{
    QString consoleMessage = QString("%1: Data was sent: %2 %3 %4 %5")
            .arg(QTime::currentTime().toString())
            .arg(sensorData.byte00, 3)
            .arg(sensorData.byte01, 3)
            .arg(sensorData.byte02, 3)
            .arg(sensorData.byte03, 3);

    ui->textEdit_ConsoleOutput->setTextColor(Qt::black);
    ui->textEdit_ConsoleOutput->append(consoleMessage);
}

void MainWindow::onSensorDataChanged()
{
    updateSensorInput();
}

void MainWindow::onSensorSet(SensorData sensorData)
{
    updateSensorInput();

    quint8 sensorId = static_cast<quint8>(sensorData.byte01);
    qint8 integralPart = static_cast<qint8>(sensorData.byte02);
    quint8 fractionalPart = static_cast<quint8>(sensorData.byte03);

    qreal temperatureTarget;
    if (0 > integralPart) {
        temperatureTarget = integralPart - static_cast<qreal>(fractionalPart) / 100;
    }
    else {
        temperatureTarget = integralPart + static_cast<qreal>(fractionalPart) / 100;
    }

    QString consoleMessage = QString("Set: sensorId: %1, target temperature: %2")
            .arg(sensorId, 3)
            .arg(temperatureTarget, 6);

    ui->textEdit_ConsoleOutput->setTextColor(Qt::red);
    ui->textEdit_ConsoleOutput->append(consoleMessage);
}

void MainWindow::onServerStarted(QString ipV4Address, quint16 port)
{
    QString message("Server listening at IP: " + ipV4Address + ":" + QString::number(port));
    ui->textEdit_ConsoleOutput->setTextColor(Qt::black);
    ui->textEdit_ConsoleOutput->append(message);
}

///
/// \brief MainWindow::onClicked_pushButton_SetData
///
/// Reads user input from Command Settings/Bytes Box (GUI) and emits
/// sensorDataChanged(SensorData) signal.
///
void MainWindow::onClicked_pushButton_SetData()
{
    SensorData sensorData;
    setDataFromInput(sensorData);

    emit dataSet(sensorData);

    QString consoleMessage = QString("New data set: %1 %2 %3 %4")
            .arg(sensorData.byte00, 3)
            .arg(sensorData.byte01, 3)
            .arg(sensorData.byte02, 3)
            .arg(sensorData.byte03, 3);

    ui->textEdit_ConsoleOutput->setTextColor(Qt::green);
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
            setSensorInput(_sensors->at(currentSensorIndex));
        }
    }
}
