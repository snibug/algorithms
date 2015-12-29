#include <math.h>
#include <stdbool.h>

int shft = 17;
int sz = 1<<17;

int RB(int i) {
	int revbit = 0;
	for (int j = 0; j < shft; j++) if (i&(1<<j)) revbit |= (1<<(shft-j-1));
	return revbit;
}

void rbarray(double *a) {
	for (int i = 0; i < sz; i++) {
		if (RB(i) < i) {
			double t1 = a[i*2], t2 = a[i*2+1];
			a[i*2] = a[RB(i)*2];
			a[i*2+1] = a[RB(i)*2+1];
			a[RB(i)*2] = t1;
			a[RB(i)*2+1] = t2;
		}
	}
}

void dft(double *buf, bool rev) {
	const double PI = atan(1.0)*4;
	double unit = -2*PI;
	if (rev) unit = -unit;
	for (int step = 1; step < sz; step += step) {
		for (int s = 0; s < sz; s += 2*step) {
			for (int i = 0; i < step; i++) {
				double rad = unit*i/(step*2);
				double tr = cos(rad), ti = sin(rad);
				double r0 = buf[(s+i)*2];
				double i0 = buf[(s+i)*2+1];
				double r1 = buf[(s+i+step)*2]*tr - buf[(s+i+step)*2+1]*ti;
				double i1 = buf[(s+i+step)*2]*ti + buf[(s+i+step)*2+1]*tr;
				buf[(s+i)*2] = r0+r1;
				buf[(s+i)*2+1] = i0+i1;
				buf[(s+i+step)*2] = r0-r1;
				buf[(s+i+step)*2+1] = i0-i1;
			}
		}
	}
}


#include <stdio.h>
#include <stdlib.h>

int main()
{
	double *a = calloc(sz*2, sizeof(double));
	for (int i = 0; i < 4; i++) {
		scanf("%lf",&a[RB(i)*2]);
		a[RB(i)*2+1] = 0;
	}
	dft(a, false);
	for (int i = 0; i < sz; i++) {
		double rr = a[i*2], ii = a[i*2+1];
		a[i*2] = rr*rr-ii*ii;
		a[i*2+1] = rr*ii+rr*ii;
	}

	rbarray(a);
	dft(a, true);

	for (int i = 0; i < 7; i++) {
		printf("%.6f\n", a[i*2] / sz);
	}
	free(a);
	return 0;
}
