#include <vector>
#include <algorithm>
#include <limits>
#include <queue>

using namespace std;

// tests
//  SRM 334 Hard

// implementation of Stoer-Wagner algorithm
//usage
// MinCut mc;
// mc.init(n);
// for (each edge) mc.addEdge(a,b,weight);

// mincut = mc.solve();
// mc.cut = {0,1}^n describing which side the vertex belongs to.
struct MinCut
{
	typedef int cap_t;

	struct edge_t {
		int next;
		cap_t weight;
	};

	int n;
	vector<vector<edge_t>> graph;

	void init(int _n) {
		n = _n;
		graph.clear();
		graph.resize(n);
	}

	void addEdge(int a, int b, cap_t w) {
		edge_t forward = {b, w};
		edge_t backward = {a, w};
		graph[a].push_back(forward);
		graph[b].push_back(backward);
	}

	pair<cap_t, pair<int,int>> stMinCut(vector<int> &active) {
		vector<cap_t> key(n);
		vector<int> v(n);
		priority_queue<pair<int,int>> pq;
		pq.emplace(0, active[0]);
		int s = active[0], t = -1;
		while(!pq.empty()) {
			int cur = pq.top().second; pq.pop();
			if (v[cur]) continue;
			v[cur] = 1;
			t = s; s = cur;
			for(const auto &edge : graph[cur]) {
				int next = edge.next;
				if (v[next]) continue;
				key[next] += edge.weight;
				pq.emplace(key[next], next);
			}
		}
		return make_pair(key[s], make_pair(s,t));
	}

	vector<int> cut;

	struct remove_edge_pred {
		remove_edge_pred(int nod) : nod(nod){}
		int nod;
		bool operator ()(const edge_t &edge) {
			return edge.next == nod;
		}
	};

	cap_t solve() {
		cap_t res = numeric_limits<cap_t>::max();
		vector<vector<int>> grps;
		vector<int> active;
		cut.resize(n);
		for(int i = 0; i < n; i++) grps.emplace_back(1,i);
		for(int i = 0; i < n; i++) active.push_back(i);
		while(active.size() >= 2) {
			auto stcut = stMinCut(active);

			if (stcut.first < res) {
				res = stcut.first;
				fill(cut.begin(),cut.end(),0);
				for(auto v : grps[stcut.second.first]) cut[v] = 1;
			}

			int s = stcut.second.first, t = stcut.second.second;
			if (grps[s].size() < grps[t].size()) swap(s,t);

			active.erase(find(active.begin(),active.end(),t));
			grps[s].insert(grps[s].end(), grps[t].begin(), grps[t].end());

			graph[s].resize(remove_if(graph[s].begin(), graph[s].end(), remove_edge_pred(t)) - graph[s].begin());
			for(auto &edge : graph[t]) if (edge.next != s) graph[s].push_back(edge);
			for(auto v : active) for(auto &edge : graph[v]) if (edge.next == t) edge.next = s;
		}
		return res;
	}
};


struct MinCutMatrix
{
	typedef int cap_t;

	int n;
	vector<vector<cap_t>> graph;

	void init(int _n) {
		n = _n;
		graph = vector<vector<cap_t>>(n, vector<cap_t>(n, 0));
	}

	void addEdge(int a, int b, cap_t w) {
		if (a == b) return;
		graph[a][b] += w;
		graph[b][a] += w;
	}

	pair<cap_t, pair<int,int>> stMinCut(vector<int> &active) {
		vector<cap_t> key(n);
		vector<int> v(n);
		int s = -1, t = -1;
		for(int i = 0; i < active.size(); i++) {
			cap_t maxv = -1;
			int cur = -1;
			for(auto j : active) {
				if (v[j] == 0 && maxv < key[j]) {
					maxv = key[j];
					cur = j;
				}
			}
			t = s; s = cur;
			v[cur] = 1;
			for(auto j : active) key[j] += graph[cur][j];
		}
		return make_pair(key[s], make_pair(s,t));
	}

	vector<int> cut;

	cap_t solve() {
		cap_t res = numeric_limits<cap_t>::max();
		vector<vector<int>> grps;
		vector<int> active;
		cut.resize(n);
		for(int i = 0; i < n; i++) grps.emplace_back(1,i);
		for(int i = 0; i < n; i++) active.push_back(i);
		while(active.size() >= 2) {
			auto stcut = stMinCut(active);

			if (stcut.first < res) {
				res = stcut.first;
				fill(cut.begin(),cut.end(),0);
				for(auto v : grps[stcut.second.first]) cut[v] = 1;
			}

			int s = stcut.second.first, t = stcut.second.second;
			if (grps[s].size() < grps[t].size()) swap(s,t);

			active.erase(find(active.begin(),active.end(),t));
			grps[s].insert(grps[s].end(), grps[t].begin(), grps[t].end());
			for(int i = 0; i < n; i++) {graph[i][s] += graph[i][t]; graph[i][t] = 0;}
			for(int i = 0; i < n; i++) {graph[s][i] += graph[t][i]; graph[t][i] = 0;}
			graph[s][s] = 0;
		}
		return res;
	}
};
