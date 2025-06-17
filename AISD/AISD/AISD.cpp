#include <iostream>
#include <vector>
#include <string>

enum QUERRIES {
    ADD,
    LOOKUP,
    DELETE,
    PRINT_ROTATIONS,
    PRINT_COUNT_BLACK_KEYS,
    PRINT_COUNT_RED_KEYS
};

QUERRIES convertToQuery(std::string query) {
    if (query == "ADD") return QUERRIES::ADD;
    else if (query == "LOOKUP") return QUERRIES::LOOKUP;
    else if (query == "DELETE") return QUERRIES::DELETE;
    else if (query == "PRINT_ROTATIONS") return QUERRIES::PRINT_ROTATIONS;
    else if (query == "PRINT_COUNT_BLACK_KEYS") return QUERRIES::PRINT_COUNT_BLACK_KEYS;
    else if (query == "PRINT_COUNT_RED_KEYS") return QUERRIES::PRINT_COUNT_RED_KEYS;
}


/*
Adapted code from these sources : 
https://www.geeksforgeeks.org/introduction-to-red-black-tree/ 
https://www.geeksforgeeks.org/deletion-in-red-black-tree/
https://www.programiz.com/dsa/deletion-from-a-red-black-tree
*/
enum COLOR { RED, BLACK };
template<typename T>
class Node {
public:
    int key;
    T data;
    COLOR color;
    Node* parent, * left, * right;
    Node(int key, T data)
        : key(key), data(data), color(RED), parent(nullptr), left(nullptr), right(nullptr) {}
};



template <typename T>
class RedBlackTree {
private:

    Node<T>* _root;
    Node<T>* _NIL; 
    int _rotationCount;

    void _leftRotate(Node<T>* x) {
        Node<T>* y = x->right;
        x->right = y->left;
        if (y->left != _NIL)
            y->left->parent = x;
        y->parent = x->parent;
        if (x->parent == nullptr)
            _root = y;
        else if (x == x->parent->left)
            x->parent->left = y;
        else
            x->parent->right = y;
        y->left = x;
        x->parent = y;
        _rotationCount++;
    }

    void _rightRotate(Node<T>* y) {
        Node<T>* x = y->left;
        y->left = x->right;
        if (x->right != _NIL)
            x->right->parent = y;
        x->parent = y->parent;
        if (y->parent == nullptr)
            _root = x;
        else if (y == y->parent->left)
            y->parent->left = x;
        else
            y->parent->right = x;
        x->right = y;
        y->parent = x;
        _rotationCount++;
    }

    void _insertFixUp(Node<T>* z) {
        while (z->parent && z->parent->color == RED) {
            if (z->parent == z->parent->parent->left) {
                Node<T>* y = z->parent->parent->right;
                if (y->color == RED) {
                    z->parent->color = BLACK;
                    y->color = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent;
                }
                else {
                    if (z == z->parent->right) {
                        z = z->parent;
                        _leftRotate(z);
                    }
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    _rightRotate(z->parent->parent);
                }
            }
            else {
                Node<T>* y = z->parent->parent->left;
                if (y->color == RED) {
                    z->parent->color = BLACK;
                    y->color = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent;
                }
                else {
                    if (z == z->parent->left) {
                        z = z->parent;
                        _rightRotate(z);
                    }
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    _leftRotate(z->parent->parent);
                }
            }
            if (z == _root)
                break;
        }
        _root->color = BLACK;
    }

    void _transplant(Node<T>* u, Node<T>* v) {
        if (u->parent == nullptr)
            _root = v;
        else if (u == u->parent->left)
            u->parent->left = v;
        else
            u->parent->right = v;
        v->parent = u->parent;
    }

    Node<T>* _minimum(Node<T>* x) {
        while (x->left != _NIL)
            x = x->left;
        return x;
    }


    void _deleteFixUp(Node<T>* x) {
        while (x != _root && x->color == BLACK) {
            if (x == x->parent->left) {
                Node<T>* w = x->parent->right;
                if (w->color == RED) {
                    w->color = BLACK;
                    x->parent->color = RED;
                    _leftRotate(x->parent);
                    w = x->parent->right;
                }
                if (w->left->color == BLACK && w->right->color == BLACK) {
                    w->color = RED;
                    x = x->parent;
                }
                else {
                    if (w->right->color == BLACK) {
                        w->left->color = BLACK;
                        w->color = RED;
                        _rightRotate(w);
                        w = x->parent->right;
                    }
                    w->color = x->parent->color;
                    x->parent->color = BLACK;
                    w->right->color = BLACK;
                    _leftRotate(x->parent);
                    x = _root;
                }
            }
            else {
                Node<T>* w = x->parent->left;
                if (w->color == RED) {
                    w->color = BLACK;
                    x->parent->color = RED;
                    _rightRotate(x->parent);
                    w = x->parent->left;
                }
                if (w->left->color == BLACK && w->right->color == BLACK) {
                    w->color = RED;
                    x = x->parent;
                }
                else {
                    if (w->left->color == BLACK) {
                        w->right->color = BLACK;
                        w->color = RED;
                        _leftRotate(w);
                        w = x->parent->left;
                    }
                    w->color = x->parent->color;
                    x->parent->color = BLACK;
                    w->left->color = BLACK;
                    _rightRotate(x->parent);
                    x = _root;
                }
            }
        }
        x->color = BLACK;
    }

