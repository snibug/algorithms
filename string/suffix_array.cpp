#include <cstdio>
#include <numeric>
#include <cstring>
#include <algorithm>
#include <vector>

using namespace std;
template<typename Ty>
struct SuffixArray
{
	vector<Ty> in;
	vector<int> out;
	template<typename Pt>
		SuffixArray(Pt begin, Pt end) : in(begin, end) {}

	vector<int> build() {
		int n = (int)in.size(), c = 0;
		vector<int> temp(n), pos2bckt(n), bckt(n), bpos(n);
		out.resize(n);
		for (int i = 0; i < n; i++) out[i] = i;
		sort(out.begin(), out.end(), [&](int a,int b) { return in[a] < in[b];});
		for(int i = 0; i < n; i++) { bckt[i] = c; if (i+1 == n || in[out[i]] != in[out[i+1]]) c++; }
		for(int h = 1; h < n && c < n; h <<= 1) {
			for (int i = 0; i < n; i++) pos2bckt[out[i]] = bckt[i];
			for (int i = n-1; i>=0; i--) bpos[bckt[i]] = i;
			for (int i = 0; i < n; i++) if (out[i] >= n-h) temp[bpos[bckt[i]]++] = out[i];
			for (int i = 0; i < n; i++) if (out[i] >= h) temp[bpos[pos2bckt[out[i]-h]]++] = out[i]-h;
			c = 0;
			for (int i = 0; i + 1 < n; i++) {
				int a = (bckt[i] != bckt[i+1]) || (temp[i] >= n-h) || pos2bckt[temp[i+1]+h] != pos2bckt[temp[i]+h];
				bckt[i] = c;
				c += a;
			}
			bckt[n-1] = c++;
			temp.swap(out);
		}
		return out;
	}

	vector<int> lcparray() {
		if (in.size() != out.size()) build();
		int n = (int)in.size();
		if (n == 0)
			return vector<int>();
		vector<int> rank(n); // temporary
		vector<int> height(n-1); // output lcp array

		for(int i = 0; i < n; i ++) rank[out[i]] = i;
		int h = 0;
		for(int i = 0; i < n; i ++) {
			if(rank[i] == 0) continue;
			int j = out[rank[i]-1];
			while(i+h < n && j+h < n && in[i+h] == in[j+h]) h++;
			height[rank[i]-1] = h;
			if(h > 0) h--;
		}
		return height;
	}
};


#include <cstdlib>
#include <ctime>
#include <string>

int main(){
	string s;
	for (int i = 0; i < 1048576; i++) s.push_back(rand()%64+48);
	SuffixArray<char> sa(s.begin(), s.end());
	sa.lcparray();
	printf("%f\n", (double) (clock()*1000.0/CLOCKS_PER_SEC));
	return 0;
}
