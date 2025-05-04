#include <algorithm>
#include <vector>

// https://leetcode.com/problems/sort-items-by-groups-respecting-dependencies/submissions/1620360743/

using namespace std;

namespace {
void dfs(int v, vector<int>& t, vector<bool>& seen, const vector<vector<int>>& g) {
    seen[v] = true;
    for (int u : g[v])
        if (!seen[u]) dfs(u, t, seen, g);
    t.push_back(v);
}

vector<int> topsort(const vector<vector<int>>& g) {
    vector<bool> seen(g.size());
    vector<int> t;

    for (int v = 0; v < g.size(); v++)
        if (!seen[v]) dfs(v, t, seen, g);
    reverse(t.begin(), t.end());
    return t;
}
}  // namespace

class Solution {
public:
    vector<int> sortItems(int n_items, int m_groups, vector<int> group,
                          const vector<vector<int>>& preceding_items) {
        int gcnt = *std::max_element(group.begin(), group.end()) + 1;
        for (int& g : group) {
            if (g == -1) g = gcnt++;
        }
        m_groups = gcnt;

        vector<vector<int>> item_g(n_items);
        vector<vector<int>> group_g(m_groups);

        for (int i = 0; i < n_items; ++i)
            for (int x : preceding_items[i])
                item_g[x].push_back(i);

        vector<vector<bool>> seen_groups(m_groups, vector<bool>(m_groups));
        for (int i = 0; i < n_items; ++i) {
            for (int next : item_g[i]) {
                if (group[next] != group[i] && !seen_groups[group[i]][group[next]]) {
                    seen_groups[group[i]][group[next]] = true;
                    group_g[group[i]].push_back(group[next]);
                }
            }
        }

        auto group_order = topsort(group_g);
        auto item_order = topsort(item_g);
        auto ordered_groups = vector<vector<int>>(m_groups);

        for (int i : item_order) {
            ordered_groups[group[i]].push_back(i);
        }

        vector<int> result;
        for (int g : group_order)
            for (int i : ordered_groups[g])
                result.push_back(i);

        vector<bool> seen_items(n_items);
        for (int i : result) {
            seen_items[i] = true;
            for (int j : preceding_items[i]) {
                if (!seen_items[j]) return {};
            }
        }

        return result;
    }
};
