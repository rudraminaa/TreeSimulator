#include "NodeItem.h"
#include <QPainter>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QCursor>

NodeItem::NodeItem(int value, QGraphicsItem* parent)
    : QGraphicsObject(parent), value_(value)
{
    setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setZValue(10);
}

QRectF NodeItem::boundingRect() const {
    qreal m = 8.0;
    qreal top = showBF_ ? -(Radius + 24) : -Radius;
    return QRectF(-Radius - m, top - m,
                  2 * (Radius + m),
                  (showBF_ ? Radius + 24 : Radius) + Radius + 2 * m);
}

void NodeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setRenderHint(QPainter::TextAntialiasing, true);

    QColor fill = fillColor();
    QColor text = textColor();
    QColor glow = glowColor();
    qreal r = Radius;

    // ── Outer glow (on hover or special states) ───────────────
    if (hovered_ || state_ != State::Default) {
        for (int i = 4; i >= 1; --i) {
            QColor g = glow;
            g.setAlpha(hovered_ ? 10 * i : 6 * i);
            painter->setPen(Qt::NoPen);
            painter->setBrush(g);
            painter->drawEllipse(QPointF(0, 0), r + 3 * i, r + 3 * i);
        }
    }

    // ── Drop shadow ───────────────────────────────────────────
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(0, 0, 0, 40));
    painter->drawEllipse(QPointF(1.5, 2.5), r, r);

    // ── Main circle ───────────────────────────────────────────
    QRadialGradient grad(QPointF(-r * 0.25, -r * 0.25), r * 2.0);
    grad.setColorAt(0.0, fill.lighter(hovered_ ? 140 : 120));
    grad.setColorAt(0.6, fill);
    grad.setColorAt(1.0, fill.darker(115));
    painter->setBrush(grad);

    QPen borderPen(fill.lighter(150), 1.5);
    if (hovered_) {
        borderPen.setColor(QColor("#9B85FF"));
        borderPen.setWidthF(2.0);
    }
    painter->setPen(borderPen);
    painter->drawEllipse(QPointF(0, 0), r, r);

    // ── Specular highlight (top-left) ─────────────────────────
    QRadialGradient specular(QPointF(-r * 0.35, -r * 0.35), r * 0.8);
    specular.setColorAt(0.0, QColor(255, 255, 255, 25));
    specular.setColorAt(1.0, QColor(255, 255, 255, 0));
    painter->setPen(Qt::NoPen);
    painter->setBrush(specular);
    painter->drawEllipse(QPointF(0, 0), r, r);

    // ── Value text ────────────────────────────────────────────
    painter->setPen(text);
    QFont font("Inter", 12, QFont::DemiBold);
    painter->setFont(font);
    painter->drawText(QRectF(-r, -r, 2 * r, 2 * r),
                      Qt::AlignCenter, QString::number(value_));

    // ── Balance factor chip ───────────────────────────────────
    if (showBF_) {
        bool balanced = (bf_ >= -1 && bf_ <= 1);
        QColor chipBg = balanced ? QColor(0x0F, 0x35, 0x25, 210)
                                 : QColor(0x4A, 0x1A, 0x1A, 210);
        QColor chipFg = balanced ? QColor("#27AE60") : QColor("#E74C3C");

        qreal chipW = 34, chipH = 16;
        QRectF chipRect(-chipW / 2, -(r + 21), chipW, chipH);

        painter->setPen(Qt::NoPen);
        painter->setBrush(chipBg);
        painter->drawRoundedRect(chipRect, 8, 8);

        QFont sf("Inter", 8, QFont::Bold);
        painter->setFont(sf);
        painter->setPen(chipFg);
        painter->drawText(chipRect, Qt::AlignCenter, QString("%1%2")
                          .arg(bf_ > 0 ? "+" : "").arg(bf_));
    }
}

void NodeItem::setState(State s) { state_ = s; update(); }

void NodeItem::setShowBalanceFactor(bool show, int bf) {
    showBF_ = show;
    bf_ = bf;
    update();
}

void NodeItem::setChildInfo(bool hasLeft, bool hasRight) {
    hasLeft_ = hasLeft;
    hasRight_ = hasRight;
}

void NodeItem::hoverEnterEvent(QGraphicsSceneHoverEvent*) {
    hovered_ = true;
    setCursor(QCursor(Qt::PointingHandCursor));
    update();
}

void NodeItem::hoverLeaveEvent(QGraphicsSceneHoverEvent*) {
    hovered_ = false;
    setCursor(QCursor(Qt::ArrowCursor));
    update();
}

void NodeItem::contextMenuEvent(QGraphicsSceneContextMenuEvent* event) {
    QMenu menu;
    menu.setStyleSheet(
        "QMenu { background: #1C2035; color: #ECEDF0; border: 1px solid #232741; border-radius: 8px; padding: 4px; }"
        "QMenu::item { padding: 8px 20px; border-radius: 4px; }"
        "QMenu::item:selected { background: #7C5CFC; }"
    );

    QAction* deleteAct = menu.addAction(QString("Delete node %1").arg(value_));
    QAction* insertAct = menu.addAction("Insert new value...");

    QAction* chosen = menu.exec(event->screenPos());
    if (chosen == deleteAct) emit deleteRequested(value_);
    else if (chosen == insertAct) emit insertRequested();
}

// ── Color helpers (Strello palette) ──────────────────────────

QColor NodeItem::fillColor() const {
    switch (state_) {
        case State::Highlighted: return QColor("#7C5CFC");
        case State::Found:       return QColor("#1B5E30");
        case State::NotFound:    return QColor("#8B1A1A");
        case State::Traversal:   return QColor("#7C5800");
        case State::Pivot:       return QColor("#D4A017");
        default:                 return QColor("#1C2035");
    }
}

QColor NodeItem::textColor() const {
    switch (state_) {
        case State::Found:    return QColor("#5AE87B");
        case State::NotFound: return QColor("#F28B8B");
        case State::Traversal:return QColor("#FFE082");
        case State::Pivot:    return QColor("#FFF8E1");
        default:              return QColor("#ECEDF0");
    }
}

QColor NodeItem::glowColor() const {
    switch (state_) {
        case State::Highlighted: return QColor("#7C5CFC");
        case State::Found:       return QColor("#27AE60");
        case State::NotFound:    return QColor("#E74C3C");
        case State::Traversal:   return QColor("#F39C12");
        case State::Pivot:       return QColor("#F1C40F");
        default:                 return QColor("#7C5CFC");
    }
}
