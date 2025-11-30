#include "graph.hh"
#include <cassert>

namespace {
AdjMatrix adjMatrixFromAdjList(const AdjList& adjList) {
    AdjMatrix res(adjList.size(), std::vector<std::optional<size_t>>(adjList.size()));

    for (size_t u = 0; u < adjList.size(); ++u) {
        for (const auto& [v, d] : adjList[u]) {
            res[u][v] = d;
        }
    }

    return res;
}
} // namespace

std::pair<AdjList, AdjMatrix> randomGraph(std::random_device& r, size_t nodes) {
    std::uniform_int_distribution<size_t> d(1, 1'000);

    AdjList g(nodes);
    for (size_t i = 0; i < nodes; ++i) {
        for (size_t j = i + 1; j < nodes; ++j) {
            size_t dist = d(r);
            g[i].emplace_back(j, dist);
            g[j].emplace_back(i, dist);
        }
    }

    return {g, adjMatrixFromAdjList(g)};
}