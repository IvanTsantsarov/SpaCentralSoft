#include <QString>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QtNetwork/QHostAddress>

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
    ConfigLoader configLoader(CONFIG_FILE, this);
    _programSettings = configLoader.getProgramSettings();

    _scene = new CGraphicsScene(this);
    ui->setupUi(this);
    connect(ui->action_About, SIGNAL(triggered()), this, SLOT(onActionAboutTriggered()));

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

    _listIndicatorProperties = configLoader.getIndicatorProperties();

    for (IndicatorProperties currentIndicatorProperties: _listIndicatorProperties) {
        TemperatureIndicator* temperatureIndicator = new TemperatureIndicator(currentIndicatorProperties,
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

    // Connect QListWidget to TemperatureIndicators
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


    connect(&_socket, SIGNAL(readyRead()), this, SLOT(onDataRecieved()) );
    connect(&_socket, SIGNAL(disconnected()), this, SLOT(onDisconnected()) );

    _socket.connectToHost(_programSettings.server.ipV4Address,
                          _programSettings.server.port);

    qDebug() << "Local address" << QString("%1:%2").arg(_socket.localAddress().toString())
                                           .arg(_socket.localPort());
    qDebug() << "Peer address:" << QString("%1:%2").arg(_socket.peerAddress().toString())
                                           .arg(_socket.localPort());
//    _socket.connectToHost(CUSTOM_IPV4ADDRESS, CUSTOM_PORT);
//    if( !_socket.waitForConnected(-1))
//    {
//        qDebug()<< "Error connecting!";
//    }
}

void MainWindow::onDataRecieved()
{
    QByteArray data = _socket.readAll();
    qDebug()<< QString::fromLatin1(data.toHex(), data.size());

    char cmd = data[0];
    //char id = data[1];

    switch(cmd)
    {
    case 'G':
        qDebug()<< "GET";

        _socket.write(QByteArray("g000"));
        break;
    case 'S':
        qDebug()<< "SET";
        _socket.write(QByteArray("s000"));
        break;
    case 'E':
        qDebug()<< "ERROR";
        _socket.write(QByteArray("e000"));
        break;
    default:
        qDebug()<< "Error:Unknown command byte!";
    }
}

void MainWindow::onDisconnected()
{
    qDebug() << "Disconnected!";
    _socket.connectToHost(_programSettings.server.ipV4Address,
                          _programSettings.server.port);
//    if( !_socket.waitForConnected(-1))
//    {
//        qDebug()<< "Error connecting!";
//    }
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showDialogChangeTemperature()
{
    qreal temperatureDesired = _currentTemperatureIndicator->getTemperatureDesired();
    qreal temperatureCurrent = _currentTemperatureIndicator->getTemperatureCurrent();
    bool temperatureIndicatorFunctional = _currentTemperatureIndicator->isSensorFunctional();
    QString dialogTitle = _currentTemperatureIndicator->text();

    DialogChangeTemperature dialogChangeTemperature(this, dialogTitle, temperatureIndicatorFunctional, temperatureDesired, temperatureCurrent);

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
       // item = static_cast<QGraphicsItem*>(iteratorItems.next());
        item = qgraphicsitem_cast<QGraphicsItem*>(iteratorItems.next());

        if (CGraphicsRectItem::Type == item->type()) {
            //rectItem = static_cast<CGraphicsRectItem*>(item);
            rectItem = qgraphicsitem_cast<CGraphicsRectItem*>(item);

            qDebug() << "Clicked CGraphicsRectItem at: " + QString::number(rectItem->x()) + ", " + QString::number(rectItem->x());
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