    void _inOrderHelper(Node<T>* node, int& redCount, int& blackCount) {
        if (node == nullptr || node == _NIL) return;

        _inOrderHelper(node->left, redCount, blackCount);

        if (node->color == RED)
            redCount++;
        else
            blackCount++;

        _inOrderHelper(node->right, redCount, blackCount);
    }


public:
    RedBlackTree() : _root(nullptr), _rotationCount(0) {
        _NIL = new Node<T>(0, NULL);
        _NIL->color = BLACK;
        _NIL->left = _NIL->right = _NIL;
        _root = _NIL;
    }

    void insert(int key, T data) {
        Node<T>* z = new Node<T>(key, data);
        z->left = z->right = _NIL;
        Node<T>* y = nullptr;
        Node<T>* x = _root;
        while (x != _NIL) {
            y = x;
            if (z->key < x->key)
                x = x->left;
            else if (z->key > x->key)
                x = x->right;
            else {
                delete z;
            }
        }
        z->parent = y;
        if (y == nullptr)
            _root = z;
        else if (z->key < y->key)
            y->left = z;
        else
            y->right = z;
        if (z->parent == nullptr) {
            z->color = BLACK;
            return;
        }
        if (z->parent->parent == nullptr)
            return;
        _insertFixUp(z);
    }
    Node<T>* search(int key) {
        Node<T>* current = _root;
        while (current != _NIL && current->key != key) {
            if (key < current->key)
                current = current->left;
            else
                current = current->right;
        }
        return (current == _NIL) ? nullptr : current;
    }


    void deleteNode(int key) {
        Node<T>* z = _root;
        while (z != _NIL && z->key != key) {
            if (key < z->key)
                z = z->left;
            else
                z = z->right;
        }
        Node<T>* y = z;
        COLOR yOriginalColor = y->color;
        Node<T>* x;
        if (z->left == _NIL) {
            x = z->right;
            _transplant(z, z->right);
        }
        else if (z->right == _NIL) {
            x = z->left;
            _transplant(z, z->left);
        }
        else {
            y = _minimum(z->right);
            yOriginalColor = y->color;
            x = y->right;
            if (y->parent == z) {
                x->parent = y;
            }
            else {
                _transplant(y, y->right);
                y->right = z->right;
                y->right->parent = y;
            }
            _transplant(z, y);
            y->left = z->left;
            y->left->parent = y;
            y->color = z->color;
        }
        delete z;
        if (yOriginalColor == BLACK)
            _deleteFixUp(x);
    }

    int getRotationCount() {
        return _rotationCount;
    }

    int getRedKeyCount() {
        int redCount = 0;
        int blackCount = 0;
        _inOrderHelper(_root, redCount, blackCount);
        return redCount;
    }

    int getBlackKeyCount() {
        int redCount = 0;
        int blackCount = 0;
        _inOrderHelper(_root, redCount, blackCount);
        return blackCount;
    }
};

int main() {
    int n;
    std::cin >> n;
    RedBlackTree<int> tree;

    for (int i = 0; i < n; i++) {
        std::string queryStr;
        std::cin >> queryStr;
        QUERRIES query = convertToQuery(queryStr);
        switch (query) {
        case ADD: {
            int key, value;
            std::cin >> key >> value;
            if (tree.search(key) == nullptr) tree.insert(key, value);
            else std::cout << "KEY ALREADY EXIST" << std::endl;
            break;
        }
        case LOOKUP: {
            int key;
            std::cin >> key;
            Node<int>* node = tree.search(key);
            if (node != nullptr) std::cout << node->data << std::endl;
            else std::cout << "KEY NOT FOUND" << std::endl;
            break;
        }
        case DELETE: {
            int key;
            std::cin >> key;
            if (tree.search(key) != nullptr) tree.deleteNode(key);
            else std::cout << "KEY NOT FOUND" << std::endl;
            break;
        }
        case PRINT_ROTATIONS:
            std::cout << tree.getRotationCount() << std::endl;
            break;
        case PRINT_COUNT_BLACK_KEYS:
            std::cout << tree.getBlackKeyCount() << std::endl;
            break;
        case PRINT_COUNT_RED_KEYS: {
            std::cout << tree.getRedKeyCount() << std::endl;
            break;
        }
        default:
            break;
        }
    }
    return 0;
}