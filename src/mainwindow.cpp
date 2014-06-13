#include <QString>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>

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

#ifndef USE_OBSOLETE
    QString imageFilePath = ASSETS_PATH + _programSettings.backgroundImageFileName;
    QImage imageBackgroundImage(ASSETS_PATH + _programSettings.backgroundImageFileName);
    if (imageBackgroundImage.isNull()) {
        qDebug() << "Background image failed to load: " + imageFilePath;
    }
#else
    QImage imageBackgroundImage(ASSETS_BMP_MAP);
#endif  // USE_OBSOLETE

    ui->graphicsView->setBackgroundBrush(QBrush(QColor(50, 50, 50)));

    QRect sceneRect(0, 0, imageBackgroundImage.width(), imageBackgroundImage.height());
    QGraphicsRectItem* sceneRectItemBackground = _scene->addRect(sceneRect);
    sceneRectItemBackground->setBrush(QBrush(imageBackgroundImage));

    _listIndicatorProperties = configLoader.getIndicatorProperties();

#ifndef USE_OBSOLETE
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
#else
    QString path("dark");

    // Create the TemperatureIndicators
    TemperatureIndicator* ti1 = new TemperatureIndicator("Sauna 1/Сауна 1", "assets/" + path + "/zone1.png", ui->listWidget, _scene);
    _temperatureIndicators.push_back(ti1);
    QPointF point1(555, 273);
    ti1->setPosition(point1);
    ti1->setConnected(true);
    ti1->update();

    TemperatureIndicator* ti2 = new TemperatureIndicator("Sauna 2/Сауна 2", "assets/" + path + "/zone2.png", ui->listWidget,_scene);
    _temperatureIndicators.push_back(ti2);
    QPointF point2(830, 200);
    ti2->setPosition(point2);
    ti2->setSensorState(SensorState::Disconnected);
    ti2->update();

    TemperatureIndicator* ti3 = new TemperatureIndicator("Pool/Басейн", "assets/" + path + "/zone3.png", ui->listWidget,_scene);
    _temperatureIndicators.push_back(ti3);
    QPointF point3(1165, 269);
    ti3->setPosition(point3);
    ti3->setSensorState(SensorState::TemperatureNormal);
    ti3->update();

    TemperatureIndicator* ti4 = new TemperatureIndicator("Massage 1/Масаж 1", "assets/" + path + "/zone4.png", ui->listWidget,_scene);
    _temperatureIndicators.push_back(ti4);
    QPointF point4(1152, 490);
    ti4->setPosition(point4);
    ti4->setSensorState(SensorState::TemperatureHigher);
    ti4->update();

    TemperatureIndicator* ti5 = new TemperatureIndicator("Massage 2/Масаж 2", "assets/" + path + "/zone5.png", ui->listWidget,_scene);
    _temperatureIndicators.push_back(ti5);
    QPointF point5(688, 663);
    ti5->setPosition(point5);
    ti5->setSensorState(SensorState::TemperatureLower);
    ti5->update();

    TemperatureIndicator* ti6 = new TemperatureIndicator("Hydromassage 1/Хидро Масаж 1", "assets/" + path + "/zone6.png", ui->listWidget,_scene);
    _temperatureIndicators.push_back(ti6);
    QPointF point6(903, 645);
    ti6->setPosition(point6);
    ti6->update();

    TemperatureIndicator* ti7 = new TemperatureIndicator("Hydromassage 2/Хидро Масаж 2", "assets/" + path + "/zone7.png", ui->listWidget,_scene);
    _temperatureIndicators.push_back(ti7);
    QPointF point7(1094, 824);
    ti7->setPosition(point7);
    ti7->update();

    bool isOk;
    for(auto indicator: _temperatureIndicators) {
        isOk = connect(indicator,
                       SIGNAL(doubleClicked(QGraphicsSceneMouseEvent*)),
                       this,
                       SLOT(onTemperatureIndicatorDoubleClicked(QGraphicsSceneMouseEvent*)));
        Q_ASSERT(isOk);
        Q_UNUSED(isOk);
    }

    _currentTemperatureIndicator = ti1;
    _currentTemperatureIndicator->setIndicatorSelected(true);

#endif // USE_OBSOLETE

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
