#pragma once

#include <cstddef> // size_t
#include <iterator> // std::bidirectional_iterator_tag
#include <type_traits> // std::is_same, std::enable_if


template <class T>
class List {

    private:
        struct Node {
            Node* next;
            Node* prev;
            T data;

            explicit Node(Node* prev = nullptr, Node* next = nullptr)
            : next{next}, prev{prev} {}

            explicit Node(const T& data, Node* prev = nullptr, Node* next = nullptr)
            : next{next}, prev{prev}, data{data} {}
            
            explicit Node(T&& data, Node* prev = nullptr, Node* next = nullptr)
            : next{next}, prev{prev}, data{std::move(data)} {}
        };
    
    template <typename pointer_type, typename reference_type>
    class basic_iterator {

        public:
            using iterator_category = std::bidirectional_iterator_tag;
            using value_type        = T;
            using difference_type   = ptrdiff_t;
            using pointer           = pointer_type;
            using reference         = reference_type;

        private:
            friend class List<value_type>;
            using Node = typename List<value_type>::Node;

            Node* node;

            explicit basic_iterator(Node* ptr) noexcept : node{ptr} {}
            explicit basic_iterator(const Node* ptr) noexcept : node{const_cast<Node*>(ptr)} {}

        public:
            basic_iterator() { node = nullptr; };
            basic_iterator(const basic_iterator&) = default;
            basic_iterator(basic_iterator&&) = default;
            ~basic_iterator() = default;
            basic_iterator& operator=(const basic_iterator&) = default;
            basic_iterator& operator=(basic_iterator&&) = default;

            reference operator*() const { return node->data; }

            pointer operator->() const { return &(node->data); }

            // Prefix Increment: ++a
            basic_iterator& operator++() { node = node->next; return *this; }
        
            // Postfix Increment: a++
            basic_iterator operator++(int) { basic_iterator result = *this; ++(*this) ; return result; }

            // Prefix Decrement: --a
            basic_iterator& operator--() { node = node->prev; return *this; }

            // Postfix Decrement: a--
            basic_iterator operator--(int) { basic_iterator result = *this; --(*this) ; return result; }

            bool operator==(const basic_iterator& other) const noexcept { return this->node == other.node; }

            bool operator!=(const basic_iterator& other) const noexcept { return !operator==(other); }
        };

    public:
        using value_type      = T;
        using pointer         = value_type*;
        using const_pointer   = const value_type*;
        using reference       = value_type&;
        using const_reference = const value_type&;
        using iterator        = basic_iterator<pointer, reference>;
        using const_iterator  = basic_iterator<const_pointer, const_reference>;
        using size_type       = size_t;
        using difference_type = ptrdiff_t;

    private:
        Node head, tail;
        size_type _size;

    public:
        List() : _size(0) { 
            head.next = &tail; tail.prev = &head; 
        } 
            
        List(size_type count, const T& value) : _size(0) {  
            head.next = &tail; 
            tail.prev = &head;

            for (size_t i = 0; i < count; i++) { 
                push_back(value); 
            }
        }

        explicit List(size_type count) : _size(0) {
            head.next = &tail; 
            tail.prev = &head;

            for (size_t i = 0; i < count; i++) { 
                push_back({}); 
            }
        }

        // Copy constructor
        List(const List& other) : _size(0) {
            head.next = &tail; tail.prev = &head;

            for (const_iterator it = other.begin(); it != other.end(); it++) {
                push_back(*it);
            }
        }

        // Move constructor
        List(List&& other) : _size(other._size) {
            if (_size == 0) {
                head.next = &tail; tail.prev = &head;
                return;
            }

            head.next = other.head.next; 
            tail.prev = other.tail.prev;
            head.next->prev = &head;
            tail.prev->next = &tail;

            other._size = 0; 

            other.head.next = &(other.tail); 
            other.tail.prev = &(other.head);
        }

        // Copy assignment operator
        List& operator=(const List& other) {
            if (this != &other) {
                clear();
        
                for (const_iterator it = other.begin(); it != other.end(); it++) {
                    push_back(*it);
                }
            }
            return *this;
        }

        // Move assignment operator
        List& operator=(List&& other) noexcept {
            if (this != &other) {
                clear();

                _size = other._size; 

                if (_size == 0) { return *this; }
                
                head.next = other.head.next; 
                tail.prev = other.tail.prev;
                head.next->prev = &head;
                tail.prev->next = &tail;

                other._size = 0; 

                other.head.next = &(other.tail); 
                other.tail.prev = &(other.head);
            }

            return *this;
        }

