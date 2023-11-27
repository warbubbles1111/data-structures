#pragma once

#include <functional> // std::less
#include <iostream>
#include <queue> // std::queue
#include <utility> // std::pair

using std::queue;

template <typename K, typename V, typename Comparator = std::less<K>>
class BinarySearchTree {

    public:
        using key_type        = K;
        using value_type      = V;
        using pair            = std::pair<key_type, value_type>;
        using pointer         = pair*;
        using const_pointer   = const pair*;
        using reference       = pair&;
        using const_reference = const pair&;
        using size_type       = size_t;
        using difference_type = ptrdiff_t;
  

    private:
        struct BinaryNode {
            pair element;
            BinaryNode* left;
            BinaryNode* right;
         
            BinaryNode(const pair & x, BinaryNode *lt, BinaryNode *rt) : element{x}, left{lt}, right{rt} {}
            
            BinaryNode(pair && x, BinaryNode *lt, BinaryNode *rt) : element{std::move(x)}, left{lt}, right{rt} {}
        };

    public:
        using Node = BinaryNode;

    private:
        Node* _root;
        size_type _size;
        Comparator comp;

    public:
        BinarySearchTree() : _root(nullptr), _size(0), comp(Comparator{}) {}

        // Copy constructor
        BinarySearchTree(const BinarySearchTree & rhs) : _root(nullptr), _size(rhs._size), comp(Comparator{}) {
            _root = clone(rhs._root);
        }

        void swap(BinarySearchTree & src, BinarySearchTree & dst) {
            std::swap(src._size, dst._size);
            std::swap(src._root, dst._root);
        }

        // Move constructor
        BinarySearchTree(BinarySearchTree && rhs) : _root(nullptr), _size(rhs._size), comp(Comparator{}) {
            swap(*this, rhs);
        }

        // Copy assignment operator
        BinarySearchTree & operator=(const BinarySearchTree & rhs) {
            if (this != &rhs) {
                clear(); 
                _size = rhs._size;
                _root = clone(rhs._root);
            }
            return *this;
        }

        // Move assignment operator
        BinarySearchTree & operator=(BinarySearchTree && rhs) {
            if (this != &rhs) {
                clear();
                _size = 0;
                _root = nullptr;
                swap(*this, rhs);
            }
            return *this; 
        }

        // Destructor
        ~BinarySearchTree() {
            clear();
        }

        const_reference min() const { return min(_root)->element; }

        const_reference max() const { return max(_root)->element; }

        const_reference root() const { return _root->element; }
    

        bool contains(const key_type & key) const { return contains(key, _root); }

        value_type & find(const key_type & key) { return find(key, _root)->element.second; }

        const value_type & find(const key_type & key) const { return find(key, _root)->element.second; }


        bool empty() const { return _size == 0; }

        size_type size() const { return _size; }

        void clear() { clear(_root); _size = 0; _root = nullptr; }


        void insert(const pair & x) { insert(x, _root); }

        void insert(pair && x) { insert(std::move(x), _root); }

        void erase(const key_type & key) { erase(key, _root); }


    private:
        const Node* min(const Node* node) const {
            return (node->left == nullptr) ? node : min(node->left);
        }

        const Node* max(const Node* node) const {
            return (node->right == nullptr) ? node : max(node->right);
        }
        
        void insert(const pair & x, Node*& node) {
            if (node == nullptr) {
                node = new Node(x, 0, 0);
                _size++;
                return;
            }

            if (comp(x.first, node->element.first)) {
                insert(x, node->left);
            }
            else if (comp(node->element.first, x.first)) {
                insert(x, node->right);
            }
            else {
                node->element.second = x.second; 
            }
        }

        void insert(pair && x, Node*& node) {
            if (node == nullptr) {
                node = new Node(std::move(x), 0, 0);
                _size++;
                return;
            }

            if (comp(x.first, node->element.first)) {
                insert(std::move(x), node->left);
            }
            else if (comp(node->element.first, x.first)) {
                insert(std::move(x), node->right);
            }
            else {
                node->element.second = std::move(x.second); 
            }
        }

