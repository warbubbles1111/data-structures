#pragma once

#include <unordered_map>

#include "weighted-graph.hpp"
#include "graph-types.h"

template <typename T>
void computeIndegrees(const WeightedGraph<T>& graph, std::unordered_map<value_type<T>, int>& indegrees) {
    for (auto& item : graph) { 
        indegrees[item.first] = 0;
    }

    for (auto& item : graph) {
        for (auto& inner : item.second) {
            indegrees[inner.first] += 1;
        }
    }
}
