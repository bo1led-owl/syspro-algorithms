#include "bellmanHeldKarp.hh"

#include "set.hh"
#include "util.hh"

#include <algorithm>
#include <deque>

namespace {
class BellmanHeldKarp {
    size_t nodes_;
    const AdjMatrix& adjMatrix_;
    std::deque<Set> subsets_;
    std::vector<std::unordered_map<Set, size_t>> table_;

    void populateSets(size_t prevSize) {
        for (;;) {
            Set& prevSet = subsets_.front();
            if (prevSet.size() > prevSize) {
                break;
            }

            auto maxItemIt = std::max_element(prevSet.cbegin(), prevSet.cend());

            for (size_t i = *maxItemIt + 1; i < nodes_; ++i) {
                Set s(prevSet);
                s.insert(i);
                subsets_.emplace_back(std::move(s));
            }

            subsets_.pop_front();
        }
    }

    void processSubset(const Set& s) {
        for (size_t v : s) {
            if (v == 0) {
                continue;
            }

            std::optional<size_t> cur;

            Set sPrime(s);
            sPrime.erase(v);

            for (size_t w : sPrime) {
                if (!table_[w].contains(sPrime)) {
                    continue;
                }
                if (!adjMatrix_[v][w].has_value()) {
                    continue;
                }

                cur = optMin(cur, table_[w][sPrime] + *adjMatrix_[v][w]);
            }

            if (cur.has_value()) {
                table_[v].insert_or_assign(s, *cur);
            }
        }
    }

    std::optional<size_t> getResult() const {
        Set fullSet;
        for (size_t i = 0; i < nodes_; ++i) {
            fullSet.insert(i);
        }

        std::optional<size_t> res;
        for (size_t v = 1; v < nodes_; ++v) {
            auto edgeLen = adjMatrix_[v][0];
            if (!edgeLen.has_value()) {
                continue;
            }
            if (!table_[v].contains(fullSet)) {
                continue;
            }

            res = optMin(res, table_[v].at(fullSet) + *edgeLen);
        }
        return res;
    }

public:
    BellmanHeldKarp(const AdjMatrix& adjMatrix)
        : nodes_{adjMatrix.size()}, adjMatrix_{adjMatrix}, subsets_{Set{0}},
          table_(adjMatrix.size()) {
        table_[0][Set{0}] = 0;
    }

    std::optional<size_t> run() {
        for (size_t prevSize = 1; prevSize < nodes_; ++prevSize) {
            populateSets(prevSize);

            for (const Set& s : subsets_) {
                processSubset(s);
            }
        }

        return getResult();
    }
};
} // namespace

std::optional<size_t> bellmanHeldKarp(const AdjMatrix& adjMatrix) {
    return BellmanHeldKarp(adjMatrix).run();
}
