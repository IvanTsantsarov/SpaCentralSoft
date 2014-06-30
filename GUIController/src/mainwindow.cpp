#include <QString>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QtNetwork/QHostAddress>
#include <QMessageBox>

#include <QDebug>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "forms/dialogabout.h"
#include "forms/dialogchangetemperature.h"

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
    qreal temperatureDesired = _currentTemperatureIndicator->getTemperatureDesired();
    qreal temperatureCurrent = _currentTemperatureIndicator->getTemperatureCurrent();

    DialogChangeTemperature dialogChangeTemperature(this,
                                                    dialogTitle,
                                                    sensorId,
                                                    temperatureIndicatorFunctional,
                                                    temperatureDesired,
                                                    temperatureCurrent);

    // Remove ? from TitleBar
    Qt::WindowFlags windowFlags = dialogChangeTemperature.windowFlags();
    windowFlags &= ~Qt::WindowContextHelpButtonHint;
    dialogChangeTemperature.setWindowFlags(windowFlags);

    if(dialogChangeTemperature.exec()) {
        _currentTemperatureIndicator->setTemperatureDesired(dialogChangeTemperature.getValue());
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

    if(nullptr ==_currentTemperatureIndicator) {
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
    Q_ASSERT(_socket == nullptr);

    _socket = new QTcpSocket();
    _socket->connectToHost(_programSettings.server.ipV4Address,
                           _programSettings.server.port);
}

void MainWindow::sendTemperatureDesired()
{
    //ui->
}

// Private slots
void MainWindow::onDataRecieved()
{
    QByteArray data = _socket->readAll();
    qDebug()<< QString::fromLatin1(data.toHex(), data.size());

    char cmd = data[0];
    //char id = data[1];

    switch(cmd)
    {
    case 'G':
        qDebug()<< "GET";

        _socket->write(QByteArray("g000"));
        break;
    case 'S':
        qDebug()<< "SET";
        _socket->write(QByteArray("s000"));
        break;
    case 'E':
        qDebug()<< "ERROR";
        _socket->write(QByteArray("e000"));
        break;
    default:
        qDebug()<< "Error:Unknown command byte!";
    }
}

void MainWindow::onConnected()
{
    qDebug()<< "Connected!";
    qDebug() << "Local address" << QString("%1:%2").arg(_socket->localAddress().toString())
                                           .arg(_socket->localPort());
    qDebug() << "Peer address:" << QString("%1:%2").arg(_socket->peerAddress().toString())
                                           .arg(_socket->localPort());
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
