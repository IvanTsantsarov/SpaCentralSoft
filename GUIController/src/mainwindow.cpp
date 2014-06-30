#include <QString>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QtNetwork/QHostAddress>
#include <QMessageBox>
#include <QtMath>

#include <QDebug>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "forms/dialogabout.h"
#include "forms/dialogtemperaturetarget.h"

#include "custom/constants.h"
#include "custom/types.h"
#include "custom/structures.h"
#include "cgraphicsrectitem.h"
#include "temperatureindicator.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{  
    // Load program settings from "assets/config.xml"
    ConfigLoader configLoader(CONFIG_FILE, this);
    _programSettings = configLoader.getProgramSettings();

    // Load program setting from persistant storage
    QCoreApplication::setOrganizationName(ORGANIZATION_NAME);
    QCoreApplication::setOrganizationDomain(ORGANIZATION_DOMAIN);
    QCoreApplication::setApplicationName(APPLICATION_NAME);

    _programSettingsPersistant = new QSettings(this);

    // Setup the GUI
    ui->setupUi(this);  
    connect(ui->action_About, SIGNAL(triggered()), this, SLOT(onActionAboutTriggered()));

    // Setup the indicators display area
    _scene = new CGraphicsScene(this);
    ui->graphicsView->setScene(_scene);

    QString imageFilePath = ASSETS_PATH + _programSettings.backgroundImageFileName;
    QImage imageBackgroundImage(ASSETS_PATH + _programSettings.backgroundImageFileName);
    if (imageBackgroundImage.isNull()) {
        qDebug() << "Background image failed to load: " + imageFilePath;
    }

    ui->graphicsView->setBackgroundBrush(QBrush(QColor(50, 50, 50)));

    QRect sceneRect(0, 0, imageBackgroundImage.width(), imageBackgroundImage.height());
    QGraphicsRectItem* sceneRectItemBackground = _scene->addRect(sceneRect);
    sceneRectItemBackground->setBrush(QBrush(imageBackgroundImage));

    // Load the indicators
    _listIndicatorProperties = configLoader.getIndicatorProperties();

    for (IndicatorProperties currentIndicatorProperties: _listIndicatorProperties) {
        TemperatureIndicator* temperatureIndicator =
                new TemperatureIndicator(currentIndicatorProperties,
                                         _programSettings,
                                         ui->listWidget,
                                         _scene);
        temperatureIndicator->setPosition(currentIndicatorProperties.position);
        temperatureIndicator->setSensorState(SensorState::Disconnected);
        temperatureIndicator->update();

        QString key = QString::number(temperatureIndicator->getSensorId());
        if (_programSettingsPersistant->contains(key))  {
            qreal temperature = _programSettingsPersistant->value(key).toDouble();
            temperatureIndicator->setTemperatureTarget(temperature);
        }
        else {
            temperatureIndicator->setTemperatureTarget(currentIndicatorProperties.temperatureTarget);
            _programSettingsPersistant->setValue(key, temperatureIndicator->getTemperatureTarget());
        }

        _temperatureIndicators.push_back(temperatureIndicator);
    }

    bool isOk;
    for(auto indicator: _temperatureIndicators) {
        isOk = connect(indicator,
                       SIGNAL(doubleClicked(QGraphicsSceneMouseEvent*)),
                       this,
                       SLOT(onTemperatureIndicatorDoubleClicked(QGraphicsSceneMouseEvent*)));
        Q_ASSERT(isOk);
        Q_UNUSED(isOk);
    }

    _currentTemperatureIndicator = _temperatureIndicators[0];
    _currentTemperatureIndicator->setIndicatorSelected(true);

    // Connect QListWidget signal to TemperatureIndicators
    isOk = connect(ui->listWidget,
                   SIGNAL(itemClicked(QListWidgetItem*)),
                   this,
                   SLOT(onListWidgetItemClicked(QListWidgetItem*)));
    Q_ASSERT(isOk);
    Q_UNUSED(isOk);

    isOk = connect(ui->listWidget,
                   SIGNAL(itemDoubleClicked(QListWidgetItem*)),
                   this,
                   SLOT(onListWidgetItemDoubleClicked(QListWidgetItem*)));
    Q_ASSERT(isOk);
    Q_UNUSED(isOk);

    // Setup socket
    connectSocket();

    connect(_socket, SIGNAL(readyRead()), this, SLOT(onDataRecieved()) );
    connect(_socket, SIGNAL(connected()), this, SLOT(onConnected()) );
    connect(_socket, SIGNAL(disconnected()), this, SLOT(onDisconnected()) );
    connect(_socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(onErrorSocket(QAbstractSocket::SocketError)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showDialogChangeTemperature()
{
    QString dialogTitle = _currentTemperatureIndicator->text();
    quint8 sensorId = _currentTemperatureIndicator->getSensorId();
    bool temperatureIndicatorFunctional = _currentTemperatureIndicator->isSensorFunctional();
    qreal temperatureTarget = _currentTemperatureIndicator->getTemperatureTarget();
    qreal temperatureCurrent = _currentTemperatureIndicator->getTemperatureCurrent();

    DialogTemperatureTarget dialogChangeTemperature(this,
                                                    dialogTitle,
                                                    sensorId,
                                                    temperatureIndicatorFunctional,
                                                    temperatureTarget,
                                                    temperatureCurrent);

    // Remove ? from TitleBar
    Qt::WindowFlags windowFlags = dialogChangeTemperature.windowFlags();
    windowFlags &= ~Qt::WindowContextHelpButtonHint;
    dialogChangeTemperature.setWindowFlags(windowFlags);

    if(dialogChangeTemperature.exec()) {
        qreal temperature = dialogChangeTemperature.getValue();
        _currentTemperatureIndicator->setTemperatureTarget(temperature);
        QString key = QString::number(_currentTemperatureIndicator->getSensorId());
        _programSettingsPersistant->setValue(key, temperature);
        sendTemperatureTarget(_currentTemperatureIndicator);
    }
}

void MainWindow::selectTemperatureIndicator(QPointF point)
{
    QList<QGraphicsItem*> items = _scene->items(point.x(),
                                                point.y(),
                                                1,
                                                1,
                                                Qt::IntersectsItemBoundingRect,
                                                Qt::AscendingOrder);

    QListIterator<QGraphicsItem*> iteratorItems(items);
    QGraphicsItem* item;
    CGraphicsRectItem* rectItem;

    while(iteratorItems.hasNext())
    {
        item = qgraphicsitem_cast<QGraphicsItem*>(iteratorItems.next());

        if (CGraphicsRectItem::Type == item->type()) {
            rectItem = qgraphicsitem_cast<CGraphicsRectItem*>(item);

            qDebug() << "Clicked CGraphicsRectItem at: "
                        + QString::number(rectItem->x()) + ", "
                        + QString::number(rectItem->x());
            break;
        }
    }

    for (TemperatureIndicator* indicator: _temperatureIndicators) {
        if (indicator->getGraphicsRectItem() == rectItem) {
            qDebug() << "Clicked on the indicator:" << indicator->text();
            _currentTemperatureIndicator->setIndicatorSelected(false);
            _currentTemperatureIndicator = indicator;
            _currentTemperatureIndicator->setIndicatorSelected(true);
        }
    }
}

void MainWindow::onActionAboutTriggered()
{
    DialogAbout dialogAbout;
    dialogAbout.exec();
}

void MainWindow::onListWidgetItemClicked(QListWidgetItem *item)
{
    QString text = item->text();
    qDebug() << "Clicked QListWidgetItem: " << text;

    // Match the clicked QListWidgetItem to the corresponding TemperatureIndicator
    for (auto indicator: _temperatureIndicators) {
        if (indicator->text() == text) {
            _currentTemperatureIndicator->setIndicatorSelected(false);
            _currentTemperatureIndicator = indicator;
            _currentTemperatureIndicator->setIndicatorSelected(true);
        }
    }
}

void MainWindow::onListWidgetItemDoubleClicked(QListWidgetItem* item)
{
    Q_UNUSED(item);

    if(nullptr == _currentTemperatureIndicator) {
        return;
    }

    showDialogChangeTemperature();
}

void MainWindow::onTemperatureIndicatorDoubleClicked(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);

    if(nullptr == _currentTemperatureIndicator) {
        return;
    }

    showDialogChangeTemperature();
}

// Connection
void MainWindow::connectSocket()
{
    Q_ASSERT(nullptr == _socket);

    _socket = new QTcpSocket();
    _socket->connectToHost(_programSettings.server.ipV4Address,
                           _programSettings.server.port);
}

void MainWindow::sendTemperatureTarget(TemperatureIndicator* temperatureIndicator)
{
    if (!_socket) {
        return;
    }

    if (_socket->isValid()) {
        qreal temperatureTarget = temperatureIndicator->getTemperatureTarget();
        qint8 integralPart = qFloor(temperatureTarget);
        quint8 fractionalPart = (temperatureTarget - integralPart) * FRACTIONAL_BASE;

        QByteArray commandOutgoing;
        commandOutgoing.append(QChar(COMMAND_SET).toUpper().toLatin1());
        commandOutgoing.append(static_cast<char>(temperatureIndicator->getSensorId()));
        commandOutgoing.append(static_cast<char>(integralPart));
        commandOutgoing.append(static_cast<char>(fractionalPart));

        _socket->write(commandOutgoing);
    }
}

void MainWindow::setTemperatureIndicator(quint8 sensorId, qreal temperature)
{
    for (TemperatureIndicator* currentTemperatureIndicator: _temperatureIndicators) {
        if (currentTemperatureIndicator->getSensorId() == sensorId) {   

            currentTemperatureIndicator->setTemperatureCurrent(temperature);
        }
    }
}

// Private slots
void MainWindow::onDataRecieved()
{
    QByteArray dataIncomming = _socket->read(4);

    while( dataIncomming.size() > 0 )
    {
        qDebug()<< QString::fromLatin1(dataIncomming.toHex(), dataIncomming.size());

        char byte00_Command = QChar(dataIncomming[0]).toUpper().toLatin1();
        quint8 byte01_SensorId = static_cast<quint8>(dataIncomming[1]);
        qint8 byte02 = static_cast<qint8>(dataIncomming[2]);
        quint8 byte03 = static_cast<quint8>(dataIncomming[3]);

        switch(byte00_Command)
        {
        case COMMAND_SET:
        {
            qDebug()<< "SET";
            qreal integralPart = static_cast<qreal>(byte02);
            qreal fractionalPart = static_cast<qreal>(byte03) / FRACTIONAL_BASE;
            qreal temperature;
            if (0 > integralPart) {
                temperature = integralPart - fractionalPart;
            }
            else {
                temperature = integralPart + fractionalPart;
            }
            setTemperatureIndicator(byte01_SensorId, temperature);
        }
            break;
        case COMMAND_ERROR:
            qDebug()<< "ERROR";
            break;
        default:
            qDebug()<< "Error:Unknown command byte!";
        }

        dataIncomming = _socket->read(4);
    }
}

void MainWindow::onConnected()
{
    qDebug()<< "Connected!";
    qDebug() << "Local address" << QString("%1:%2").arg(_socket->localAddress().toString())
                                           .arg(_socket->localPort());
    qDebug() << "Peer address:" << QString("%1:%2").arg(_socket->peerAddress().toString())
                                           .arg(_socket->localPort());

    // Send all target temperatures on connect
    for (TemperatureIndicator* currentTemperatureIndicator: _temperatureIndicators) {
        sendTemperatureTarget(currentTemperatureIndicator);
    }
}

void MainWindow::onErrorSocket(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);

    qDebug()<< "Error connecting: " << _socket->errorString();

    QMessageBox::critical(this,
                          "Connection error",
                          "Connection error: " + _socket->errorString()
                          + "\n\nPlease restart the application",
                          QMessageBox::Close);

    this->close();
}

void MainWindow::onDisconnected()
{
    qDebug() << "Disconnected!";

    delete _socket;
    _socket = nullptr;

    connectSocket();
}
