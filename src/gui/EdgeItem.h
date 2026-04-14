#ifndef EDGEITEM_H
#define EDGEITEM_H

#include <QGraphicsLineItem>

/**
 * @brief Visual edge (line) connecting a parent node to a child node.
 */
class EdgeItem : public QGraphicsLineItem {
public:
    EdgeItem(qreal x1, qreal y1, qreal x2, qreal y2, QGraphicsItem* parent = nullptr);
};

#endif // EDGEITEM_H
