#include "temperatureindicator.h"

#include <QFontMetrics>
#include <QDebug>

TemperatureIndicator::TemperatureIndicator(IndicatorProperties indicatorProperties,
                                           ProgramSettings programSettings,
                                           QListWidget *listWidget,
                                           QObject *parent) :
    QObject(parent)
{
    // Add the item to ListView
    _indicatorProperties = indicatorProperties;
    _programSettings = programSettings;

    _text = _indicatorProperties.text;

    _listWidget = listWidget;
    _listWidget->addItem(_text);

    // Load the highlight image map
    QString imageFileName(ASSETS_PATH
                          + _programSettings.assetsPath
                          + _indicatorProperties.highlightImageFileName);

#if defined USE_GRAPHICSRECTITEM_ZONE && defined USE_HIGHLIGHTING
    QImage image(imageFileName);

    if (image.isNull()) {
        qDebug() << "Image failed to load:" << imageFileName;
    }
    else {
        qDebug() << "Image loaded:" << imageFileName;
    }

    _graphicsRectItem_Zone = new QGraphicsRectItem();
    _graphicsRectItem_Zone->setBrush(image);
    _graphicsRectItem_Zone->setRect(0, 0, image.width(), image.height());
    _graphicsRectItem_Zone->setOpacity(programSettings.highlightOpacity);
    _graphicsRectItem_Zone->setVisible(false);
    _graphicsRectItem_Zone->setZValue(0);
#elif USE_HIGHLIGHTING
    QPixmap pixmap(imageFileName);
    if (pixmap.isNull()) {
        qDebug() << "Pixmap failed to load:" << imageFileName;
    }
    else {
        qDebug() << "Pixmap loaded:" << imageFileName;
    }

    _graphicsPixmapItem = new QGraphicsPixmapItem(pixmap);
    _graphicsPixmapItem->setOpacity(_programSettings.opacity);
    _graphicsPixmapItem->setVisible(false);
    _graphicsPixmapItem->setZValue(0);
#endif // USE_GRAPHICSRECTITEM_ZONE

    // Add item to the GraphicsScene
    _graphicsScene = static_cast<CGraphicsScene*>(parent);

    _graphicsRectItem = new CGraphicsRectItem();

    _graphicsRectItem->setOpacity(_programSettings.indicator.opacity);
    _graphicsRectItem->setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsSelectable);
    _graphicsRectItem->setZValue(1);
#ifdef DEBUG_MODE_FINETUNING
    _graphicsRectItem->setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsMovable);
#endif // DEBUG_MODE_FINETUNING

    _graphicsTextItem = new QGraphicsTextItem();
    _graphicsTextItem->setFont(_programSettings.indicator.font);
    QColor color(_programSettings.indicator.fontColor);
    if (!color.isValid()) {
        color.setNamedColor("White");
    }
    _graphicsTextItem->setDefaultTextColor(color);
    _graphicsTextItem->setZValue(2);

#if defined USE_GRAPHICSRECTITEM_ZONE && defined USE_HIGHLIGHTING
    _graphicsScene->addItem(_graphicsRectItem_Zone);
#elif USE_HIGHLIGHTING
    _graphicsScene->addItem(_graphicsPixmapItem);
#endif
    _graphicsScene->addItem(_graphicsRectItem);
    _graphicsScene->addItem(_graphicsTextItem);

    // Connect signals
    bool isOk;
    isOk = connect(this, SIGNAL(selected()), this, SLOT(onSelected()));
    Q_ASSERT(isOk);
    Q_UNUSED(isOk);
    isOk = connect(_graphicsRectItem,
                   SIGNAL(doubleClicked(QGraphicsSceneMouseEvent*)),
                   this,
                   SIGNAL(doubleClicked(QGraphicsSceneMouseEvent*)));
    Q_ASSERT(isOk);
    Q_UNUSED(isOk);
}

CGraphicsRectItem* TemperatureIndicator::getGraphicsRectItem() const
{
    return _graphicsRectItem;
}

QString TemperatureIndicator::text() const
{
    return _text;
}

quint8 TemperatureIndicator::getSensorId()
{
    return _indicatorProperties.sensorId;
}

bool TemperatureIndicator::isSensorFunctional()
{
    return (SensorState::Disconnected != _sensorState) && (SensorState::Undefined != _sensorState);
}

qreal TemperatureIndicator::getTemperatureCurrent()
{
    return _temperatureCurrent;
}

qreal TemperatureIndicator::getTemperatureTarget()
{
    return _temperatureTarget;
}

void TemperatureIndicator::setConnected(bool connected)
{
    _sensorConnected = connected;
}

void TemperatureIndicator::setPosition(QPointF& position)
{
#if defined USE_GRAPHICSRECTITEM_ZONE && defined USE_HIGHLIGHTING
    _graphicsRectItem_Zone->setPos(0, 0);
#elif USE_HIGHLIGHTING
    _graphicsPixmapItem->setPos(0, 0);
#endif // USE_GRAPHICSRECTITEM_ZONE
    _graphicsRectItem->setPos(position);
    _graphicsTextItem->setPos(position);
}

