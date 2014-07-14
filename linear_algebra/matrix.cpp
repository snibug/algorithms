// tested problems:
//  Codeforces #FF DZY Loves Games (multiply, power, inverse)
//  SRM 425 Hard RoadsOfKingdom (determinant)
//  SRM 396 Hard MoreNim (rowechelonform)
#include <cmath>

using namespace std;

template<typename elem_t>
struct SqMatrix
{
	inline elem_t add (elem_t a, elem_t b) { return a + b; }
	inline elem_t sub (elem_t a, elem_t b) { return a - b; }
	inline elem_t mul (elem_t a, elem_t b) { return a * b; }
	inline elem_t inv (elem_t a) { return 1 / a; }
	inline bool isZero(elem_t a) { return fabs(a) < 1e-9; }

	// A *= B
	template<int sz>
	void Multiply(int n, elem_t A[][sz], const elem_t B[][sz]) {
		static elem_t tmp[sz][sz];
		for(int i = 0; i < n; i++) {
			for(int j = 0; j < n; j++) {
				elem_t sum = 0;
				for(int k = 0; k < n; k++) {
					sum = add(sum, mul(A[i][k], B[k][j]));
				}
				tmp[i][j] = sum;
			}
		}
		for(int i = 0; i < n; i++) {
			memcpy(A[i], tmp[i], sizeof(tmp[i][0])*n);
		}
	}

	// out = A*B
	template<int sz>
	void Multiply(int n, elem_t out[][sz], const elem_t A[][sz], const elem_t B[][sz]) {
		if (out == A) return Multiply(n, A,B);
		for(int i = 0; i < n; i++) {
			for(int j = 0; j < n; j++) {
				elem_t sum = 0;
				for(int k = 0; k < n; k++) {
					sum = add(sum, mul(A[i][k], B[k][j]));
				}
				out[i][j] = sum;
			}
		}
	}

	// R = A^p; A = invalid;
	template<int sz>
	void Power(int n, elem_t R[][sz], elem_t A[][sz], unsigned long long p) {
		for(int i = 0; i < n; i++) for(int j = 0; j < n; j++) R[i][j] = 0;
		for(int i = 0; i < n; i++) R[i][i] = 1;
		while(p) {
			if (p & 1) Multiply(n,R,A);
			Multiply(n,A,A);
			p >>= 1;
		}
	}

	// R = A^(-1); A = invalid;
	template<int sz>
	bool Inverse(int n, elem_t out[][sz], elem_t A[][sz]) {
		for(int i = 0; i < n; i++) for(int j = 0; j < n; j++) out[i][j] = 0;
		for(int i = 0; i < n; i++) out[i][i] = 1;
		for(int i = 0; i < n; i++) {
			if (isZero(A[i][i])) {
				elem_t maxValue = 0;
				int maxInd = -1;
				for(int j = i+1; j < n; j++) {
					auto cur = abs(A[j][i]);
					if (maxValue < cur) {
						maxValue = cur;
						maxInd = j;
					}
				}
				if (maxInd == -1 || isZero(A[maxInd][i])) return false;
				for(int k = 0; k < n; k++) A  [i][k] = add(A  [i][k], A  [maxInd][k]);
				for(int k = 0; k < n; k++) out[i][k] = add(out[i][k], out[maxInd][k]);
			}
			elem_t coeff = inv(A[i][i]);
			for(int j = 0; j < n; j++) A  [i][j] = mul(A  [i][j], coeff);
			for(int j = 0; j < n; j++) out[i][j] = mul(out[i][j], coeff);
			for(int j = 0; j < n; j++) {
				if (j == i) continue;
				elem_t mp = A[j][i];
				for(int k = 0; k < n; k++) A  [j][k] = sub(A  [j][k], mul(A  [i][k], mp));
				for(int k = 0; k < n; k++) out[j][k] = sub(out[j][k], mul(out[i][k], mp));
			}
		}
		return true;
	}

