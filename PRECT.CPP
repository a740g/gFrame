/****************************************************************************\
*																			 *
*	Copyright (c) Samuel Gomes (Blade), 1998-2003.							 *
*	All rights reserved.													 *
*																			 *
*	mailto: blade_go@hotmail.com || blade_g@rediffmail.com					 *
*																			 *
*	Point and Rectange classes.   											 *
*																			 *
\****************************************************************************/

#include <gFrame.h>

Point::Point(int X, int Y) {
	x = X;
	y = Y;
}

Point::Point() {
	x = y = 0;
}

Point& Point::operator +=(const Point& adder) {
	x += adder.x;
	y += adder.y;

	return *this;
}

Point& Point::operator -=(const Point& subber) {
	x -= subber.x;
	y -= subber.y;

	return *this;
}

Point Point::operator -(const Point& p) {
	return Point(x - p.x, y - p.y);
}

Point Point::operator +(const Point& p) {
	return Point(x + p.x, y + p.y);
}

bool Point::operator ==(const Point& p) const {
	return bool(x == p.x && y == p.y);
}

bool Point::operator !=(const Point& p) const {
	return bool(x != p.x || y != p.y);
}

Rect::Rect(int ax, int ay, int bx, int by) {
	a.x = ax;
	a.y = ay;
	b.x = bx;
	b.y = by;
}

Rect::Rect(Point p1, Point p2) {
	a = p1;
	b = p2;
}

Rect::Rect() {
}

void Rect::Move(int aDX, int aDY) {
	a.x += aDX;
	a.y += aDY;
	b.x += aDX;
	b.y += aDY;
}

void Rect::Grow(int aDX, int aDY) {
	a.x -= aDX;
	a.y -= aDY;
	b.x += aDX;
	b.y += aDY;
}

void Rect::Intersect(const Rect& r) {
	a.x = MAX(a.x, r.a.x);
	a.y = MAX(a.y, r.a.y);
	b.x = MIN(b.x, r.b.x);
	b.y = MIN(b.y, r.b.y);
}

void Rect::Union(const Rect& r) {
	a.x = MIN(a.x, r.a.x);
	a.y = MIN(a.y, r.a.y);
	b.x = MAX(b.x, r.b.x);
	b.y = MAX(b.y, r.b.y);
}

bool Rect::Contains(const Point& p) const {
	return bool(p.x >= a.x && p.x < b.x && p.y >= a.y && p.y < b.y);
}

bool Rect::operator ==(const Rect& r) const {
	return bool(a == r.a && b == r.b);
}

bool Rect::operator !=(const Rect& r) const {
	return bool(!(*this == r));
}

bool Rect::IsEmpty() {
	return bool(a.x >= b.x || a.y >= b.y);
}
