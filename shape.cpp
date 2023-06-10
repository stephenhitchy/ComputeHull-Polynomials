#include <iostream>
#include <algorithm>
#include <math.h>
#include <set>
#include "shape.h"
#include "poly_utils.h"

using namespace std;

list<Point *> *Shape::computeHull(Shape *c1, Shape *c2)
{
    Point *smallest = new Point(PAGE_DIMENSION + 1, PAGE_DIMENSION + 1);
    list<Point*> *output = new list<Point*>;
    Point *q, *i, *l;
    for (int i = 0; i < c1->hull->size(); i++)
    {
        if (smallest->x > c1->hull->back()->x || smallest->x > c2->hull->back()->x)
        {
            if (c1->hull->back()->x < c2->hull->back()->x)
            {
                smallest = c1->hull->back();
            }
            else
            {
                smallest = c2->hull->back();
            }
        }
        c1->hull->push_front(c1->hull->back());
        c2->hull->push_front(c2->hull->back());
        c1->hull->pop_back();
        c2->hull->pop_back();
    }
    output->push_back(smallest);
    l = smallest;
    int x = 0;
    while (x < c1->hull->size() + c2->hull->size())
    {
        q = c2->hull->front();
        for (int y = 0; y < c1->hull->size() + c2->hull->size(); y++)
        {
            if (y < c1->hull->size())
            {
                i = c1->hull->back();
                c1->hull->push_front(c1->hull->back());
                c1->hull->pop_back();
            }
            else
            {
                i = c2->hull->back();
                c2->hull->push_front(c2->hull->back());
                c2->hull->pop_back();
            }
            if (ccw(*l, *i, *q) == true || (direction(*l, *i, *q) == 0 && point_distance(*i, *l) > point_distance(*q, *l)))
            {
                q = i;
            }
        }
        l = q;
        output->push_front(q);

        if (l == smallest)
        {
            break;
        }
    }
    return output;
}

Shape::Shape(Shape *shape1, Shape *shape2)
{
    if (shape1->id < shape2->id)
    {
        this->id = shape1->id + "-" + shape2->id;
    }
    else
    {
        this->id = shape2->id + "-" + shape1->id;
    }
    this->s1 = shape1;
    this->s2 = shape2;
    this->hull = computeHull(shape1, shape2);
}

Shape::Shape(string ident, bool randomize)
{
    hull = new list<Point *>();
    if (randomize == true)
    {
        for (int x = 0; x < 3; x++)
        {
            double first = (rand() / (double)RAND_MAX) * PAGE_DIMENSION;
            double second = (rand() / (double)RAND_MAX) * PAGE_DIMENSION;
            while (abs(second - first) > DEFAULT_MAX_RADIUS)
            {
                second = (rand() / (double)RAND_MAX) * PAGE_DIMENSION;
            }
            Point *p = new Point(first, second);
            hull->push_back(p);
        }
    }
    this->id = ident;
    this->s1 = nullptr;
    this->s2 = nullptr;
}

double *Shape::bounding_box()
{
    double *output = new double[4];
    double minX = PAGE_DIMENSION, minY = PAGE_DIMENSION, maxX = 0, maxY = 0;
    for (int x = 0; x < this->hull->size(); x++)
    {
        if (minX > this->hull->back()->x)
        {
            minX = this->hull->back()->x;
        }
        if (minY > this->hull->back()->y)
        {
            minY = this->hull->back()->y;
        }
        if (maxX < this->hull->back()->x)
        {
            maxX = this->hull->back()->x;
        }
        if (maxY < this->hull->back()->y)
        {
            maxY = this->hull->back()->y;
        }
        this->hull->push_front(this->hull->back());
        this->hull->pop_back();
    }
    output[0] = minX;
    output[1] = maxY;
    output[2] = maxX;
    output[3] = minY;
    return output;
}

bool Shape::isLeaf() const
{
    if (this->s1 == nullptr && this->s2 == nullptr)
    {
        return true;
    }
    return false;
}

