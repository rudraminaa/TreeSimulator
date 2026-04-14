#ifndef BSTREE_H
#define BSTREE_H

#include "BSTNode.h"
#include <vector>
#include <functional>

/**
 * @brief A plain (unbalanced) Binary Search Tree.
 *
 * Provides insert, remove, search, and four standard traversals.
 * The AVLTree class extends this with self-balancing rotations.
 */
class BSTree {
public:
    BSTree();
    virtual ~BSTree();

    virtual void insert(int value);
    virtual void remove(int value);
    bool search(int value) const;

    std::vector<int> inOrder()    const;
    std::vector<int> preOrder()   const;
    std::vector<int> postOrder()  const;
    std::vector<int> levelOrder() const;

    BSTNode* getRoot() const;
    void clear();
    int  size() const;

protected:
    BSTNode* root_;

    // Helpers – virtual so AVLTree can override insert/remove recursion
    virtual BSTNode* insertNode(BSTNode* node, int value);
    virtual BSTNode* removeNode(BSTNode* node, int value);

    BSTNode* findMin(BSTNode* node) const;
    int  getHeight(BSTNode* node) const;
    void updateHeight(BSTNode* node);
    void destroyTree(BSTNode* node);
    int  countNodes(BSTNode* node) const;

    // Traversal helpers
    void inOrderHelper(BSTNode* node, std::vector<int>& result)    const;
    void preOrderHelper(BSTNode* node, std::vector<int>& result)   const;
    void postOrderHelper(BSTNode* node, std::vector<int>& result)  const;
};

#endif // BSTREE_H
