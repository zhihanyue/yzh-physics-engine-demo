#pragma once
#include <cmath>
struct SVecF
{
	double x, y;
	SVecF(){ x = 0.0; y = 0.0; };
	SVecF(double _x, double _y){ x = _x; y = _y; };
	double Angle() { return atan2(y, x); }
	static SVecF Add(SVecF a, SVecF b) { return SVecF(a.x + b.x, a.y + b.y); }
	static SVecF Dec(SVecF a, SVecF b) { return SVecF(a.x - b.x, a.y - b.y); }
	static double Dot(SVecF a, SVecF b) { return a.x * b.x + a.y * b.y; }
	double Norm(){ return std::sqrt(Dot(*this, *this)); }
	SVecF operator *(double a) { return SVecF(x*a,y*a); }
	SVecF operator /(double a) { return SVecF(x/a, y/a); }
	SVecF operator +(const SVecF &a) { return SVecF(x + a.x, y + a.y); }
	SVecF operator -(const SVecF &a) { return SVecF(x - a.x, y - a.y); }
	bool operator ==(const SVecF &a) { return x == a.x && y == a.y; }
	bool operator !=(const SVecF &a) { return x != a.x || y != a.y; }
};
typedef SVecF SPointF;