Point *Shape::getCentroid() const
{
    list<Point *> *hullCopy = hull;
    Point *a, *b, *c;
    a = hullCopy->back();
    hullCopy->pop_back();
    b = hullCopy->back();
    c = hullCopy->front();

    double xCentroid, yCentroid;
    xCentroid = (a->x + b->x + c->x) / 3;
    yCentroid = (a->y + b->y + c->y) / 3;

    Point *centroid = new Point(xCentroid, yCentroid);

    return centroid;
}

double Shape::getCentroidX() const
{
    list<Point *> *hullCopy = hull;
    Point *a, *b, *c;
    a = hullCopy->back();
    hullCopy->pop_back();
    b = hullCopy->back();
    c = hullCopy->front();

    double *xCentroid = new double();
    *xCentroid = (a->x + b->x + c->x) / 3;
    return *xCentroid;
}

double Shape::getCentroidY() const
{
    list<Point *> *hullCopy = hull;
    Point *a, *b, *c;
    a = hullCopy->back();
    hullCopy->pop_back();
    b = hullCopy->back();
    c = hullCopy->front();
    double *yCentroid = new double();
    *yCentroid = (a->y + b->y + c->y) / 3;
    return *yCentroid;
}

double Shape::area() const
{
    double *area = new double();
    double *small = new double();
    *small = PAGE_DIMENSION + 1;
    *area = 0.0;
    for (int x = 0; x < this->hull->size(); x++)
    {
        if (*small > this->hull->back()->x)
        {
            *small = this->hull->back()->x;
        }
        this->hull->push_front(this->hull->back());
        this->hull->pop_back();
    }

    while (*small != this->hull->back()->x)
    {
        this->hull->push_front(this->hull->back());
        this->hull->pop_back();
    }

    for (int x = 0; x < this->hull->size(); x++)
    {
        *area = *area + this->hull->back()->x * this->hull->front()->y;
        *area = *area - this->hull->back()->y * this->hull->front()->x;
        this->hull->push_front(this->hull->back());
        this->hull->pop_back();
    }
    return abs(*area) * .5;
}

list<Point *> *Shape::getPoints() const
{
    list<Point *> *output = new list<Point *>;
    output = this->hull;
    return output;
}

string Shape::getId() const
{
    return this->id;
}

Shape *Shape::getFirstSubShape() const
{
    return nullptr;
}

Shape *Shape::getSecondSubShape() const
{
    return nullptr;
}

Shape *Shape::operator<<(Point *p)
{
    this->hull->push_back(p);
    return this;
}

bool Shape::operator<(Shape &other)
{
    return this->area() < other.area();
}

Shape *Shape::operator()(double x_in, double y_in)
{
    Shape *cur = new Shape("shape", false);
    Shape *output = new Shape("shape", false);
    Shape *left = new Shape("shape", false);
    Shape *right = new Shape("shape", false);
    cur = this;
    output = NULL;
    if (cur->s1 != nullptr && cur->s2 != nullptr)
    {
        
        left = (*(cur->s1))(x_in, y_in);
        right = (*(cur->s2))(x_in, y_in);
        if (left != NULL)
        {
            if (left->area() < cur->area() && convexPolyContains(left->hull, x_in, y_in))
            {
                output = left;
            }
        }
        if (right != NULL)
        {
            if (cur->area() > right->area() && convexPolyContains(right->hull, x_in, y_in))
            {
                output = right;
                if (left != NULL)
                {
                    if (left->area() < right->area() && convexPolyContains(left->hull, x_in, y_in))
                    {
                        output = left;
                    }
                }
            }
        }
    }
    if (convexPolyContains(cur->hull, x_in, y_in))
    {
        return cur;
    }
    return output;
}

Shape &Shape::operator=(Shape &rhs)
{
    this->hull = rhs.hull;
    this->id = rhs.id;
    this->s1 = rhs.s1;
    this->s2 = rhs.s2;
    return *this;
}

Shape *Shape::operator,(Shape &other)
{
    Shape *output = new Shape(this, &other);
    return output;
}

