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

template<typename val_t>
struct PersistentPointUpdateRangeQuery {
    struct node_t {
        val_t value;
        int parent;
        int left, right;
        node_t() : value(), parent(0), left(0), right(0) {}
    };

    typedef val_t result_t; // TODO: change here

    int size; // is power of two to keep levels of leaves equal
    vector<node_t> nodes;
    vector<int> roots;

    PersistentPointUpdateRangeQuery(const int inputSize, const int expectedUpdateCount) {
        int depth = 0;
        for (int i = 1; i < max(4, inputSize); i += i) depth++;
        size = (1 << depth);
        int expectedSize = (depth + 1) * (expectedUpdateCount) + 200;
        nodes.reserve(expectedSize);
        roots.reserve(expectedUpdateCount + 10);

        nodes.emplace_back(); // 0 is a nil node
        nodes.emplace_back(); // 1 is initial root node
        roots.push_back(1);
    }

    int getRoot() const {
        return roots.back();
    }
    int getRoot(int previousUpdateCount) const {
        return roots[previousUpdateCount];
    }

    /// val_t F(const val_t &original_leaf_value, OP operand, int pos)
    template<typename OP, typename F>
    void setPoint(const int old_root, const int pos, OP operand, F leafOperation) {
        const int new_root = appendNewNode();
        roots.push_back(new_root);

        int pos_min = 0, pos_max = size - 1;

        int old_p = old_root, new_p = new_root;
        while (pos_min != pos_max) {
            const int pos_mid = (pos_min + pos_max) >> 1; // [pos_min, pos_mid], (pos_mid, pos_max]
            const int new_node = appendNewNode();
            nodes[new_node].parent = new_p;
            if (pos <= pos_mid) {
                nodes[new_p].left = new_node;
                nodes[new_p].right = nodes[old_p].right;
                tie(old_p, new_p) = make_pair(nodes[old_p].left, new_node);
                pos_max = pos_mid;
            } else {
                nodes[new_p].left = nodes[old_p].left;
                nodes[new_p].right = new_node;
                tie(old_p, new_p) = make_pair(nodes[old_p].right, new_node);
                pos_min = pos_mid + 1;
            }
        }

        // now, pos == pos_min && pos == pos_max

        // new_p is a new node, so it's safe to apply operation.
        nodes[new_p].value = leafOperation(nodes[old_p].value, operand, pos);

        for (;;) {
            int parent = nodes[new_p].parent;
            if (parent == 0) break;
            if (nodes[parent].left == new_p) {
                pos_max += pos_max - pos_min + 1;
            } else {
                pos_min -= pos_max - pos_min + 1;
            }
            new_p = parent;

            subtreeOperation(new_p, pos_min, pos_max);
        }
    }

    /// bool F(root1_left_value, root2_left_value, root1_right_value, root2_right_value, pos_min, pos_mid, pos_max)
    /// if F is true, go to left subtree.
    template<typename F>
    int followCondition(const int root1, const int root2, F goLeft) {
        int p = root1, q = root2;
        int pos_min = 0, pos_max = size - 1;
        while (pos_min != pos_max) {
            const int pos_mid = (pos_min + pos_max) >> 1;
            if (goLeft(
                    nodes[nodes[p].left].value, nodes[nodes[q].left].value,
                    nodes[nodes[p].right].value, nodes[nodes[q].right].value,
                    pos_min, pos_mid, pos_max
            )) {
                p = nodes[p].left;
                q = nodes[q].left;
                pos_max = pos_mid;
            } else {
                p = nodes[p].right;
                q = nodes[q].right;
                pos_min = pos_mid + 1;
            }
        }
        return pos_min;
    }

    result_t queryRange(const int root, const int pos_min, const int pos_max) const {
        int left, left_pos_min, left_pos_max;
        int right, right_pos_min, right_pos_max;
        {
            int split = root;
            int split_pos_min = 0, split_pos_max = size - 1;
            while (split_pos_min < split_pos_max) {
                const int split_pos_mid = (split_pos_min + split_pos_max) >> 1;
                if (pos_max <= split_pos_mid) {
                    split = nodes[split].left;
                    split_pos_max = split_pos_mid;
                } else if (split_pos_mid + 1 <= pos_min) {
                    split = nodes[split].right;
                    split_pos_min = split_pos_mid + 1;
                } else {
                    break;
                }
            }
            if (split_pos_min == split_pos_max) {
                // reached leaf node
                return valueToResult(nodes[split].value, split_pos_min, split_pos_max);
            } else {
                const int split_pos_mid = (split_pos_min + split_pos_max) >> 1;
                left = nodes[split].left;
                right = nodes[split].right;
                tie(left_pos_min, left_pos_max) = make_pair(split_pos_min, split_pos_mid);
                tie(right_pos_min, right_pos_max) = make_pair(split_pos_mid + 1, split_pos_max);
            }
        }
        result_t result = result_t();
        while (left_pos_min < left_pos_max) {
            const int left_pos_mid = (left_pos_min + left_pos_max) >> 1;
            if (pos_min <= left_pos_mid) {
                const auto &gapResult = valueToResult(nodes[nodes[left].right].value, left_pos_mid + 1, left_pos_max);
                result = mergeResult(gapResult, result);
                left_pos_max = left_pos_mid;
                left = nodes[left].left;
            } else {
                left_pos_min = left_pos_mid + 1;
                left = nodes[left].right;
            }
            const int right_pos_mid = (right_pos_min + right_pos_max) >> 1;
            if (pos_max <= right_pos_mid) {
                right_pos_max = right_pos_mid;
                right = nodes[right].left;
            } else {
                const auto &gapResult = valueToResult(nodes[nodes[right].left].value, right_pos_min, right_pos_mid);
                result = mergeResult(result, gapResult);
                right_pos_min = right_pos_mid + 1;
                right = nodes[right].right;
            }
        }
        result = mergeResult(result, valueToResult(nodes[right].value, right_pos_min, right_pos_max));
        result = mergeResult(valueToResult(nodes[left].value, left_pos_min, left_pos_max), result);
        return result;
    }

private:
    int appendNewNode() {
        const auto new_node_index = (int)nodes.size();
        nodes.emplace_back();
        return new_node_index;
    }

