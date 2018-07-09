#include <algorithm>
#include <climits>
#include <memory.h>
#include <vector>
#include <queue>

using namespace std;
// flow_t가 정수형인 경우에만
struct MaxFlowScalingDinic
{
	typedef long long flow_t;

	struct Edge
	{
		int next;
		int inv; /* inverse edge index */
		flow_t res; /* residual */
	};

	int n;
	vector<vector<Edge>> graph;
	flow_t maxCap;

	vector<unsigned> q, l, start;
	int vcnt;

	void Init(int _n){
		n = _n;
		graph.resize(n);
		maxCap = 0;
		vcnt = 0;
		for(int i = 0; i < n; i++) graph[i].clear();
	}
	void AddNodes(int count) {
		n += count;
		graph.resize(n);
	}
	void AddEdge(int s, int e, flow_t cap, flow_t caprev = 0) {
		Edge forward = { e, graph[e].size() + (s == e ? 1 : 0), cap };
		Edge reverse = { s, graph[s].size(), caprev };
		maxCap = max(maxCap, cap);
		graph[s].push_back(forward);
		graph[e].push_back(reverse);
	}

	bool AssignLevel(int source, int sink, flow_t limit) {
		int t = 0;
		memset(&l[0], 0, sizeof(l[0]) * l.size());
		l[source] = 1;
		q[t++] = source;
		for(int h = 0; h < t && !l[sink]; h++) {
			int cur = q[h];
			for(unsigned i = 0; i < graph[cur].size(); i++) {
				int next = graph[cur][i].next;
				if (l[next]) continue;
				if (graph[cur][i].res >= limit) {
					l[next] = l[cur] + 1;
					q[t++] = next;
				}
			}
		}
		return l[sink] != 0;
	}

	flow_t BlockFlow(int cur, int sink, flow_t limit) {
		flow_t sum = 0;
		if (cur == sink) return limit;
		for(unsigned &i = start[cur]; i < graph[cur].size(); i++) {
			int next = graph[cur][i].next;
			if (graph[cur][i].res < limit || l[next] != l[cur]+1)
				continue;
			flow_t res = BlockFlow(next, sink, limit);
			if (res == 0) continue;
			int inv = graph[cur][i].inv;
			graph[cur][i].res -= limit;
			graph[next][inv].res += limit;
			if (l[cur] > 1) return res;
			sum += res;
		}
		return sum;
	}

	flow_t Solve(int source, int sink)
	{
		q.resize(n);
		l.resize(n);
		start.resize(n);
		flow_t scaleValue = 1, ans = 0;
		for(flow_t div = maxCap; div>1; div >>= 1, scaleValue <<= 1);
		while(scaleValue > 0) {
			while(AssignLevel(source,sink,scaleValue)) {
				ans += BlockFlow(source,sink,scaleValue);
			}
			scaleValue >>= 1;
		}
		return ans;
	}
};

struct MaxFlowPushRelabel
{
	typedef long long flow_t;

	struct Edge
	{
		int next;
		int inv; /* inverse edge index */
		flow_t res; /* residual */
	};

	int n;
	vector<vector<Edge>> graph;
	vector<int> L;

	vector<int> height, curedge;
	vector<flow_t> excess;

	// global relabel related
	vector<int> v; int vcnt;
	queue<int> q;
	int sink, relabelCount;

	void Init(int _n){
		n = _n;
		graph.resize(n);

		for(int i = 0; i < n; i++) graph[i].clear();
	}
	void AddNodes(int count) {
		n += count;
		graph.resize(n);
	}
	void AddEdge(int s, int e, flow_t cap, flow_t caprev = 0) {
		Edge forward = { e, graph[e].size() + (s == e ? 1 : 0), cap };
		Edge reverse = { s, graph[s].size(), caprev };
		graph[s].push_back(forward);
		graph[e].push_back(reverse);
	}
	void globalRelabel() {
		L.clear();
		++vcnt;
		v[sink] = vcnt;
		height[sink] = 0;
		q.push(sink);
		while(!q.empty()){
			int cur = q.front(); q.pop();
			for(unsigned i = 0; i < graph[cur].size(); i++) {
				int next = graph[cur][i].next;
				if (v[next] == vcnt) continue;
				int inv = graph[cur][i].inv;
				flow_t res = graph[next][inv].res;
				if (res == 0) continue;
				v[next] = vcnt;
				height[next] = height[cur] + 1;
				q.push(next);
				L.push_back(next);
			}
		}
		for(int i = 0; i < n; i++) {
			if (v[i] != vcnt) {
				v[i] = vcnt;
				height[i] = n+1;
			}
		}
		reverse(L.begin(),L.end());
	}
	bool push(int v, int edgeIndex) {
		int next = graph[v][edgeIndex].next, invIndex = graph[v][edgeIndex].inv;
		flow_t residual = graph[v][edgeIndex].res;
		if (residual <= 0 || height[v] <= height[next]) return false;
		flow_t rem = min(excess[v], residual);
		graph[v][edgeIndex].res -= rem;
		graph[next][invIndex].res += rem;
		excess[v] -= rem;
		excess[next] += rem;
		return true;
	}
	bool relabel(int v) {
		if (++relabelCount == n) {
			globalRelabel();
			relabelCount = 0;
			return true;
		}
		int minH = INT_MAX;
		for(unsigned i = 0; i < graph[v].size(); i++) {
			if (graph[v][i].res > 0) {
				int next = graph[v][i].next;
				minH = min(minH, height[next]);
				height[v] = minH+1;
			}
		}
		return false;
	}
	bool discharge(int v) {
		bool globalRelabeled = false;
		int edgeCnt = graph[v].size();
		for(; excess[v] > 0;) {
			if (curedge[v] < edgeCnt) {
				if (!push(v, curedge[v])) {
					curedge[v]++;
				}
			} else {
				globalRelabeled |= relabel(v);
				curedge[v] = 0;
			}
		}
		return globalRelabeled;
	}
	flow_t Solve(int source, int sink) {
		height.clear(); height.resize(n);
		excess.clear(); excess.resize(n);
		curedge.clear(); curedge.resize(n);
		v.clear(); v.resize(n); vcnt = 0;
		this->sink = sink;
		height[source] = n;
		for(unsigned i = 0; i < graph[source].size(); i++) {
			excess[source] = graph[source][i].res;
			push(source, i);
		}
		globalRelabel();
		for(unsigned i = 0; i < L.size(); i++) {
			int v = L[i];
			int oldH = height[v];
			if (oldH >= n+1) continue;
			if (discharge(v)) {
				i = -1;
				continue;
			}
			if (height[v] != oldH) {
				memmove(&L[1], &L[0], sizeof(L[0])*i);
				L[0] = v;
				i = 0;
			}
		}
		return excess[sink];
	}
};

