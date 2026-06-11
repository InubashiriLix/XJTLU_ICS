#include <algorithm>
#include <iostream>

struct Node
{
    int   key;
    Node* left;
    Node* right;
    int   height;
    Node(int key) : key(key), left(nullptr), right(nullptr), height(1) {}
};

int height(Node* node) {
    return node ? node->height : 0;
}

int getBalance(Node* node) {
    return node ? height(node->left) - height(node->right) : 0;
}

Node* rightRotate(Node* y) {
    Node* x  = y->left;
    Node* t2 = x->right;

    x->right = y;
    y->left  = t2;

    y->height = 1 + std::max(height(y->left), height(y->right));
    x->height = 1 + std::max(height(x->left), height(x->right));

    return x;
}

Node* leftRotate(Node* x) {
    Node* y  = x->right;
    Node* t2 = y->left;

    y->left  = x;
    x->right = t2;

    x->height = 1 + std::max(height(x->left), height(x->right));
    y->height = 1 + std::max(height(y->left), height(y->right));

    return y;
}

Node* insert(Node* node, int key) {
    if (node == nullptr) {
        return new Node(key);
    }

    // insert that first
    if (key < node->key) {
        node->left = insert(node->left, key);
    }
    else if (key > node->key) {
        node->right = insert(node->right, key);
    }
    else {
        return node; // duplicated keys are not allowed
    }

    // check the balance of the Tree
    node->height = 1 + std::max(height(node->left), height(node->right));
    auto balance = getBalance(node);
    // Left higher, with new key smaller than left child => right rotation
    // NOTE: LL status
    if (balance > 1 && key < node->left->key) {
        return rightRotate(node);
    }

    // NOTE: RR status
    // right higher, with new key bigger than right child => left rotate
    if (balance < -1 && key > node->right->key) {
        return leftRotate(node);
    }

    // if the left higher, with new key bigger than left child => left rotate on left child, then right rotate on node
    // NOTE: LR status
    if (balance > 1 && key > node->left->key) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    // if the right higher, with new key smaller than right child => right rotate on right child, then left rotate on node
    // NOTE: RL status
    if (balance < -1 && key < node->right->key) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    return node;
}

int main(int argc, char* argv[]) {

    return 0;
}
