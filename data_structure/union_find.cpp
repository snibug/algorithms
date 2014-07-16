#include <vector>
#include <algorithm>

using namespace std;

namespace UF{
	vector<int> parent, grpsize;

	void init(int n){
		parent.clear();
		for(int i = 0; i < n; i++) parent.push_back(i);
		grpsize.clear();
		for(int i = 0; i < n; i++) grpsize.push_back(1);
	}

	int getRoot(int a) {
		int p = a;
		while(p != parent[p]) p = parent[p];
		while(a != parent[a]) {
			int next = parent[p];
			parent[p] = p;
			a = next;
		}
		return p;
	}

	void merge(int a, int b) {
		a = getRoot(a), b = getRoot(b);
		if (grpsize[a] > grpsize[b]) {
			swap(a,b);
		}
		grpsize[b] += grpsize[a];
		par[a] = b;
	}
}
