#include <vector>
#include <queue>
#include <algorithm>

using namespace std;


/* 
 * requirement:
 * - edge_t { int next; int id; } next는 연결된 반대편 vertex이고, id는 edge에 매겨진 번호이다.
 * - edge.id >= 0
 */
template<typename edge_t>
struct BridgeAndCut
{
	typedef vector<vector<edge_t>> graph_t;
	const graph_t *graph;

	BridgeAndCut() : graph(nullptr) {}
	BridgeAndCut(const graph_t &graph) : graph(&graph) {}

	int vtime;
	vector<int> visit;
	vector<int> up;

	vector<int> bridges;
	vector<int> cuts;

	void rec(int cur, int parent_edge = -1) {
		up[cur] = visit[cur] = ++vtime;
		int childcnt = 0; // for cut
		for (const auto &edge : (*graph)[cur]) {
			if (edge.id == parent_edge)
				continue;
			int next = edge.next;
			if (visit[next] == 0) {
				childcnt++;
				rec(next, edge.id);
				if (up[next] == visit[next])
					bridges.push_back(edge.id); // for bridge
				else
					up[cur] = min(up[cur], up[next]);
			}
			else if (visit[next] < visit[cur]) {
				/* back edge */
				up[cur] = min(up[cur], visit[next]);
			}
		}
		// for cut
		if ( (parent_edge == -1 && childcnt >= 2) ||
			(parent_edge != -1 && childcnt >= 1 && up[cur] == visit[cur]))
			cuts.push_back(cur);
	}

	void nonrec(int root) {
		struct state_t {
			int cur;
			int parent_edge;
			int edge_index;
			int child_cnt;
		};
		vector<state_t> stack;
		stack.emplace_back(state_t{ root, -1, -1, 0 });
		while (!stack.empty()) {
			state_t &s = stack.back();
			int cur = s.cur;
			const auto &adj_list = (*graph)[cur];
			if (s.edge_index == -1) {
				up[cur] = visit[cur] = ++vtime;
				s.edge_index = 0;
				continue;
			}
			else if (s.edge_index < (int)adj_list.size()) {
				const auto &edge = adj_list[s.edge_index];
				if (edge.id == s.parent_edge) {
					s.edge_index++;
					continue;
				}
				int next = edge.next;
				if (visit[next] == 0) {
					s.child_cnt++;
					stack.emplace_back(state_t{ next, edge.id, -1, 0 });
					continue;
				}
				else if (visit[next] < visit[cur]) {
					up[cur] = min(up[cur], visit[next]);
					s.edge_index++;
					continue;
				}
			}
			else if (s.edge_index >= (int)adj_list.size()) {
				int parent_edge = s.parent_edge;
				if (parent_edge == -1) {
					if (s.child_cnt >= 2)
						cuts.push_back(cur);
				} else {
					if (s.child_cnt >= 1 && up[cur] == visit[cur])
						cuts.push_back(cur);
				}
				stack.pop_back();
				if (!stack.empty()) {
					if (up[cur] == visit[cur])
						bridges.push_back(parent_edge);
					else
						up[stack.back().cur] = min(up[stack.back().cur], up[cur]);
					stack.back().edge_index++;
				}
			}
		}
	}

	void find(bool recursive = true) {
		int n = (int)graph->size();
		visit.assign(n, 0);
		up.assign(n, 0);
		vtime = 0;
		bridges.clear();
		cuts.clear();
		for (int cur = 0; cur < n; cur++) {
			if (visit[cur] != 0)
				continue;
			if (recursive)
				rec(cur);
			else
				nonrec(cur);
		}
	}
};

struct edge_t {
	int next;
	int id;
};

int main()
{
	for (;;) {
		int n = rand()%100;
		int m = rand()%300;
		vector<vector<edge_t>> graph(n);
		for (int i = 0; i < m; i++){
			int a = rand()%n;
			int b = rand()%n;
			graph[a].emplace_back(edge_t{b, i});
			graph[b].emplace_back(edge_t{a, i});
		}
		BridgeAndCut<edge_t> bc(graph);
		bc.find(false);
		vector<int> brg = bc.bridges, cut = bc.cuts;
		bc.find(true);
		if (brg != bc.bridges) {
			printf("Bridge differ\n");
		}
		if (cut != bc.cuts) {
			printf("cut differ\n");
		}
	}
	return 0;
}
