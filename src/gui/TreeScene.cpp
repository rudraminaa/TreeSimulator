#include "TreeScene.h"
#include "EdgeItem.h"

#include <QPainter>
#include <QParallelAnimationGroup>
#include <QInputDialog>
#include <QGraphicsView>
#include <cmath>
#include <algorithm>

static constexpr qreal kVerticalGap  = 85.0;
static constexpr qreal kMinHorizGap  = 65.0;
static constexpr qreal kPlusBtnOffset = 40.0;

TreeScene::TreeScene(QObject* parent)
    : QGraphicsScene(parent)
{
    setBackgroundBrush(QColor("#0B0D18"));
}

// ── Subtle dot-grid background ───────────────────────────────

void TreeScene::drawBackground(QPainter* painter, const QRectF& rect) {
    QGraphicsScene::drawBackground(painter, rect);

    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(255, 255, 255, 8));

    qreal spacing = 30.0;
    qreal dotR = 1.0;
    qreal left = std::floor(rect.left() / spacing) * spacing;
    qreal top  = std::floor(rect.top()  / spacing) * spacing;

    for (qreal x = left; x < rect.right(); x += spacing)
        for (qreal y = top; y < rect.bottom(); y += spacing)
            painter->drawEllipse(QPointF(x, y), dotR, dotR);
}

// ── Layout computation ───────────────────────────────────────

void TreeScene::computeLayout(BSTNode* node, int depth,
                               qreal left, qreal right,
                               std::vector<LayoutInfo>& out) {
    if (!node) return;
    qreal x = (left + right) / 2.0;
    qreal y = depth * kVerticalGap;
    out.push_back({x, y, node});

    computeLayout(node->left,  depth + 1, left, x - kMinHorizGap / 2, out);
    computeLayout(node->right, depth + 1, x + kMinHorizGap / 2, right, out);
}

// ── Plus buttons at empty child slots ────────────────────────

void TreeScene::createPlusButtons(BSTNode* root) {
    // Clear old plus buttons
    for (auto* btn : plusButtons_) {
        removeItem(btn);
        delete btn;
    }
    plusButtons_.clear();

    if (!root) {
        // No tree — single "+" in the center to start
        auto* btn = new PlusButtonItem();
        btn->setPos(0, 0);
        btn->setOpacity(0.7);
        addItem(btn);
        plusButtons_.push_back(btn);
        connect(btn, &PlusButtonItem::clicked, this, &TreeScene::insertValueRequested);
        return;
    }

    // Walk tree and add "+" at empty child positions
    std::function<void(BSTNode*)> walk = [&](BSTNode* node) {
        if (!node) return;

        auto it = nodeItems_.find(node->value);
        if (it == nodeItems_.end()) return;
        qreal px = it->second->pos().x();
        qreal py = it->second->pos().y();

        auto makePlus = [&](qreal x, qreal y) {
            auto* btn = new PlusButtonItem();
            btn->setPos(x, y);
            btn->setOpacity(0.0); // will be shown on parent hover
            addItem(btn);
            plusButtons_.push_back(btn);
            connect(btn, &PlusButtonItem::clicked, this, &TreeScene::insertValueRequested);
        };

        if (!node->left) {
            makePlus(px - kPlusBtnOffset, py + kVerticalGap * 0.55);
        }
        if (!node->right) {
            makePlus(px + kPlusBtnOffset, py + kVerticalGap * 0.55);
        }

        walk(node->left);
        walk(node->right);
    };
    walk(root);
}

// ── Rebuild ──────────────────────────────────────────────────

