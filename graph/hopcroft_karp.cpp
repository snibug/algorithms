// in: n, m, graph
// out: match
namespace HopcroftKarp
{
	const int MAX_N = 503, MAX_M = 503;
	int n, m;
	vector<int> graph[MAX_N];
	int match[MAX_N];

	int matched[MAX_M], depth[MAX_N], q[MAX_N];
	int v[MAX_N], vcnt;
	bool BFS(){
		int t = 0;
		for(int i = 0; i < n; i++)
			if (match[i] == -1) depth[i] = 0, q[t++] = i;
			else depth[i] = -1;
		for(int h = 0; h < t; h++) {
			int cur = q[h];
			int curDepth = depth[cur];
			for(int i = 0; i < graph[cur].size(); i++) {
				int adj = graph[cur][i];
				if (matched[adj] == -1)
					return true;
				int next = matched[adj];
				if (depth[next] != -1) continue;
				depth[next] = curDepth + 1, q[t++] = next;
			}
		}
		return false;
	}

	bool DFS(int nod) {
		v[nod] = vcnt;
		for(int i = 0; i < graph[nod].size(); i++) {
			int adj = graph[nod][i];
			int next = matched[adj];
			if (next >= 0 && (v[next] == vcnt || depth[next] != depth[nod] + 1))
				continue;
			if (next == -1 || DFS(next)) {
				match[nod] = adj;
				matched[adj] = nod;
				return true;
			}
		}
		return false;
	}

	int Match()
	{
		memset(match, -1, sizeof(match));
		memset(matched, -1, sizeof(matched));
		int ans = 0;
		while(BFS()) {
			++vcnt;
			for(int i = 0; i < n; i++) if (depth[i] == 0 && DFS(i)) ans++;
		}
		return ans;
	}
}

