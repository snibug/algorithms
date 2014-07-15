#include <vector>
#include <memory.h>
#include <limits>
#include <algorithm>
#include <functional>
#include <queue>

using namespace std;

// precondition: there is no negative cycle.
// usage:
//  MCMF::init(n);
//  for(each edges) MCMF::addEdge(from, to, capacity, cost);
//  if (negative cost edges were added) MCMF::normalizeNegativeEdges(source);
//  MCMF::solve(source, sink);
namespace MCMF
{
	typedef int cap_t;
	typedef int cost_t;

	bool isZeroCap(cap_t cap) {
		return cap == 0;
	}

	const cap_t CAP_MAX = numeric_limits<cap_t>::max();
	const cost_t COST_MAX = numeric_limits<cost_t>::max();

	struct edge_t {
		int target;
		cap_t cap;
		cost_t cost;
		int rev;
	};

	int n;
	vector<vector<edge_t>> graph;
	vector<cost_t> pi;

	vector<cost_t> dist;
	vector<cap_t> mincap;
	vector<int> from, v;

	void init(int _n) {
		n = _n;
		graph.clear(); graph.resize(n);
		pi.clear(); pi.resize(n);

		dist.resize(n);
		mincap.resize(n);
		from.resize(n);
		v.resize(n);
	}

	void addEdge(int a, int b, cap_t cap, cost_t cost) {
		edge_t forward = {b, cap, cost, (int)graph[b].size()};
		edge_t backward = {a, 0, -cost, (int)graph[a].size()};
		graph[a].push_back(forward);
		graph[b].push_back(backward);
	}

	// returns false if there is a negative cycle.
	bool normalizeNegativeEdges(int s){
		fill(dist.begin(),dist.end(),COST_MAX);
		memset(&v[0], 0, n*sizeof(v[0]));
		dist[s] = 0;
		queue<int> q;
		vector<int> relaxed(n);
		v[s] = 1; q.push(s);
		while(!q.empty()) {
			int cur = q.front();
			v[cur] = 0; q.pop();
			if (++relaxed[cur] >= n) return false;
			for(const auto &edge : graph[cur]) {
				if (isZeroCap(edge.cap)) continue;
				int next = edge.target;
				cost_t nextDist = dist[cur] + edge.cost;
				if (dist[next] > nextDist) {
					dist[next] = nextDist;
					if (v[next]) continue;
					v[next] = 1; q.push(next);
				}
			}
		}
		for(int i = 0; i < n; i++) pi[i] = dist[i];
		return true;
	}

	bool dijkstra(int s, int t) {
		typedef pair<cost_t, int> pq_t;
		priority_queue<pq_t, vector<pq_t>, greater<pq_t>> pq;
		fill(dist.begin(),dist.end(),COST_MAX);
		memset(&from[0], -1, n*sizeof(from[0]));
		memset(&v[0], 0, n*sizeof(v[0]));
		dist[s] = 0;
		mincap[s] = CAP_MAX;
		pq.emplace(dist[s], s);
		while(!pq.empty()) {
			int cur = pq.top().second; pq.pop();
			if (v[cur]) continue;
			v[cur] = 1;
			for (const auto &edge : graph[cur]) {
				int next = edge.target;
				if (v[next]) continue;
				if (isZeroCap(edge.cap)) continue;
				cost_t potCost = dist[cur] + edge.cost - pi[next] + pi[cur];
				if (dist[next] <= potCost) continue;
				dist[next] = potCost;
				mincap[next] = min(mincap[cur], edge.cap);
				from[next] = edge.rev;
				pq.emplace(dist[next], next);
			}
		}
		if (dist[t] == COST_MAX) return false;
		for(int i = 0; i < n; i++) {
			if (dist[i] == COST_MAX) continue;
			pi[i] += dist[i];
		}
		return true;
	}

	pair<cap_t, cost_t> solve(int source, int sink) {
		cap_t total_flow = 0;
		cost_t total_cost = 0;

		while(dijkstra(source, sink)) {
			cap_t f = mincap[sink];
			total_flow += f;
			for(int p = sink; p != source; ) {
				auto &backward = graph[p][from[p]];
				auto &forward = graph[backward.target][backward.rev];
				forward.cap -= f;
				backward.cap += f;
				total_cost += forward.cost * f;
				p = backward.target;
			}
		}

		return make_pair(total_flow, total_cost);
	}
}

