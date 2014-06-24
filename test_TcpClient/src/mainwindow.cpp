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
    _sensorIds.push_back(CUSTOM_SENSOR03);

    for (auto sensor: _sensorIds) {
        ui->comboBox_SensorId->addItem(QString::number(sensor), sensor);
    }

    _tcpClient = new TcpClient(CUSTOM_IPV4ADDRESS,
                               CUSTOM_PORT,
                               _sensorIds,
                               CUSTOM_QUERYINTERVAL,
                               this);

    bool isOk;
    isOk = connect(ui->pushButton_TemperatureSet, SIGNAL(clicked()),
                   this, SLOT(setTemperatureDesired()));
    Q_ASSERT(isOk);
    Q_UNUSED(isOk);

    _tcpClient->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setTemperatureDesired()
{
    _tcpClient->setData(ui->comboBox_SensorId->currentData().toDouble(),
                        static_cast<qreal>(ui->doubleSpinBox_TemperatureSet->value()));
}
