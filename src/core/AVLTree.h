#ifndef AVLTREE_H
#define AVLTREE_H

#include "BSTree.h"
#include <vector>

/**
 * @brief Records a single AVL rotation that occurred during rebalancing.
 */
struct RotationStep {
    enum Type { Left, Right, LeftRight, RightLeft };
    Type type;
    int pivotValue; // the node that was imbalanced
};

/**
 * @brief Self-balancing AVL Tree with rotation tracking.
 *
 * Extends BSTree with automatic rotations and records each rotation
 * step so the GUI can visualize the balancing process.
 */
class AVLTree : public BSTree {
public:
    AVLTree() = default;
    ~AVLTree() override = default;

    void insert(int value) override;
    void remove(int value) override;

    int getBalanceFactor(BSTNode* node) const;

    const std::vector<RotationStep>& rotationLog() const { return rotationLog_; }
    void clearRotationLog() { rotationLog_.clear(); }

protected:
    BSTNode* insertNode(BSTNode* node, int value) override;
    BSTNode* removeNode(BSTNode* node, int value) override;

private:
    BSTNode* rotateLeft(BSTNode* node);
    BSTNode* rotateRight(BSTNode* node);
    BSTNode* balance(BSTNode* node);

    std::vector<RotationStep> rotationLog_;
};

#endif // AVLTREE_H
