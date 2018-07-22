#include <climits>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <tuple>

using namespace std;

// For most functions, -2^63 is not safe.

// Assumption: m > 0
long long positive_mod(long long a, long long m) {
	long long res = a % m;
	return (res < 0) ? res + m : res;
}

/* find (gcd, c, d) s.t. ac + bd = gcd
 * Returns (gcd, c, d). gcd can be negative if a or b is negative
 * |c| < |b/gcd|, |d| < |a/gcd| if a and b are non-multiple
 * Dependencies: none */
tuple<long long, long long, long long> extended_gcd(long long a, long long b) {
	long long s = 1, t = 0;
	long long u = 0, v = 1;
	// loop invariant: (A,B) = (input a, input b), a = s*A + t*B, b = u*A + v*B
	while (b != 0) {
		long long q = a / b;
		tie(a, b) = make_pair(b, a % b);
		// b' = A(s-uq) + B(t-vq)
		tie(s, t, u, v) = make_tuple(u, v, s - u * q, t - v * q);
	}
	return make_tuple(a, s, t);
}

/* returns (c, d), ac + bd = gcd */
pair<long long, long long> extended_gcd_rec(long long a, long long b) {
	if (b == 0) return make_pair(1, 0);
	pair<long long, long long> t = extended_gcd_rec(b, a % b);
	return make_pair(t.second, t.first - t.second * (a / b));
}

/* Find x in [0,m) s.t. ax ≡ gcd(a, m) (mod m)
 * Assumption: m > 0
 * Dependencies: extended_gcd(a, b) */
long long modinverse(long long a, long long m) {
	a = a % m;
	if (a < 0) a += m;
	long long value = get<1>(extended_gcd(a, m)) % m;
	return (value < 0) ? value + m : value;
}

/* Calculate gcd(a,b)
 * Dependencies: none */
long long gcd(long long a, long long b) {
	while (b != 0) {
		long long t = a % b;
		a = b;
		b = t;
	}
	return abs(a);
}

/* Calculate gcd(a,b), 2~3 times faster */
long long binary_gcd(long long a, long long b) {
	if (a < 0) a = -a;
	if (b < 0) b = -b;
	if (a == 0) return b;
	if (b == 0) return a;
	int twos = __builtin_ctzll(a | b);
	b >>= __builtin_ctzll(b);
	do {
		a >>= __builtin_ctzll(a);
		if (a < b) {
			tie(a, b) = make_pair(b - a, a);
		} else {
			tie(a, b) = make_pair(a - b, b);
		}
	} while (a);
	return b << twos;
}


/* Solve a*x === b (mod m)
 *
 * Assumption: 0 <= a, b < m <= 3037000500 (3.03*10^9)
 *
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

/* Chinese Remainder Theorem solver
 *
 * Assumption: LCM(n) < 2^63
 * 
 * return (M, N) such that (M + N*x) == a[i] (mod n[i]).
 * return (-1, -1) if impossible
 *
 * Call crt_solve(a, n)
 */
long long get_crt_divisor(const long long a, const long long g) {
	const long long keeping_prime_powers = binary_gcd(a / g, g);
	long long divisor = g;
	for (;;) {
		const long long discard = binary_gcd(keeping_prime_powers, divisor);
		if (discard <= 1) break;
		divisor /= discard;
	}
	return divisor;
}
pair<long long, long long> crt_solve(const vector<int> &a, const vector<int> &n) {
	if (a.empty()) return make_pair(0, 1);
	long long n1 = abs(n[0]);
	long long a1 = positive_mod(a[0], n1);
	for (int i = 1; i < a.size(); i++) {
		long long n2 = abs(n[i]);
		long long a2 = positive_mod(a[i], n2);
		long long g = binary_gcd(n1, n2);
		if (g > 1) {
			if (a1 % g != a2 % g) return make_pair(-1, -1);
			long long d1 = get_crt_divisor(n1, g);
			long long d2 = g / d1;
			n1 /= d1;
			n2 /= d2;
			a1 %= n1;
			a2 %= n2;
			// now gcd(n1, n2) == 1
		}
		long long m1;
		tie(std::ignore, m1, std::ignore) = extended_gcd(n1, n2);
		// x === a1 + n1*alpha (mod n1*n2),
		// n1*alpha === a2-a1 (mod n2)
		long long alpha = positive_mod(a2 - a1, n2) * positive_mod(m1, n2) % n2;
		a1 = a1 + alpha * n1;
		n1 = n1 * n2;
	}
	return make_pair(a1, n1);
}

/* Calculate ceil(a/b)
 * Dependencies: none */
