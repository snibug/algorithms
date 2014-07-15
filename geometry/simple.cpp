void rotate_angle(double &x, double &y, double angle) {
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


