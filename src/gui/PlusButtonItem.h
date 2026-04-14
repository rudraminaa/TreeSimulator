#ifndef PLUSBUTTONITEM_H
#define PLUSBUTTONITEM_H

#include <QGraphicsObject>

/**
 * @brief Small "+" button that appears at empty child positions in the tree.
 *
 * When clicked, signals the scene to open an insert dialog so
 * users can add nodes directly from the canvas.
 */
class PlusButtonItem : public QGraphicsObject {
    Q_OBJECT

public:
    explicit PlusButtonItem(QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;

    static constexpr qreal Radius = 12.0;

signals:
    void clicked();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

private:
    bool hovered_ = false;
};

#endif // PLUSBUTTONITEM_H
