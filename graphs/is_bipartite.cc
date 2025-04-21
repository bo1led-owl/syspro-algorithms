// https://leetcode.com/problems/is-graph-bipartite/submissions/1613108021/

#include <cassert>
#include <vector>

using namespace std;

class Solution {
    vector<int> colors_;

    static int switchColor(int color) { return (color == 1) ? 2 : 1; }

    bool internal(const vector<vector<int>>& g, int v, int color = 1) {
        assert(colors_[v] == 0);
        colors_[v] = color;
        bool res = true;
        for (auto u : g[v]) {
            if (colors_[u] == color) return false;
            if (colors_[u] == 0) { res &= internal(g, u, switchColor(color)); }
        }
        return res;
    }

public:
    bool isBipartite(vector<vector<int>>& graph) {
        colors_ = vector<int>(graph.size());
        for (int i = 0; i < graph.size(); ++i) {
            if (colors_[i] == 0) {
                if (!internal(graph, i)) { return false; }
            }
        }
        return true;
    }
};
