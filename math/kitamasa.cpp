//// old implementation, use FastLinearRecurrence
#include <cstdio>
#include <numeric>
#include <vector>

using namespace std;


typedef long long val;

const int mod = 1000000007;

val add(val a, val b) { return (a + b) % mod; }
val mul(val a, val b) { return a * b % mod; }

/* A_n = a_0 A_0 + ... + a_n-1 A_n-1 + d */
/* A_m = result \cdot (A_0, ..., A_n-1) + result_last */
vector<val> GetCoefficients(const vector<val> &a, val d, long long m)
{
	int n = a.size();
	vector<val> result;
	if (m < n) {
		result.resize(n);
		result[m] = 1;
		result.push_back(0);
		return result;
	}
	if (m & 1) {
		auto sub = GetCoefficients(a, d, m-1);
		for(int i = 0; i < n; i++)
			result.push_back(add(mul(a[i], sub[n-1]), i ? sub[i-1] : 0));
		result.push_back(add(mul(d, sub[n-1]), d));
		return result;
	}
	auto sub = GetCoefficients(a, d, m / 2);
	val newd = sub[n];
	result.resize(2*n-1);
	for(int i = 0; i < n; i++) {
		for(int j = 0; j < n; j++) result[i+j] = add(result[i+j], mul(sub[i],sub[j]));
		newd = add(newd, mul(sub[i], sub[n]));
	}
	for(int i = 2*n-2; i >= n; i--) {
		for(int j = 0; j < n; j++) result[i-n+j] = add(result[i-n+j], mul(result[i],a[j]));
		newd = add(newd, mul(result[i],d));
		result.pop_back();
	}
	result.push_back(newd);
	return result;
}


int main()
{
	int k, n;
	scanf("%d%d",&k,&n);
	vector<long long> a(k,1);
	vector<long long> res = GetCoefficients(a, 0, n-1);
	long long sum = accumulate(res.begin(), res.end(), 0ll);
	printf("%lld\n", sum%mod);
	return 0;
}
