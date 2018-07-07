#include <vector>
#include <limits>
#include <utility>
#include <queue>
#include <functional>

using namespace std;

struct Dijkstra {
  typedef long long dist_t;

  vector<dist_t> distance;
  vector<int> parent;

  template<typename TEdge> /* member: target, dist */
  void solve(const vector<vector<TEdge>> &graph, int start) {
    int n = graph.size();
    distance.assign(n, numeric_limits<dist_t>::max());
    parent.assign(n, -1);
    typedef pair<dist_t, int> queue_t;
    priority_queue<queue_t, vector<queue_t>, greater<queue_t>> pq;
    distance[start] = 0;
    parent[start] = -1;
    pq.push(make_pair(0, start));
    while (!pq.empty()) {
      int cur;
      dist_t cur_dist;
      tie(cur_dist, cur) = pq.top(); pq.pop();
      if (cur_dist != distance[cur]) continue;
      for (const auto &edge : graph[cur]) {
        int next = edge.target;
        dist_t new_dist = cur_dist + edge.dist;
        if (distance[next] <= new_dist) continue;
        distance[next] = new_dist;
        parent[next] = cur;
        pq.push(make_pair(new_dist, next));
      }
    }
  }

  vector<int> getPath(int v) {
    vector<int> ans;
    if (distance[v] == numeric_limits<dist_t>::max()) {
      return ans;
    }
    do {
      ans.push_back(v);
      v = parent[v];
    } while (v >= 0);
    reverse(ans.begin(), ans.end());
    return ans;
  }
};