	// return determ(A); A = invalid;
	template<int sz>
	elem_t Determinant(int n, elem_t A[][sz]) {
		elem_t res = 1;
		for(int i = 0; i < n; i++) {
			if (isZero(A[i][i])) {
				elem_t maxValue = 0;
				int maxInd = -1;
				for(int j = i+1; j < n; j++) {
					auto cur = abs(A[j][i]);
					if (maxValue < cur) {
						maxValue = cur;
						maxInd = j;
					}
				}
				if (maxInd == -1 || isZero(A[maxInd][i])) return 0;
				for(int k = 0; k < n; k++) A[i][k] = add(A[i][k], A[maxInd][k]);
			}
			res = mul(res, A[i][i]);
			elem_t coeff = inv(A[i][i]);
			for(int j = 0; j < n; j++) A[i][j] = mul(A[i][j], coeff);
			for(int j = 0; j < n; j++) {
				if (j == i) continue;
				elem_t mp = A[j][i];
				for(int k = 0; k < n; k++) A[j][k] = sub(A[j][k], mul(A[i][k], mp));
			}
		}
		return res;
	}

	// A is n x m matrix
	template<int sz>
	int RowEchelonForm(int n, int m, elem_t A[][sz]) {
		int rank = 0, entryColumn = 0;
		for(int i = 0; i < n; i++) {
			for(;entryColumn < m && isZero(A[i][entryColumn]); entryColumn++) {
				elem_t maxValue = 0;
				int maxInd = -1;
				for(int j = i+1; j < n; j++) {
					auto cur = abs(A[j][entryColumn]);
					if (maxValue < cur) {
						maxValue = cur;
						maxInd = j;
					}
				}
				if (maxInd == -1 || isZero(A[maxInd][entryColumn])) continue;
				for(int k = 0; k < m; k++) A[i][k] = add(A[i][k], A[maxInd][k]);
				break;
			}
			if (entryColumn >= m) break;
			rank++;
			elem_t coeff = inv(A[i][entryColumn]);
			for(int j = 0; j < m; j++) A[i][j] = mul(A[i][j], coeff);
			for(int j = i+1; j < n; j++) {
				elem_t mp = A[j][entryColumn];
				for(int k = 0; k < m; k++) A[j][k] = sub(A[j][k], mul(A[i][k], mp));
			}
			entryColumn++;
		}
		return rank;
	}

	// A is n x m matrix, c is n x 1 vector
	// A = invalid;
	// C = invalid;
	// returns
	//  solution -> vector<pair<isFreeVariable, value>> containing root values
	//  no solution -> empty vector
	template<int sz>
	vector<pair<bool, elem_t>> SolveLinearSystem(int n, int m, elem_t A[][sz], elem_t c[]) {
		vector<pair<bool, elem_t>> root;
		int entryColumn = 0;
		for(int i = 0; i < n; i++) {
			for(;entryColumn < m && isZero(A[i][entryColumn]); entryColumn++) {
				elem_t maxValue = 0;
				int maxInd = -1;
				for(int j = i+1; j < n; j++) {
					auto cur = abs(A[j][entryColumn]);
					if (maxValue < cur) {
						maxValue = cur;
						maxInd = j;
					}
				}
				if (maxInd == -1 || isZero(A[maxInd][entryColumn])) {
					root.emplace_back(true, 0);
					continue;
				}
				for(int k = 0; k < m; k++) A[i][k] = add(A[i][k], A[maxInd][k]);
				c[i] = add(c[i], c[maxInd]);
				break;
			}
			if (entryColumn >= m) {
				// no solution check
				for(int j = i; j < n; j++) {
					if (!isZero(c[j])) {
						root.clear();
						return root;
					}
				}
				break;
			}
			root.emplace_back(false, 0);
			elem_t coeff = inv(A[i][entryColumn]);
			for(int j = 0; j < m; j++) A[i][j] = mul(A[i][j], coeff);
			c[i] = mul(c[i], coeff);
			for(int j = 0; j < n; j++) {
				if (j == i) continue;
				elem_t mp = A[j][entryColumn];
				for(int k = 0; k < m; k++) A[j][k] = sub(A[j][k], mul(A[i][k], mp));
				c[j] = sub(c[j], mul(c[i], mp));
			}
			entryColumn++;
		}
		for(int i = 0, j = 0; i < n; i++) {
			for(;j < m && root[j].first;j++);
			if (j >= m) break;
			root[j].second = c[i];
			j++;
		}
		return root;
	}
};
