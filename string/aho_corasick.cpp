#include <algorithm>
#include <map>
#include <vector>
#include <queue>
#include <functional>
#include <string>

using namespace std;

/*
* build complete DFA and output link for pattern matching.
*
* In this version, next[alphabet] is always available. Tracing suffix link is not necessary.
*
* Usage:
*
* add(id, begin, end, translation function): add word.
* build(): build DFA from added words.
* countMatch(...): example of pattern matching procedure.
*
* Example:
*
* string patterns[] = { "abcabc", "bc", "c" };
* string match = "abcabc";
* AhoCorasick ac(256);
* for (int i = 0; i < 3; i++) ac.add(i, patterns[i].begin(), patterns[i].end(), identity<char>());
* ac.build();
* auto freq = ac.countMatch(match.begin(), match.end(), identity<char>());
*/
struct AhoCorasick
{
  struct node {
    node() : back(0), output_link(0) {}
    explicit node(int alphabet) : next(alphabet), back(0), output_link(0) {}
    // alphabet to node
    vector<int> next;
    // list of pattern id
    vector<int> report;
    // max suffix node
    int back;
    // max suffix pattern node -> skips some suffix nodes
    int output_link;
  };

  const int alphabet;
  vector<node> dfa;
  int maxid;

  AhoCorasick(int alphabet) : alphabet(alphabet), dfa(1, node(alphabet)), maxid(0) { }
  template<typename InIt, typename Fn> void add(int id, InIt first, InIt last, Fn func) {
    int cur = 0;
    for (; first != last; ++first) {
      auto s = func(*first);
      if (auto next = dfa[cur].next[s]) cur = next;
      else {
        int new_node = (int)dfa.size();
        dfa[cur].next[s] = new_node;
        cur = new_node;
        dfa.emplace_back(alphabet);
      }
    }
    dfa[cur].report.push_back(id);
    maxid = max(maxid, id);
  }
  void build() {
    queue<int> q;
    q.push(0);
    while (!q.empty()) {
      auto cur = q.front(); q.pop();
      int backnode = dfa[cur].back;
      dfa[cur].output_link = backnode;
      if (dfa[backnode].report.empty())
        dfa[cur].output_link = dfa[backnode].output_link;
      for (int s = 0; s < alphabet; s++) {
        int &next = dfa[cur].next[s];
        if (next > 0) {
          if (cur > 0) dfa[next].back = dfa[backnode].next[s];
          q.push(next);
        }
        else {
          next = dfa[backnode].next[s];
        }
      }
    }
  }
  template<typename InIt, typename Fn> vector<int> countMatch(InIt first, InIt last, Fn func) {
    int cur = 0;
    vector<int> ret(maxid + 1);
    for (; first != last; ++first) {
      cur = dfa[cur].next[func(*first)];
      for (int p = cur; p; p = dfa[p].output_link) for (auto id : dfa[p].report) ret[id]++;
    }
    return ret;
  }
};














/*
* build complete DFA and output link for pattern matching.
*
* In this version, next[alphabet] is sometimes available. Tracing suffix link *is* necessary.
*
* Usage:
*
* add(id, begin, end, translation function): add word.
* build(): build DFA from added words.
* countMatch(...): example of pattern matching procedure.
*
* Example:
*
* string patterns[] = { "abcabc", "bc", "c" };
* string match = "abcabc";
* AhoCorasickMap ac;
* for (int i = 0; i < 3; i++) ac.add(i, patterns[i].begin(), patterns[i].end());
* ac.build();
* auto freq = ac.countMatch(match.begin(), match.end());
*/
struct AhoCorasickMap
{
  typedef int alphabet_t;
  struct node {
    node() : back(0), output_link(0) {}
    // alphabet to node
    map<alphabet_t, int> next;
    // list of pattern id
    vector<int> report;
    // max suffix node
    int back;
    // max suffix pattern node -> skips some suffix nodes
    int output_link;
  };
  vector<node> dfa;
  int maxid;
  AhoCorasickMap() : dfa(1), maxid(0) { }
  int get_next(int node, alphabet_t c) const {
    auto I = dfa[node].next.find(c);
    if (I == dfa[node].next.end()) return 0;
    return I->second;
  }
  template<typename It>
  void add(int id, It first, It last) {
    int cur = 0;
    for (; first != last; ++first) {
      alphabet_t s = *first;
      auto I = dfa[cur].next.find(s);
      if (I == dfa[cur].next.end()) {
        int new_node = (int)dfa.size();
        dfa[cur].next[s] = new_node;
        cur = new_node;
        dfa.emplace_back();
      }
      else {
        cur = I->second;
      }
    }
    dfa[cur].report.push_back(id);
    maxid = max(maxid, id);
  }
  void build() {
    queue<int> q;
    q.push(0);
    while (!q.empty()) {
      const int cur = q.front(); q.pop();
      const int backnode = dfa[cur].back;
      dfa[cur].output_link = backnode;
      if (dfa[backnode].report.empty())
        dfa[cur].output_link = dfa[backnode].output_link;
      for (auto kv : dfa[cur].next) {
        alphabet_t s = kv.first;
        int next = kv.second;

        int falls = backnode;
        while (falls != 0 && get_next(falls, s) == 0) falls = dfa[falls].back;
        if (cur > 0) dfa[next].back = get_next(falls, s);
        q.push(next);
      }
    }
  }
  template<typename It>
  vector<int> countMatch(It first, It last) {
    int cur = 0;
    vector<int> ret(maxid + 1);
    for (; first != last; ++first) {
      alphabet_t c = *first;
      while (cur != 0 && get_next(cur, c) == 0) cur = dfa[cur].back;
      cur = get_next(cur, c);
      for (int p = cur; p; p = dfa[p].output_link) for (auto id : dfa[p].report) ret[id]++;
    }
    return ret;
  }
};

#include <cstdio>

int main() {
  vector<string> patterns = { "a", "ab", "bab", "bc", "bca", "c", "caa" };
  string match = "abccab";
  AhoCorasickMap ac;
  int ind = 0;
  for (auto pattern : patterns) ac.add(ind++, pattern.begin(), pattern.end());
  ac.build();
  auto freq = ac.countMatch(match.begin(), match.end());
  for (auto v : freq) {
    printf("%d\n", (int)v);
  }
  return 0;
}

