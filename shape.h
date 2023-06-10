#ifndef SHAPE
#define SHAPE

#include <set>
#include <list>
#include <iostream>
#include "point.h"

using namespace std;

class Shape {
 private:
  list<Point*> *hull;
  Shape* s1;
  Shape* s2;
  list<Point*> *computeHull(Shape *c1, Shape *c2 );
  string id;
  
 public:
  static const int PAGE_DIMENSION = 200;
  static const int DEFAULT_MAX_RADIUS = 20;
  
  Shape( Shape* s1,  Shape* s2 );
  Shape( string id, bool randomize );
  double *bounding_box();
  
  bool isLeaf() const;
  Point* getCentroid() const;
  double getCentroidX( ) const;
  double getCentroidY() const;
  double area() const;
  list<Point*> *getPoints() const;

  string getId() const; 
  Shape* getFirstSubShape() const;
  Shape* getSecondSubShape() const;
  
  Shape* operator<<(Point *p);
  bool operator<( Shape& other );
  Shape* operator()( double x_in, double y_in);
  Shape& operator=( Shape& rhs);
  Shape* operator,( Shape& other ); 
  double operator-( Shape& rhs );

  // THESE FUNCTIONS ARE NOT CLASS MEMBERS
  // THEY ARE DEFINED OUTSIDE OF THE CLASS
  // BUT REQUIRE ACCESS TO PRIVATE FIELDS
  friend ostream& operator<<(ostream&, Shape&);
  friend ostream& operator/(ostream&, Shape&);
  friend Shape* reduce( set<Shape*>&  );
  friend bool ccw( Point& p1, Point& p2, Point& p3);
};

#endif
