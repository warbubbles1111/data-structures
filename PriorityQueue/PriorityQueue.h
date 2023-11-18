#pragma once

#include <functional>
#include <utility>
#include <vector>

template <class T, class Container = std::vector<T>, class Compare = std::less<T>>
class PriorityQueue {
public:
    using value_compare = Compare;
    using value_type = T;
    using size_type = size_t;
    using reference = T&;
    using const_reference = const T&;

private:
    Container c;

    value_compare comp;

    size_type parent(size_type index) { return (index - 1) / 2; }

    size_type left_child(size_type index) { return 2 * index + 1; }

    size_type right_child(size_type index) { return 2 * (index + 1); }

    bool is_internal(size_t index) { return left_child(index) < c.size(); }

    bool is_leaf(size_t index) { return left_child(index) >= c.size(); }

    
    /**
     * @brief Recursively (or Iteratively) swap the value at index up the heap until it is in the correct position.
     * @param index the current position to swap upwards
     */
    void upheap(size_type index) {
        reference parent_val = c[parent(index)];
        reference val = c[index];

        if (index > 0 and comp(parent_val, val)) {
            std::swap(parent_val, val);
            upheap(parent(index));
        }
    }
    
    /**
     * @brief Recursively swap the value at index down the heap until it is in the correct position.
     * @param index the current position to swap downwards
     */
    void downheap(size_type index) {
        if (is_leaf(index)) { return; }

        size_type max_child_index;

        if (right_child(index) < c.size()) {
            if (comp(c[right_child(index)], c[left_child(index)])) {
                max_child_index = left_child(index);
            }
            else {
                max_child_index = right_child(index);
            }
        }
        else {
            max_child_index = left_child(index);
        }

        reference val = c[index];
        reference child_val = c[max_child_index];

        if (comp(val, child_val)) {
            std::swap(val, child_val);
            downheap(max_child_index);
        }
    }

public:
    PriorityQueue() = default;
    PriorityQueue(const PriorityQueue& other) = default;
    PriorityQueue(PriorityQueue&& other) = default;
    ~PriorityQueue() = default;
    PriorityQueue& operator=(const PriorityQueue& other) = default;
    PriorityQueue& operator=(PriorityQueue&& other) = default;

    /**
     * @brief Return a const reference to the element at the top of the heap.
     * @return const_reference to the element at the top of the heap.
     */
    const_reference top() const { return c.front(); }

    /**
     * @brief Return whether the heap is empty. This is the same as whether the underlying container, c, is empty.
     * @return true c is empty; false otherwise
     */
    bool empty() const { return c.empty(); }

    /**
     * @brief Return the number of elements in the heap. This is the same as the number of elements in the underlying container, c.
     * @return size_type of the number of elements in the heap
     */
    size_type size() const { return c.size(); }
	
    /**
     * @brief Insert element and sorts the underlying container, c;
     * @param value inserted by copying into c 
     */
    void push(const value_type& value) {
        c.push_back(value);
        upheap(c.size() - 1);
    }

    /**
     * @brief Insert element and sorts the underlying container, c
     * @param value inserted by moving into c 
     */
	void push(value_type&& value) {
        c.push_back(std::move(value));
        upheap(c.size() - 1);
    }

    /**
     * @brief Remove the top element
     */
    void pop() {
        c.front() = std::move(c.back()); 
        c.pop_back();
        downheap(0);
    }
};