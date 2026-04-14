#ifndef NODEITEM_H
#define NODEITEM_H

#include <QGraphicsObject>

/**
 * @brief Visual tree node — Strello-style dark circle with value.
 *
 * Supports state-based colors, hover glow, balance factor chip,
 * and emits signals for context-menu interaction.
 */
class NodeItem : public QGraphicsObject {
    Q_OBJECT
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)

public:
    enum class State { Default, Highlighted, Found, NotFound, Traversal, Pivot };

    explicit NodeItem(int value, QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;

    int  value() const { return value_; }
    void setState(State state);
    State state() const { return state_; }
    void setShowBalanceFactor(bool show, int bf = 0);

    bool hasLeftChild()  const { return hasLeft_; }
    bool hasRightChild() const { return hasRight_; }
    void setChildInfo(bool hasLeft, bool hasRight);

    static constexpr qreal Radius = 24.0;

signals:
    void deleteRequested(int value);
    void insertRequested();

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;

private:
    int    value_;
    State  state_     = State::Default;
    bool   showBF_    = false;
    int    bf_        = 0;
    bool   hovered_   = false;
    bool   hasLeft_   = false;
    bool   hasRight_  = false;

    QColor fillColor() const;
    QColor textColor() const;
    QColor glowColor() const;
};

#endif // NODEITEM_H
