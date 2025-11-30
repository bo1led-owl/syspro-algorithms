#pragma once

#include "graph.hh"

#include <optional>

std::optional<size_t> bellmanHeldKarp(const AdjMatrix& adjMatrix);
