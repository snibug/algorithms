#include <vector>

/* boolean (xor ⊻ 연산을 쓰는) linear system에서 free variable 개수를 센다.
 * mat의 각 원소는 vector<int>이고, 이것을 q이라 하면
 * q[0]*x_0 ⊻ q[1]*x_1 ⊻ ... ⊻ q[m-1]*x_(m-1) == q[m]을 의미한다.
 * SRM 620 DIV1 hard 문제 */

int countFreeVars(vector<vector<int>> mat) {
	int n = mat.size(), m = mat[0].size() - 1;
	int p = 0, i;
	for(i = 0; i < n; i++) {
		for (;p < m && mat[i][p] == 0; p++) {
			for(int j = i+1; j < n; j++) {
				if (mat[j][p]) {
					for(int k = 0; k <= m; k++) mat[i][k] ^= mat[j][k];
					break;
				}
			}
			if (mat[i][p]) break;
		}
		if (p == m) break;
		for(int j = i+1; j < n; j++)
			if (mat[j][p])
				for(int k = 0; k <= m; k++) mat[j][k] ^= mat[i][k];
		p++;
	}
	for(int j = 0; j < n; j++)
		if (mat[j][m] && accumulate(mat[j].begin(), --mat[j].end(), 0) == 0)
			return -1;
	return m - i;
}
