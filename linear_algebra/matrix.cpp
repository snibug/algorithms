#include <cmath>

using namespace std;

template<typename elem_t>
struct SqMatrix
{
	// examples for elem_t=double
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
				if (maxInd == -1) return false;
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
};
