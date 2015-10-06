#include <vector>
#include <algorithm>
#include <limits>

using namespace std;

/*
Note:
all ranges are in [s,e) format.

Usage:
lazy_propagation_tree(size) or lazy_propagation_tree(start, end);
tree.update(start, end, value);
tree.get(start, end);
 */
struct lazy_propagation_tree
{
/*
sample operation:
- update(): add value to the range
- get(): get maximum value among the range
*/
	typedef int val_t;
	struct node_t
	{
		/* TODO: edit here.
		each node consists of at least two types of fields, pending and value */
		val_t pending;
		val_t value;
	};

	vector<node_t> tree;
	typedef vector<node_t>::size_type index_t;
	size_t base;

	lazy_propagation_tree(size_t size) {
		base = size;
		for (int s = 1; base & (base + 1); s++) base |= base >> s;
		base++;
		tree.resize(base * 2);
	}
	
	template<typename It>
	lazy_propagation_tree(It begin, It end) {
		base = end - begin;
		for (int s = 1; base & (base + 1); s++) base |= base >> s;
		base++;
		tree.resize(base * 2);
		for (auto p = base; begin != end; p++)
			tree[p].value = *begin++; /* TODO: edit here */
		for (auto p = base - 1; base > 0; base--) {
			auto &cur = tree[p], &left = tree[p*2], &right = tree[p*2+1];
			cur.value = max(left.value, right.value); /* TODO: edit here */
		}
	}

	void propagate(index_t p, index_t rs, index_t re) {
		auto &cur = tree[p];
		if (p < base) {
			auto &left = tree[p*2], &right = tree[p*2+1];
			auto rm = (rs+re)/2;
			/* propagate pending values to children */
			left.pending += cur.pending;/* TODO: edit here */
			left.value += cur.pending;/* TODO: edit here */
			right.pending += cur.pending;/* TODO: edit here */
			right.value += cur.pending;/* TODO: edit here */
		}
		/* clear pending */
		cur.pending = 0;/* TODO: edit here */
	}
	void update(index_t s, index_t e, val_t value, index_t p, index_t rs, index_t re) {
		if (s >= e) return;
		auto &cur = tree[p];
		if (s == rs && re == e) {
			/* put value on pending */
			cur.pending += value;/* TODO: edit here */
			cur.value += value;/* TODO: edit here */
			return;
		}
		auto rm = (rs+re)/2;
		auto &left = tree[p*2], &right = tree[p*2+1];
		propagate(p, rs, re);
		update(max(rs, s), min(rm, e), value, p*2+0, rs, rm);
		update(max(rm, s), min(re, e), value, p*2+1, rm, re);
		/* update value */
		cur.value = max(left.value, right.value); /* TODO: edit here */
	}
	void update(index_t s, index_t e, val_t value) { return update(s, e, value, 1, 0, base); }
	val_t get(index_t s, index_t e, index_t p, index_t rs, index_t re) {
		if (s >= e) return numeric_limits<val_t>::min(); /* TODO: edit here. */
		propagate(p, rs, re);

		/* range fitted */
		if (s <= rs && re <= e) {
			auto &cur = tree[p];
			return cur.value; /* TODO: edit here */
		}
		auto rm = (rs+re)/2;
		auto left_value  = get(max(rs, s), min(rm, e), p*2+0, rs, rm);
		auto right_value = get(max(rm, s), min(re, e), p*2+1, rm, re);

		/* combine values from left and right */
		return max(left_value, right_value); /* TODO: edit here */
	}
	auto get(index_t s, index_t e) -> decltype(get(s,e,1,0,base)) { return get(s, e, 1, 0, base); }
};
