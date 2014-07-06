/* range addition / query range sum */
const int bt = 131072;
struct itnode
{
	long long sum;
	long long uniformAdd;
};
itnode itree[bt*2];

void AddRange(int s, int e, int val) {
	s += bt; e += bt;
	int lens = 0, lene = 0, len = 1;
	int rs = s, re = e;
	while(rs >= 1) {
		itree[rs].sum += lens * (long long)val;
		itree[re].sum += lene * (long long)val;
		if (s <= e) {
			if (s & 1) {
				itree[s].sum += len * (long long)val;
				itree[s].uniformAdd += val;
				lens += len;
			}
			if (!(e&1)) {
				itree[e].sum += len * (long long)val;
				itree[e].uniformAdd += val;
				lene += len;
			}
		}
		rs >>= 1, re >>= 1;
		s = (s+1)>>1; e = (e-1)>>1;
		len <<= 1;
	}
}
long long GetRange(int s, int e) {
	long long ans = 0;
	s += bt; e += bt;
	int lens = 0, lene = 0, len = 1;
	int rs = s, re = e;
	while(rs >= 1) {
		ans += lens * itree[rs].uniformAdd;
		ans += lene * itree[re].uniformAdd;
		if (s <= e) {
			if (s & 1) {
				ans += itree[s].sum;
				lens += len;
			}
			if (!(e&1)) {
				ans += itree[e].sum;
				lene += len;
			}
		}
		rs >>= 1, re >>= 1;
		s = (s+1)>>1; e = (e-1)>>1;
		len <<= 1;
	}
	return ans;
}

