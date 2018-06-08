#include <climits>
#include <cmath>
#include <cstdint>
#include <vector>
#include <algorithm>

using namespace std;

/* Calculate ceil(a/b)
 * Assumption: |a|,|b| \le (2^63)-1 (does not cover -2^63)
 * Dependencies: none */
long long ceildiv(long long a, long long b){
	if (b < 0) return ceildiv(-a, -b);
	if (a < 0) return a / b;
	return ((unsigned long long)a+(unsigned long long)b-1ull)/b;
}

/* Calculate floor(a/b)
 * Assumption: |a|,|b| \le (2^63)-1 (does not cover -2^63)
 * Dependencies: none */
long long floordiv(long long a, long long b){
	if (b < 0) return floordiv(-a, -b);
	if (a >= 0) return a / b;
	return -(long long)(((unsigned long long)(-a)+b-1)/b);
}

/* Calculate n^k mod m
 * Dependencies: none */
uint64_t power(uint64_t n, uint64_t k, uint64_t m) {
	uint64_t ret = 1;
	n %= m;
	while (k) {
		if (k & 1) ret = (ret * n % m);
		n = (n * n % m);
		k >>= 1;
	}
	return ret;
}

/* Calculate n^k mod m
 * Dependencies: none */
long long power(long long n, long long k, long long m) {
	if (m < 0) m = -m;
	n %= m; if (n < 0) n += m;
	return power((uint64_t)n, (uint64_t)k, (uint64_t)m);
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

/* FactorInteger
 * Dependencies: none */
vector<pair<unsigned long long, int>> factorInteger(unsigned long long B) {
	vector<pair<unsigned long long, int>> factors;
	for(unsigned long long i = 2; i <= B / i; i++) {
		int cnt = 0;
		while(B % i == 0) { B /= i; cnt++; }
		if (cnt > 0)  factors.emplace_back(i, cnt);
	}
	if (B>1) factors.emplace_back(B, 1);
	return factors;
}

/* range modular inverse.
 * useful for inverse factorial calculation */
vector<int> range_mod_inverse(int n, const int mod){
	vector<int> ret(n+1);
	ret[1] = 1;
	for(int i = 2; i <= n; i++)
		ret[i] = (long long) (mod - mod/i) * ret[mod%i] % mod;
	return ret;
}

vector<int> getFactorials(int n, const int mod) {
	vector<int> ret(1,1);
	for (int i = 1; i <= n; i++) ret.push_back((long long)ret.back()*i%mod);
	return ret;
}

vector<int> getInverseFactorials(int n, const int mod) {
	vector<int> invmod(range_mod_inverse(n, mod));
	vector<int> ret(1,1);
	for (int i = 1; i <= n; i++) ret.push_back((long long)ret.back()*invmod[i]%mod);
	return ret;
}


/* (a+b)%m */
uint64_t large_mod_add(uint64_t a, uint64_t b, uint64_t m) {
	// assumption: 0 <= a < m, 0 <= b < m, m > 0
	if (m <= (1ull<<63)) {
		return (a+b)%m;
	}
	uint64_t s = (a+b)%m;
	bool overflow = b && (a >= -b); // a + b >= 2^64
	if (!overflow) return s;
	// 0 <= s < m
	// m < s+2^64 < 3m
	// 0 < s+(2^64-m) < 2m
	// overflow condition: s+(2^64-m) >= 2^64 <=> s >= m, contradiction
	return (s-m)%m;
}

/* (a-b)%m */
uint64_t large_mod_sub(uint64_t a, uint64_t b, uint64_t m) {
	// assumption: 0 <= a < m, 0 <= b < m, m > 0
	if (a > b) return a - b;
	else if (a == b) return 0;
	else return m - b + a;
}

/* (a*b)%m */
uint64_t large_mod_mul(unsigned __int128 a, unsigned __int128 b, unsigned __int128 m) {
	return a*b%m;
}

/*
 * Eratosthenes sieve, only odd numbers.
 * If x is odd, sieve[x>>1] gives the result.
 */
vector<bool> odd_sieve(unsigned int n) {
  // assumption: n < 4294836225
  vector<bool> val((n + 1) / 2, true);
  val[0] = false;
  for (unsigned int i = 3; i * i <= n; i += 2) {
    if (!val[i>>1]) continue;
    for (unsigned int j = i * i; j <= n; j += 2 * i) {
      val[j >> 1] = false;
    }
  }
  return val;
}


namespace Primality {
	bool testWitness(uint64_t a, uint64_t n, uint64_t s) {
		if (a >= n) a %= n;
		if (a <= 1) return true;
		uint64_t d = n>>s;
		uint64_t x = power(a,d,n);
		if (x == 1 || x == n-1) return true;
		while(s-->1) {
			x = large_mod_mul(x,x,n);
			if (x == 1) return false;
			if (x == n-1) return true;
		}
		return false;
	}

	bool IsPrime(uint64_t n) {
		if (n == 2) return true;
		if (n < 2 || n%2 == 0) return false;

		uint64_t d = n>>1, s = 1;
		for(;(d&1) == 0;s++) d>>=1;

#define T(a) testWitness(a##ull, n, s)
		if (n < 341531ull) return T(9345883071009581737);
		if (n < 1050535501ull) return T(336781006125) && T(9639812373923155);
		if (n < 350269456337ull) return T(4230279247111683200) && T(14694767155120705706) && T(16641139526367750375);
		return T(2) && T(325) && T(9375) && T(28178) && T(450775) && T(9780504) && T(1795265022);
#undef T
	}
}


/* Solve a*x === b (mod m)
 *
 * precondition: 0 <= a, b < m
 * (x, M) if x (mod M) satisfies
 * (-1,-1) for no solution */
pair<long long, long long> SolveAXB(long long a, long long b, long long m) {
	if (a == 0) {
		if (b == 0) return make_pair(0, 1);
		return make_pair(-1, -1);
	}
	long long g = gcd(a, m);
	if (b % g) return make_pair(-1, -1);
	long long ap = a / g, bp = b / g, mp = m / g;
	return make_pair((bp * modinverse(ap, mp)) % mp, mp);
}




#define myasrt(expr) do { if (expr); else { puts("asrt failed: " #expr); return 1;}  } while(0)
int main() {
	myasrt(ceildiv(5,3) == 2);
	myasrt(ceildiv(-5,-3) == 2);
	myasrt(ceildiv(-5,3) == -1);
	myasrt(ceildiv(0,3) == 0);
	return 0;
}

