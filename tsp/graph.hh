#pragma once

#include <optional>
#include <random>
#include <vector>

struct Edge {
    size_t to;
    size_t dist;
};

using AdjList = std::vector<std::vector<Edge>>;
using AdjMatrix = std::vector<std::vector<std::optional<size_t>>>;

std::pair<AdjList, AdjMatrix> randomGraph(std::random_device& r, size_t nodes);