#include <cstdio>
#include <vector>
#include <algorithm>
#include <climits>

using namespace std;

/* usage

input: n, treeAdj
usage:
HLDecompose::Init();
HLDecompose::Decompose();

HLDecompose::RangeEdges(a, b) // query index tree (querie edge values between a~>b)
HLDecompose::RangeVertices(a, b) // query index tree (querie node values between a~>b (inclusive))

implement "clearIndexTree(), queryIndexTree(), querySingleIndexTree()"
*/
namespace HLDecompose
{
	typedef int val_t;

	const int MAX_N = 50000;
	const int LOG_MAX_N = 17;
	const int BT = 65536;

	int n;
	vector<int> treeAdj[MAX_N];

	int v[MAX_N], vcnt;

	int parentLCA[MAX_N][LOG_MAX_N];
	int depthLCA[MAX_N];

	// hl
	//    dfs related
	int sizeSub[MAX_N];
	int heavyChild[MAX_N];
	//    dfs related

	//    decomposition
	int groupNum[MAX_N]; // node -> hlgroup
	int indexInGroup[MAX_N]; // node -> index in group[groupNum[node]]
	int groupStart[MAX_N]; // hlgroup -> indexTree index
	vector<int> group[MAX_N]; // hlgroup -> nodes
	int groupcnt; // the number of hlgroups
	//    decomposition
	// end hl

	// indextree
	val_t itree[BT*2];
	// indextree

	int getLCA(int a, int b) {
		if (depthLCA[a] < depthLCA[b]) swap(a,b);
		int diff = depthLCA[a] - depthLCA[b];
		for(int i = 0; diff; i++) {
			if (diff & 1) a = parentLCA[a][i];
			diff >>= 1;
		}
		if (a == b) return a;
		for(int i = LOG_MAX_N-1; i >= 0; i--)
			if (parentLCA[a][i] != parentLCA[b][i])
				a = parentLCA[a][i], b = parentLCA[b][i];
		return parentLCA[a][0];
	}

	void dfs(int nod, int par = -1, int depth = 0) {
		v[nod] = vcnt;
		// lca
		depthLCA[nod] = depth;
		int prev = parentLCA[nod][0] = (par == -1 ? nod : par);
		for(int i = 1; i < LOG_MAX_N; i++)  prev = (parentLCA[nod][i] = parentLCA[prev][i-1]);
		// lca
		// hl decomposition
		int curSize = 1;
		int maxChild = -1, maxChildNode = -1;
		for(int i = 0; i < treeAdj[nod].size(); i++) {
			int next = treeAdj[nod][i];
			if (v[next] == vcnt) { continue; }
			dfs(next, nod, depth + 1);
			if (sizeSub[next] > maxChild) {
				maxChild = sizeSub[next];
				maxChildNode = next;
			}
			curSize += sizeSub[next];
		}
		heavyChild[nod] = maxChildNode;
		sizeSub[nod] = curSize;
		// hl decomposition
	}

	void decomposeHL(int nod, int curGroup) {
		for(; nod != -1; nod = heavyChild[nod]) {
			groupNum[nod] = curGroup;
			indexInGroup[nod] = group[curGroup].size();
			group[curGroup].push_back(nod);
			v[nod] = vcnt;
			for(int i = 0; i < treeAdj[nod].size(); i++) {
				int next = treeAdj[nod][i];
				if (v[next] == vcnt) continue;
				if (next == heavyChild[nod]) continue;
				decomposeHL(next, groupcnt++);
			}
		}
	}

	void updateRange(int exL, int inR, bool isEdgeRange) {
		while(groupNum[exL] != groupNum[inR]){
			int curGroup = groupNum[inR];
			int baseIndex = groupStart[curGroup];

			queryIndexTree(baseIndex, baseIndex + indexInGroup[inR]);

			int groupParentNode = parentLCA[group[curGroup][0]][0];
			if (isEdgeRange && groupParentNode == exL) { // 특수 케이스
				return;
			}
			inR = groupParentNode;
		}
		{
			int curGroup = groupNum[inR];
			int baseIndex = groupStart[curGroup];
			queryIndexTree(baseIndex + indexInGroup[exL] + (isEdgeRange ? 1 : 0), baseIndex + indexInGroup[inR]);
		}
	}
	
	void Init(){
		memset(v, 0, sizeof(v[0])*n);
		vcnt = 0;
		for(int i = 0; i < n; i++) {
			group[i].clear();
		}
		groupcnt = 0;
		clearIndexTree();
	}

	void Decompose() {
		++vcnt;
		for(int i = 0; i < n; i++) if (v[i] != vcnt) dfs(i);
		++vcnt;
		for(int i = 0; i < n; i++) if (v[i] != vcnt) decomposeHL(i, groupcnt++);
		for(int i = 0, totalSize = 0; i < groupcnt; i++) {
			groupStart[i] = totalSize;
			totalSize += group[i].size();
		}
	}
	
	void RangeEdges(int a, int b) {
		int lca = getLCA(a,b);
		updateRange(lca, a, true);
		updateRange(lca, b, true);
	}

	void RangeVertices(int a, int b) {
		int lca = getLCA(a,b);
		updateRange(lca, a, false); // include lca this time.
		updateRange(lca, b, true); // exclude lca this time.
	}

	void PinPointEdge(int a, int b) {
		int childNode = depthLCA[a] > depthLCA[b] ? a : b;
		int itreeIndex = groupStart[groupNum[childNode]] + indexInGroup[childNode];
		querySingleIndexTree(itreeIndex);
	}

	void PinPointVertex(int a) {
		int itreeIndex = groupStart[groupNum[a]] + indexInGroup[a];
		querySingleIndexTree(itreeIndex);
	}
}

int main(){
	return 0;
}

