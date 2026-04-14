#ifndef TREESCENE_H
#define TREESCENE_H

#include <QGraphicsScene>
#include <QTimer>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <vector>
#include <map>
#include <functional>

#include "../core/BSTNode.h"
#include "NodeItem.h"
#include "PlusButtonItem.h"

/**
 * @brief Manages the QGraphicsScene for the tree visualization.
 *
 * Handles layout computation, visual rebuild with animations,
 * plus-button insertion, and traversal/search highlighting.
 */
class TreeScene : public QGraphicsScene {
    Q_OBJECT

public:
    explicit TreeScene(QObject* parent = nullptr);

    void rebuildFromTree(BSTNode* root, bool isAVL,
                         std::function<int(BSTNode*)> avlBFGetter = nullptr);

    void animateTraversal(const std::vector<int>& order, int delayMs);
    void highlightNode(int value, bool found, int durationMs = 1200);
    void highlightPivots(const std::vector<int>& pivotValues, int durationMs = 2000);
    void resetHighlights();
    void setAnimationSpeed(int ms);

signals:
    void traversalFinished();
    void insertValueRequested();       // user clicked "+" or context menu insert
    void deleteValueRequested(int value); // user right-clicked → delete

protected:
    void drawBackground(QPainter* painter, const QRectF& rect) override;

private:
    struct LayoutInfo {
        qreal x, y;
        BSTNode* node;
    };

    void computeLayout(BSTNode* node, int depth, qreal left, qreal right,
                       std::vector<LayoutInfo>& out);

    void createPlusButtons(BSTNode* root);

    std::map<int, NodeItem*> nodeItems_;
    std::vector<PlusButtonItem*> plusButtons_;
    int animSpeed_ = 350;
    QTimer* traversalTimer_ = nullptr;
};

#endif // TREESCENE_H
