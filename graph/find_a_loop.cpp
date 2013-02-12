/* iterative DFS loop finding algorithm (undirected graph)
 * 사용 방법: n, gnext, ginv 초기화. find_loop(탐색시작노드) */
#include <vector>

const int MAX_N = 100000;

int n;
vector<int> gnext[MAX_N];
vector<int> ginv[MAX_N];

int from[MAX_N];
int v[MAX_N],vcnt;

vector<int> find_loop(int beginNode) {
	++vcnt;
	vector<int> loop, s, par;
	s.push_back(beginNode);
	par.push_back(-1);
	while(!s.empty()) {
		int node = s.back(); s.pop_back();
		int parent_edge = par.back(); par.pop_back();
		v[node] = vcnt;
		for(int i = 0; i < gnext[node].size(); i++) {
			if (i == parent_edge) continue;
			int next = gnext[node][i];
			from[next] = node;
			if (v[next] == vcnt) {
				int p = next;
				do{
					loop.push_back(p);
					p = from[p];
				} while(p != next);
				return loop;
			}
			s.push_back(next);
			par.push_back(ginv[node][i]);
		}
	}
	return loop;
}

/* gnext, ginv 초기화 방법 - 순서가 중요하다. (loop edge가 있는 경우 고려) */
for(int i = 0; i < m; i++) {
	int a,b,v;
	scanf("%d%d%d",&a,&b,&v);
	ginv[b].push_back(gnext[a].size());
	gnext[a].push_back(b);
	gcost[a].push_back(v);
	ginv[a].push_back(gnext[b].size());
	gnext[b].push_back(a);
	gcost[b].push_back(v);
}

