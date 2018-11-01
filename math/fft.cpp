#include <vector>
#include <complex>
#include <cmath>
#include <cstdlib>

using namespace std;

typedef complex<long double> cp;
 
 
int RB(int v, int bits) {
    int r = 0;
    for (int i = 0; i < bits; i++) {
        r <<= 1;
        r += (v & 1);
        v >>= 1;
    }
    return r;
}
 
vector<cp> fft(vector<cp> poly, int bits, bool is_inverse) {
    const double PI = atan(1.0) * 4;
    double whole = -PI;
    if (is_inverse) {
        whole = -whole;
    }
    int n = (1<<bits);
    for (int i = 0; i < n; i++) {
        int r = RB(i, bits);
        if (i < r) {
            swap(poly[i], poly[r]);
        }
    }
 
    for (int step = 1; step < n; step += step) {
        for (int s = 0; s < n; s += 2*step) {
            for (int i = 0; i < step; i++) {
                double rad = whole * i / step;
                cp t(cos(rad), sin(rad));
                cp a = poly[s + i] + t * poly[s + i + step];
                cp b = poly[s + i] - t * poly[s + i + step];
                poly[s+i] = a;
                poly[s+i+step] = b;
            }
        }
    }
    if (is_inverse) {
        for (int i = 0; i < n; i++) {
            poly[i] /= n;
        }
    }
    return poly;
}
 
