#include "EdgeItem.h"
#include <QPen>

EdgeItem::EdgeItem(qreal x1, qreal y1, qreal x2, qreal y2, QGraphicsItem* parent)
    : QGraphicsLineItem(x1, y1, x2, y2, parent)
{
    QPen pen(QColor("#2A2D45"));
    pen.setWidthF(2.2);
    pen.setCapStyle(Qt::RoundCap);
    setPen(pen);
    setZValue(1);
}
