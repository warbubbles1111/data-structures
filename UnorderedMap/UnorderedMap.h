#include <cstddef>    // size_t
#include <functional> // std::hash
#include <ios>
#include <utility>    // std::pair
#include <iostream>

#include "primes.h"

template <typename Key, typename T, typename Hash = std::hash<Key>, typename Pred = std::equal_to<Key>>
class UnorderedMap {
    public:

    using key_type = Key;
    using mapped_type = T;
    using const_mapped_type = const T;
    using hasher = Hash;
    using key_equal = Pred;
    using value_type = std::pair<const key_type, mapped_type>;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = value_type *;
    using const_pointer = const value_type *;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    private:

    struct HashNode {
        HashNode *next;
        value_type val;

        HashNode(HashNode * next = nullptr) : next{next} {}

        HashNode(const value_type & val, HashNode * next = nullptr) : next{next}, val {val} {}

        HashNode(value_type && val, HashNode * next = nullptr) : next{next}, val{std::move(val)} {}
    };

    size_type _bucket_count;
    HashNode **_buckets;

    HashNode * _head;
    size_type _size;

    Hash _hash;
    key_equal _equal;

    static size_type _range_hash(size_type hash_code, size_type bucket_count) {
        return hash_code % bucket_count;
    }

    public:

    template <typename pointer_type, typename reference_type, typename _value_type>
    class basic_iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = _value_type;
        using difference_type = ptrdiff_t;
        using pointer = value_type *;
        using reference = value_type &;

    private:
        friend class UnorderedMap<Key, T, Hash, key_equal>;
        using HashNode = typename UnorderedMap<Key, T, Hash, key_equal>::HashNode;

        const UnorderedMap * _map;
        HashNode * _ptr;

        explicit basic_iterator(UnorderedMap const * map, HashNode * ptr) noexcept { _map = map; _ptr = ptr; }

    public:
        basic_iterator() { _map = nullptr; _ptr = nullptr; };

        basic_iterator(const basic_iterator &) = default;
        basic_iterator(basic_iterator &&) = default;
        ~basic_iterator() = default;
        basic_iterator &operator=(const basic_iterator &) = default;
        basic_iterator &operator=(basic_iterator &&) = default;

        reference operator*() const { return _ptr->val; }

        pointer operator->() const { return &(_ptr->val); }

        basic_iterator &operator++() { 
            if (_ptr->next != nullptr) {
                _ptr = _ptr->next; 
                return *this;
            }

            size_type index = _map->_bucket(_ptr->val);
            HashNode ** array = _map->_buckets;
        
            while (index < _map->_bucket_count - 1) {
                index++;
                if (array[index] != nullptr) {
                    _ptr = array[index];
                    return *this;
                }
            }
            _ptr = nullptr; 
            return *this;
        }

        basic_iterator operator++(int) { basic_iterator it = *this; ++(*this); return it;  }

        bool operator==(const basic_iterator &other) const noexcept { return this->_ptr == other._ptr; }
        bool operator!=(const basic_iterator &other) const noexcept { return !operator==(other); }
    };

    using iterator = basic_iterator<pointer, reference, value_type>;
    using const_iterator = basic_iterator<const_pointer, const_reference, const value_type>;

    class local_iterator {
        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = std::pair<const key_type, mapped_type>;
            using difference_type = ptrdiff_t;
            using pointer = value_type *;
            using reference = value_type &;

        private:
            friend class UnorderedMap<Key, T, Hash, key_equal>;
            using HashNode = typename UnorderedMap<Key, T, Hash, key_equal>::HashNode;

            HashNode * _node;

            explicit local_iterator(HashNode * node) noexcept { _node = node; }

        public:
            local_iterator() { _node = nullptr; }

            local_iterator(const local_iterator &) = default;
            local_iterator(local_iterator &&) = default;
            ~local_iterator() = default;
            local_iterator &operator=(const local_iterator &) = default;
            local_iterator &operator=(local_iterator &&) = default;

            reference operator*() const { return _node->val;  }

            pointer operator->() const { return &(_node->val); }

            local_iterator & operator++() { _node = _node->next; return *this; }

            local_iterator operator++(int) { local_iterator it = *this; ++(*this); return it; }

            bool operator==(const local_iterator &other) const noexcept { return this->_node == other._node;}

            bool operator!=(const local_iterator &other) const noexcept { return !operator==(other); }
    };

