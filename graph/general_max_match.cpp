// based on Tarjan, "Data Structures and Network Algorithms"
// time complexity: O(M |E| alpha(|V|,|E|)) where M = |maximum matching| <= |V|/2

// tested
// http://acm.timus.ru/problem.aspx?space=1&num=1099

#include <cstdio>
#include <tuple>
#include <queue>
#include <set>
#include <vector>
#include <algorithm>

using namespace std;

struct DisjointSet
{
	vector<int> parent, cnt;

	DisjointSet() { }

	DisjointSet(int n) : cnt(n, 1) {
		parent.reserve(n);
		for(int i = 0; i < n; i++) parent.push_back(i);
	}
	
	void reset(int sz) {
		parent.clear(); cnt.clear();
		for(int i = 0; i < sz; i++) {
			parent.push_back(i);
			cnt.push_back(1);
		}
	}

	void increase(int sz) {
		int base = parent.size();
		for(int i = base; i < base+sz; i++) {
			parent.push_back(i);
			cnt.push_back(1);
		}
	}

	int find(int p) {
		int np = p;
		while(p != parent[p]) p = parent[p];
		while(np != parent[np]) {
			int tmp = parent[np];
			parent[np] = p;
			np = tmp;
		}
		return p;
	}

	void merge(int a, int b) {
		a = find(a), b = find(b);
		if (cnt[a] > cnt[b]) {
			cnt[a] += cnt[b];
			parent[b] = a;
		} else {
			cnt[b] += cnt[a];
			parent[a] = b;
		}
	}
};

struct MaxMatching
{
	int n;
	vector<vector<int>> gnext;
	vector<int> matched;

	int vcnt;

	MaxMatching(int n) :
		n(n),
		gnext(n),
		matched(n, -1)
	{
	}
	void AddEdge(int a, int b) {
		gnext[a].push_back(b);
		gnext[b].push_back(a);
	}

	int Match() {
		int ans = 0;
		while(findAugment()) {
			ans++;
		}
		return ans;
	}

	vector<int> parent; // shrunken -> real
	vector<int> forest;
	vector<int> level;
	vector<pair<int,int>> bridge;
	queue<int> q;
	DisjointSet blossomSet;
	vector<int> origin; // blossomSet number to -> origin vertex
	vector<int> ancestorChecker;
	int ancestorCheckerValue;


	vector<int> marker;
	void markBlossomPath(int vv, pair<int,int> vu, int ancestor){
		int p = vv;
		marker.clear();
		while(p != ancestor) {
			int np;
			np = origin[blossomSet.find(parent[p])];
			marker.push_back(p);
			p = np;

			np = origin[blossomSet.find(parent[p])];
			marker.push_back(p);
			bridge[p] = vu; // need original vertex number
			q.push(p); // odd level edges were not considered
			p = np;
		}
		for(auto x : marker) blossomSet.merge(ancestor, x);
		origin[blossomSet.find(ancestor)] = ancestor;
	}

	void mergeBlossom(int vv, int uu, int v, int u){
		if (uu == vv) return;
		++ancestorCheckerValue;
		int p1 = uu, p2 = vv;
		int ancestor = -1;
		for(;;) {
			if (p1 >= 0) {
				if (ancestorChecker[p1] == ancestorCheckerValue) {
					ancestor = p1;
					break;
				}
				ancestorChecker[p1] = ancestorCheckerValue;
				if (parent[p1] >= 0) p1 = origin[blossomSet.find(parent[p1])]; else p1 = -1;
			}
			if (p2 >= 0) {
				if (ancestorChecker[p2] == ancestorCheckerValue) {
					ancestor = p2;
					break;
				}
				ancestorChecker[p2] = ancestorCheckerValue;
				if (parent[p2] >= 0) p2 = origin[blossomSet.find(parent[p2])]; else p2 = -1;
			}
		}
		markBlossomPath(uu, make_pair(u, v), ancestor);
		markBlossomPath(vv, make_pair(v, u), ancestor);
	}

	vector<int> augmentPathLink;

	void getRootPath(int v, int w, bool reversed){
		if (v == w) return;
		if (level[v] & 1) {
			// odd. use bridge
			int x, y, mate = matched[v];
			tie(x,y) = tie(bridge[v].first, bridge[v].second);
			getRootPath(x, mate, !reversed);
			getRootPath(y, w, reversed);
			if (reversed) {
				augmentPathLink[y] = x;
				augmentPathLink[mate] = v;
			} else {
				augmentPathLink[v] = mate;
				augmentPathLink[x] = y;
			}
		} else {
			// even
			int mate = matched[v];
			getRootPath(parent[mate], w, reversed);
			if (reversed) {
				augmentPathLink[parent[mate]] = mate;
				augmentPathLink[mate] = v;
			} else {
				augmentPathLink[v] = mate;
				augmentPathLink[mate] = parent[mate];
			}
		}
	}

	void augmentPath(int v, int w) {
		augmentPathLink = vector<int>(n,-1);
		int x = forest[v];
		int y = forest[w];
		getRootPath(v,x,true);
		getRootPath(w,y,false);
		augmentPathLink[v] = w;
		int p = x;
		for(;;) {
			int q = augmentPathLink[p];
			matched[p] = q;
			matched[q] = p;
			if (q == y) break;
			p = augmentPathLink[q];
		}
	}

	bool findAugment() {
		parent = vector<int>(n,-1);
		forest = vector<int>(n,-1);
		level = vector<int>(n);
		bridge = vector<pair<int,int>>(n,make_pair(-1,-1));
		q = queue<int>();
		blossomSet.reset(n);
		origin = vector<int>(n);
		ancestorChecker = vector<int>(n);
		ancestorCheckerValue = 0;

		for(int i = 0; i < n; i++) {
			origin[i] = i;
			if (matched[i] == -1) {
				forest[i] = i;
				q.push(i);
			}
		}
		bool foundPath = false;
		while(!q.empty() && !foundPath) {
			int v = q.front(); q.pop();
			for(auto u : gnext[v]) {
				int vv = origin[blossomSet.find(v)];
				int uu = origin[blossomSet.find(u)];
				if (forest[uu] == -1) {
					// assert(u == uu)
					parent[uu] = v;
					forest[uu] = forest[vv];
					level [uu] = level [vv] + 1;
					parent[matched[uu]] = uu;
					forest[matched[uu]] = forest[vv];
					level [matched[uu]] = level [vv] + 2;
					q.push(matched[uu]);
				} else if (level[uu]&1) {
					// odd level
				} else if (forest[uu] != forest[vv]){
					// found path. both are even level
					foundPath = true;
					augmentPath(v,u);
					break;
				} else {
					// blossom formed
					mergeBlossom(vv, uu, v, u);
				}
			}
		}

		return foundPath;
	}
};

