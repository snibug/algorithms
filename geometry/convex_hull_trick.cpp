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
#include <utility>

using namespace std;

struct UpperHull {
    typedef long long coord_t;
    struct Line {
        // y = mx + b
        coord_t b;
        coord_t m;

        bool isEvaluation;
        mutable coord_t s, e; // [s, e)

        Line() : b(0), m(0), isEvaluation(false), s(numeric_limits<coord_t>::lowest()), e(numeric_limits<coord_t>::max()) { }

        bool operator <(const Line &other) const {
            if (isEvaluation) {
                coord_t x = b;
                if (other.s <= x && x < other.e) {
                    return false; // equals
                } else {
                    return x < other.e;
                }
            }
            if (other.isEvaluation) {
                coord_t x = other.b;
                if (s <= x && x < e) {
                    return false; // equals
                } else {
                    return s <= x;
                }
            }
            return m < other.m;
        }

        coord_t evaluate(coord_t xp) const {
            return b + m * xp;
        }

        // ceil division
        coord_t cross_point(const Line &other) const {
            // b1 + m1 * x == b2 + m2 * x
            // (b1 - b2) / (m2 - m1) = x
            coord_t up = b - other.b;
            coord_t down = other.m - m;
            if (down < 0) { up = -up; down = -down; }
            if (up < 0) return up / down;
            return (up + (down - 1)) / down;
        }
    };

    set<Line> hull;

    bool ok(const Line &a, const Line &b, const Line &c) const {
        coord_t x1 = a.cross_point(b);
        coord_t x2 = b.cross_point(c);
        return x1 < x2;
    }

    coord_t evaluate(coord_t x) const {
        Line evaluation;
        evaluation.b = x;
        evaluation.m = x;
        evaluation.isEvaluation = true;
        return hull.find(evaluation)->evaluate(x);
    }

    void addLine(coord_t x, coord_t y, coord_t incline) {
        Line cand;
        cand.b = y - x * incline;
        cand.m = incline;
        auto Next = hull.lower_bound(cand);
        // same incline, so higher b wins
        if (Next != hull.end() && Next->m == cand.m) {
            if (Next->b >= cand.b) {
                // higher one is there.
                return;
            } else {
                // lower one, remove first.
                Next = hull.erase(Next);
            }
        }
        if (Next != hull.begin() && Next != hull.end()) {
            auto Prev = Next; --Prev;
            if (ok(*Prev, cand, *Next)) {
                // can be added
            } else {
                return;
            }
        }
        for (;;) {
            auto Prev = Next;
            if (Prev == hull.begin()) break;
            --Prev;
            auto PPrev = Prev;
            if (PPrev == hull.begin()) break;
            --PPrev;
            if (ok(*PPrev, *Prev, cand)) {
                break;
            }
            hull.erase(Prev);
        }
        for (;;) {
            if (Next == hull.end()) break;
            auto NNext = Next; ++NNext;
            if (NNext == hull.end()) break;
            if (ok(cand, *Next, *NNext)) {
                break;
            }
            Next = hull.erase(Next);
        }
        insertAndAssign(cand);
    }

private:
    void insertAndAssign(const Line &l) {
        auto iter = hull.insert(l).first;
        auto N = iter; ++N;
        if (N != hull.end()) {
            coord_t x = iter->cross_point(*N);
            iter->e = x;
            N->s = x;
        }
        auto P = iter;
        if (P != hull.begin()) {
            --P;
            coord_t x = P->cross_point(*iter);
            iter->s = x;
            P->e = x;
        }
    }
};

int main() {
    UpperHull up;
    up.addLine(0, 15, -1);
    for (int x = -30; x <= 30; x++) { printf("%d,", (int)up.evaluate(x)); } printf("\n");
    up.addLine(0, 3, 0);
    for (int x = -30; x <= 30; x++) { printf("%d,", (int)up.evaluate(x)); } printf("\n");
    up.addLine(0, 4, 0);
    for (int x = -30; x <= 30; x++) { printf("%d,", (int)up.evaluate(x)); } printf("\n");
    up.addLine(0, 2, 0);
    for (int x = -30; x <= 30; x++) { printf("%d,", (int)up.evaluate(x)); } printf("\n");
    up.addLine(10, 15, 1);
    for (int x = -30; x <= 30; x++) { printf("%d,", (int)up.evaluate(x)); } printf("\n");
    up.addLine(15, 12, 2);
    for (int x = -30; x <= 30; x++) { printf("%d,", (int)up.evaluate(x)); } printf("\n");
    up.addLine(0, 12, 0);
    for (int x = -30; x <= 30; x++) { printf("%d,", (int)up.evaluate(x)); } printf("\n");
    up.addLine(0, 30, 5);
    for (int x = -30; x <= 30; x++) { printf("%d,", (int)up.evaluate(x)); } printf("\n");
    return 0;
}
