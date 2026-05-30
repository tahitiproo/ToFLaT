#ifndef TYPES_H
#define TYPES_H

#include <vector>

struct Edge {
    int u;
    int v;
    int len;
};

struct AppData {
    int n = 0;
    int m = 0;
    std::vector<Edge> edges;
    int k = 0;
    std::vector<int> capitals;
    std::vector<int> city_state;
};

#endif // TYPES_H
