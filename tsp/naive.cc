#include "naive.hh"
#include "set.hh"
#include "util.hh"

namespace {
std::optional<size_t> naiveTsp(size_t u, const AdjList& adjList, Set& visited) {
    assert(!visited.contains(u));
    visited.insert(u);

    std::optional<size_t> res;

    if (visited.size() == adjList.size()) {
        auto it = std::find_if(adjList[u].begin(), adjList[u].end(),
                               [](const Edge& edge) { return edge.to == 0; });
        if (it != adjList[u].end()) {
            res = it->dist;
        }
    } else {
        for (const Edge& e : adjList[u]) {
            if (visited.contains(e.to)) {
                continue;
            }

            auto subRes = naiveTsp(e.to, adjList, visited);
            if (subRes.has_value()) {
                res = optMin(res, *subRes + e.dist);
            }
        }
    }

    visited.erase(u);
    return res;
}
} // namespace

std::optional<size_t> naiveTsp(const AdjList& adjList) {
    Set visited;
    return naiveTsp(0, adjList, visited);
}
