#include "BSTree.h"
#include <queue>
#include <algorithm>

BSTree::BSTree() : root_(nullptr) {}

BSTree::~BSTree() {
    clear();
}

// ── Public API ────────────────────────────────────────────────

void BSTree::insert(int value) {
    root_ = insertNode(root_, value);
}

void BSTree::remove(int value) {
    root_ = removeNode(root_, value);
}

bool BSTree::search(int value) const {
    BSTNode* cur = root_;
    while (cur) {
        if (value == cur->value) return true;
        cur = (value < cur->value) ? cur->left : cur->right;
    }
    return false;
}

BSTNode* BSTree::getRoot() const {
    return root_;
}

void BSTree::clear() {
    destroyTree(root_);
    root_ = nullptr;
}

int BSTree::size() const {
    return countNodes(root_);
}

// ── Traversals ────────────────────────────────────────────────

std::vector<int> BSTree::inOrder() const {
    std::vector<int> result;
    inOrderHelper(root_, result);
    return result;
}

std::vector<int> BSTree::preOrder() const {
    std::vector<int> result;
    preOrderHelper(root_, result);
    return result;
}

std::vector<int> BSTree::postOrder() const {
    std::vector<int> result;
    postOrderHelper(root_, result);
    return result;
}

std::vector<int> BSTree::levelOrder() const {
    std::vector<int> result;
    if (!root_) return result;
    std::queue<BSTNode*> q;
    q.push(root_);
    while (!q.empty()) {
        BSTNode* node = q.front(); q.pop();
        result.push_back(node->value);
        if (node->left)  q.push(node->left);
        if (node->right) q.push(node->right);
    }
    return result;
}

// ── Protected helpers ─────────────────────────────────────────

BSTNode* BSTree::insertNode(BSTNode* node, int value) {
    if (!node) return new BSTNode(value);
    if (value < node->value)
        node->left  = insertNode(node->left, value);
    else if (value > node->value)
        node->right = insertNode(node->right, value);
    // Duplicate values are ignored
    updateHeight(node);
    return node;
}

BSTNode* BSTree::removeNode(BSTNode* node, int value) {
    if (!node) return nullptr;

    if (value < node->value) {
        node->left = removeNode(node->left, value);
    } else if (value > node->value) {
        node->right = removeNode(node->right, value);
    } else {
        // Found the node to delete
        if (!node->left && !node->right) {
            delete node;
            return nullptr;
        }
        if (!node->left) {
            BSTNode* temp = node->right;
            delete node;
            return temp;
        }
        if (!node->right) {
            BSTNode* temp = node->left;
            delete node;
            return temp;
        }
        // Two children – replace with in-order successor
        BSTNode* successor = findMin(node->right);
        node->value = successor->value;
        node->right = removeNode(node->right, successor->value);
    }
    updateHeight(node);
    return node;
}

BSTNode* BSTree::findMin(BSTNode* node) const {
    while (node && node->left) node = node->left;
    return node;
}

int BSTree::getHeight(BSTNode* node) const {
    return node ? node->height : 0;
}

void BSTree::updateHeight(BSTNode* node) {
    if (node)
        node->height = 1 + std::max(getHeight(node->left), getHeight(node->right));
}

void BSTree::destroyTree(BSTNode* node) {
    if (!node) return;
    destroyTree(node->left);
    destroyTree(node->right);
    delete node;
}

int BSTree::countNodes(BSTNode* node) const {
    if (!node) return 0;
    return 1 + countNodes(node->left) + countNodes(node->right);
}

void BSTree::inOrderHelper(BSTNode* node, std::vector<int>& result) const {
    if (!node) return;
    inOrderHelper(node->left, result);
    result.push_back(node->value);
    inOrderHelper(node->right, result);
}

void BSTree::preOrderHelper(BSTNode* node, std::vector<int>& result) const {
    if (!node) return;
    result.push_back(node->value);
    preOrderHelper(node->left, result);
    preOrderHelper(node->right, result);
}

void BSTree::postOrderHelper(BSTNode* node, std::vector<int>& result) const {
    if (!node) return;
    postOrderHelper(node->left, result);
    postOrderHelper(node->right, result);
    result.push_back(node->value);
}
