#include <functional>
#include <queue>
#include <limits>
#include <vector>
#include <algorithm>

using namespace std;

// tests
// Kaka's matrix travel (POJ)
// https://www.acmicpc.net/problem/8992
// SRM 506 mid
// ACM-ICPC World Finals 2015 C Catering

// precondition: there is no negative cycle.
// usage:
//  MinCostFlow mcf(n);
//  for(each edges) mcf.addEdge(from, to, cost, capacity);
//  mcf.solve(source, sink); // min cost max flow
//  mcf.solve(source, sink, 0); // min cost flow
//  mcf.solve(source, sink, goal_flow); // min cost flow with total_flow >= goal_flow if possible
struct MinCostFlow
{
	typedef int cap_t;
	typedef int cost_t;

	bool iszerocap(cap_t cap) { return cap == 0; }

	struct edge {
		int target;
		cost_t cost;
		cap_t residual_capacity;
		cap_t orig_capacity;
		size_t revid;
	};

	int n;
	vector<vector<edge>> graph;
	vector<cost_t> pi;
	bool needNormalize;

	MinCostFlow(int n) : graph(n), n(n), pi(n, 0), needNormalize(false) {}
	void addEdge(int s, int e, cost_t cost, cap_t cap)
	{
		if (s == e) return;
		edge forward={e, cost, cap, cap, graph[e].size()};
		edge backward={s, -cost, 0, 0, graph[s].size()};
		if (cost < 0) {
			needNormalize = true;
		}
		graph[s].emplace_back(forward);
		graph[e].emplace_back(backward);
	}

	bool normalize(int s) {
		auto infinite_cost = numeric_limits<cost_t>::max();
		vector<cost_t> dist(n, infinite_cost);
		dist[s] = 0;
		queue<int> q;
		vector<int> v(n), relax_count(n);
		v[s] = 1; q.push(s);
		while(!q.empty()) {
			int cur = q.front();
			v[cur] = 0; q.pop();
			if (++relax_count[cur] >= n) return false;
			for (const auto &e : graph[cur]) {
				if (iszerocap(e.residual_capacity)) continue;
				auto next = e.target;
				auto ncost = dist[cur] + e.cost;
				if (dist[next] > ncost) {
					dist[next] = ncost;
					if (v[next]) continue;
					v[next] = 1; q.push(next);
				}
			}
		}
		for (int i = 0; i < n; i++) pi[i] = dist[i];
		return true;
	}

	pair<cost_t, cap_t> augmentShortest(int s, int e, cap_t flow_goal) {
		auto infinite_cost = numeric_limits<cost_t>::max();
		auto infinite_flow = numeric_limits<cap_t>::max();
		typedef pair<cost_t, int> pq_t;
		priority_queue<pq_t, vector<pq_t>, greater<pq_t>> pq;
		vector<pair<cost_t, cap_t>> dist(n, make_pair(infinite_cost, 0));
		vector<int> from(n, -1), v(n);

		if (needNormalize) {
			normalize(s);
			needNormalize = false;
		}

		dist[s] = pair<cost_t, cap_t>(0, infinite_flow);
		pq.emplace(dist[s].first, s);
		while(!pq.empty()) {
			auto cur = pq.top().second; pq.pop();
			if (v[cur]) continue;
			v[cur] = 1;
			for (const auto &edge : graph[cur]) {
				auto next = edge.target;
				if (v[next]) continue;
				if (iszerocap(edge.residual_capacity)) continue;
				auto ncost = dist[cur].first + edge.cost - pi[next] + pi[cur];
				auto nflow = min(dist[cur].second, edge.residual_capacity);
				if (dist[next].first <= ncost) continue;
				dist[next] = make_pair(ncost, nflow);
				from[next] = edge.revid;
				pq.emplace(dist[next].first, next);
			}
		}
		/** augment the shortest path **/
		/* update potential, dist[i].first becomes invalid */
		for (int i = 0; i < n; i++) {
			if (iszerocap(dist[i].second)) continue;
			pi[i] += dist[i].first;
		}
		auto flow = dist[e].second;
		// e is unreachable
		if (iszerocap(flow)) return make_pair(0, 0);
		// reduced cost of augment path is 0, actual cost is potential difference
		auto pathcost = pi[e] - pi[s];
		// if it costs, we want minimum flow
		if (pathcost >= 0) flow = min(flow, flow_goal);
		if (flow <= 0 || iszerocap(flow)) return make_pair(0, 0);

		for (auto cur = e; from[cur] != -1; ) {
			auto back_edge = from[cur];
			auto prev = graph[cur][back_edge].target;
			auto forward_edge = graph[cur][back_edge].revid;
			graph[cur][back_edge].residual_capacity += flow;
			graph[prev][forward_edge].residual_capacity -= flow;
			cur = prev;
		}
		return make_pair(pathcost * flow, flow);
	}

	pair<cost_t,cap_t> solve(int s, int e, cap_t flow_goal = numeric_limits<cap_t>::max()) {
		cost_t total_cost = 0;
		cap_t total_flow = 0;
		for(;;) {
			auto res = augmentShortest(s, e, flow_goal - total_flow);
			if (res.second <= 0) break;
			total_cost += res.first;
			total_flow += res.second;
		}
		return make_pair(total_cost, total_flow);
	}
};
