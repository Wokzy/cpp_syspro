
#include <iostream>
#include <cassert>
#include <cmath>

#define fp64 double

using namespace std;

struct Point {
    const fp64 x;
    const fp64 y;

    Point(fp64 x, fp64 y) : x(x), y(y) {}
};

struct Line {
    // y = ax + b

private:
    fp64 a;
    fp64 b;
    bool is_vertical = false;

public:
    Line(fp64 a, fp64 b) : a(a), b(b) {}
    Line(Point &pt1, Point &pt2) {
        if (pt1.x == pt2.x) {
            is_vertical = true;
            a = INFINITY;
            b = pt1.x;
            return;
        }

        a = (pt1.y - pt2.y) / (pt1.x - pt2.x);
        b = (pt2.y - a*pt2.x);
    }

    Point FindIntersection(Line &other) {
        if (((is_vertical && other.is_vertical) || (a == other.a)) && (b != other.b)) {
            return Point(INFINITY, INFINITY);
        }

        if (((is_vertical && other.is_vertical) || (a == other.a)) && (b == other.b)) {
            return Point(0.0, b);
        }

        if (is_vertical && !other.is_vertical) {
            return Point(b, other.a * b + other.b);
        }

        if (other.is_vertical && !is_vertical) {
            return Point(other.b, a * other.b + b);
        }

        fp64 x = (other.b - b) / (a - other.a);
        return Point(x, a * x + b);
    }

    Line FindOrthogonal(Point &pt) {
        if (is_vertical) {
            return Line(0.0, pt.y);
        }

        if (a == 0) {
            return Line(pt, pt);
        }

        return Line(-1 / a, (1 / a) * pt.x + pt.y);
    }

    fp64 GetA() {
        return a;
    }

    fp64 GetB() {
        return b;
    }

    fp64 IsVertical() {
        return is_vertical;
    }
};

void test_orth_1() {
    Line ln = Line(2.0, 7.0);
    Point pt = Point(5.0, 3.0);
    Line orth = ln.FindOrthogonal(pt);

    assert(orth.GetA() == -0.5);
    assert(orth.GetB() == 5.5);
}

void test_orth_2() {
    Point pt1 = Point(4.0, 8.0);
    Point pt2 = Point(4.0, 9.0);
    Line ln = Line(pt1, pt2);

    Point pt = Point(4.0, 5.1);
    Line orth = ln.FindOrthogonal(pt);

    assert(orth.GetA() == 0.0);
    assert(orth.GetB() == 5.1);
}

void test_orth_3() {
    Point pt1 = Point(2.0, 8.0);
    Point pt2 = Point(1.0, 8.0);
    Line ln = Line(pt1, pt2);

    Point pt = Point(3.1415926535, 8.0);
    Line orth = ln.FindOrthogonal(pt);

    assert(orth.IsVertical());
    assert(orth.GetB() == 3.1415926535);
}

void test_intersection_1() {
    Line ln1 = Line(2.0, 0.0);
    Line ln2 = Line(2.0, 1.0);

    Point pt1 = ln1.FindIntersection(ln2);

    assert(pt1.x == INFINITY);
    assert(pt1.y == INFINITY);
}

void test_intersection_2() {
    Line ln1 = Line(0.0, 3.0);
    Line ln2 = Line(0.0, 3.0);

    Point pt1 = ln1.FindIntersection(ln2);

    assert(pt1.x == 0.0);
    assert(pt1.y == 3.0);
}

void test_intersection_3() {
    Point pt1 = Point(4.0, 8.0);
    Point pt2 = Point(4.0, 9.0);
    Line ln = Line(pt1, pt2);

    Point pt = Point(4.0, 5.1);
    Line orth = ln.FindOrthogonal(pt);

    Point intersect = ln.FindIntersection(orth);

    assert(intersect.x == 4.0);
    assert(intersect.y == 5.1);
}



signed main() {
    test_orth_1();
    test_orth_2();
    test_orth_3();

    test_intersection_1();
    test_intersection_2();
    test_intersection_3();

    return 0;
}


