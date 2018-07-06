#include <vector>
#include <algorithm>
#include <utility>

using namespace std;

// returns indices of Longest Increasing Subsequence
// (strict = true) is tested
template<typename T>
vector<int> solveLIS(const vector<T> &dat, bool strict = true) {
  int n = dat.size();
  vector<pair<T, int>> st;
  st.reserve(n);
  vector<int> prev(n);
  for (int i = 0; i < n; i++) {
    auto I = lower_bound(st.begin(), st.end(), make_pair(dat[i], strict ? -1 : n));
    int index = (int)(I - st.begin());
    prev[i] = ((index == 0) ? -1 : st[index - 1].second);
    if (I == st.end()) st.emplace_back(dat[i], i);
    else *I = make_pair(dat[i], i);
  }
  vector<int> sequence(st.size());
  int p = st.empty() ? -1 : st.back().second;
  for (int i = (int)sequence.size() - 1; p >= 0; i--) {
    sequence[i] = p;
    p = prev[p];
  }
  return sequence;
}

template<typename T>
int strict_LIS_size(const vector<T> &dat) {
  int n = dat.size();
  vector<T> st;
  st.reserve(n);
  for (int i = 0; i < n; i++) {
    if (st.empty() || st.back() < dat[i]) {
      st.emplace_back(dat[i]);
    }
    else {
      auto I = lower_bound(st.begin(), st.end(), dat[i]);
      *I = dat[i];
    }
  }
  return st.size();
}

template<typename T>
int monotonic_LIS_size(const vector<T> &dat) {
  int n = dat.size();
  vector<T> st;
  st.reserve(n);
  for (int i = 0; i < n; i++) {
    if (st.empty() || st.back() <= dat[i]) {
      st.emplace_back(dat[i]);
    }
    else {
      auto I = upper_bound(st.begin(), st.end(), dat[i]);
      *I = dat[i];
    }
  }
  return st.size();
}


#include <iostream>

int main() {
	ios_base::sync_with_stdio(false); cin.tie(NULL);
	int n;
	cin >> n;
	vector<int> dat(n);
	for (int i = 0; i < n; i++) {
		cin >> dat[i];
	}

	printf("strict size: %d\n", strict_LIS_size(dat));
	{
		auto result = solveLIS(dat, true);
		printf("index: ");
		for (auto v : result) {
			printf("%d ", v);
		}
		printf("\n");
	}
	printf("monotonic size: %d\n", monotonic_LIS_size(dat));
	{
		auto result = solveLIS(dat, false);
		printf("index: ");
		for (auto v : result) {
			printf("%d ", v);
		}
		printf("\n");
	}
	return 0;
}
