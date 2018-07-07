#include <vector>

using namespace std;

// Get pattern and return pi array
// P[i] := length of longest identical pre/suffix in a[0..=i] (inclusive range)
template<typename T>
vector<int> get_pi(const T &a) { 
	int n = a.size();
	vector<int> pi(n);
	int j = 0;
	for (int i = 1; i < n; i++) {
		while (j > 0 && a[i] != a[j]) j = pi[j - 1];
		j += ((a[i] == a[j]) ? 1 : 0);
		pi[i] = j;
	}
	return pi;
}

// pi를 받으면 문자열 주기를 반환. ABABAB이면 AB가 반복되므로 {2}, AAAAAA이면 A, AA, AAA이므로 {1,2,3}
vector<int> get_complete_period(vector<int> pi) {
	vector<int> ans;
	if (pi.empty()) return ans;
	int n = pi.size();
	int len = pi[n - 1];
	while (len > 0) {
		int period = n - len;
		if (n % period == 0) ans.push_back(period);
		len = pi[len - 1];
	}
	return ans;
}


// Get text, pattern and pi, then return all start indices
template<typename T>
vector<int> find_all(const T &text, const T &pattern, const vector<int> &pi) {
	// assumption: pi is created from pattern
	vector<int> ans;
	if (pattern.empty()) return ans;
	int n = text.size();
	int m = pattern.size();
	int match_len = 0;
	for (int i = 0; i < n; i++) {
		while (match_len > 0 && text[i] != pattern[match_len]) match_len = pi[match_len - 1];
		if (text[i] == pattern[match_len]) match_len++;
		if (match_len == m) {
			ans.push_back(i + 1 - m);
			match_len = pi[match_len - 1];
		}
	}
	return ans;
}

#include <cstdio>
#include <string>

int main() {
	{
		string a = "ABACABABC";
		auto result = get_pi(a);
		for (int l : result) {
			printf("%d\n", l);
		}
	}

	{
		string a = "ABABAB";
		auto result = get_complete_period(get_pi(a));
		printf("%s\n", a.c_str());
		for (int a : result) printf("%d,",a);
		printf("\n");
	}

	{
		string a = "AAAAAA";
		auto result = get_complete_period(get_pi(a));
		printf("%s\n", a.c_str());
		for (int a : result) printf("%d,",a);
		printf("\n");
	}

	{
		string pattern = "ABCABAB";
		string text = "ABCABABCABABBABCABABABCABABBAABC";
		printf("%s\n%s\n", pattern.c_str(), text.c_str());
		vector<int> positions = find_all(text, pattern, get_pi(pattern));
		for (auto p : positions) {
			printf("%d\n", p);
		}
	}
	{
		string pattern = "ABAB";
		string text = "ABABABBAAABAABABABABBAABBABAB";
		printf("%s\n%s\n", pattern.c_str(), text.c_str());
		vector<int> positions = find_all(text, pattern, get_pi(pattern));
		for (auto p : positions) {
			printf("%d\n", p);
		}
	}
	return 0;
}
