#ifndef VECTOR_H
#define VECTOR_H

#include <algorithm> 
#include <cstddef> 
#include <stdexcept> 
#include <type_traits> 
#include <utility> 

template <class T>
class Vector {
    public:
        class iterator;

    private:
        T* array;
        size_t _capacity, _size;

        void grow() { 
            _capacity = (_capacity == 0) ? 1 : 2 * _capacity;
            T* newArray = new T[_capacity] {};

            for (size_t i = 0; i < _size; ++i) { 
                newArray[i] = std::move(array[i]);
            }

            delete[] array;
            array = newArray;
        }

    public:
        Vector() noexcept : _capacity(0),  _size(0) { 
            array = nullptr; 
        }

        Vector(size_t count, const T& value) : _capacity(count), _size(count) { 
            array = new T[_capacity] {};

            for (size_t i = 0; i < _size; i++) {
                array[i] = value;
            } 
        }

        explicit Vector(size_t count) : _capacity(count), _size(count) { 
            array = new T[_capacity] {}; 
        }

        // Copy constructor
        Vector(const Vector& other) : _capacity(other.capacity()), _size(other.size()) {
            array = new T[_capacity] {};

            for (size_t i = 0; i < _size; i++) { 
                array[i] = other.at(i); 
            }
        }

        void swap(Vector & src, Vector & dst) {
            std::swap(src._size, dst._size);
            std::swap(src._capacity, dst._capacity);
            std::swap(src.array, dst.array);
        }

        // Move constructor
        Vector(Vector&& other) noexcept : _capacity(0), _size(0), array(nullptr) {
            swap(*this, other);
        }

        // Copy assignment operator
        Vector& operator=(const Vector& other) {
            if (this != &other) {
                delete[] array;
                _capacity = other._capacity; 
                _size = other._size;

                array = new T[_capacity] {};

                for (size_t i = 0; i < _size; i++) { 
                    array[i] = other.at(i);
                }
            }
            return *this;
        }

        // Move assignment operator
        Vector& operator=(Vector&& other) noexcept {
            if (this != &other) {
                delete[] array;
                array = nullptr;
                _capacity = _size = 0;
                swap(*this, other);
            }
            return *this;
        }

        // Destructor
        ~Vector() { 
            delete[] array;
            _capacity = _size = 0;
        }

        iterator begin() noexcept { return iterator(array); }

        iterator end() noexcept { return iterator(array) + _size; }

        [[nodiscard]] bool empty() const noexcept { return _size == 0; }

        size_t size() const noexcept { return _size; }

        size_t capacity() const noexcept { return _capacity; }

        T& at(size_t pos) { 
            if (pos >= _size || pos < 0) { throw std::out_of_range(""); }
            return array[pos]; 
        }

        const T& at(size_t pos) const {
            if (pos >= _size || pos < 0) { throw std::out_of_range(""); }
            return array[pos]; 
        }
        
        T& operator[](size_t pos) { return array[pos]; }

        const T& operator[](size_t pos) const { return array[pos]; }

        T& front() { return array[0]; }

        const T& front() const { return array[0]; }

        T& back() { return (_size == 0) ? array[0] : array[_size - 1]; }

        const T& back() const { return (_size == 0) ? array[0] : array[_size - 1]; }

        void push_back(const T& value) {
            if (_size >= _capacity) { grow(); }
            array[_size] = value;
            _size++;
        }

        void push_back(T&& value) { 
            if (_size >= _capacity) { grow(); }
            array[_size] = std::move(value);
            _size++; 
        }

        void pop_back() {
            _size = (_size == 0) ? 0 : _size - 1;
        }

        iterator insert(iterator pos, const T& value) { 
            size_t position = pos - begin(); 

            if (_size >= _capacity) { grow(); }

            for (size_t i = _size; i > position; i--) { 
                array[i] = std::move(array[i-1]);
            }

            array[position] = value;
            _size++;
            return begin() + position;
        }

