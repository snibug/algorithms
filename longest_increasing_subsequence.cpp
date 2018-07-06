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
  pair<int, int> ans = make_pair(0, 0);
  for (int i = 0; i < n; i++) {
    auto I = lower_bound(st.begin(), st.end(), make_pair(dat[i], strict ? -1 : n));
    int index = I - st.begin();
    ans = max(ans, make_pair(index + 1, i));
    if (index == 0) prev[i] = -1;
    else prev[i] = st[index - 1].second;
    if (I == st.end()) st.emplace_back(dat[i], i);
    else *I = make_pair(dat[i], i);
  }
  vector<int> sequence(ans.first);
  int p = ans.second;
  for (int i = ans.first - 1; p >= 0; i--) {
    sequence[i] = p;
    p = prev[p];
  }
  return sequence;
}
