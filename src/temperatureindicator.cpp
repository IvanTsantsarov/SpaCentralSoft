#include "temperatureindicator.h"

#include <QFontMetrics>
#include <QDebug>

TemperatureIndicator::TemperatureIndicator(QString text, QString imageFileName, QListWidget* listWidget, QObject *parent) :
    QObject(parent)
{
    // Add the item to ListView
    _text = text;
    _listWidget = listWidget;
    _listWidget->addItem(_text);

#ifdef USE_GRAPHICSRECTITEM_ZONE
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
    _graphicsRectItem_Zone->setOpacity(SETTINGS_HIGHLIGHTOPACITY);
    _graphicsRectItem_Zone->setVisible(false);
    _graphicsRectItem_Zone->setZValue(0);
#else
    QPixmap pixmap(imageFileName);
    if (pixmap.isNull()) {
        qDebug() << "Pixmap failed to load:" << imageFileName;
    }
    else {
        qDebug() << "Pixmap loaded:" << imageFileName;
    }

    _graphicsPixmapItem = new QGraphicsPixmapItem(pixmap);
    _graphicsPixmapItem->setOpacity(SETTINGS_HIGHLIGHTOPACITY);
    _graphicsPixmapItem->setVisible(false);
    _graphicsPixmapItem->setZValue(0);
#endif // USE_GRAPHICSRECTITEM_ZONE

    // Add item to the GraphicsScene
    _graphicsScene = static_cast<CGraphicsScene*>(parent);

    _graphicsRectItem = new CGraphicsRectItem();
    _graphicsRectItem->setOpacity(SETTINGS_OPACITY);
    _graphicsRectItem->setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsSelectable);
    _graphicsRectItem->setZValue(1);
#ifdef DEBUG_MODE_FINETUNING
    _graphicsRectItem->setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsMovable);
#endif // DEBUG_MODE_FINETUNING

    _graphicsTextItem = new QGraphicsTextItem();
    QFont font(SETTINGS_FONTFAMILY, SETTINGS_FONTSIZE);
    font.setBold(SETTINGS_FONTWEIGHTBOLD);
    _graphicsTextItem->setFont(font);
    QColor color(SETTINGS_FONTCOLOR);
    _graphicsTextItem->setDefaultTextColor(color);
    _graphicsTextItem->setZValue(2);

#ifdef USE_GRAPHICSRECTITEM_ZONE
    _graphicsScene->addItem(_graphicsRectItem_Zone);
#else
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

bool TemperatureIndicator::isSensorFunctional()
{
    return (SensorState::Disconnected != _sensorState) && (SensorState::Undefined != _sensorState);
}

qreal TemperatureIndicator::getTemperatureCurrent()
{
    return _temperatureCurrent;
}

qreal TemperatureIndicator::getTemperatureDesired()
{
    return _temperatureDesired;
}

void TemperatureIndicator::setConnected(bool connected)
{
    _sensorConnected = connected;
}

void TemperatureIndicator::setPosition(QPointF& position)
{
#ifdef USE_GRAPHICSRECTITEM_ZONE
    _graphicsRectItem_Zone->setPos(0, 0);
#else
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
        indication += QString::number(_temperatureCurrent);
    }
    else {
        indication +=QString("?");
    }
    indication += QString("° / ");
    indication += QString::number(_temperatureDesired);
    indication += QString("°");
    qDebug() << "INDICATION: " << indication;

    _graphicsTextItem->setPlainText(indication);

    // Prepare the background rectangle
    // Calculate the size for the rectangle
    QFontMetrics fontMetrics(_graphicsTextItem->font());
    int textWidth = fontMetrics.width(indication) + SETTINGS_RECTPADDING;
    int textHeight = fontMetrics.height() + SETTINGS_RECTPADDING;
    qDebug() << "Text width: " << textWidth << " Text height: " << textHeight;

    QRectF rect = _graphicsRectItem->rect();
    rect.setWidth(textWidth);
    rect.setHeight(textHeight);
    _graphicsRectItem->setRect(rect);

    QBrush brush(Qt::black, Qt::BrushStyle::SolidPattern);

    int alpha = _indicatorSelected ? 255 : SETTINGS_ALPHA;

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
        QPen pen(Qt::white, SETTINGS_BORDERWIDTH);
#ifdef USE_GRAPHICSRECTITEM_ZONE
        _graphicsRectItem_Zone->setVisible(true);
#else
        _graphicsPixmapItem->setVisible(true);
#endif // USE_GRAPHICSRECTITEM_ZONE
        _graphicsRectItem->setPen(pen);
    }
    else {
        QPen pen(Qt::NoPen);
#ifdef USE_GRAPHICSRECTITEM_ZONE
        _graphicsRectItem_Zone->setVisible(false);
#else
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

bool TemperatureIndicator::setTemperatureDesired(qreal temp)
{
    _temperatureDesired = temp;
    this->update();

    return true;
}

bool TemperatureIndicator::setTemperatureCurrent(qreal temp)
{
    Q_UNUSED(temp);

    this->update();

    return true;
}
