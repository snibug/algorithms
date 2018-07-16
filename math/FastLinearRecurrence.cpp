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
#include <cassert>

using namespace std;

const int mod = 1'000'000'009;

struct modint {
    // always save [0, mod)
    int val;

    modint() : val(0) {}
    modint(int val) : val(val) {}

    modint operator +() const { return *this; }
    modint operator -() const {
        auto r = mod - val;
        return (r >= mod) ? r - mod : r;
    }
    modint operator +(const modint &other) const {
        auto r = (val + other.val);
        return (r >= mod) ? r - mod : r;
    }
    modint operator -(const modint &other) const {
        auto r = val + (mod - other.val);
        return (r >= mod) ? r - mod : r;
    }
    modint operator *(const modint &other) const {
        return static_cast<int>(((long long)val * other.val) % mod);
    }

    bool operator ==(const modint &other) const {
        return val == other.val;
    }
    bool operator !=(const modint &other) const {
        return val != other.val;
    }

    int get() const { return val; }
};

struct FastLinearRecurrence {
    typedef modint num_t;

    struct Poly {
        vector<num_t> c;
        num_t d;
        int jump;

        Poly() : c(), d(), jump(0) {}
        Poly(vector<num_t> c, num_t d, int jump) : c(move(c)), d(d), jump(jump) {}
        Poly(const Poly &) = default;
        Poly(Poly &&) = default;
        Poly &operator =(const Poly &) = default;
        Poly &operator =(Poly &&) = default;
    };

    int m;

    vector<Poly> powerJumps; // some pre-calculated jumps

    /*
     * c and d satisfies the following
     * a[n] = sum(a[n-m + i] * c[i]) + d
     * O(C(m) (log m)^2)
     * O(m^2 log m) if convolution is naive algorithm
     * Assumption: m > 0
     */
    FastLinearRecurrence(vector<num_t> orig_c, num_t d) {
        Poly q(move(orig_c), d, 1);
        m = (int)q.c.size();
        powerJumps.reserve(65);
        powerJumps.emplace_back(q);
        while (q.jump * 2 <= m) {
            q = double_jump(q);
            powerJumps.emplace_back(q);
        }
        // collected 2^i <= m.
        // therefore m <= sum(jump)
    }

    vector<num_t> convolution(const vector<num_t> &a, const vector<num_t> &b) const {
        // TODO: make it efficient if you want
        vector<num_t> result(a.size() + b.size() - 1);
        for (int i = 0; i < (int)a.size(); i++) {
            for (int j = 0; j < (int)b.size(); j++){
                result[i+j] = result[i+j] + a[i] * b[j];
            }
        }
        return result;
    }

    /* Make size <= m
     *
     * Supported length [0, m + sum(powerJump.jump)] or [0, 2m]
     * because m <= sum(powerJump.jump)
     *
     * O(C(m) log m) where C(m) = convolution time complexity,
     * O(m^2) if convolution is naive algorithm */
    Poly reduce(Poly p) const {
        auto &coefficient = p.c;
        if ((int)coefficient.size() <= m) {
            // already reduced size
            return p;
        }
        for (int jump_index = (int)powerJumps.size() - 1; jump_index >= 0; jump_index--) {
            const Poly &jump = powerJumps[jump_index];
            int start = m + (1 << jump_index) - 1;
            if (start >= (int)coefficient.size()) continue;
            const auto &part = convolution(
                    vector<num_t>(coefficient.begin() + start, coefficient.end()), /* length <= (1<<jump_index) */
                    jump.c /* length = m */
            );
            /* part.length <= m + (1<<jump_index) - 1 = start */
            for (int i = start; i < (int)coefficient.size(); i++) {
                p.d = p.d + jump.d * coefficient[i];
            }
            p.jump += (int)coefficient.size() - start;

            coefficient.resize(start);
            for (int i = 0; i < (int)part.size(); i++) {
                coefficient[i] = coefficient[i] + part[i];
            }
        }
        return p;
    }

    /* O(C(m) log m) */
    Poly double_jump(const Poly &a) const {
        int offset = a.jump;
        Poly result(vector<num_t>(a.c.size() + offset), a.d, a.jump);
        copy(a.c.begin(), a.c.end(), result.c.begin() + offset);
        return reduce(move(result));
    }

    /* O(C(m) log m log jump) */
    Poly calculate(long long jump) const {
        if (jump - powerJumps.back().jump <= m) {
            for (int jump_index = (int)powerJumps.size() - 1; jump_index >= 0; jump_index--) {
                const Poly &p = powerJumps[jump_index];
                if (p.jump == jump) {
                    return p;
                }
                if (p.jump < jump) {
                    auto offset = static_cast<int>(jump - p.jump); // <= m
                    Poly result(vector<num_t>(p.c.size() + offset), p.d, p.jump); // <= 2m
                    copy(p.c.begin(), p.c.end(), result.c.begin() + offset);
                    return reduce(move(result));
                }
                // jump - p.jump < 0, and the largest jump <= m, so skipping adds m/2 or less
                // therefore jump - p.jump < m
            }
            // it means 0 or less jump
            return Poly();
        }
        long long factor = jump - (m - 1);
        if (factor % 2 == 0) {
            const Poly &half = calculate(factor / 2);
            return reduce(Poly(
                    convolution(half.c, half.c),
                    half.d + half.d * accumulate(half.c.begin(), half.c.end(), num_t()),
                    half.jump + half.jump
            ));
        } else {
            Poly result = calculate(jump - 1);
            result.c.insert(result.c.begin(), num_t());
            return reduce(move(result));
        }
    }