double Shape::operator-(Shape &rhs)
{
    list<Point *> *hullCopy = this->hull;
    list<Point *> *secondCopy = rhs.hull;
    double *length = new double();
    double temp;
    *length = (double)PAGE_DIMENSION + 1;
    for (int x = 0; x < this->hull->size(); x++)
    {
        for (int y = 0; y < rhs.hull->size(); y++)
        {
            temp = point_distance(*hullCopy->back(), *secondCopy->back());
            if (temp < *length)
            {
                *length = point_distance(*hullCopy->back(), *secondCopy->back());
            }
            secondCopy->push_front(secondCopy->back());
            secondCopy->pop_back();
        }
        hullCopy->push_front(hullCopy->back());
        hullCopy->pop_back();
    }
    return *length;
}

ostream &operator<<(ostream &os, Shape &s)
{
    os << '[' << s.id << ", (";
    for (int x = 0; x < s.hull->size(); x++)
    {
        if (x != s.hull->size() - 1)
        {
            os << s.hull->back()->x << ", " << s.hull->back()->y << "), (";
        }
        else
        {
            os << s.hull->back()->x << ", " << s.hull->back()->y << ")";
        }
        s.hull->push_front(s.hull->back());
        s.hull->pop_back();
    }
    os << ']' << endl;
    return os;
}

ostream &operator/(ostream &os, Shape &s)
{
    if (s.s1 != NULL && s.s2 != NULL)
    {
        os / *s.s1;
        os / *s.s2;
        
        os << "<polyline points= \"";
        for (int x = 0; x < s.hull->size(); x++)
        {
            os << s.hull->back()->x << " " << s.hull->back()->y << " ";
            s.hull->push_front(s.hull->back());
            s.hull->pop_back();
        }
        os << " \" style=\"stroke:blue;stroke-width:.15;stroke-opacity:.25;fill:green;fill-opacity:.05;\" name=\"" << s.id << "\" />" << endl;
    }
    else if (s.s1 == NULL && s.s2 == NULL)
    {
        os << "<polyline points= \"";
        for (int x = 0; x < s.hull->size(); x++)
        {
            os << s.hull->back()->x << " " << s.hull->back()->y << " ";
            s.hull->push_front(s.hull->back());
            s.hull->pop_back();
        }
        os << " \" style=\"stroke:black;stroke-width:.1;stroke-opacity:.9;fill:orange;fill-opacity:.4\" name=\"" << s.id << "\" />" << endl;
    }
    return os;
}

Shape *reduce(set<Shape *> &shapes)
{
    std::set<Shape *>::iterator end = shapes.end();
    while (shapes.size() > 1)
    {
        std::set<Shape *>::iterator regionsIterator = shapes.begin();
        auto *firstShape = *regionsIterator;
        regionsIterator++;
        auto *secondShape = *regionsIterator;
        double baseDist = (*firstShape - *secondShape);
        for (set<Shape *>::iterator regionsIterator1 = shapes.begin(); regionsIterator1 != shapes.end(); ++regionsIterator1)
        {
            Shape *iteratorValueFirst = *regionsIterator1;
            set<Shape *>::iterator start = regionsIterator1;
            for (set<Shape *>::iterator regionsIterator2 = ++start; regionsIterator2 != end; ++regionsIterator2)
            {
                Shape *iteratorValueSecond = *regionsIterator2;
                double dist = (*iteratorValueFirst - *iteratorValueSecond);
                if (dist < baseDist)
                {
                    firstShape = iteratorValueFirst;
                    secondShape = iteratorValueSecond;
                    baseDist = dist;
                }
                else if (abs(dist - baseDist) < .000001)
                {

                    double area1 = iteratorValueFirst->area(), area2 = iteratorValueSecond->area();
                    if (area1 < area2)
                    {
                        firstShape = iteratorValueFirst;
                        secondShape = iteratorValueSecond;
                    }
                    else
                    {
                        firstShape = iteratorValueSecond;
                        secondShape = iteratorValueFirst;
                    }
                }
            }
        }
        Shape *sh1 = firstShape, *sh2 = secondShape;
        shapes.erase(firstShape);
        shapes.erase(secondShape);

        Shape *output = new Shape(sh1, sh2);

        shapes.insert(output);
    }

    return *shapes.begin();
}
