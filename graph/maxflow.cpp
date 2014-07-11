#include <vector>
#include <climits>
#include <algorithm>
#include <cstdio>
#include <memory.h>
#include <limits>
#include <queue>

using namespace std;

// usage:
// MaxFlowDinic::Init(n);
// MaxFlowDinic::AddEdge(0, 1, 100, 100); // for bidirectional edge
// MaxFlowDinic::AddEdge(1, 2, 100); // directional edge
// result = MaxFlowDinic::Solve(0, 2); // source -> sink
// graph[i][edgeIndex].res -> residual
struct MaxFlowDinic
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

	vector<unsigned> q, l, start;

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
		Edge forward = { e, graph[e].size(), cap };
		Edge reverse = { s, graph[s].size(), caprev };
		graph[s].push_back(forward);
		graph[e].push_back(reverse);
	}

	bool AssignLevel(int source, int sink) {
		int t = 0;
		memset(&l[0], 0, sizeof(l[0]) * l.size());
		l[source] = 1;
		q[t++] = source;
		for(int h = 0; h < t && !l[sink]; h++) {
			int cur = q[h];
			for(unsigned i = 0; i < graph[cur].size(); i++) {
				int next = graph[cur][i].next;
				if (l[next]) continue;
				if (graph[cur][i].res > 0) {
					l[next] = l[cur] + 1;
					q[t++] = next;
				}
			}
		}
		return l[sink] != 0;
	}

	flow_t BlockFlow(int cur, int sink, flow_t currentFlow) {
		if (cur == sink) return currentFlow;
		for(unsigned &i = start[cur]; i < graph[cur].size(); i++) {
			int next = graph[cur][i].next;
			if (graph[cur][i].res == 0 || l[next] != l[cur]+1)
				continue;
			if (flow_t res = BlockFlow(next, sink, min(graph[cur][i].res, currentFlow))) {
				int inv = graph[cur][i].inv;
				graph[cur][i].res -= res;
				graph[next][inv].res += res;
				return res;
			}
		}
		return 0;
	}

	flow_t Solve(int source, int sink)
	{
		q.resize(n);
		l.resize(n);
		start.resize(n);
		flow_t ans = 0;
		while(AssignLevel(source,sink)) {
			memset(&start[0], 0, sizeof(start[0])*n);
			while(flow_t flow = BlockFlow(source,sink,numeric_limits<flow_t>::max())) {
				ans += flow;
			}
		}
		return ans;
	}
};

int main()
{
	int n,m;
	scanf("%d%d",&n,&m);
	MaxFlowDinic mf;
	mf.Init(n);
	for(int i = 0; i < m; i++) {
		int a,b,c;
		scanf("%d%d%d",&a,&b,&c);
		mf.AddEdge(a-1,b-1,c,c);
	}
	printf("%lld\n", mf.Solve(0,n-1));
	return 0;
}
