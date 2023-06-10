#include <list>
#include <cmath>
#include "poly_utils.h"
#include "point.h"
#include "shape.h"

using namespace std;

Point add( Point& p1, Point& p2 ) {
   return Point( p1.x + p2.x, p1.y + p2.y );
}

Point subtract( Point& p1, Point& p2 ) {
  return Point( p1.x - p2.x, p1.y - p2.y);
}

double cross_product(Point& p1, Point& p2 ) {
  return p1.x * p2.y - p2.x * p1.y;
}

double direction( Point& p1, Point& p2, Point& p3 ) {
   Point p4 = subtract(p3, p1);
   Point p5 = subtract(p2, p1);
   return cross_product(p4, p5);
}

double distance( double x1, double y1, double x2, double y2 ) {
  double dx = x1 - x2;
  double dy = y1 - y2;
  return sqrt( dx * dx + dy * dy );
}

double point_distance( Point& p1, Point& p2 ) {
  return distance( p1.x, p1.y, p2.x, p2.y );
}

bool ccw(Point &p1, Point &p2, Point &p3)
{
  return direction(p1, p2, p3) < 0;
}

bool boxes_intersect( double * r1, double *r2 ) {
      return 
      r1[0] <= r2[2] &&
      r1[2] >= r2[0] &&
      r1[1] <= r2[3] &&
      r1[3] >= r2[1];
}

bool convexPolyContains( list<Point*> *points, double x, double y) {
  int first = 1;
  bool isPositive = true;
  Point* p1 = points->back();
  for( list<Point*>::iterator it1 = points->begin(); it1 != points->end(); it1++ ) {
    Point* p2 = *it1;

    double a = -( p2->y - p1->y);
    double b = p2->x - p1->x;
    double c = -( a * p1->x + b * p1->y );
    int side = a * x + b * y + c;

    if( first && side != 0 ) {
      isPositive = side > 0;
      first = 0;
    } 

    if( !first ) {
      if( side > 0 && !isPositive ) return false;
      if( side < 0 && isPositive ) return false;
    }

    p1 = p2;
  }

  return true;
}