void TreeScene::rebuildFromTree(BSTNode* root, bool isAVL,
                                 std::function<int(BSTNode*)> avlBFGetter) {
    // Remember old positions for animation
    std::map<int, QPointF> oldPositions;
    for (auto& [val, item] : nodeItems_)
        oldPositions[val] = item->pos();

    clear();
    nodeItems_.clear();
    plusButtons_.clear();

    if (!root) {
        // Empty tree — add a centered "+" button
        createPlusButtons(nullptr);
        setSceneRect(-200, -50, 400, 100);
        return;
    }

    // Compute layout
    int treeSize = 0;
    {
        std::function<int(BSTNode*)> cnt = [&](BSTNode* n) -> int {
            return n ? 1 + cnt(n->left) + cnt(n->right) : 0;
        };
        treeSize = cnt(root);
    }

    qreal spread = std::max(400.0, treeSize * kMinHorizGap * 1.1);
    std::vector<LayoutInfo> layout;
    computeLayout(root, 0, -spread, spread, layout);

    // Create node items
    for (auto& info : layout) {
        auto* item = new NodeItem(info.node->value);
        item->setPos(info.x, info.y);
        item->setChildInfo(info.node->left != nullptr, info.node->right != nullptr);

        if (isAVL && avlBFGetter) {
            item->setShowBalanceFactor(true, avlBFGetter(info.node));
        }

        // Connect node signals
        connect(item, &NodeItem::deleteRequested,
                this, &TreeScene::deleteValueRequested);
        connect(item, &NodeItem::insertRequested,
                this, &TreeScene::insertValueRequested);

        addItem(item);
        nodeItems_[info.node->value] = item;
    }

    // Create edges
    std::function<void(BSTNode*)> drawEdges = [&](BSTNode* node) {
        if (!node) return;
        auto it = nodeItems_.find(node->value);
        if (it == nodeItems_.end()) return;
        qreal nx = it->second->pos().x();
        qreal ny = it->second->pos().y();

        auto connectChild = [&](BSTNode* child) {
            if (!child) return;
            auto cit = nodeItems_.find(child->value);
            if (cit == nodeItems_.end()) return;
            addItem(new EdgeItem(nx, ny + NodeItem::Radius,
                                 cit->second->pos().x(),
                                 cit->second->pos().y() - NodeItem::Radius));
        };

        connectChild(node->left);
        connectChild(node->right);
        drawEdges(node->left);
        drawEdges(node->right);
    };
    drawEdges(root);

    // Create plus buttons
    createPlusButtons(root);

    // Animate nodes from old positions to new positions
    auto* group = new QParallelAnimationGroup(this);
    for (auto& [val, item] : nodeItems_) {
        QPointF target = item->pos();
        auto oldIt = oldPositions.find(val);
        if (oldIt != oldPositions.end() && oldIt->second != target) {
            item->setPos(oldIt->second);
            auto* anim = new QPropertyAnimation(item, "pos");
            anim->setDuration(animSpeed_);
            anim->setStartValue(oldIt->second);
            anim->setEndValue(target);
            anim->setEasingCurve(QEasingCurve::OutCubic);
            group->addAnimation(anim);
        }
    }
    if (group->animationCount() > 0)
        group->start(QAbstractAnimation::DeleteWhenStopped);
    else
        delete group;

    // Make plus buttons visible (faded)
    for (auto* btn : plusButtons_) {
        auto* fadeIn = new QPropertyAnimation(btn, "opacity");
        fadeIn->setDuration(400);
        fadeIn->setStartValue(0.0);
        fadeIn->setEndValue(0.45);
        fadeIn->setEasingCurve(QEasingCurve::OutCubic);
        fadeIn->start(QAbstractAnimation::DeleteWhenStopped);
    }

    // Adjust scene rect
    QRectF bounds = itemsBoundingRect();
    bounds.adjust(-100, -70, 100, 70);
    setSceneRect(bounds);
}

// ── Highlight pivot nodes (rotation visualization) ───────────

void TreeScene::highlightPivots(const std::vector<int>& pivotValues, int durationMs) {
    for (int val : pivotValues) {
        auto it = nodeItems_.find(val);
        if (it != nodeItems_.end()) {
            it->second->setState(NodeItem::State::Pivot);
        }
    }
    QTimer::singleShot(durationMs, this, [this, pivotValues]() {
        for (int val : pivotValues) {
            auto it = nodeItems_.find(val);
            if (it != nodeItems_.end())
                it->second->setState(NodeItem::State::Default);
        }
    });
}

// ── Highlight single node (search) ──────────────────────────

void TreeScene::highlightNode(int value, bool found, int durationMs) {
    auto it = nodeItems_.find(value);
    if (it != nodeItems_.end()) {
        it->second->setState(found ? NodeItem::State::Found : NodeItem::State::NotFound);
        QTimer::singleShot(durationMs, this, [this, value]() {
            auto it2 = nodeItems_.find(value);
            if (it2 != nodeItems_.end())
                it2->second->setState(NodeItem::State::Default);
        });
    }
}

// ── Traversal animation ─────────────────────────────────────

void TreeScene::animateTraversal(const std::vector<int>& order, int delayMs) {
    resetHighlights();

    if (order.empty()) {
        emit traversalFinished();
        return;
    }

    auto* indexPtr = new int(0);
    auto orderCopy = order;

    if (traversalTimer_) {
        traversalTimer_->stop();
        delete traversalTimer_;
    }

    traversalTimer_ = new QTimer(this);
    traversalTimer_->setInterval(delayMs);

    connect(traversalTimer_, &QTimer::timeout, this, [this, indexPtr, orderCopy]() {
        if (*indexPtr > 0) {
            int prevVal = orderCopy[*indexPtr - 1];
            auto pit = nodeItems_.find(prevVal);
            if (pit != nodeItems_.end())
                pit->second->setState(NodeItem::State::Default);
        }

        if (*indexPtr >= (int)orderCopy.size()) {
            traversalTimer_->stop();
            delete indexPtr;
            emit traversalFinished();
            return;
        }

        int val = orderCopy[*indexPtr];
        auto it = nodeItems_.find(val);
        if (it != nodeItems_.end())
            it->second->setState(NodeItem::State::Traversal);

        (*indexPtr)++;
    });

    traversalTimer_->start();
}

// ── Reset ────────────────────────────────────────────────────

void TreeScene::resetHighlights() {
    for (auto& [val, item] : nodeItems_)
        item->setState(NodeItem::State::Default);
}

void TreeScene::setAnimationSpeed(int ms) {
    animSpeed_ = ms;
}
