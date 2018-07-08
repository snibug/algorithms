#include <cstdint>
#include <vector>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <stdexcept>

using namespace std;

/**
 * Balanced Binary Search Tree with Sub-Tree Size
 *
 * ScapegoatTree<int> tree;
 * tree.add(3);
 * tree.add(5);
 *
 * tree.select(0); // get 0-th element
 *
 * 디버깅 편의를 위해 일부러 C++답지 않게 구현함
 *
 *
 * interface: add, select, get, lower_bound, count, size
 */

template<typename T>
struct ScapegoatTree {
  typedef int index_t;

  enum {
    nil = -1,
    nume = 70,
    deno = 100
  };

  // 1/2 < nume / deno < 1
  static_assert(deno < 2 * nume, "0.5 < alpha");
  static_assert(nume < deno, "alpha < 1");

  struct Node {
    T value;
    index_t left, right;
    size_t size;
    Node(T value) : value(value), left(nil), right(nil), size(1) {}
  };

  vector<Node> nodes;
  index_t root;

  ScapegoatTree() : root(nil) { }

  // return true if insertion was successful
  // return false if same value was there
  bool add(const T &value) {
    if (root == nil) {
      root = 0;
      nodes.emplace_back(value);
      return true;
    }
    auto depth_limit = (size_t)floor(log(nodes.size() + 1) / log(deno/(double)nume));

    vector<index_t> path;
    path.reserve(depth_limit + 10);
    for (auto pos = root;;) {
      path.push_back(pos);
      auto &cur = nodes[pos];
      if (cur.value < value) {
        if (cur.right == nil) {
          cur.right = nodes.size();
          nodes.emplace_back(value);
          break;
        }
        else {
          pos = cur.right;
        }
      }
      else if (value < cur.value) {
        if (cur.left == nil) {
          cur.left = nodes.size();
          nodes.emplace_back(value);
          break;
        }
        else {
          pos = cur.left;
        }
      }
      else {
        return false;
      }
    }

    for (auto i = path.size(); i-- > 0;) {
      nodes[path[i]].size++;
    }

    if (path.size() <= depth_limit) {
      return true;
    }

    size_t subSize = 1;
    for (auto i = path.size(); i-- > 0;) {
      size_t curSize = nodes[path[i]].size;
      if (deno * subSize <= nume * curSize) {
        subSize = curSize;
        continue;
      }

      auto newSubtree = rebuild(path[i]);
      if (i > 0) {
        auto &parNode = nodes[path[i - 1]];
        if (parNode.left == path[i]) {
          parNode.left = newSubtree;
        }
        else {
          parNode.right = newSubtree;
        }
      }
      else {
        root = newSubtree;
      }
      break;
    }
    return true;
  }

  // get order-th element (whose rank is "order")
  T& select(size_t order) {
    index_t node_index = root;
    while (node_index != nil) {
      Node &cur = nodes[node_index];
      if (cur.left != nil) {
        if (order < nodes[cur.left].size) {
          node_index = cur.left;
          continue;
        }
        order -= nodes[cur.left].size;
      }
      if (order == 0) {
        return cur.value;
      }
      order--;
      node_index = cur.right;
    }
    throw runtime_error("out of range");
  }

  // get element by key. Not safe if key is missing
  T& get(const T &key) {
    auto order = lower_bound(key);
    return nodes[order.second].value;
  }

  // Find smallest idx s.t. (key <= nodes[idx].value)
  // return (rank, idx)
  // return (size(), nil = -1) if no such idx
  pair<size_t, index_t> lower_bound(const T &key) const {
    pair<size_t, index_t> best_guess(nodes.size(), nil);

    index_t node_index = root;
    size_t rank = 0;
    while (node_index != nil) {
      const Node &cur = nodes[node_index];
      size_t leftsize = 0;
      if (cur.left != nil) { leftsize = nodes[cur.left].size; }
      if (cur.value < key) {
        node_index = cur.right;
        rank += leftsize + 1;
      } else {
        best_guess = make_pair(rank + leftsize, node_index);
        node_index = cur.left;
      }
    }
    return best_guess;
  }

  // count element by key.
  int count(const T &key) const {
    index_t node_index = root;
    while (node_index != nil) {
      const Node &cur = nodes[node_index];
      if (cur.value < key) {
        node_index = cur.right;
      } else if (key < cur.value) {
        node_index = cur.left;
      } else {
        return 1;
      }
    }
    return 0;
  }

  // size of tree
  size_t size() const { return nodes.size(); }


  //internal method
  index_t rebuild(index_t ind) {
    vector<index_t> sorted_list;
    sorted_list.reserve(nodes[ind].size);
    pack(ind, sorted_list);
    return rebuild_rec(sorted_list, 0, sorted_list.size());
  }

  //internal method
  index_t rebuild_rec(vector<index_t> &ord, index_t s, index_t e) {
    if (s == e)
      return nil;
    index_t m = s + (e - s - 1) / 2;
    Node &cur = nodes[ord[m]];
    cur.size = 1;
    cur.left = rebuild_rec(ord, s, m);
    if (cur.left != nil)
      cur.size += nodes[cur.left].size;
    cur.right = rebuild_rec(ord, m + 1, e);
    if (cur.right != nil)
      cur.size += nodes[cur.right].size;
    return ord[m];
  }

  //internal method
  void pack(index_t ind, vector<index_t> &ord) {
    const Node &cur = nodes[ind];
    if (cur.left != nil)
      pack(cur.left, ord);
    ord.push_back(ind);
    if (cur.right != nil)
      pack(cur.right, ord);
  }
};


#include <climits>

int main() {
  ScapegoatTree<int> tree;
  vector<int> values = {2,17,3,11,5,7,13};
  for (int value : values) {
    tree.add(value);
    for (int i = 0; i < (int)tree.size(); i++) {
      printf("%d ", tree.select(i));
    }
    printf("\n");
  }

  int c1 = clock();
  for (int i = 0; i < 1000000; i++) {
    tree.add(rand()%1234567);
  }
  int c2 = clock();
  printf("added %d, elapsed %d tick\n", (int)tree.size(), c2 - c1);

  for (int i = 0; i < (int)tree.size(); i++) {
    auto selected = tree.lower_bound(tree.select(i));
    if ((int)selected.first != i || tree.nodes[selected.second].value != tree.select(i)) {
      printf("Something went wrong %d\n", i);
      return 1;
    }
    if (tree.count(tree.select(i)) != 1) {
      printf("Something went wrong %d\n", i);
      return 1;
    }
  }
  {
    auto outrange = tree.lower_bound(INT_MAX);
    printf("%d %d\n", (int)outrange.first, (int)outrange.second);
  }
  return 0;
}
