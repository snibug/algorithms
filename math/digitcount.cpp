#include <string>
#include <vector>

using namespace std;

/**
 * countdigits - count digits in base from 1 to n (inclusive, n >= 0)
 */
vector<long long> countdigits(long long n, unsigned int base = 10)
{
	++n;
	vector<long long> ans(base);
	string lim = to_string(n);
	long long powbase = 1;
	long long rest = 0;
	for (int i = lim.size()-1; i >= 0; i--) {
		for (int digit = 0; digit < base; digit++) ans[digit] += (lim[i]-'0') * (lim.size()-i-1) * (powbase/base);
		ans[0] -= (powbase/base);
		for (int digit = (i==0); digit < lim[i]-'0'; digit++) ans[digit] += powbase;
		ans[lim[i]-'0'] += rest;
		rest += (lim[i]-'0')*powbase;
		powbase *= base;
	}
	return ans;
}