        // Destructor     
        ~List() { 
            clear();
        }

        void clear() noexcept {
            while(!empty()) { pop_back(); }
        }
        
        reference front() { return head.next->data; }

        const_reference front() const { return head.next->data; } 
        
        reference back() { return tail.prev->data; }

        const_reference back() const { return tail.prev->data; } 

        
        iterator begin() noexcept { return iterator(head.next);  }

        const_iterator begin() const noexcept { return const_iterator(head.next); }

        const_iterator cbegin() const noexcept { return const_iterator(head.next); }

        iterator end() noexcept { return iterator(&tail); }

        const_iterator end() const noexcept { return const_iterator(&tail); }
      
        const_iterator cend() const noexcept { return const_iterator(&tail); }


        bool empty() const noexcept { return _size == 0; }

        size_type size() const noexcept { return _size; }

        iterator insert(const_iterator pos, const T& value) {
            Node* temp = pos.node;
            Node* node = new Node(value, temp->prev, temp);
            temp->prev = node;
            node->prev->next = node;

            _size++;

            return iterator(node);
        }


        iterator insert(const_iterator pos, T&& value) {
            Node* temp = pos.node;
            Node* node = new Node(std::move(value), temp->prev, temp);
            temp->prev = node;
            node->prev->next = node;

            _size++;

            return iterator(node);
        }


        iterator erase(const_iterator pos) {
            Node* temp = pos.node;
            Node* after = temp->next;
            temp->prev->next = after;
            after->prev = temp->prev;
            delete temp;

            _size--;

            return iterator(after);
        }


        void push_back(const T& value) {
            Node* node = new Node(value, tail.prev, &tail);
            node->prev->next = node;
            tail.prev = node;

            _size++;
        }   


        void push_back(T&& value) {
            Node* node = new Node(std::move(value), tail.prev, &tail);
            node->prev->next = node;
            tail.prev = node;
            
            _size++;
        }


        void pop_back() {
            Node* last = tail.prev;
            tail.prev = tail.prev->prev;
            tail.prev->next = &tail;
            delete last;

            _size--;
        }
        
        void push_front(const T& value) {
            Node* node = new Node(value, &head, head.next);
            node->next->prev = node;
            head.next = node;

            _size++;
        }

        void push_front(T&& value) {
            Node* node = new Node(std::move(value), &head, head.next);
            node->next->prev = node;
            head.next = node;

            _size++;
        }

        void pop_front() {
            Node* first = head.next;
            head.next = head.next->next;
            head.next->prev = &head;
            delete first;

            _size--;
        }
    
    
    // These methods provide the non-const complement of the const_iterator methods provided above.
    iterator insert(iterator pos, const T & value) { 
        return insert((const_iterator &) (pos), value);
    }

    iterator insert(iterator pos, T && value) {
        return insert((const_iterator &) (pos), std::move(value));
    }

    iterator erase(iterator pos) {
        return erase((const_iterator&)(pos));
    }
};


// These methods provide an overload to compare const and non-const iterators safely.
namespace {
    template<typename Iter, typename ConstIter, typename T>
    using enable_for_list_iters = typename std::enable_if<
        std::is_same<
            typename List<typename std::iterator_traits<Iter>::value_type>::iterator, 
            Iter
        >{} && std::is_same<
            typename List<typename std::iterator_traits<Iter>::value_type>::const_iterator,
            ConstIter
        >{}, T>::type;
}


template<typename Iterator, typename ConstIter>
enable_for_list_iters<Iterator, ConstIter, bool> operator==(const Iterator & lhs, const ConstIter & rhs) {
    return (const ConstIter &)(lhs) == rhs;
}

template<typename Iterator, typename ConstIter>
enable_for_list_iters<Iterator, ConstIter, bool> operator==(const ConstIter & lhs, const Iterator & rhs) {
    return (const ConstIter &)(rhs) == lhs;
}

template<typename Iterator, typename ConstIter>
enable_for_list_iters<Iterator, ConstIter, bool> operator!=(const Iterator & lhs, const ConstIter & rhs) {
    return (const ConstIter &)(lhs) != rhs;
}

template<typename Iterator, typename ConstIter>
enable_for_list_iters<Iterator, ConstIter, bool> operator!=(const ConstIter & lhs, const Iterator & rhs) {
    return (const ConstIter &)(rhs) != lhs;
}