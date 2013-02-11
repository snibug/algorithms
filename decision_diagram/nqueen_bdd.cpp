#include <stdio.h>
#include <memory.h>
#include <unordered_map>
#include <vector>
#include <algorithm>

using namespace std;

namespace bdd
{
	struct node{
		int var, l, r; 
		bool operator ==(const node &other) const
		{
			return memcmp(this, &other, sizeof(*this)) == 0;
		}
	};
}

size_t hash_combine(size_t seed, size_t v) { return seed ^ (v + 0x9e3779b9 + (seed << 6) + (seed >> 2)); }

namespace std
{
	template<> struct hash<bdd::node>
	{
		size_t operator()(const bdd::node &x) const
		{
			return hash_combine(x.var, hash_combine(x.l, x.r));
		}
	};
	template<> struct hash<pair<int,pair<int,int>>>
	{
		size_t operator()(const pair<int,pair<int,int>> &x) const
		{
			return hash_combine(x.first, hash_combine(x.second.first, x.second.second));
		}
	};
}

// variable order: top(large) -> bottom(small) -> node 0 or node 1
namespace bdd
{
	typedef unsigned long long count_t;

	unordered_map<pair<int, pair<int,int>>,int> combines;
	vector<node> nodes;
	vector<int> negation;
	vector<count_t> counts;
	unordered_map<node, int> inode;
	int varCnt;

	int regnode(const node &n)
	{
		auto IPair = inode.find(n);
		if (IPair == inode.end()) {
			int ret = inode[n] = nodes.size();
			nodes.push_back(n);
			negation.push_back(0);
			counts.push_back(0);
			return ret;
		}
		return IPair->second;
	}

	void init(int varCnt)
	{
		bdd::varCnt = varCnt;
		node nfalse, ntrue;
		nfalse.var = ntrue.var = -1;
		nfalse.l = nfalse.r = -2;
		ntrue.l = ntrue.r = -1;
		regnode(nfalse);
		regnode(ntrue);
	}

	int always() { return 1; }
	int never() { return 0; }

	/* from variable number */
	int single(int var, bool negate)
	{
		node tmp;
		tmp.var = var;
		tmp.l = (negate?1:0);
		tmp.r = (negate?0:1);
		return regnode(tmp);
	}

	/* manipulate nodes */
	int negate(int n)
	{
		if (n <= 1) return 1^n;
		if (negation[n] != 0) return negation[n];
		node tmp;
		tmp.var = nodes[n].var;
		tmp.l = negate(nodes[n].l);
		tmp.r = negate(nodes[n].r);
		int ret = regnode(tmp);
		negation[n] = ret;
		negation[ret] = n;
		return ret;
	}

	int getpart(int n, int var, bool right)
	{
		return nodes[n].var == var ? (right ? nodes[n].r : nodes[n].l) : n;
	}

	enum combine_mode
	{
		INTERSECTION = 0,
		UNION = 1
	};

	template<int mode>
	int combine(int n1, int n2)
	{
		if (n1 == n2) return n1;
		if (n1 == mode || n2 == mode) return mode;
		if (n1 == 1-mode) return n2;
		if (n2 == 1-mode) return n1;

		pair<int, pair<int, int>> state(mode, make_pair(n1,n2));
		auto IPair = combines.find(state);
		if (IPair != combines.end())
			return IPair->second;

		int topvar = max(nodes[n1].var, nodes[n2].var);

		int nf1 = getpart(n1, topvar, false);
		int nt1 = getpart(n1, topvar, true);
		int nf2 = getpart(n2, topvar, false);
		int nt2 = getpart(n2, topvar, true);

		int nf = combine<mode>(nf1, nf2);
		int nt = combine<mode>(nt1, nt2);

		if (nf == nt) return combines[state] = nf;
		node tmp;
		tmp.var = topvar;
		tmp.l = nf;
		tmp.r = nt;
		return combines[state] = regnode(tmp);
	}

	int create_term(int *vars, int *uses, int cnt)
	{
		sort(vars, vars + cnt);
		int prev = 1;
		for(int i = 0; i < cnt; i++)
		{
			node tmp;
			tmp.var = vars[i];
			tmp.l = uses[i] ? 0 : prev;
			tmp.r = uses[i] ? prev : 0;
			prev = regnode(tmp);
		}
		return prev;
	}

	inline int union_combine(int n1, int n2) { return combine<UNION>(n1,n2); }
	inline int intersection_combine(int n1, int n2) { return combine<INTERSECTION>(n1,n2); }

	count_t count_roots_rec(int n, int var, const count_t mod)
	{
		count_t base = 1;
		for(int i = 0; i < var - nodes[n].var; i ++) base = (base<<1) % mod;
		if (n == 0) return 0;
		if (n == 1) return base;
		if (count_t subcnt = counts[n]) return subcnt * base % mod;
		count_t sum = 0;
		sum += count_roots_rec(nodes[n].l, nodes[n].var-1, mod);
		sum += count_roots_rec(nodes[n].r, nodes[n].var-1, mod);
		sum %= mod;
		return (counts[n] = sum) * base % mod;
	}
	count_t count_roots(int n, const count_t mod)
	{
		return count_roots_rec(n, varCnt - 1, mod);
	}


} // end of namespace bdd



using namespace bdd;

int calc(int n)
{
	// [0, n*n) row*n+column, if used row,column position
	bdd::init(n*n);
	int finaln = 1;

	// rook condition
	for(int i = 0; i < n; i ++)
	{
		int all = 0;
		int allv = 0;
		int vars[n], varsv[n];
		int uses[n];
		for(int k = 0;k < n; k ++) {vars[k] = i*n+k; varsv[k] = k*n+i;}
		for(int j = 0;j < n; j ++)
		{
			for(int k = 0;k < n; k ++)
			{
				uses[k] = (j == k);
			}
			all = union_combine(all, create_term(vars, uses, n));
			allv = union_combine(allv, create_term(varsv, uses, n));
		}
		finaln = intersection_combine(finaln, all);
		finaln = intersection_combine(finaln, allv);
	}

	// queen condition
	for(int i = 0; i < n * 2 - 1; i++)
	{
		int merge = 0;
		for(int j = -1; j <= i; j++)
		{
			vector<int> vars, uses;
			for(int k = 0; k < n; k++) // row
			{
				int x = i-k;
				if(x >= 0 && x < n)
				{
					vars.push_back(k*n + x);
					uses.push_back(x == j);
				}
			}
			int nsub = create_term(&vars[0], &uses[0], vars.size());
			merge = union_combine(merge, nsub);
		}
		finaln = intersection_combine(finaln, merge);

		merge = 0;
		for(int j = -1; j < n; j++)
		{
			vector<int> vars, uses;
			for(int k = 0; k < n; k++) // row
			{
				// i == n-1+r-j  == k - x + n-1
				int x = k-i+n-1;
				if(x >= 0 && x < n)
				{
					vars.push_back(k*n + x);
					uses.push_back(x == j);
				}
			}
			int nsub = create_term(&vars[0], &uses[0], vars.size());
			merge = union_combine(merge, nsub);
		}
		finaln = intersection_combine(finaln, merge);
	}

	return (int)count_roots(finaln, 20130203);
}

int main()
{
	int ans[20]={0,};
	for(int i = 1; i <= 12; i ++)
	{
		ans[i] = calc(i);
		printf("%d\n",ans[i]);
	}
	return 0;
}
