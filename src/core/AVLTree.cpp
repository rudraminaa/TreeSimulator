#include "AVLTree.h"
#include <algorithm>

int AVLTree::getBalanceFactor(BSTNode* node) const {
    if (!node) return 0;
    return getHeight(node->left) - getHeight(node->right);
}

// ── Public API (clears rotation log before each operation) ────

void AVLTree::insert(int value) {
    rotationLog_.clear();
    root_ = insertNode(root_, value);
}

void AVLTree::remove(int value) {
    rotationLog_.clear();
    root_ = removeNode(root_, value);
}

// ── Rotations ─────────────────────────────────────────────────

BSTNode* AVLTree::rotateLeft(BSTNode* x) {
    BSTNode* y  = x->right;
    BSTNode* T2 = y->left;

    y->left  = x;
    x->right = T2;

    updateHeight(x);
    updateHeight(y);
    return y;
}

BSTNode* AVLTree::rotateRight(BSTNode* y) {
    BSTNode* x  = y->left;
    BSTNode* T2 = x->right;

    x->right = y;
    y->left  = T2;

    updateHeight(y);
    updateHeight(x);
    return x;
}

// ── Balance (with rotation logging) ──────────────────────────

BSTNode* AVLTree::balance(BSTNode* node) {
    if (!node) return nullptr;
    updateHeight(node);
    int bf = getBalanceFactor(node);

    // Left-heavy
    if (bf > 1) {
        if (getBalanceFactor(node->left) < 0) {
            // Left-Right case
            rotationLog_.push_back({RotationStep::LeftRight, node->value});
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }
        // Left-Left case
        rotationLog_.push_back({RotationStep::Right, node->value});
        return rotateRight(node);
    }

    // Right-heavy
    if (bf < -1) {
        if (getBalanceFactor(node->right) > 0) {
            // Right-Left case
            rotationLog_.push_back({RotationStep::RightLeft, node->value});
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }
        // Right-Right case
        rotationLog_.push_back({RotationStep::Left, node->value});
        return rotateLeft(node);
    }

    return node;
}

// ── Insert / Remove ──────────────────────────────────────────

BSTNode* AVLTree::insertNode(BSTNode* node, int value) {
    if (!node) return new BSTNode(value);

    if (value < node->value)
        node->left  = insertNode(node->left, value);
    else if (value > node->value)
        node->right = insertNode(node->right, value);
    else
        return node; // duplicate

    return balance(node);
}

BSTNode* AVLTree::removeNode(BSTNode* node, int value) {
    if (!node) return nullptr;

    if (value < node->value) {
        node->left = removeNode(node->left, value);
    } else if (value > node->value) {
        node->right = removeNode(node->right, value);
    } else {
        if (!node->left || !node->right) {
            BSTNode* temp = node->left ? node->left : node->right;
            delete node;
            return temp;
        }
        BSTNode* successor = findMin(node->right);
        node->value = successor->value;
        node->right = removeNode(node->right, successor->value);
    }

    return balance(node);
}
