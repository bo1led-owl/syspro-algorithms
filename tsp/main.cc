#include <cassert>
#include <chrono>
#include <future>
#include <iomanip>
#include <iostream>

#include "bellmanHeldKarp.hh"
#include "graph.hh"
#include "naive.hh"
#include "util.hh"

using namespace std::chrono_literals;

namespace {
constexpr size_t NODES_MIN = 5;
constexpr size_t NODES_MAX = 20;
constexpr size_t NODES_STEP = 1;
constexpr std::chrono::duration TIMEOUT = 4s;

using clock = std::chrono::high_resolution_clock;

template <typename T>
std::optional<T> waitOrTimeout(std::future<T>& f, std::chrono::time_point<clock> limit) {
    switch (std::future_status status = f.wait_until(limit); status) {
    case std::future_status::ready:
        return f.get();
    case std::future_status::timeout:
        return std::nullopt;
    case std::future_status::deferred:
        std::terminate();
        break;
    }
}

template <typename F, typename... Args> auto runMeasured(F f, Args&&... args) {
    std::promise<std::pair<std::optional<size_t>, double>> p;
    auto future = p.get_future();
    std::thread(
        [f, &args...](std::promise<std::pair<std::optional<size_t>, double>> p) {
            auto start = clock::now();
            auto res = f(std::forward<Args>(args)...);
            auto end = clock::now();

            double time =
                std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1e6;

            p.set_value_at_thread_exit({res, time});
        },
        std::move(p))
        .detach();
    return future;
}

bool measure(const AdjList& adjList, const AdjMatrix& adjMatrix) {
    auto limit = clock::now() + TIMEOUT;

    auto naiveFuture = runMeasured(naiveTsp, adjList);
    auto bhkFuture = runMeasured(bellmanHeldKarp, adjMatrix);

    auto naive = waitOrTimeout(naiveFuture, limit);
    auto bhk = waitOrTimeout(bhkFuture, limit);

    std::cout << "N = " << adjList.size() << '\n';
    if (bhk.has_value() && naive.has_value() && bhk->first != naive->first) {
        std::cout << "answers differ: bhk produced " << bhk->first << " while naive produced "
                  << naive->first << '\n';
    }
    std::cout << std::fixed;

    std::cout << "  bhk: ";
    if (!bhk.has_value()) {
        std::cout << "TIMEOUT\n";
    } else {
        std::cout << std::setw(8) << bhk->second << "s\n";
    }

    std::cout << "  naive: ";
    if (!naive.has_value()) {
        std::cout << "TIMEOUT\n";
    } else {
        std::cout << std::setw(8) << naive->second << "s\n";
    }
    std::cout.flush();

    return naive.has_value() || bhk.has_value();
}

std::vector<std::pair<AdjList, AdjMatrix>> setupGraphs() {
    std::random_device r{};
    std::vector<std::pair<AdjList, AdjMatrix>> res;

    for (size_t i = NODES_MIN; i <= NODES_MAX; i += NODES_STEP) {
        res.emplace_back(randomGraph(r, i));
    }

    return res;
}
} // namespace

int main() {
    auto graphs = setupGraphs();

    for (const auto& [adjList, adjMatrix] : graphs) {
        if (!measure(adjList, adjMatrix)) {
            break;
        }
    }
}