long long ceildiv(long long a, long long b){
	if (b < 0) return ceildiv(-a, -b);
	if (a < 0) return a / b;
	return ((unsigned long long)a+(unsigned long long)b-1ull)/b;
}

/* Calculate floor(a/b)
 * Dependencies: none */
long long floordiv(long long a, long long b){
	if (b < 0) return floordiv(-a, -b);
	if (a >= 0) return a / b;
	return -(long long)(((unsigned long long)(-a)+b-1)/b);
}

/* Calculate n^k mod m
 * Assumption: m <= 3037000500 (3.03*10^9)
 * Dependencies: modinverse */
long long power(long long n, long long k, long long m) {
	if (m < 0) m = -m;
	n %= m;
	if (k < 0) {
		n = modinverse(n, m);
		k = -k;
	}
	long long ret = 1;
	while (k) {
		if (k & 1) ret = (ret * n % m);
		n = (n * n % m);
		k >>= 1;
	}
	return ret;
}

/* FactorInteger
 * Dependencies: none */
template<typename T>
vector<pair<T, int>> factorInteger(T B) {
	vector<pair<T, int>> factors;
	for(T i = 2; i <= B / i; i++) {
		int cnt = 0;
		while (B % i == 0) { B /= i; cnt++; }
		if (cnt > 0)  factors.emplace_back(i, cnt);
	}
	if (B > 1) factors.emplace_back(B, 1);
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


namespace Prime {
	// depends on large_mod_mul
	bool test_witness(uint64_t a, uint64_t n, uint64_t s) {
		a %= n;
		if (a <= 1) return true;
		uint64_t d = n>>s;
		uint64_t x = 1; // a^d (mod n)
		{
			uint64_t pa = a;
			while (d) {
				if (d & 1) x = large_mod_mul(x, pa, n);
				pa = large_mod_mul(pa, pa, n);
				d >>= 1;
			}
		}
		if (x == 1 || x == n-1) return true;
		while(s-->1) {
			x = large_mod_mul(x,x,n);
			if (x == 1) return false;
			if (x == n-1) return true;
		}
		return false;
	}

	bool is_prime(uint64_t n) {
		if (n == 2) return true;
		if (n < 2 || n%2 == 0) return false;

		uint64_t d = n>>1, s = 1;
		for(;(d&1) == 0;s++) d>>=1;

#define T(a) test_witness(a##ull, n, s)
		// from https://miller-rabin.appspot.com/
		if (n < 341531ull) return T(9345883071009581737);
		if (n < 1050535501ull) return T(336781006125) && T(9639812373923155);
		if (n < 350269456337ull) return T(4230279247111683200) && T(14694767155120705706) && T(16641139526367750375);
		return T(2) && T(325) && T(9375) && T(28178) && T(450775) && T(9780504) && T(1795265022);
#undef T
	}
}





#define myasrt(expr) do { if (expr); else { puts("asrt failed: " #expr); return 1;}  } while(0)

void benchmark_gcd() { 
	unsigned long long ans = 0;
	int loopcnt = 5000000;
	int t0 = clock();
	for (int i = 0; i < loopcnt; i++) {
		unsigned long long a = 0, b = 0;
		a <<= 20; a += rand();
		a <<= 20; a += rand();
		a <<= 20; a += rand();
		b <<= 20; b += rand();
		b <<= 20; b += rand();
		b <<= 20; b += rand();
		a >>= 2;
		b >>= 2;
		ans += a^b;
	}
	int ta = clock();
	printf("%d\n", ta - t0);
	for (int i = 0; i < loopcnt; i++) {
		unsigned long long a = 0, b = 0;
		a <<= 20; a += rand();
		a <<= 20; a += rand();
		a <<= 20; a += rand();
		b <<= 20; b += rand();
		b <<= 20; b += rand();
		b <<= 20; b += rand();
		a >>= 2;
		b >>= 2;
		ans += gcd(a, b);
	}
	int tb = clock(); 
	printf("%d, %llu\n", tb - ta - (ta - t0), ans);
	for (int i = 0; i < loopcnt; i++) {
		unsigned long long a = 0, b = 0;
		a <<= 20; a += rand();
		a <<= 20; a += rand();
		a <<= 20; a += rand();
		b <<= 20; b += rand();
		b <<= 20; b += rand();
		b <<= 20; b += rand();
		a >>= 2;
		b >>= 2;
		ans += binary_gcd(a, b);
	}
	int tc = clock();
	printf("%d, %llu\n", tc - tb - (ta - t0), ans);
}

void benchmark_extended_gcd() { 
	unsigned long long ans = 0;
	int loopcnt = 5000000;
	int t0 = clock();
	for (int i = 0; i < loopcnt; i++) {
		unsigned long long a = 0, b = 0;
		a <<= 20; a += rand();
		a <<= 20; a += rand();
		a <<= 20; a += rand();
		b <<= 20; b += rand();
		b <<= 20; b += rand();
		b <<= 20; b += rand();
		a >>= 2;
		b >>= 2;
		ans += a^b;
	}
	int ta = clock();
	printf("%d\n", ta - t0);
	for (int i = 0; i < loopcnt; i++) {
		unsigned long long a = 0, b = 0;
		a <<= 20; a += rand();
		a <<= 20; a += rand();
		a <<= 20; a += rand();
		b <<= 20; b += rand();
		b <<= 20; b += rand();
		b <<= 20; b += rand();
		a >>= 2;
		b >>= 2;
		ans += get<1>(extended_gcd(a, b));
	}
	int tb = clock(); 
	printf("%d, %llu\n", tb - ta - (ta - t0), ans);
	for (int i = 0; i < loopcnt; i++) {
		unsigned long long a = 0, b = 0;
		a <<= 20; a += rand();
		a <<= 20; a += rand();
		a <<= 20; a += rand();
		b <<= 20; b += rand();
		b <<= 20; b += rand();
		b <<= 20; b += rand();
		a >>= 2;
		b >>= 2;
		ans += extended_gcd_rec(a, b).first;
	}
	int tc = clock();
	printf("%d, %llu\n", tc - tb - (ta - t0), ans);
}

int compare_gcd() {
	for (int a = -200; a <= 200; a++) {
		for (int b = -200; b <= 200; b++) {
			myasrt(gcd(a, b) == binary_gcd(a, b));
		}
	}
	return 0;
}

int compare_extended_gcd() {
	for (int a = -200; a <= 200; a++) {
		for (int b = -200; b <= 200; b++) {
			auto v = extended_gcd(a, b);
			long long c, d, g;
			tie(g, c, d) = v;
			myasrt(g == a*c + b*d);
			myasrt(a == 0 || b == 0 || abs(c) <= abs(b / g));
			myasrt(a == 0 || b == 0 || abs(d) <= abs(a / g));
			myasrt(make_pair(get<1>(v), get<2>(v)) == extended_gcd_rec(a, b));
		}
	}
	return 0;
}


int main() {
	//benchmark_gcd();
	//benchmark_extended_gcd();
	if (compare_gcd()) return 1;
	if (compare_extended_gcd()) return 1;
	myasrt(ceildiv(5,3) == 2);
	myasrt(ceildiv(-5,-3) == 2);
	myasrt(ceildiv(-5,3) == -1);
	myasrt(ceildiv(0,3) == 0);

	myasrt(!Prime::is_prime(0));
	myasrt(!Prime::is_prime(1));
	myasrt(Prime::is_prime(2));
	myasrt(Prime::is_prime(3));
	myasrt(!Prime::is_prime(4));
	myasrt(Prime::is_prime(5));
	myasrt(!Prime::is_prime(6));
	myasrt(Prime::is_prime(7));
	myasrt(!Prime::is_prime(8));
	myasrt(Prime::is_prime(13));
	myasrt(!Prime::is_prime(705));
	myasrt(!Prime::is_prime(15251));
	myasrt(Prime::is_prime(12345678901253ull));
	myasrt(!Prime::is_prime(12345678901257ull));

	myasrt(7 == modinverse(4, 9));
	myasrt(27625430687295043ll == modinverse(48382834818537ll, 39769844199555331ll));

	myasrt(make_pair(0LL, 1LL) == crt_solve(vector<int>(), vector<int>()));
	myasrt(make_pair(2244122LL, 5292000LL) == crt_solve(
				vector<int>{228122,     921122},
				vector<int>{32*9*125*7, 8*27*125*49}));
	myasrt(make_pair(-1LL, -1LL) == crt_solve(
				vector<int>{228122,     921122,      80},
				vector<int>{32*9*125*7, 8*27*125*49, 128*5}));
	myasrt(make_pair(457356122LL, 756756000LL) == crt_solve(
				vector<int>{228122,     921122,      80},
				vector<int>{32*9*125*7, 8*27*125*49, -2*11*13}));
	myasrt(make_pair(457356122LL, 756756000LL) == crt_solve(
				vector<int>{228122,     921122,      80,       100},
				vector<int>{32*9*125*7, 8*27*125*49, -2*11*13, 1}));

	printf("Finished all tests successfully\n");
	return 0;
}