private:
    size_type _bucket(size_t code) const { return _range_hash(code, _bucket_count); }

    size_type _bucket(const Key & key) const { return _bucket(_hash(key)); }

    size_type _bucket(const value_type & val) const { return _bucket(val.first); }

    HashNode*& _find(size_type code, size_type bucket, const Key & key) {
        HashNode** it = &(_buckets[bucket]);

        while (*it != nullptr) {
            if (_equal((*it)->val.first, key)) { return *it; }
            it = &((*it)->next);
        }
        return *it;
    }

    HashNode*& _find(const Key & key) { return _find(_hash(key), _bucket(key), key); }

    HashNode * _insert_into_bucket(size_type bucket, value_type && value) { 
        HashNode* next = _buckets[bucket];
        HashNode* node = new HashNode(std::move(value), next);

        _buckets[bucket] = node;

        if (_head == nullptr) { _head = node; }

        if (bucket <= _bucket(_head->val)) { _head = node; }

        _size++;

        return node;
    }

    void _move_content(UnorderedMap & src, UnorderedMap & dst) {
        std::swap(src._hash, dst._hash);
        std::swap(src._equal, dst._equal);
        std::swap(src._size, dst._size);
        std::swap(src._bucket_count, dst._bucket_count);
        std::swap(src._head, dst._head);
        std::swap(src._buckets, dst._buckets);
    }