        void erase(const key_type & key, Node*& node) {
            if (node == nullptr) { return; }

            if (comp(key, node->element.first)) {
                return erase(key, node->left);
            }
            else if (comp(node->element.first, key)) {
                return erase(key, node->right);
            }
            else {
                if (node->left == nullptr && node->right == nullptr) {
                    delete node;
                    node = nullptr;
                    _size--;
                }
                else if (node->left == nullptr && node->right != nullptr) {
                    Node* temp = node;
                    node = node->right;
                    delete temp;
                    _size--;
                }
                else if (node->left != nullptr && node->right == nullptr) {
                    Node* temp = node;
                    node = node->left;
                    delete temp;
                    _size--;
                }
                else {
                    const Node* successor = min(node->right);
                    node->element = successor->element;
                    erase(successor->element.first, node->right);
                }
            }
        }

        bool contains(const key_type & key, const Node* node) const {
            if (node == nullptr) { return false; }

            if (comp(key, node->element.first)) {
                return contains(key, node->left);
            }
            else if (comp(node->element.first, key)) {
                return contains(key, node->right);
            }
            else {
                return true;
            }
        }

        Node* find(const key_type & key, Node* node) {
            if (node == nullptr) { return node; }

            if (comp(key, node->element.first)) {
                return find(key, node->left);
            }
            else if (comp(node->element.first, key)) {
                return find(key, node->right);
            }
            else {
                return node;
            }
        }

        const Node* find(const key_type & key, const Node* node) const {
            if (node == nullptr) { return node; }

            if (comp(key, node->element.first)) {
                return find(key, node->left);
            }
            else if (comp(node->element.first, key)) {
                return find(key, node->right);
            }
            else {
                return node;
            }
        }

        void clear(Node* node) {
            // base case
            if (node == nullptr) { return; }

            clear(node->left); 
            clear(node->right);

            delete node;      
        }
        
        Node* clone (const Node* node) const {
            // base case
            if (node == nullptr) { return nullptr; }
 
            Node* copy = new Node(node->element, 0, 0);

            copy->left = clone(node->left);
            copy->right = clone(node->right);

            return copy;
        }

    public:
        template <typename KK, typename VV, typename CC>
        friend void printLevelByLevel(const BinarySearchTree<KK, VV, CC>& bst, std::ostream & out);

        template <typename KK, typename VV, typename CC>
        friend std::ostream& printNode(std::ostream & o, const typename BinarySearchTree<KK, VV, CC>::Node & node);
};

template <typename KK, typename VV, typename CC>
std::ostream& printNode(std::ostream & o, const typename BinarySearchTree<KK, VV, CC>::Node & node) {
    return o << "(" << node.element.first << ", " << node.element.second << ") ";
}

template <typename KK, typename VV, typename CC>
void printLevelByLevel(const BinarySearchTree<KK, VV, CC>& bst, std::ostream & out = std::cout) {
    using Node = typename BinarySearchTree<KK, VV, CC>::Node;

    if (bst._root == nullptr) { return; }

    queue<Node*> q;
    q.push(bst._root);
    int elementsInLevel = 1;
    bool nonNullChild = false;

    while (elementsInLevel > 0) {
        Node* node = q.front();
        q.pop();
        elementsInLevel--;
        if (node != nullptr) {
            printNode<KK, VV, CC>(out, *node);
            q.push(node->left);
            q.push(node->right);
            if (node->left != nullptr || node->right != nullptr) { nonNullChild = true; }
        }
        else {
            out << "null ";
            q.push(nullptr);
            q.push(nullptr);
        }

        if (elementsInLevel == 0) {
            out << std::endl;
            if (nonNullChild) {
                nonNullChild = false;
                elementsInLevel = q.size();
            }
        }
    }
}