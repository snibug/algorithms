#include <stdio.h>
#include <climits>
#include <algorithm>

using namespace std;

int n, match[190], matched[190];
int dat[190][190];
int q[190], v[190], vcnt;
int f[190], reach[190], reach2[190], rcnt;

int augment(int r){
	int h,t=0;
	v[r] = ++vcnt;
	q[t++] = r;
	for(h=0;h<t;h++){
		int qh = q[h];
		for(int j=0;j<n;j++) {
			if (dat[qh][j] != 0) continue;
			int next = matched[j];
			if (next == -1) {
				for(;;){
					int org = match[qh];
					match[qh] = j; matched[j] = qh;
					if(qh==r) return 1;
					qh=f[qh]; j = org;
				}
			} else if(v[next] != vcnt) {
				v[next] = vcnt, f[next] = qh, q[t++] = next;
			}
		}
	}
	for(int i=0;i<n;i++)
		if (v[i] == vcnt) {
			reach[i] = rcnt;
			if (i != r) reach2[match[i]] = rcnt;
		}
	return 0;
}
int main() {
	int ans = 0;
	scanf("%d",&n);
	for(int i=0;i<n;i++) for(int j=0;j<n;j++) scanf("%d",&dat[i][j]);
	for(int i=0;i<n;i++) match[i] = matched[i] = -1;
	for(int i=0;i<n;i++) {
		int minv = *min_element(dat[i],dat[i]+n);
		for(int j = 0;j < n;j++) dat[i][j] -= minv;
		ans += minv;
		minv = INT_MAX;
		for(int j=0;j<n;j++) minv = min(minv, dat[j][i]);
		for(int j=0;j<n;j++) dat[j][i] -= minv;
		ans += minv;
	}
	for(;;) {
		++rcnt;
		bool needMore = false;
		for(int i=0;i<n;i++) {
			if (match[i] >= 0) continue;
			if (!augment(i)) needMore = true;
		}
		if(!needMore) break;
		int minv = INT_MAX;
		for(int i=0;i<n;i++) {
			if (reach[i]!=rcnt) continue;
			for(int j=0;j<n; j++) {
				if (reach2[j]==rcnt) continue;
				minv = min(minv, dat[i][j]);
			}
		}
		for(int i=0;i<n;i++) {
			if (match[i]<0) ans += minv;
			for(int j=0;j<n;j++) {
				if (reach[i]!=rcnt) dat[i][j] += minv;
				if (reach2[j]!=rcnt) dat[i][j] -= minv;
			}
		}
	}
	printf("%d\n",ans);
	return 0;
}