    /* O(C(m) log m log index) */
    num_t calculate(long long index, const vector<num_t> &seed) {
        if (index < 0) {
            throw std::invalid_argument("negative index");
        }
        if ((int)seed.size() < m) {
            throw std::invalid_argument("seed is not enough");
        }
        if (index < (int)seed.size()) {
            return seed[index];
        }
        const Poly &poly = calculate(index - (m - 1));
        num_t result = poly.d;
        for (int i = 0; i < m; i++) {
            result = result + seed[i] * poly.c[i];
        }
        return result;
    }
};


/************************
 * start of test
 ************************/

void test1() {
    // a(n) = 2 a(n-1) + 3
    FastLinearRecurrence flr(
            {modint(2)},
            modint(3)
    );
    // a(0) = 1, a(1) = 2, a(2) = 7
    vector<modint> seed = {modint(1), modint(5), modint(13)};
    vector<modint> realseed = seed;
    for (int i = 0; i < 1000; i++) {
        modint new_val =
                modint(2) * seed[seed.size() - 1] +
                modint(3);
        seed.push_back(new_val);
    }
    for (int i = 0; i < 10; i++) {
        modint t = flr.calculate(i, realseed);
        printf("should match: %d %d\n", seed[i].val, t.val);
        if (seed[i] != t) {
            printf("mismatch!!\n");
        }
    }
    {
        // some large number
        modint t = flr.calculate(800, realseed);
        printf("should match: %d %d\n", seed[800].val, t.val);
        if (seed[800] != t) {
            printf("mismatch!!\n");
        }
    }
}

void test2() {
    // a(n) = 2 a(n-2) + 7 a(n-1) + 3
    FastLinearRecurrence flr(
            {modint(2), modint(7)},
            modint(3)
    );
    // a(0) = 1, a(1) = 2
    vector<modint> seed = {modint(1), modint(2)};
    vector<modint> realseed = seed;
    for (int i = 0; i < 1000; i++) {
        modint new_val =
                modint(2) * seed[seed.size() - 2] +
                modint(7) * seed[seed.size() - 1] +
                modint(3);
        seed.push_back(new_val);
    }
    for (int i = 0; i < 10; i++) {
        modint t = flr.calculate(i, realseed);
        printf("should match: %d %d\n", seed[i].val, t.val);
        if (seed[i] != t) {
            printf("mismatch!!\n");
        }
    }
    {
        // some large number
        modint t = flr.calculate(800, realseed);
        printf("should match: %d %d\n", seed[800].val, t.val);
        if (seed[800] != t) {
            printf("mismatch!!\n");
        }
    }
}

void test3() {
    // a(n) = 2 a(n-3) + 6 a(n-2) + 5 a(n-1) + 3
    FastLinearRecurrence flr(
            {modint(2), modint(6), modint(5)},
            modint(3)
    );
    // a(0) = 1, a(1) = 2, a(2) = 7
    vector<modint> seed = {modint(1), modint(2), modint(7)};
    vector<modint> realseed = seed;
    for (int i = 0; i < 1000; i++) {
        modint new_val =
                modint(2) * seed[seed.size() - 3] +
                modint(6) * seed[seed.size() - 2] +
                modint(5) * seed[seed.size() - 1] +
                modint(3);
        seed.push_back(new_val);
    }
    for (int i = 0; i < 10; i++) {
        modint t = flr.calculate(i, realseed);
        printf("should match: %d %d\n", seed[i].val, t.val);
        if (seed[i] != t) {
            printf("mismatch!!\n");
        }
    }
    {
        // some large number
        modint t = flr.calculate(800, realseed);
        printf("should match: %d %d\n", seed[800].val, t.val);
        if (seed[800] != t) {
            printf("mismatch!!\n");
        }
    }
}

void test4() {
    // a(n) = 2 a(n-4) + 6 a(n-3) + 5 a(n-2) + 7 a(n-1) + 3
    FastLinearRecurrence flr(
            {modint(2), modint(6), modint(5), modint(7)},
            modint(3)
    );
    // a(0) = 5, a(1) = 2, a(2) = 7, a(3) = -1
    vector<modint> seed = {modint(5), modint(2), modint(7), modint(mod-1)};
    vector<modint> real_seed = seed;
    for (int i = 0; i < 1000; i++) {
        modint new_val =
                modint(2) * seed[seed.size() - 4] +
                modint(6) * seed[seed.size() - 3] +
                modint(5) * seed[seed.size() - 2] +
                modint(7) * seed[seed.size() - 1] +
                modint(3);
        seed.push_back(new_val);
    }
    for (int i = 0; i < 10; i++) {
        modint t = flr.calculate(i, real_seed);
        printf("should match: %d %d\n", seed[i].val, t.val);
        if (seed[i] != t) {
            printf("mismatch!!\n");
        }
    }
    {
        // some large number
        modint t = flr.calculate(800, real_seed);
        printf("should match: %d %d\n", seed[800].val, t.val);
        if (seed[800] != t) {
            printf("mismatch!!\n");
        }
    }
}

int main() {
    test1();
    test2();
    test3();
    test4();
    return 0;
}

