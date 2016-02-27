#include <cstdio>
#include <cstdlib>
#include <vector>
#include <algorithm>

using namespace std;

struct SumTreap
{
	typedef int key_t;
	typedef long long val_t;

	static val_t add(val_t a, val_t b) { return a+b; }

	unsigned long long lastval;
	unsigned int myrand(){
		lastval *= 25214903917;
		lastval += 11;
		return (unsigned int)(lastval >> 13);
	}

	struct node
	{
		key_t key;
		unsigned int heap_val;

		val_t val, subsum;
		int par, left, right;

		node() : par(-1), left(-1), right(-1) {}
	};

	int rootid;
	vector<node> nodes;

	SumTreap() : rootid(-1) {}

	/*
	find insert position by key.
	sum of values of (key and its left nodes).
	*/
	pair<int, val_t> findInsertIndex(key_t key)
	{
		val_t sum = 0;
		if (rootid == -1)
			return make_pair(-1, sum);
		int id = rootid;
		for (;;) {
			int next = -1;
			if (key < nodes[id].key) {
				next = nodes[id].left;
				if (next == -1)
					break;
				id = next;
				continue;
			} else if (nodes[id].key < key) {
				next = nodes[id].right;
			}
			sum = add(sum, nodes[id].val);
			int left = nodes[id].left;
			if (left != -1)
				sum = add(sum, nodes[left].subsum);
			if (next == -1)
				break;
			id = next;
		}
		return make_pair(id, sum);
	}

	/* contains key */
	bool contains(key_t key) {
		int pos = findInsertIndex(key).first;
		return (pos != -1 && nodes[pos].key == key);
	}

	/* return sum of values of keys in (-\infty, right] */
	val_t sumRange(key_t key) {
		return findInsertIndex(key).second;
	}

	void updateSum(int p) {
		node &cur = nodes[p];
		cur.subsum = cur.val;
		if (cur.left != -1)
			cur.subsum += nodes[cur.left].subsum;
		if (cur.right != -1)
			cur.subsum += nodes[cur.right].subsum;
	}

	void adjust(int pos) {
		for (;;) {
			int par = nodes[pos].par;
			if (par == -1)
				return;
			if (nodes[par].heap_val >= nodes[pos].heap_val)
				return;
			bool inleft = (nodes[par].left == pos);
			int parpar = nodes[par].par;
			int subL = nodes[pos].left;
			int subR = nodes[pos].right;

			if (parpar == -1) {
				rootid = pos;
				nodes[pos].par = -1;
			} else {
				if (nodes[parpar].left == par) {
					nodes[parpar].left = pos;
				} else {
					nodes[parpar].right = pos;
				}
				nodes[pos].par = parpar;
			}
			nodes[par].par = pos;
			if (inleft) {
				nodes[pos].right = par;

				nodes[par].left = subR;
				if (subR != -1)
					nodes[subR].par = par;
			} else {
				nodes[pos].left = par;

				nodes[par].right = subL;
				if (subL != -1)
					nodes[subL].par = par;
			}
			updateSum(par);
			updateSum(pos);
		}
	}

	/* duplicate -> merge */
	void insert(key_t key, val_t val)
	{
		int insertIndex = findInsertIndex(key).first;
		int cur;
		if (insertIndex != -1 && key == nodes[insertIndex].key) {
			cur = insertIndex;
			nodes[cur].val += val;
		} else {
			cur = nodes.size();
			node curNode;
			curNode.key = key;
			curNode.heap_val = myrand();
			curNode.val = curNode.subsum = val;
			curNode.par = insertIndex;
			nodes.push_back(curNode);
			if (insertIndex != -1) {
				if (nodes[insertIndex].key < key) {
					nodes[insertIndex].right = cur;
				} else {
					nodes[insertIndex].left = cur;
				}
			} else {
				rootid = cur;
			}
		}
		for (int p = cur; p != -1; p = nodes[p].par) {
			updateSum(p);
		}
		adjust(cur);
	}
};

