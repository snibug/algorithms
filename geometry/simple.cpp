#include <cmath>

using namespace std;

const double eps = 1e-9;
const double PI = atan(1.0)*4;

struct circle
{
	double x, y, r;
};

double getCenterDist(const circle &a, const circle &b) {
	double dx = a.x - b.x;
	double dy = a.y - b.y;
	return sqrt(dx*dx + dy*dy);
}


bool isIn(const circle &a, const circle &b) {
	double d = getCenterDist(a,b);
	return d + a.r <= b.r + eps;
}

// obtained by Green's theorem
// integral_{C} (L dx + M dy) = doubleintegral_{D} [partial_x(M) - partial_y(L) dx dy]
//
//  1
// --- integral_{C} [x dy - y dx]
//  2
//
//  x = x_c + r_c cos t
//  y = y_c + r_c sin t
//
//  t parametrization
//  dx = -r_c sin t dt
//  dy =  r_c cos t dt
double getLineIntegral(double x, double y, double r, double th1, double th2) {
	double ds = sin(th2) - sin(th1);
	double dc = cos(th2) - cos(th1);
	return 0.5 * ( r*(x*ds-y*dc) + r*r*(th2 - th1));
}

double rangeAngle(double a, double b, double d) {
	return acos((d*d+a*a-b*b)/(2*a*d));
}


void rotateAngle(double &x, double &y, double angle) {
	double nx = x*cos(angle) - y*sin(angle);
	double ny = x*sin(angle) + y*cos(angle);
	x = nx, y = ny;
}

Point circumCenter(Point p1, Point p2, Point p3) {
	double cx, cy;
	{
		double x1 = p1.x, x2 = p2.x, x3 = p3.x;
		double y1 = p1.y, y2 = p2.y, y3 = p3.y;
		double a = x1*(y2-y3)+y1*(x3-x2)+x2*y3-y2*x3;
		double f1 = x1*x1+y1*y1;
		double f2 = x2*x2+y2*y2;
		double f3 = x3*x3+y3*y3;
		double bx = +(y2*(f3-f1)+y3*(f1-f2)+y1*(f2-f3));
		double by = -(x2*(f3-f1)+x3*(f1-f2)+x1*(f2-f3));
		cx = -bx/(2*a);
		cy = -by/(2*a);
	}
	return Point(cx, cy);
}


