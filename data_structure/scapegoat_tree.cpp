#include <cstdint>
#include <vector>

using namespace std;

template<typename T>
struct ScapegoatTreeWithSize {
  static const size_t nil = -1;
  static const int nume = 70;
  static const int deno = 100;
  // 1/2 < nume / deno < 1
  static_assert(deno < 2 * nume, "0.5 < alpha");
  static_assert(nume < deno, "alpha < 1");
  struct Node {
    T value;
    size_t left, right;
    size_t size;
    Node(T value) : value(value), left(nil), right(nil), size(1) {}
  };
  vector<Node> nodes;
  size_t root;
  ScapegoatTreeWithSize() : root(nil) { }
  size_t rebuild(size_t ind) {
    vector<size_t> sorted_list;
    sorted_list.reserve(nodes[ind].size);
    pack(ind, sorted_list);
    return rebuild_rec(sorted_list, 0, sorted_list.size());
  }
  size_t rebuild_rec(vector<size_t> &ord, size_t s, size_t e) {
    if (s == e)
      return nil;
    size_t m = s + (e - s - 1) / 2;
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
  void pack(size_t ind, vector<size_t> &ord) {
    const Node &cur = nodes[ind];
    if (cur.left != nil)
      pack(cur.left, ord);
    ord.push_back(ind);
    if (cur.right != nil)
      pack(cur.right, ord);
  }
  bool add(const T &value) {
    if (root == nil) {
      root = 0;
      nodes.emplace_back(value);
      return true;
    }
    auto depth_limit = (size_t)floor(log(nodes.size() + 1) / log(deno/(double)nume));

    vector<size_t> path;
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

    if (path.size() > depth_limit) {
      size_t subSize = 1;
      for (auto i = path.size(); i-- > 0;) {
        size_t curSize = nodes[path[i]].size;
        if (deno * subSize > nume * curSize) {
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
        subSize = curSize;
      }
    }
    return true;
  }
};

