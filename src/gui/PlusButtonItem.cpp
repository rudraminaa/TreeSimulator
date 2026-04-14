#include "PlusButtonItem.h"
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <QCursor>

PlusButtonItem::PlusButtonItem(QGraphicsItem* parent)
    : QGraphicsObject(parent)
{
    setAcceptHoverEvents(true);
    setZValue(15);
    setOpacity(0.0); // hidden by default, shown on parent hover
}

QRectF PlusButtonItem::boundingRect() const {
    qreal m = 3.0;
    return QRectF(-Radius - m, -Radius - m, 2*(Radius+m), 2*(Radius+m));
}

void PlusButtonItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    painter->setRenderHint(QPainter::Antialiasing, true);

    qreal r = Radius;
    QColor bg = hovered_ ? QColor("#7C5CFC") : QColor("#232741");
    QColor border = hovered_ ? QColor("#9B85FF") : QColor("#3A3F5C");
    QColor fg = hovered_ ? QColor("#FFFFFF") : QColor("#6B7185");

    // Dashed circle when not hovered, solid when hovered
    painter->setPen(Qt::NoPen);
    painter->setBrush(bg);
    painter->drawEllipse(QPointF(0, 0), r, r);

    QPen outPen(border, 1.5);
    if (!hovered_) {
        outPen.setStyle(Qt::DashLine);
        outPen.setDashPattern({3, 3});
    }
    painter->setPen(outPen);
    painter->setBrush(Qt::NoBrush);
    painter->drawEllipse(QPointF(0, 0), r, r);

    // Plus sign
    painter->setPen(QPen(fg, 2.0, Qt::SolidLine, Qt::RoundCap));
    qreal s = r * 0.45;
    painter->drawLine(QPointF(-s, 0), QPointF(s, 0));
    painter->drawLine(QPointF(0, -s), QPointF(0, s));
}

void PlusButtonItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        emit clicked();
        event->accept();
    }
}

void PlusButtonItem::hoverEnterEvent(QGraphicsSceneHoverEvent*) {
    hovered_ = true;
    setCursor(QCursor(Qt::PointingHandCursor));
    update();
}

void PlusButtonItem::hoverLeaveEvent(QGraphicsSceneHoverEvent*) {
    hovered_ = false;
    setCursor(QCursor(Qt::ArrowCursor));
    update();
}
