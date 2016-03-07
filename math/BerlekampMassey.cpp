#include <vector>

/* returns C such that C[0]*S[n] + C[1]*S[n-1] + C[2]*S[n-2] + ...  = 0 && C[0] = 1 */
/* TODO: SRM 684 med test */
template<typename field>
vector<field> BerlekampMassey(const vector<field> &S)
{
	const int N = S.size();
	vector<field> C(N+1,0);
	vector<field> B(N+1,0);
	C[0] = 1;
	B[0] = 1;
	int L = 0, m = 1;
	field b = 1;
	for (int n = 0; n < N; n++) {
		field d = S[n];
		for (int i = 1; i <= L; i++) {
			d += C[i] * S[n-i];
		}
		if (d == 0) {
			m++;
		} else if (2*L <= n) {
			vector<field> T = C;
			field coeff = -d / b;
			for (int i = 0; i <= N-m; i++) {
				C[m+i] += coeff * B[i];
			}
			L = n+1 - L;
			b = d;
			m = 1;
			B = move(T);
		} else {
			field coeff = -d / b;
			for (int i = 0; i <= N-m; i++) {
				C[m+i] += coeff * B[i];
			}
			m++;
		}
	}
	C.resize(L + 1);
	return C;
}

