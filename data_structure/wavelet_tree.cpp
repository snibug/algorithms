#include <cstdio>
#include <algorithm>
#include <cstdlib>
#include <cmath>
#include <climits>
#include <cstring>
#include <string>
#include <vector>
#include <queue>
#include <numeric>
#include <functional>
#include <set>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <thread>
#include <tuple>

using namespace std;

template<typename T>
struct WaveletTree {
  struct node_t {
    T min, max;
    int n;
    // count of left elements
    // interpretation: index in left node
    // example:
    //    lrrllrll
    // i: 012345678
    // lc:011123345
    // rc:001222333
    vector<int> left_count;
    // children
    int left, right;

    // returns index in current node
    int convert_left_index(int index_in_left) const {
      if (index_in_left < 0) return -1;
      return (int)(upper_bound(left_count.begin(), left_count.end(), index_in_left) - left_count.begin()) - 1;
    }

    // returns index in current node
    int convert_right_index(int index_in_right) const {
      if (index_in_right < 0) return -1;
      int low = 0, high = n - 1;
      while (low <= high) {
        int mid = (low + high) / 2;
        int rcnt = mid - left_count[mid];
        bool is_right = (left_count[mid] == left_count[mid + 1]);
        if (rcnt == index_in_right && is_right) return mid;
        if (rcnt <= index_in_right) {
          low = mid + 1;
        }
        else {
          high = mid - 1;
        }
      }
      return -1;
    }
  };

  vector<typename WaveletTree<T>::node_t> nodes;
  int root;

  WaveletTree(vector<T> data) {
    vector<T> sorted(data);
    sort(sorted.begin(), sorted.end());
    nodes.reserve(data.size() * 2 + 10);
    root = build(move(data), sorted.begin());
  }


  int build(vector<T> data, typename vector<T>::iterator sorted_iter) {
    if (data.empty()) return -1;
    int n = (int)data.size();
    int id = (int)nodes.size(); nodes.emplace_back();
    T min = sorted_iter[0], max = sorted_iter[n - 1];
    nodes[id].min = min;
    nodes[id].max = max;
    nodes[id].n = n;
    nodes[id].left = -1;
    nodes[id].right = -1;
    if (min == max) return id;

    T mid = sorted_iter[n / 2];
    if (min == mid) mid++; // avoid zero split
    // [min, mid), [mid, max]

    nodes[id].left_count.reserve(n + 1);
    int cnt = 0;
    nodes[id].left_count.push_back(cnt);
    for (int i = 0; i < n; i++) {
      if (data[i] < mid) cnt++;
      nodes[id].left_count.push_back(cnt);
    }

    vector<T> left;
    left.reserve(cnt);
    for (int i = 0, j = 0; i < n; i++) {
      if (data[i] < mid) {
        left.push_back(data[i]);
      }
      else {
        data[j] = data[i];
        j++;
      }
    }
    data.resize(n - cnt);

    int left_child = build(move(left), sorted_iter);
    int right_child = build(move(data), sorted_iter + cnt);
    nodes[id].left = left_child; // do not inline, UB
    nodes[id].right = right_child; // do not inline, UB
    return id;
  }

  // count values in subarray[s, e) that are <= key
  int count_less_eq(int s, int e, T key) const {
    return count_less_eq_impl(root, s, e, key);
  }
  int count_less_eq_impl(int idx, int s, int e, T key) const {
    if (idx == -1 || s >= e) return 0;
    const auto &cur = nodes[idx];
    if (key < (cur.min)) return 0;
    if (cur.max <= key) return e - s;
    int left_s = cur.left_count[s], left_e = cur.left_count[e];
    int right_s = s - left_s, right_e = e - left_e;
    return
      count_less_eq_impl(cur.left, left_s, left_e, key) +
      count_less_eq_impl(cur.right, right_s, right_e, key);
  }

  // count values in subarray[s, e) that are == key
  int count(int s, int e, T key) const {
    int idx = root;
    for (;;) {
      if (idx == -1 || s >= e) return 0;
      const auto &cur = nodes[idx];
      if (key < (cur.min) || (cur.max) < key) return 0;
      if (cur.min == cur.max) return e - s;
      const auto &left = nodes[cur.left];
      int left_s = cur.left_count[s], left_e = cur.left_count[e];
      if (key <= left.max) {
        idx = cur.left;
        tie(s, e) = make_pair(left_s, left_e);
      }
      else {
        int right_s = s - left_s, right_e = e - left_e;
        idx = cur.right;
        tie(s, e) = make_pair(right_s, right_e);
      }
    }
  }