public:
    explicit UnorderedMap(size_type bucket_count, const Hash & hash = Hash{}, const key_equal & equal = key_equal{}) { 
        _size = 0;
        _head = nullptr;
        _bucket_count = next_greater_prime(bucket_count);
        _buckets = new HashNode *[_bucket_count] {};
    }

    // Copy constructor
    UnorderedMap(const UnorderedMap & other) { 
        _hash = other._hash;
        _equal = other._equal; 

        _size = 0; 
        _bucket_count = other._bucket_count;
        _head = nullptr;
        _buckets = new HashNode *[_bucket_count] {};

        for (size_type index = 0; index < other._bucket_count; index++) {
            HashNode* node = other._buckets[index];
            while (node != nullptr) {
                insert(node->val);
                node = node->next;
            }
        }
    }

    // Move constructor
    UnorderedMap(UnorderedMap && other) { 
        _hash = Hash{};
        _equal = key_equal{};

        _size = 0;
        _bucket_count = other._bucket_count;
        _head = nullptr;
        _buckets = new HashNode *[_bucket_count] {};

        _move_content(*this, other);
    }

    // Copy assignment constructor
    UnorderedMap & operator=(const UnorderedMap & other) {
        if (this != &other) {
            _hash = Hash{};
            _equal = key_equal{};

            clear();

            for (size_type i = 0; i < _bucket_count; i++) {
                delete _buckets[i];
            }
            delete [] _buckets;

            _bucket_count = other._bucket_count;
            _buckets = new HashNode *[_bucket_count] {};

            for (size_type index = 0; index < other._bucket_count; index++) {
                HashNode* node = other._buckets[index];
                while (node != nullptr) {
                    insert(node->val);
                    node = node->next;
                }
            }
        }
        return *this;
    }

    // Move assignment constructor
    UnorderedMap & operator=(UnorderedMap && other) {
        if (this != &other) {
            clear();

            for (size_type i = 0; i < _bucket_count; i++) {
                delete _buckets[i];
            }
            delete [] _buckets;

            _bucket_count = other._bucket_count;
            _buckets = new HashNode *[_bucket_count] {};

            _move_content(*this, other);
        }
        return *this;
    }

    // Destructor
    ~UnorderedMap() { 
        clear();

        for (size_type i = 0; i < _bucket_count; i++) {
            delete _buckets[i];
        }
        delete [] _buckets;
    }

    void clear() noexcept { 
        while (_head != nullptr) {
            erase(iterator(this, _head));
        }
    }

    size_type size() const noexcept { return _size; }

    bool empty() const noexcept { return _size == 0; }

    size_type bucket_count() const noexcept { return _bucket_count; }

    iterator begin() { return iterator(this, _head); }

    iterator end() { return iterator(this, nullptr); }

    const_iterator cbegin() const { return const_iterator(this, _head);  }

    const_iterator cend() const { return const_iterator(this, nullptr); }

    local_iterator begin(size_type n) { return local_iterator(_buckets[n]); }

    local_iterator end(size_type n) { return local_iterator(); }

    size_type bucket_size(size_type n) { 
        int count = 0;

        for (local_iterator it = begin(n); it != end(n); it++) { count++; }

        return count;
    }

    float load_factor() const { return (float)_size / _bucket_count; }

    size_type bucket(const Key & key) const { return _bucket(key); }


    std::pair<iterator, bool> insert(value_type && value) {
        HashNode* temp = _find(value.first);

        if (temp == nullptr) { 
            HashNode* node = _insert_into_bucket(_bucket(value), std::move(value));
            return std::make_pair(iterator(this, node), true);
        }
        else {
            return std::make_pair(iterator(this, temp), false);
        }
    }

    std::pair<iterator, bool> insert(const value_type & value) { 
        HashNode* temp = _find(value.first);

        if (temp == nullptr) { 
            HashNode* node = _insert_into_bucket(_bucket(value), value_type(value));
            return std::make_pair(iterator(this, node), true);
        }
        else {
            return std::make_pair(iterator(this, temp), false);
        }
    }

    iterator find(const Key & key) { 
        return iterator(this, _find(key));
    }

    T& operator[](const Key & key) {
        HashNode* temp = _find(key);

        if (temp == nullptr) {
            HashNode* node = _insert_into_bucket(_bucket(key), value_type(std::make_pair(key, T())));
            return node->val.second;
        }
        else {
            return temp->val.second;
        }
    }

    iterator erase(iterator pos) {
        HashNode* temp = pos._ptr;
        _size--;
        pos++;

        if (temp == _head) { _head = pos._ptr; }

        size_type bucket_index = _bucket(temp->val.first);
        HashNode* node = _buckets[bucket_index];

        if (node == temp) { 
            _buckets[bucket_index] = node->next;
            delete temp;
        }
        else {
            while (node->next != nullptr) {
                if (node->next == temp) {
                    node->next = temp->next;
                    delete temp;
                    break; 
                }
                node = node->next;
            }
        }
        return pos;
    }


    size_type erase(const Key & key) {
        HashNode* temp = _find(key);

        if (temp == nullptr) { 
            return 0;
        }
        else {
            erase(iterator(this, temp));
            return 1;
        }
    }

    template<typename KK, typename VV>
    friend void print_map(const UnorderedMap<KK, VV> & map, std::ostream & os);
};

template<typename K, typename V>
void print_map(const UnorderedMap<K, V> & map, std::ostream & os = std::cout) {
    using size_type = typename UnorderedMap<K, V>::size_type;
    using HashNode = typename UnorderedMap<K, V>::HashNode;

    for(size_type bucket = 0; bucket < map.bucket_count(); bucket++) {
        os << bucket << ": ";

        HashNode const * node = map._buckets[bucket];

        while(node) {
            os << "(" << node->val.first << ", " << node->val.second << ") ";
            node = node->next;
        }

        os << std::endl;
    }
}