    void subtreeOperation(int c, int pos_min, int pos_max) {
        int pos_mid = (pos_min + pos_max) >> 1;
        int l = nodes[c].left; // [pos_min, pos_mid]
        int r = nodes[c].right; // (pos_mid, pos_max]
        // TODO: change here
        nodes[c].value = nodes[l].value + nodes[r].value;
    }

    result_t valueToResult(const val_t &value, int pos_min, int pos_max) const {
        // TODO: change here. Save pos_min, pos_max if you need in mergeResult()
        return value;
    }

    result_t mergeResult(const result_t &left, const result_t &right) const {
        // precondition: left and right are consecutive range
        // TODO: change here
        return left + right;
    }
};

// example operations
int solveXor(PersistentPointUpdateRangeQuery<int> &pstree, int l, int r, int x) {
    return pstree.followCondition(pstree.getRoot(l - 1), pstree.getRoot(r), [x](
            int l1, int l2,
            int r1, int r2,
            int p1, int p2, int p3
            ) {
        int bit = p2 + 1 - p1;
        if (x & bit) {
            return l1 != l2;
        } else {
            return r1 == r2;
        }
    });
}

int solveLE(PersistentPointUpdateRangeQuery<int> &pstree, int l, int r, int x) {
    int lcnt = pstree.queryRange(pstree.getRoot(l - 1), 0, x);
    int rcnt = pstree.queryRange(pstree.getRoot(r), 0, x);
    return rcnt - lcnt;
}

int solvekth(PersistentPointUpdateRangeQuery<int> &pstree, int l, int r, int x) {
    return pstree.followCondition(pstree.getRoot(l - 1), pstree.getRoot(r), [&x](
            int l1, int l2,
            int r1, int r2,
            int p1, int p2, int p3
    ) {
        int leftSubtree = l2 - l1;
        if (leftSubtree >= x) {
            return true;
        }
        x -= leftSubtree;
        return false;
    });
}

int solve13538() { //baekjoon test
    int m;
    scanf("%d",&m);
    PersistentPointUpdateRangeQuery<int> pstree(500001, 500001);
    while(m-->0) {
        int command;
        scanf("%d",&command);
        if (command == 1) {
            int x;
            scanf("%d", &x);
            pstree.setPoint(pstree.getRoot(), x, 1, [](const int &val, int operand, int pos) {
                return val + operand;
            });
        } else if (command == 2) {
            int l, r, x;
            scanf("%d%d%d", &l, &r, &x);
            printf("%d\n", solveXor(pstree, l, r, x));
        } else if (command == 3) {
            int k;
            scanf("%d", &k);
            pstree.roots.resize(pstree.roots.size() - k);
        } else if (command == 4) {
            int l, r, x;
            scanf("%d%d%d", &l, &r, &x);
            printf("%d\n", solveLE(pstree, l, r, x));
        } else if (command == 5) {
            int l, r, x;
            scanf("%d%d%d", &l, &r, &x);
            printf("%d\n", solvekth(pstree, l, r, x));
        }
    }
	return 0;
}

int testPersistentPointUpdateRangeQuery() {
    const int testSize = 128;
    vector<int> arr;
    for (int i = 0; i < testSize; i++) {
        arr.push_back(rand()%100);
    }
    PersistentPointUpdateRangeQuery<long long> ptree((int)arr.size(), testSize);
    for (int i = 0; i < testSize; i++) {
        ptree.setPoint(ptree.getRoot(), i, arr[i], [](const long long &val, int operand, int pos) -> long long {
            return operand ^ pos;
        });
    }

    for (int l = 0; l < testSize; l++) {
        for (int r = l; r < testSize; r++) {
            for (int i = 0; i <= testSize; i++) {
                long long result = ptree.queryRange(ptree.getRoot(i), l, r);
                long long myresult = 0;
                for (int k = l; k <= r; k++) {
                    if (k < i) {
                        myresult += arr[k] ^ k;
                    }
                }
                if (myresult != result) {
                    printf("Mismatch (l, r, i) = (%d, %d, %d): %lld != %lld\n", l, r, i, result, myresult);
                    return 1;
                }
            }
        }
    }
    printf("Passed point update range query\n");
    return 0;
}

int main() {
    testPersistentPointUpdateRangeQuery();
    return 0;
}

