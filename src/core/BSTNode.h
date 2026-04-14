#ifndef BSTNODE_H
#define BSTNODE_H

/**
 * @brief Node structure for Binary Search Tree and AVL Tree.
 *
 * Stores the integer value, pointers to left/right children,
 * and height for AVL balance-factor calculations.
 */
struct BSTNode {
    int value;
    int height;
    BSTNode* left;
    BSTNode* right;

    explicit BSTNode(int val)
        : value(val), height(1), left(nullptr), right(nullptr) {}
};

#endif // BSTNODE_H
