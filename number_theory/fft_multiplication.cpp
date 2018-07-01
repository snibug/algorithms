#include <stdio.h>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <vector>
#include <stdexcept>
#include <ctime>

using namespace std;

pair<long long, long long> extended_gcd(long long a, long long b) {
  if (b == 0) return make_pair(1, 0);
  pair<long long, long long> t = extended_gcd(b, a % b);
  return make_pair(t.second, t.first - t.second * (a / b));
}
long long modinverse(long long a, long long m) {
  return (extended_gcd(a, m).first % m + m) % m;
}

struct ntfft {
  static const int mod = (7 << 26) + 1;
  static const int default_prim = 3;
  typedef int elt_t;
  typedef long long mul_t;

  //static const unsigned int mod = (3u << 30) + 1;
  //static const int default_prim = 5;
  //typedef unsigned elt_t;
  //typedef unsigned long long mul_t;

  // 167772161 = 5 * 2^25+1, primitive root=3
  // 469762049 = 7 * 2^26+1, primitive root=3 (currently used)
  // 998244353 = 119 * 2^23+1, primitive root=3
  // 2013265921 = 15 * 2^27+1, primitive root=31
  // (integer overflow) 3221225473 = 3 * 2^30+1, primitive root=5

  const int n; // DO NOT CHANGE ORDER
  const elt_t w; // DO NOT CHANGE ORDER

  vector<elt_t> level_w;

  static int getpower2(int n) {
    --n;
    for (int i = 1; i < 32 && (n & (n + 1)); i += i) n = (n | (n >> i));
    return n + 1;
  }

  static elt_t getprimroot(int n) {
    // default_prim^(7*2^26) = 1
    // (default_prim^(7*2^26/n))^n = 1
    mul_t a = default_prim;
    unsigned int p = (mod-1) / n;
    mul_t w = 1;
    while (p) {
      if (p & 1) w = w * a % mod;
      p >>= 1;
      a = a * a % mod;
    }
    return (elt_t)w;
  }

  ntfft(int _n) : n(getpower2(_n)), w(getprimroot(n)) {
    mul_t wp = w;
    for (int sz = 1; sz < n; sz += sz) {
      level_w.push_back((elt_t)wp);
      wp = (wp*wp) % mod;
    }
    reverse(level_w.begin(), level_w.end());
  }

  int revbit(int b) {
    int r = 0;
    for (int i = 1; i < n; i += i) {
      r <<= 1;
      r |= (b & 1);
      b >>= 1;
    }
    return r;
  }

  void impl(vector<elt_t> &A, bool inverse) {
    A.resize(n);
    for (int i = 0; i < n; i++) {
      int j = revbit(i);
      if (i < j) swap(A[i], A[j]);
    }
    for (int m = 1, depth = 0; m < n; m += m, depth++) {
      mul_t wm = level_w[depth];
      if (inverse) wm = modinverse(wm, mod);
      for (int start = 0; start < n; start += m + m) {
        mul_t w = 1;
        for (int i = 0; i < m; i++) {
          mul_t t = w * A[start + m + i] % mod;
          mul_t u = A[start + i];
          A[start + i] = (elt_t)((u + t) % mod);
          A[start + m + i] = (elt_t)((u + (mod - t)) % mod);
          w = (w * wm) % mod;
        }
      }
    }
  }
  vector<elt_t> fft(vector<elt_t> A) {
    impl(A, false);
    return A;
  }
  vector<elt_t> ifft(vector<elt_t> A) {
    impl(A, true);
    mul_t ninv = modinverse(n, mod);
    for (int i = 0; i < n; i++) {
      A[i] = (A[i] * ninv) % mod;
    }
    return A;
  }
  void convolute_inplace(vector<elt_t> &a, const vector<elt_t> &b) {
    for (int i = 0; i < n; i++) {
      a[i] = (elt_t)(((mul_t)a[i] * b[i]) % mod);
    }
  }
  vector<elt_t> convolute(vector<elt_t> a, const vector<elt_t> &b) {
    for (int i = 0; i < n; i++) {
      a[i] = (elt_t)(((mul_t)a[i] * b[i]) % mod);
    }
    return a;
  }
};

int main() {
  ntfft fft(8);
  vector<int> a = { 4,2,3,1,0,0,0,0 };
  vector<int> b = { 1,7,5,0,0,0,0,0 };
  vector<int> c = fft.fft(a);
  vector<int> d = fft.fft(b);
  fft.convolute_inplace(c, d);
  c = fft.ifft(c);
  for (int i = 0; i < 8; i++) {
    printf("%d,", c[i]);
  }
  printf("\n");
  return 0;
}

