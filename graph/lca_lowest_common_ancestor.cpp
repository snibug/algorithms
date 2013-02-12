/* 사용법: 포레스트인 그래프가 있다고 가정. 
 * precalc_lca()의 인자로 트리의 루트로 삼을 것들이 들어간다. 
 * gnext를 채우고, precalc_lca(루트목록) 호출, 필요할 때마다
 * getlca(노드1,노드2) 호출. 반드시 노드1과 노드 2는 같은 
 * 트리에 들어있어야 한다. */

vector<int> gnext[MAX_N];

const int B_T = 262144; // at least (n * 2)
int lcaindex[MAX_N]; // node -> itree_index
int itree[B_T*2]; // itree_index -> visit_time
int itreeinv[MAX_N]; // visit_time -> node

int v[MAX_N], vcnt;

int getlca(int a, int b)
{
	a = lcaindex[a], b = lcaindex[b];
	if (a > b) swap(a,b);
	int ret = itree[a];
	while(a <= b) {
		if (  a & 1 ) ret = min(ret,itree[a]);
		if (!(b & 1)) ret = min(ret,itree[b]);
		a = (a+1)>>1;
		b = (b-1)>>1;
	}
	return itreeinv[ret];
}

void precalc_lca(vector<int> tree_roots)
{
	++vcnt;
	vector<int> s(tree_roots), // node stack
	            d(tree_roots.size()); // dummy check

	int visit_time = 0;
	int back = B_T;
	while(!s.empty()) {
		int curr = s.back(); s.pop_back();
		int dummy = d.back(); d.pop_back();
		if (dummy) {
			itree[back++] = itree[lcaindex[curr]];
			continue;
		}
		v[curr] = vcnt;
		lcaindex[curr] = back;
		itreeinv[visit_time] = curr;
		itree[back++] = visit_time++;
		for(int i = 0; i < gnext[curr].size(); i++) {
			int next = gnext[curr][i];
			if (v[next] == vcnt) continue;
			s.push_back(curr); d.push_back(1);
			s.push_back(next); d.push_back(0);
		}
	}

	for(int i = B_T-1; i > 0; i--)
		itree[i] = min(itree[i<<1], itree[(i<<1)+1]);
}

