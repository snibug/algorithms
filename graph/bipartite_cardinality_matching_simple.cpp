// not verified yet.
//

#include <vector>

using namespace std;

struct BipartiteSolver
{
// input field
	int n, m; // n: A side node count, m: B side node count
	vector<vector<int>> graph; // graph[A side node number][i] -> B side node number

// output field
	vector<int> match;
	vector<int> matched;

//private field
	vector<int> v;
	int vcnt;

	bool dfs(int nod)
	{
		v[nod] = vcnt;
		for(int i = 0; i < graph[nod].size(); i++)
		{
			int conn = graph[nod][i];
			if(matched[conn] == -1)
			{
				matched[conn] = nod;
				match[nod] = conn;
				return true;
			}
			int next = matched[conn];
			if (v[next] == vcnt) continue;
			if (dfs(next))
			{
				matched[conn] = nod;
				match[nod] = conn;
				return true;
			}
		}
		return false;
	}

// solver
	bool solve()
	{
		match = vector<int>(n, -1);
		matched = vector<int>(m, -1);
		for(int i = 0; i < n; i++)
		{
			vcnt++;
			if(!dfs(i)) return false;
		}
		return true;
	}
};