  // return index of first value in subarray[s, e) that == key
  // or -1 if no such key
  int find_first(int s, int e, T key) const {
    return find_first_impl(root, s, e, key);
  }
  int find_first_impl(int idx, int s, int e, T key) const {
    if (idx == -1 || s >= e) return -1;
    const auto &cur = nodes[idx];
    if (key < (cur.min) || (cur.max) < key) return -1; // change here if you want to change condition
    if (cur.min == cur.max) return s; //change here if you want to change condition

    int left_s = cur.left_count[s], left_e = cur.left_count[e];
    int right_s = s - left_s, right_e = e - left_e;
    int left_first = find_first_impl(cur.left, left_s, left_e, key);
    int right_first = find_first_impl(cur.right, right_s, right_e, key);

    int first1 = cur.convert_left_index(left_first);
    int first2 = cur.convert_right_index(right_first);

    if (first1 == -1) return first2;
    if (first2 == -1) return first1;
    return min(first1, first2);
  }

  // find element whose rank is "rank" (0-based) in subarray [s, e)
  // Assumption: s < e
  T kth(int s, int e, int rank) const {
    int idx = root;
    for (;;) {
      const auto &cur = nodes[idx];
      if (cur.min == cur.max) return cur.min;
      int left_s = cur.left_count[s], left_e = cur.left_count[e];
      int left_cnt = left_e - left_s;
      if (rank < left_cnt) {
        idx = cur.left;
        tie(s, e) = make_pair(left_s, left_e);
      }
      else {
        int right_s = s - left_s, right_e = e - left_e;
        rank -= left_cnt;
        idx = cur.right;
        tie(s, e) = make_pair(right_s, right_e);
      }
    }
  }
};

template<typename RanIt>
auto make_wavelet(RanIt s, RanIt e) {
  typedef typename iterator_traits<RanIt>::value_type T;
  return WaveletTree<T>(vector<T>(s, e));
}

template<typename T>
auto make_wavelet(vector<T> data) {
  return WaveletTree<T>(move(data));
}

int main() {
  {
    // heavy allocation
    vector<long long> b;
    for (int i = 0; i < 1000000; i++) {
      b.push_back(i);
    }
    for (int i = 0; i < 1000; i++) {
      b.push_back(1000000000 / 1000 * i);
    }
    random_shuffle(b.begin(), b.end());
    auto tt = make_wavelet(b);
    printf("%d, size=%d\n", (int)clock(), (int)tt.nodes.size());
  }
  {
    // test build
    vector<int> a = { 1,2,3,4,5 };
    int arr[7] = { 0,1,1,3,6,0,2 };
    auto tree = make_wavelet(a);
    auto tree2 = make_wavelet(arr, arr + 7);
    auto tree3 = make_wavelet(a.begin(), a.end());
  }
  for (int magic = 0; magic < 10; magic++) {
    vector<long long> arr;
    for (int i = 0; i < 100; i++) {
      arr.push_back(rand() % 100 - 50);
    }
    int n = (int)arr.size();
    auto tree = make_wavelet(arr);
    printf("testing count, count_less_eq, find_first\n");
    for (int num = -60; num <= 60; num++) {
      for (int i = 0; i < n; i++) {
        int anseq = 0;
        int ansle = 0;
        int ansfind = -1;
        for (int j = i; j <= n; j++) {
          int reseq = tree.count(i, j, num);
          int resle = tree.count_less_eq(i, j, num);
          int resfind = tree.find_first(i, j, num);
          if (reseq != anseq || resle != ansle || resfind != ansfind) {
            printf("Wrong answer %d %d %d\n", num, i, j);
            return 1;
          }
          if (j == n) break;
          if (arr[j] == num) anseq++;
          if (arr[j] <= num) ansle++;
          if (arr[j] == num && ansfind == -1) ansfind = j;
        }
      }
    }
    printf("testing kth\n");
    for (int i = 0; i < n; i++) {
      vector<long long> sub;
      for (int j = i; j < n; j++) {
        sub.push_back(arr[j]);
        sort(sub.begin(), sub.end());
        for (int rank = 0; rank < j - i + 1; rank++) {
          long long value = tree.kth(i, j + 1, rank);
          if (value != sub[rank]) {
            printf("Wrong answer\n");
            return 1;
          }
        }
      }
    }
  }
  return 0;
}

