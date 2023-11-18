#include <cstddef>
#include <iostream>

template <typename T>
void swap(T & a, T & b) { T temp = std::move(a); a = std::move(b); b = std::move(temp); }


template <typename T>
bool compare(const T& a, const T& b) { return a > b; }


// selection sort
template <typename T>
void selection_sort(T* array, size_t size) {
    size_t min;
    for (size_t i = 0; i < size - 1; i++) {
        min = i;
        for (size_t j = i + 1; j < size; j++) {
            if (compare(array[min], array[j])) { min = j; }
        }
        if (min != i) { swap(array[i], array[min]); }
    }
}


// insertion sort
template <typename T>
void insertion_sort(T* array, size_t size) {
    for (size_t i = 0; i < size - 1; i++) { 
        size_t j = i; 
        while (j >= 0 && compare(array[j], array[j + 1])) {
            swap(array[j], array[j + 1]); 
            j--; 
        }
    }
}


// bubble sort
template <typename T>
void bubble_sort(T* array, size_t size) { 
    bool swapped = true; 
    for (size_t i = 0; i < size - 1 && swapped; i++) {
        swapped = false;
        for (size_t j = 0; j < size - i - 1; j++) {
            if (compare(array[j], array[j + 1])) { 
                swap(array[j], array[j + 1]); 
                swapped = true; 
            }
        }
    }
}


// shell sort 
template <typename T> 
void shell_sort(T* array, size_t size) {
    for (size_t gap = size / 2; gap >= 1; gap /= 2) {
        for (size_t i = 0; i < size - gap; i++) {
            int j = i;
            while (j >= 0 && compare(array[j], array[j + gap])) {
                swap(array[j], array[j + gap]);
                j -= gap;
            }
        }
    }
}


// comb sort
template <typename T>
void comb_sort(T* array, size_t size) {
    size_t gap = size; 
    const double shrink = 1.3; 
    bool swapped = true;

    while (gap != 1 || swapped) {
        gap = (gap / shrink <= 1) ? 1 : gap / shrink;
        swapped = false;
        for (size_t j = 0; j < size - gap; j++) {
            if (compare(array[j], array[j + gap])) { 
                swap(array[j], array[j + gap]); 
                swapped = true; }
        }
    }
}


// merge sort
template <typename T>
void merge(T* array, int left, int mid, int right) {

    int left_size = mid - left + 1;
    int right_size = right - mid; 

    auto * left_array = new T[left_size];
    auto * right_array = new T[right_size];

    // move data to the temp arrays
    for (size_t i = 0; i < left_size; i++) {
        left_array[i] = std::move(array[left + i]);
    }

    for (size_t i = 0; i < right_size; i++) {
        right_array[i] = std::move(array[mid + 1 + i]);
    }

    int l = 0;           // index of left array
    int r = 0;           // index of right array
    int index = left;    // index of array


    // merge the temp arrays back into array[left ... right]
    while (l < left_size && r < right_size) { 

        if (left_array[l] <= right_array[r]) {
            array[index] = std::move(left_array[l]);
            l++;
        }
        else {
            array[index] = std::move(right_array[r]);
            r++;
        }
        index++;
    }

    while (l < left_size) {
        array[index] = std::move(left_array[l]);
        l++, index++;
    }

    while (r < right_size) {
        array[index] = std::move(right_array[r]);
        r++, index++;
    }

    delete [] left_array;
    delete [] right_array;
}

template <typename T>
void merge_sort(T* array, int left, int right) {
    // base case
    if (left >= right) { return; }

    size_t mid = (left + right) / 2;

    merge_sort(array, left, mid); 
    merge_sort(array, mid + 1, right);
    merge(array, left, mid, right); 
}


// quicksort
template <typename T>
int partition(T* array, int left, int right) {
    // we choose last element as pivot
    T* pivot = &(array[right]);
    int i = left - 1;

    for (int j = left; j <= right - 1; j++) {
        if (array[j] < *pivot) { 
            i++;
            if (i != j) { 
                swap(array[i], array[j]); 
            }
        }   
    }
    swap(array[i + 1], *pivot);

    return (i + 1);
}


template <typename T>
void quick_sort(T* array, int left, int right) {
    // base case
    if (left >= right) { return; }

    // element at index p will be in correct position after partition
    int p = partition(array, left, right);
   
    quick_sort(array, left, p - 1);
    quick_sort(array, p + 1, right);
}



int main() { 
    int* array = new int[10] {8,4,5,3,1,2,6,7,0,9};
 
    quick_sort(array, 0, 9);

    for (size_t i = 0; i < 10; i++) {
        std::cout << array[i] << " ";
    }

    std::cout << std::endl;
}
