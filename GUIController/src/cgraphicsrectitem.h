#ifndef CGRAPHICSRECTITEM_H
#define CGRAPHICSRECTITEM_H

#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>

class CGraphicsRectItem : public QObject, public QGraphicsRectItem
{
    Q_OBJECT

public:
    enum {Type = UserType + 1};
public:
    explicit CGraphicsRectItem(QGraphicsItem *parent = 0);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    int type() const {return Type;} //QGraphicsItem::Type

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);

signals:
    void doubleClicked(QGraphicsSceneMouseEvent *event);
public slots:

};

#endif // CGRAPHICSRECTITEM_H
