#include <algorithm>
#include <memory.h>

using namespace std;

//// short names version;
// input: n, in
// output: o
namespace SuffixArrayMin
{
	typedef char val_t;
	const int MAX_N = 4096;

	int n;
	val_t in[MAX_N];
	int o[MAX_N], t[MAX_N];
	int r[MAX_N];

	int b[MAX_N], m[MAX_N];
	int p[MAX_N];

	void build() {
		for (int i = 0; i < n; i++) o[i] = i;
		sort(o, o + n, [](int a,int b) { return in[a] < in[b];});
		for(int i = 0, c = 0; i < n; i++) { b[i] = c; if (i+1 == n || in[o[i]] != in[o[i+1]]) c++; }
		for(int h = 1; h < n; h <<= 1) {
			for (int i = 0; i < n; i++) r[o[i]] = b[i];
			for (int i = n-1; i>=0; i--) p[b[i]] = i;
			for (int i = 0; i < n; i++) if (o[i] >= n-h) t[p[b[i]]++] = o[i];
			for (int i = 0; i < n; i++) if (o[i] >= h) t[p[r[o[i]-h]]++] = o[i]-h;
			for (int i = 0, c = 0; i < n; i++) {
				m[i] = c;
				if (b[i] != b[i+1]) c++;
				else if(t[i+1] >= n-h || t[i] >= n-h) c++;
				else if(r[t[i+1] + h] != r[t[i]+h]) c++;
			}
			memcpy(o, t, sizeof(t[0]) * n);
			memcpy(b, m, sizeof(m[0]) * n);
		}
	}
}


//// unmanged names
// input: n, in
// output: out
namespace SuffixArray
{
	typedef char val_t;
	const int MAX_N = 4096;

	int n;
	val_t in[MAX_N];
	int out[MAX_N], temp[MAX_N];
	int pos2bckt[MAX_N];

	int bckt[MAX_N], relabel[MAX_N];
	int bpos[MAX_N];

	void build() {
		for (int i = 0; i < n; i++) out[i] = i;
		sort(out, out + n, [](int a,int bckt) { return in[a] < in[bckt];});
		for(int i = 0, c = 0; i < n; i++) { bckt[i] = c; if (i+1 == n || in[out[i]] != in[out[i+1]]) c++; }
		for(int h = 1; h < n; h <<= 1) {
			for (int i = 0; i < n; i++) pos2bckt[out[i]] = bckt[i];
			for (int i = n-1; i>=0; i--) bpos[bckt[i]] = i;
			for (int i = 0; i < n; i++) if (out[i] >= n-h) temp[bpos[bckt[i]]++] = out[i];
			for (int i = 0; i < n; i++) if (out[i] >= h) temp[bpos[pos2bckt[out[i]-h]]++] = out[i]-h;
			for (int i = 0, c = 0; i < n; i++) {
				relabel[i] = c;
				if (bckt[i] != bckt[i+1]) c++;
				else if(temp[i+1] >= n-h || temp[i] >= n-h) c++;
				else if(pos2bckt[temp[i+1] + h] != pos2bckt[temp[i]+h]) c++;
			}
			memcpy(out, temp, sizeof(temp[0]) * n);
			memcpy(bckt, relabel, sizeof(relabel[0]) * n);
		}
	}
}


#include <string.h>

int main()
{
	const char *str = "abracadabra";
	SuffixArray::n = strlen(str);
	SuffixArrayMin::n = strlen(str);
	strcpy(SuffixArray::in, str);
	strcpy(SuffixArrayMin::in, str);

	SuffixArray::build();
	SuffixArrayMin::build();

	printf("11,8,1,4,6,9,2,5,7,10,3,\n");
	for(int i = 0; i < SuffixArray::n; i++) {
		printf("%d,", SuffixArray::out[i]+1);
	}
	printf("\n");
	for(int i = 0; i < SuffixArrayMin::n; i++) {
		printf("%d,", SuffixArrayMin::o[i]+1);
	}
	printf("\n");

	return 0;
}
