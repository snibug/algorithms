#include <algorithm>
#include <vector>

using namespace std;

struct UnionRect {
  typedef long long coord_t;
  typedef long long area_t;

  struct RangeCoverTree {
    struct node_t {
      area_t masked;
      area_t size;
      int cover_cnt;
      node_t() : masked(0), size(0), cover_cnt(0) { }
    };
    vector<node_t> nodes;
    int bt;

    RangeCoverTree(const vector<coord_t> &sorted_coords) {
      int size = (int)sorted_coords.size() - 1;
      size |= (size >> 1); size |= (size >> 2); size |= (size >> 4);
      size |= (size >> 8); size |= (size >> 16);
      size++;
      bt = size;
      nodes.assign(bt*2, node_t());
      for (int i = 0; i + 1 < sorted_coords.size(); i++) {
        nodes[i + bt].size = sorted_coords[i + 1] - sorted_coords[i];
      }
      for (int i = bt - 1; i >= 1; i--) {
        nodes[i].size = nodes[i << 1].size + nodes[(i << 1) + 1].size;
      }
    }

    /* [s,e) += add. s,e: index */
    void cover(int s, int e, int add) {
      e--;
      if (s > e) return;
      s += bt, e += bt;
      int os = s, oe = e;
      while (s <= e) {
        if (s & 1) {
          nodes[s].cover_cnt += add;
          update(s);
        }
        if (!(e & 1)) {
          nodes[e].cover_cnt += add;
          update(e);
        }
        s = (s + 1) >> 1;
        e = (e - 1) >> 1;
      }
      while (os) {
        update(os);
        os >>= 1;
      }
      while (oe) {
        update(oe);
        oe >>= 1;
      }
    }

    area_t get_whole_mask() const {
      return nodes[1].masked;
    }

  private:
    void update(int i) {
      if (nodes[i].cover_cnt > 0) {
        nodes[i].masked = nodes[i].size;
        return;
      }
      if (i >= bt) {
        nodes[i].masked = 0;
      }
      else {
        nodes[i].masked = nodes[i << 1].masked + nodes[(i << 1) + 1].masked;
      }
    }
  };

  area_t solve(
    vector<pair<coord_t, pair<coord_t, coord_t>>> start,
    vector<pair<coord_t, pair<coord_t, coord_t>>> end
  ) const {
    if (start.empty()) {
      return 0;
    }
    vector<coord_t> xcomp, ycomp;
    xcomp.reserve(start.size() + end.size());
    ycomp.reserve(start.size() + end.size());
    for (const auto &line : start) {
      ycomp.emplace_back(line.first);
      xcomp.emplace_back(line.second.first);
      xcomp.emplace_back(line.second.second);
    }
    // assumption: start, end는 항상 같은 구간 쌍으로 옴
    for (const auto &line : end) {
      ycomp.emplace_back(line.first);
    }

    sort(xcomp.begin(), xcomp.end());
    xcomp.resize(unique(xcomp.begin(), xcomp.end()) - xcomp.begin());
    sort(ycomp.begin(), ycomp.end());
    ycomp.resize(unique(ycomp.begin(), ycomp.end()) - ycomp.begin());
    sort(start.begin(), start.end());
    sort(end.begin(), end.end());
    RangeCoverTree tree(xcomp);
    area_t ans = 0;
    area_t occupied_length = 0;
    coord_t lasty = ycomp[0];
    int sind = 0, eind = 0;
    for (int yind = 0; yind < ycomp.size(); yind++) {
      coord_t y = ycomp[yind];
      area_t diff = y - lasty;
      ans += occupied_length * diff;
      while (sind < start.size() && start[sind].first == y) {
        coord_t x1 = start[sind].second.first;
        coord_t x2 = start[sind].second.second;
        sind++;
        int xind1 = lower_bound(xcomp.begin(), xcomp.end(), x1) - xcomp.begin();
        int xind2 = lower_bound(xcomp.begin(), xcomp.end(), x2) - xcomp.begin();
        tree.cover(xind1, xind2, 1);
      }
      while (eind < end.size() && end[eind].first == y) {
        coord_t x1 = end[eind].second.first;
        coord_t x2 = end[eind].second.second;
        eind++;
        int xind1 = lower_bound(xcomp.begin(), xcomp.end(), x1) - xcomp.begin();
        int xind2 = lower_bound(xcomp.begin(), xcomp.end(), x2) - xcomp.begin();
        tree.cover(xind1, xind2, -1);
      }
      occupied_length = tree.get_whole_mask();
      lasty = y;
    }

    return ans;
  }
};



