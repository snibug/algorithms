#include <algorithm>
#include <vector>

using namespace std;

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
    vector<int> xcomp, ycomp;
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

