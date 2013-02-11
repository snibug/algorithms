#include <stdio.h>
#include <memory.h>
#include <unordered_map>
#include <vector>
#include <algorithm>

using namespace std;

namespace zdd
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
	template<> struct hash<zdd::node>
	{
		size_t operator()(const zdd::node &x) const
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
namespace zdd
{
	typedef unsigned long long count_t;

	unordered_map<pair<int, pair<int,int>>,int> combines;
	vector<node> nodes;
	vector<count_t> counts;
	unordered_map<node, int> inode;
	int varCnt;
	int alwaysValue;

	int regnode(const node &n)
	{
		auto IPair = inode.find(n);
		if (IPair == inode.end()) {
			int ret = inode[n] = nodes.size();
			nodes.push_back(n);
			counts.push_back(0);
			return ret;
		}
		return IPair->second;
	}

	void init(int varCnt)
	{
		zdd::varCnt = varCnt;
		node nfalse, ntrue;
		nfalse.var = ntrue.var = -1;
		nfalse.l = nfalse.r = -2;
		ntrue.l = ntrue.r = -1;
		regnode(nfalse);
		regnode(ntrue);
		alwaysValue = 0;
	}

	int always_rec(int var)
	{
		if (var < 0) return 1;
		node tmp;
		tmp.var = var;
		tmp.l = tmp.r = always_rec(var-1); 
		return regnode(tmp);
	}
	int always()
	{
		if (alwaysValue) return alwaysValue;
		return alwaysValue = always_rec(varCnt-1);
	}
	int never() { return 0; }

	/* from variable number */
	int single_rec(int var, bool negate, int cur)
	{
		if (cur < 0) return 1;
		if (var == cur && negate)
		{
			return single_rec(var, negate, cur-1);
		}
		node tmp;
		tmp.var = cur;
		if (var == cur)
		{
			tmp.l = 0;
			tmp.r = single_rec(var, negate, cur-1);
		}
		else
		{
			tmp.l = tmp.r = single_rec(var, negate, cur-1);
		}
		return regnode(tmp);
	}
	int single(int var, bool negate)
	{
		return single_rec(var, negate, varCnt-1);
	}

	int getpart(int n, int var, bool right)
	{
		return nodes[n].var == var ? (right ? nodes[n].r : nodes[n].l) : (right ? 0 : n);
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
		if (mode)
		{
			if (n1 == 0) return n2;
			if (n2 == 0) return n1;
		}
		else
		{
			if (n1 == 0 || n2 == 0) return 0;
		}

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

		if (nt == 0) return combines[state] = nf;
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
		for(int i = 0, j = 0; j < varCnt ; j++)
		{
			if ( i == cnt || vars[i] != j)
			{
				node tmp;
				tmp.var = j;
				tmp.r = tmp.l = prev;
				prev = regnode(tmp);
				continue;
			}
			if (!uses[i]) { i++; continue;}
			node tmp;
			tmp.var = j;
			tmp.l = 0;
			tmp.r = prev;
			prev = regnode(tmp);
			i++;
		}
		return prev;
	}

	inline int union_combine(int n1, int n2) { return combine<UNION>(n1,n2); }
	inline int intersection_combine(int n1, int n2) { return combine<INTERSECTION>(n1,n2); }

	count_t count_roots_rec(int n, const count_t mod)
	{
		if (n == 0) return 0;
		if (n == 1) return 1;
		if (count_t subcnt = counts[n]) return subcnt;
		count_t sum = 0;
		sum += count_roots_rec(nodes[n].l, mod);
		sum += count_roots_rec(nodes[n].r, mod);
		sum %= mod;
		return counts[n] = sum;
	}
	count_t count_roots(int n, const count_t mod)
	{
		return count_roots_rec(n, mod);
	}


} // end of namespace zdd



using namespace zdd;

int calc(int n)
{
	n *= 2;
	// [0, n*n) row*n+column, if used row,column position
	// [n*n, n*n+n) n*n+i, if i-th row is queen
	zdd::init(n*n+n);

	int finaln = always();

	// rook count = queen count
	{
		int rqperm[n];
		for(int i = 0; i < n; i++) {
			rqperm[i] = i / (n/2);
		}
		int vars[n];
		for(int i = 0;i < n;i ++) vars[i] = i + n*n;
		int all = 0;
		do {
			all = union_combine(all, create_term(vars, rqperm, n));
		} while(next_permutation(rqperm, rqperm+n));

		finaln = intersection_combine(finaln, all);
	}

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
	for(int i = 0; i < n; i++)
	{
		int merge = 0;
		for(int j = 0; j < n; j++)
		{
			vector<int> vars, uses;
			for(int k = 0; k < n; k++) // row
			{
				int x = i+j-k;
				if(x >= 0 && x < n)
				{
					vars.push_back(k*n + x);
					uses.push_back(i == k);
				}
			}
			vars.push_back(n*n + i);
			uses.push_back(1);
			int nsub = create_term(&vars[0], &uses[0], vars.size());
			merge = union_combine(merge, nsub);
		}
		merge = union_combine(merge, single(n*n + i,true));
		finaln = intersection_combine(finaln, merge);

		merge = 0;
		for(int j = 0; j < n; j++)
		{
			vector<int> vars, uses;
			for(int k = 0; k < n; k++) // row
			{
				// i - j  == k - x
				int x = k-i+j;
				if(x >= 0 && x < n)
				{
					vars.push_back(k*n + x);
					uses.push_back(i == k);
				}
			}
			vars.push_back(n*n + i);
			uses.push_back(1);
			int nsub = create_term(&vars[0], &uses[0], vars.size());
			merge = union_combine(merge, nsub);
		}
		merge = union_combine(merge, single(n*n + i,true));
		finaln = intersection_combine(finaln, merge);
	}

	return (int)count_roots(finaln, 20130203);
}

int main()
{
	init(3);
	int ans[10]={0,};
	for(int i = 1; i <= 7; i ++)
	{
		ans[i] = calc(i);
		printf("%d\n",ans[i]);
	}
	return 0;
}
