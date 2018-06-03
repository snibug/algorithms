#include <vector>
#include <algorithm>

using namespace std;

// PC: path compression
// SZ: union by size

struct PCSZ {
  // assumption: every root node points itself.
  struct node_t {
    int parent;
    int size;
    node_t() : size(1) {}
  };
  vector<node_t> nodes;

  PCSZ() { }
  PCSZ(int size) : nodes(size) {
    for (int i = 0; i < size; i++) {
      nodes[i].parent = i;
    }
  }

  // add more nodes
  void expand(int count) {
    int oldsize = nodes.size();
    nodes.resize(oldsize + count);
    for (int i = oldsize; i < oldsize + count; i++) {
      nodes[i].parent = i;
    }
  }

  void clear() {
    nodes.clear();
  }

  // find root node
  int find(int index) {
    int root = index;
    while (root != nodes[root].parent) {
      root = nodes[root].parent;
    }
    while (root != nodes[index].parent) {
      int next = nodes[index].parent;
      nodes[index].parent = root;
      index = next;
    }
    return root;
  }

  // true if merge was successful
  bool merge(int first, int second) {
    int r1 = find(first);
    int r2 = find(second);
    if (r1 == r2) {
      return false;
    }
    if (nodes[r1].size < nodes[r2].size) {
      // careful, first and second not swapped
      swap(r1, r2);
    }
    nodes[r1].size += nodes[r2].size;
    nodes[r2].parent = r1;
    return true;
  }

  int group_size(int index) {
    return nodes[find(index)].size;
  }
};

struct ParityPCSZ {
  // assumption: every root node points itself.
  struct node_t {
    int parent;
    int size;
    char edge_parity;
    node_t() : size(1), edge_parity(0) {}
  };
  vector<node_t> nodes;

  ParityPCSZ() { }
  ParityPCSZ(int size) : nodes(size) {
    for (int i = 0; i < size; i++) {
      nodes[i].parent = i;
    }
  }

  // add more nodes
  void expand(int count) {
    int oldsize = nodes.size();
    nodes.resize(oldsize + count);
    for (int i = oldsize; i < oldsize + count; i++) {
      nodes[i].parent = i;
    }
  }

  void clear() {
    nodes.clear();
  }

  // find root node
  int find(int index) {
    int root = index;
    char sum = 0;
    while (root != nodes[root].parent) {
      sum ^= nodes[root].edge_parity;
      root = nodes[root].parent;
    }
    char partial = 0;
    while (root != nodes[index].parent) {
      int next = nodes[index].parent;
      char edge = nodes[index].edge_parity;
      nodes[index].edge_parity = partial ^ sum;
      nodes[index].parent = root;
      partial ^= edge;
      index = next;
    }
    return root;
  }

  // true if merge was successful
  bool merge(int first, int second, bool parity) {
    int r1 = find(first);
    int r2 = find(second);
    if (r1 == r2) {
      return false;
    }
    if (nodes[r1].size < nodes[r2].size) {
      // careful, first and second not swapped
      swap(r1, r2);
    }
    nodes[r1].size += nodes[r2].size;
    nodes[r2].parent = r1;
    nodes[r2].edge_parity = nodes[first].edge_parity ^ nodes[second].edge_parity ^ (parity ? 1 : 0);
    return true;
  }

  // 0, 1 if related, -1 if not related
  int parity_distance(int first, int second) {
    int root_first = find(first);
    int root_second = find(second);
    if (root_first != root_second) {
      return -1;
    }
    // assumption: nodes[first].parent == nodes[second].parent because of PC(Path Compression)
    return nodes[first].edge_parity ^ nodes[second].edge_parity;
  }

  // parity distance to root
  bool parity(int index) {
    find(index);
    // assumption: nodes[first].parent == root
    return !!nodes[index].edge_parity;
  }

  int group_size(int index) {
    return nodes[find(index)].size;
  }
};


