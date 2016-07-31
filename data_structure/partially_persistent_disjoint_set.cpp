#include <cassert>
#include <vector>
#include <algorithm>

using namespace std;

/*
 * Usage:
 * PartiallyPersistentDisjointSet ds(n);
 * inc = 0;
 * for each merge query
 *    ds.merge(a, b, inc++);
 *
 * const auto &time_machine = ds;
 * time_machine.find(a, some_time);
 */
struct PartiallyPersistentDisjointSet
{
  typedef int timestamp_t; /* signed integer type */
  vector<vector<pair<timestamp_t, int>>> parent, cnt;

  timestamp_t last_time;

  PartiallyPersistentDisjointSet() { }

  PartiallyPersistentDisjointSet(int n) : cnt(n), last_time(0) {
    parent.reserve(n);
    for (int i = 0; i < n; i++) {
      parent.emplace_back();
      parent[i].emplace_back(0, i);
      cnt[i].emplace_back(0, 1);
    }
  }

  int getPar(int p, timestamp_t time) const {
    const auto &l = parent[p];
    return (--lower_bound(l.begin(), l.end(), make_pair(time + 1, -1)))->second;
  }
  int getCnt(int p, timestamp_t time) const {
    const auto &l = cnt[p];
    return (--lower_bound(l.begin(), l.end(), make_pair(time + 1, -1)))->second;
  }

  int findMutate(int p, timestamp_t time) {
    int np = p;
    int tmp;
    while (p != (tmp = getPar(p, time)))
      p = tmp;
    while (np != (tmp = getPar(np, time))) {
      parent[np].emplace_back(time, p);
      np = tmp;
    }
    return p;
  }

  int find(int p, timestamp_t time) const {
    int tmp;
    while (p != (tmp = getPar(p, time)))
      p = tmp;
    return p;
  }

  // time should be increasing.
  // returns true if success
  bool merge(int a, int b, timestamp_t time) {
    assert(time < last_time);
    last_time = time;
    a = findMutate(a, time), b = findMutate(b, time);
    if (a == b)
      return false;
    int ca = getCnt(a, time), cb = getCnt(b, time);
    if (ca > cb) {
      cnt[a].emplace_back(time, ca+cb);
      parent[b].emplace_back(time, a);
    }
    else {
      cnt[b].emplace_back(time, ca + cb);
      parent[a].emplace_back(time, b);
    }
    return true;
  }
};

