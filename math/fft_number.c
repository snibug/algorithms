int szshift = 20;
int sz = 1<<szshift;

const int mod = (7<<26)+1; /* prime number 7*2^26 + 1 */

int revbit(int n)
{
    int r = 0;
    for (int i = 0; i < szshift; i++) {
        if (n & (1<<i)) {
            r |= (1<<(szshift-i-1));
        }
    }
    return r;
}

long long powmod(long long b, long long p) {
    long long r = 1;
    while(p){
        if (p % 2) r = (r*b) % mod;
        b = (b*b)%mod;
        p /= 2;
    }
    return r;
}

void fft(int *a, bool reverse) {
    int sh = 0;
    for (int stepsize = 1; stepsize < sz; stepsize += stepsize, sh++) {
        long long base = powmod(reverse ? powmod(3, mod-2) : 3, 7);
        /* period: 2*stepsize */
        for (; powmod(base, 2*stepsize) != 1;){
            base = base*base%mod;
        }
        for (int b = 0; b < sz; b += 2*stepsize) {
            long long twiddle = 1;
            for (int i = 0; i < stepsize; i++) {
                int even = a[i+b];
                long long odd = a[i+b+stepsize];
                int todd = (odd * (int)twiddle)%mod;
                a[i+b] = (even + todd)%mod;
                a[i+b+stepsize] = (even - todd)%mod;
                twiddle = ((int)twiddle * base)%mod;
            }
        }
    }
}
