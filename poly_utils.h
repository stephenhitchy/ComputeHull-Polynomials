#ifndef UTILS
#define UTILS

#include <iostream>
#include <algorithm>    // std::min
#include <list>
#include "point.h"
#include "shape.h"

bool convexPolyContains( list<Point*> *points, double x, double y );
Point subtract( Point& p1, Point& p2 );
double cross_product(Point& p1, Point& p2 );
double direction( Point& p1, Point& p2, Point& p3 );
double distance( double x1, double y1, double x2, double y2 );
double point_distance( Point& p1, Point& p2 );
bool ccw( Point& p1, Point& p2, Point& p3 );
bool boxes_intersect( double *box1, double *box2 );

#endif