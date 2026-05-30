#ifndef TYPES_H
#define TYPES_H

#include <vector>
#include <utility>

struct Edge {
    int u;
    int v;
    int w;
};

struct AppData {
    int n = 0;
    int m = 0;
    int total_weight = 0;
    int min_circuit_weight = 0;
    std::vector<Edge> original_edges;
    std::vector<std::pair<int, int>> duplicated_edges;
    std::vector<int> circuit;
    bool has_eulerian_circuit = true;
};

#endif // TYPES_H