struct UnionRectQuadratic {
  typedef int coord_t;
  typedef long long area_t;
  area_t solve(
    vector<pair<coord_t, pair<coord_t, coord_t>>> start,
    vector<pair<coord_t, pair<coord_t, coord_t>>> end
  ) {
    if (start.empty()) {
      return 0;
    }
    vector<coord_t> xcomp, ycomp;
    xcomp.reserve(start.size() + end.size());
    ycomp.reserve(start.size() + end.size());
    for (const auto &line : start) {
      ycomp.emplace_back(line.first);
      xcomp.emplace_back(line.second.first);
      xcomp.emplace_back(line.second.second);
    }
    // assumption: start and end always come in pair.
    for (const auto &line : end) {
      ycomp.emplace_back(line.first);
    }

    sort(xcomp.begin(), xcomp.end());
    xcomp.resize(unique(xcomp.begin(), xcomp.end()) - xcomp.begin());
    sort(ycomp.begin(), ycomp.end());
    ycomp.resize(unique(ycomp.begin(), ycomp.end()) - ycomp.begin());
    sort(start.begin(), start.end());
    sort(end.begin(), end.end());

    area_t ans = 0;
    vector<int> markcnt(xcomp.size());
    area_t occupied_length = 0;
    coord_t lasty = ycomp[0];
    int sind = 0, eind = 0;
    vector<int> sub(xcomp.size());
    for (int yind = 0; yind < ycomp.size(); yind++) {
      coord_t y = ycomp[yind];
      area_t diff = y - lasty;
      ans += occupied_length * diff;
      fill(sub.begin(), sub.end(), 0);
      while (sind < start.size() && start[sind].first == y) {
        coord_t x1 = start[sind].second.first;
        coord_t x2 = start[sind].second.second;
        sind++;
        int xind1 = lower_bound(xcomp.begin(), xcomp.end(), x1) - xcomp.begin();
        int xind2 = lower_bound(xcomp.begin(), xcomp.end(), x2) - xcomp.begin();
        sub[xind1]++;
        sub[xind2]--;
      }
      while (eind < end.size() && end[eind].first == y) {
        coord_t x1 = end[eind].second.first;
        coord_t x2 = end[eind].second.second;
        eind++;
        int xind1 = lower_bound(xcomp.begin(), xcomp.end(), x1) - xcomp.begin();
        int xind2 = lower_bound(xcomp.begin(), xcomp.end(), x2) - xcomp.begin();
        sub[xind1]--;
        sub[xind2]++;
      }
      occupied_length = 0;

      int subsum = 0;
      for (int i = 0; i < xcomp.size(); i++) {
        subsum += sub[i];
        markcnt[i] += subsum;
        if (markcnt[i] > 0) {
          occupied_length += xcomp[i + 1] - xcomp[i];
        }
      }
      lasty = y;
    }

    return ans;
  }
};



#include <cstdio>

int main() {
  int n;
  scanf("%d", &n);
  vector<pair<long long, pair<long long, long long>>> start;
  vector<pair<long long, pair<long long, long long>>> end;
  for (int i = 0; i < n; i++) {
    int a, b, c, d;
    scanf("%d%d%d%d", &a, &b, &c, &d);
    start.emplace_back(b, make_pair(a, c));
    end.emplace_back(d, make_pair(a, c));
  }

  UnionRect ur;
  long long result = ur.solve(move(start), move(end));
  printf("%lld\n", result);
  return 0;
}