void TemperatureIndicator::setIndicatorSelected(bool indicatorSelected)
{
    _indicatorSelected = indicatorSelected;

    emit selected();
}

void TemperatureIndicator::setSensorState(SensorState sensorState)
{
    _sensorState = sensorState;
}

void TemperatureIndicator::update()
{
    // Build the text string for the indication in the format "10° / 11°"
    QString indication("");

    if ((SensorState::Disconnected != _sensorState) && (SensorState::Undefined != _sensorState)) {
        indication += QString::number(_temperatureCurrent, 'f', 1);
    }
    else {
        indication +=QString("?");
    }
    indication += QString("° / ");
    indication += QString::number(_temperatureTarget, 'f', 1);
    indication += QString("°");
#ifdef DEBUG
    qDebug() << "INDICATION: " << indication;
#endif // DEBUG

    _graphicsTextItem->setPlainText(indication);

    // Prepare the background rectangle
    // Calculate the size for the rectangle
    QFontMetrics fontMetrics(_graphicsTextItem->font());

    int textWidth = fontMetrics.width(indication) + _programSettings.indicator.rectPadding;
    int textHeight = fontMetrics.height() + _programSettings.indicator.rectPadding;

#ifdef DEBUG
    qDebug() << "Text width: " << textWidth << " Text height: " << textHeight;
#endif // DEBUG

    QRectF rect = _graphicsRectItem->rect();
    rect.setWidth(textWidth);
    rect.setHeight(textHeight);
    _graphicsRectItem->setRect(rect);

    QBrush brush(Qt::black, Qt::BrushStyle::SolidPattern);

    int alpha;
    if (_indicatorSelected) {
        alpha = _programSettings.indicator.alphaSelected;
    }
    else {
        alpha = _programSettings.indicator.alphaDisselected;
    }

    switch (_sensorState) {
    case SensorState::Undefined:
        qDebug() << "ERROR: Undefined SensorState";
        break;
    case SensorState::Disconnected:
        brush.setColor(QColor(128, 128, 128, alpha));
        break;
    case SensorState::TemperatureNormal:
        brush.setColor(QColor(23, 142, 0, alpha));
        break;
    case SensorState::TemperatureHigher:
        brush.setColor(QColor(244, 0, 0, alpha));
        break;
    case SensorState::TemperatureLower:
        brush.setColor(QColor(0, 75, 244, alpha));
        break;
    }

    _graphicsRectItem->setBrush(brush);

    if (_indicatorSelected) {
        QColor colorBorder;
        if(QColor::isValidColor(_programSettings.indicator.bordercolor)) {
            colorBorder.setNamedColor(_programSettings.indicator.bordercolor);
        }
        else {
            qDebug() << "Invalid \"bodercolor\" value: " + _programSettings.indicator.bordercolor;

            colorBorder = Qt::white;
        }
        QPen pen(colorBorder, _programSettings.indicator.borderwidth);

#if defined USE_GRAPHICSRECTITEM_ZONE && defined USE_HIGHLIGHTING
        _graphicsRectItem_Zone->setVisible(true);
#elif USE_HIGHLIGHTING
        _graphicsPixmapItem->setVisible(true);
#endif // USE_GRAPHICSRECTITEM_ZONE
        _graphicsRectItem->setPen(pen);
    }
    else {
        QPen pen(Qt::NoPen);
#if defined USE_GRAPHICSRECTITEM_ZONE && defined USE_HIGHLIGHTING
        _graphicsRectItem_Zone->setVisible(false);
#elif USE_HIGHLIGHTING
        _graphicsPixmapItem->setVisible(false);
#endif // USE_GRAPHICSRECTITEM_ZONE
        _graphicsRectItem->setPen(pen);
    }
}

void TemperatureIndicator::onSelected()
{
    QListWidgetItem* item;

    for (int currentItem = 0; currentItem < _listWidget->count(); currentItem++) {
        item = _listWidget->item(currentItem);
        if (item->text() == _text) {
            item->setSelected(true);
        }
    }

    this->update();
}

void TemperatureIndicator::updateState()
{
    // Set sensor color
    qreal difference = qAbs(getTemperatureTarget() - getTemperatureCurrent());
    if (TEMPERATURE_THRESHOLD > difference)
    {
        setSensorState(SensorState::TemperatureNormal);
    } else if (getTemperatureTarget() < getTemperatureCurrent()) {
        setSensorState(SensorState::TemperatureHigher);
    }
    else {
        setSensorState(SensorState::TemperatureLower);
    }
}

void TemperatureIndicator::setTemperatureTarget(qreal temperature)
{
    _temperatureTarget = temperature;
    this->updateState();
    this->update();
}

void TemperatureIndicator::setTemperatureCurrent(qreal temperature)
{
    _temperatureCurrent = temperature;
    this->updateState();
    this->update();
}
