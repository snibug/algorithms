#include <climits>
#include <cmath>
#include <vector>
#include <algorithm>

using namespace std;

/* Calculate n^k mod m
 * Dependencies: none */
long long power(long long n, long long k, long long m = LLONG_MAX) {
	long long ret = 1;
	while (k) {
		if (k & 1) ret = (ret * n) % m;
		n = (n * n) % m;
		k >>= 1;
	}
	return ret;
}

/* Calculate gcd(a,b)
 * Dependencies: none */
long long gcd(long long a, long long b) {
	if (b == 0) return a;
	return gcd(b, a % b);
}

/* find a pair (c,d) s.t. ac + bd = gcd(a,b)
 * Dependencies: none */
pair<long long, long long> extended_gcd(long long a, long long b) {
	if (b == 0) return make_pair(1, 0);
	pair<long long, long long> t = extended_gcd(b, a % b);
	return make_pair(t.second, t.first - t.second * (a / b));
}

/* Find x in [0,m) s.t. ax ≡ gcd(a, m) (mod m)
 * Dependencies: extended_gcd(a, b) */
long long modinverse(long long a, long long m) {
	return (extended_gcd(a, m).first % m + m) % m;
}


/* find x s.t. x ≡ a[i] (mod n[i])
 * Dependencies: modinverse(a, m)*/
long long chinese_remainder(long long *a, long long *n, int size) {
	if (size == 1) return *a;
	long long tmp = modinverse(n[0], n[1]);
	long long tmp2 = (tmp * (a[1] - a[0]) % n[1] + n[1]) % n[1];
	long long ora = a[1];
	long long tgcd = gcd(n[0],n[1]);
	a[1] = a[0] + n[0] / tgcd * tmp2;
	n[1] *= n[0] / tgcd;
	long long ret = chinese_remainder(a + 1, n + 1, size - 1);
	n[1] /= n[0] / tgcd;
	a[1] = ora;
	return ret;
}

/* FactorInteger
 * Dependencies: none */
vector<pair<long long, int>> factorInteger(long long B) {
	vector<pair<long long, int>> factors;
	for(long long i = 2; i * i <= B; i++) {
		int cnt = 0;
		while(B % i == 0) { B /= i; cnt++; }
		if (cnt > 0)  factors.emplace_back(i, cnt);
	}
	if (B>1) factors.emplace_back(B, 1);
	return factors;
}