        iterator insert(iterator pos, T&& value) {
            size_t position = pos - begin(); 

            if (_size >= _capacity) { grow(); }

            for (size_t i = _size; i > position; i--) { 
                array[i] = std::move(array[i-1]);
            }

            array[position] = std::move(value);
            _size++;
            return begin() + position;
        }

        iterator insert(iterator pos, size_t count, const T& value) {   
            if (count == 0) { return pos; }

            size_t position = pos - begin();

            while (_size + count > _capacity) { grow(); }

            for (size_t i = _size + count - 1; i > position + count - 1; i--) {
                array[i] = std::move(array[i-count]);
            }
            for (size_t i = position; i < position + count; i++) {
                array[i] = value;
            }

            _size += count;
            return begin() + position;
        }


        iterator erase(iterator pos) {
            iterator it = pos;
            _size--;

            while (it != end()) { 
                *(it) = std::move(*(it + 1)); 
                it++; 
            }

            return pos;
        }

        iterator erase(iterator first, iterator last) {
            int count = last - first;

            if (count <= 0) { return last; }
        
            iterator it = first;
            _size -= count;

            while (it != end()) { 
                *(it) = std::move(*(it + count)); 
                it++; 
            }

            return first;
        }

        void clear() noexcept {
            _size = 0;
        } 

    class iterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type        = T;
        using difference_type   = ptrdiff_t;
        using pointer           = T*;
        using reference         = T&;
    
    private:
        pointer ptr;

    public:
        iterator() : ptr(nullptr) {};

        iterator(pointer p) : ptr(p) {}; 
    
        iterator& operator=(const iterator&) noexcept = default;

        [[nodiscard]] reference operator*() const noexcept { return *ptr; }

        [[nodiscard]] pointer operator->() const noexcept { return ptr; }

        // Prefix Increment: ++a
        iterator& operator++() noexcept { ptr++; return *this; }

        // Postfix Increment: a++
        iterator operator++(int) noexcept { iterator result = *this; ptr++; return result; }

        // Prefix Decrement: --a
        iterator& operator--() noexcept { ptr--; return *this; }

        // Postfix Decrement: a--
        iterator operator--(int) noexcept { iterator result = *this; ptr--; return result; }

        iterator& operator+=(difference_type offset) noexcept { ptr += offset; return *this; }

        [[nodiscard]] iterator operator+(difference_type offset) const noexcept { iterator temp = *this; temp += offset; return temp; }
        
        iterator& operator-=(difference_type offset) noexcept { ptr -= offset; return *this; }

        [[nodiscard]] iterator operator-(difference_type offset) const noexcept { iterator temp = *this; temp -= offset; return temp; }

        [[nodiscard]] difference_type operator-(const iterator& rhs) const noexcept { return this->ptr - rhs.ptr; }

        [[nodiscard]] reference operator[](difference_type offset) const noexcept { return *(ptr + offset); }

        [[nodiscard]] bool operator==(const iterator& rhs) const noexcept { return this->ptr == rhs.ptr; }
        [[nodiscard]] bool operator!=(const iterator& rhs) const noexcept { return this->ptr != rhs.ptr; }
        [[nodiscard]] bool operator<(const iterator& rhs) const noexcept { return this->ptr < rhs.ptr; }
        [[nodiscard]] bool operator>(const iterator& rhs) const noexcept { return this->ptr > rhs.ptr; }
        [[nodiscard]] bool operator<=(const iterator& rhs) const noexcept { return this->ptr <= rhs.ptr; }
        [[nodiscard]] bool operator>=(const iterator& rhs) const noexcept { return this->ptr >= rhs.ptr; }
    };
};


// This ensures at compile time that the deduced argument _Iterator is a Vector<T>::iterator
// There is no way we know of to back-substitute template <typename T> for external functions
// because it leads to a non-deduced context
namespace {
    template <typename _Iterator>
    using is_vector_iterator = std::is_same<typename Vector<typename _Iterator::value_type>::iterator, _Iterator>;
}

template <typename _Iterator, bool _enable = is_vector_iterator<_Iterator>::value>
[[nodiscard]] _Iterator operator+(typename _Iterator::difference_type offset, _Iterator const& iterator) noexcept { return iterator + offset; }


#endif
