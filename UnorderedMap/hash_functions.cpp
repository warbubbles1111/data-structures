#include "hash_functions.h"

size_t polynomial_rolling_hash::operator() (std::string const & str) const {
    size_t p = 1;
    size_t hash = 0; 
    const int b = 19;
    const unsigned long int m = 3298534883309ul;

    for (char c : str) {
        hash += c * p;
        p = (p * b) % m; 
    }
    return hash;
}

size_t fnv1a_hash::operator() (std::string const & str) const {
    
    const unsigned long int prime = 0x00000100000001B3;
    const unsigned long int basis = 0xCBF29CE484222325;
    unsigned long int hash = basis;

    for (char c : str) {
        hash = hash ^ c;
        hash = hash * prime;
    }
    return size_t(hash);
}
