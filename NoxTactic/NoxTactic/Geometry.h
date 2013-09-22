#pragma once
#include <string>
#include <sstream>
#include <math.h> 
#define EPS 1E-9;
#define tg225 0.414213562373095
#define tg675 2.41421356237309
using namespace std;
enum Direction
{
	NO_DIRECTION,
	UP,
	UP_RIGHT,
	RIGHT,
	DOWN_RIGHT,
	DOWN,
	DOWN_LEFT,
	LEFT,
	UP_LEFT
};
typedef std::pair<Direction, Direction> Direction2;
template<class T> struct Vector
{
	T x, y;
    Vector(T x = T(), T y = T()): x(x), y(y){}
    //directional vector
    Vector(Direction dir) {
        switch(dir){
        case NO_DIRECTION:
            x = 0; y = 0;
        case UP:
            x = 0; y = -1;
        case DOWN:
            x = 0; y = 1;
        case UP_LEFT:
            x = -1; y = -1;
        case DOWN_LEFT:
            x = -1; y = 1;
        case UP_RIGHT:
            x = 1; y = -1;
        case DOWN_RIGHT:
            x = 1; y = 1;
        case LEFT:
            x = -1; y = 0;
        case RIGHT:
            x = 1; y = 0;
        };
    }
	//Vector(const Vector &a); //generated automatically
	Vector operator+(const Vector<T> &a) const { return Vector<T>(x+a.x, y+a.y);}
	Vector operator*(const Vector<T> &a) const { return Vector<T>(x*a.x, y*a.y);}
    Vector operator-(const Vector<T> &a) const { return Vector<T>(x-a.x, y-a.y);}
    Vector& operator+=(const Vector<T> &a) { x+=a.x; y+=a.y; return *this;}
    Vector& operator*=(const Vector<T> &a) { x*=a.x; y*=a.y; return *this;}
    Vector& operator-=(const Vector<T> &a) { x-=a.x; y-=a.y; return *this;}
    double length() { return sqrt(static_cast<double>(x*x + y*y));}
	int intlength(bool diagonal) {
        if (typeid(T) != typeid(int)) {
            throw("WRONG OPERATION");
        }
	    if (diagonal) {
		    return max(x,y);
	    } else {
		    return x + y;
        }; 
    }
	string ToString() const {
        stringstream ss;
	    ss << "[" << x << "," << y << "]";
	    return ss.str();
    }
    bool operator==(const Vector<T> &a) const {return x == a.x && y == a.y;}
    bool operator!=(const Vector<T> &a) const {return !(*this == a);}
    Vector operator-() { return (abs(x - a.x) < EPS && abs(y - a.y) < EPS);}
	//~Vector();
	friend Vector operator*(const int x, const Vector<T>& v);
	friend Vector operator*(const Vector<T> &v, const int x);
    Direction DefineDirection(const Vector<T> &a) const {
        if (*this == a) {return NO_DIRECTION;}
        Vector<T> dif = a - *this;
        if (dif.x == 0 && dif.y > 0) { return DOWN; }
        if (dif.x == 0 && dif.y < 0) { return UP; }
        if (dif.y == 0 && dif.x > 0) { return RIGHT; }
        if (dif.y == 0 && dif.x < 0) { return LEFT; }
        
        double tg = abs(dif.y / dif.x);
        if (tg < tg225) { //horizontal
            if (dif.x > 0) {return RIGHT;} else {return LEFT;}
        } else if (tg <= tg675) { //diagonal
            if (dif.x > 0 && dif.y > 0) { return UP_RIGHT; }
            if (dif.x > 0 && dif.y < 0) { return DOWN_RIGHT; }
            if (dif.x < 0 && dif.y > 0) { return UP_LEFT; }
            if (dif.x < 0 && dif.y < 0) { return DOWN_LEFT; }
        } else { //vertical
            if (dif.y > 0) {return DOWN;} else {return UP;}
        }
        return NO_DIRECTION;
    }
    Direction2 DefineDirection2(const Vector<T> &a) const {
        if (*this == a) {return make_pair(NO_DIRECTION, NO_DIRECTION);}
        Vector<T> dif = a - *this;
        if (dif.x == 0 && dif.y > 0) { return make_pair(DOWN, DOWN); }
        if (dif.x == 0 && dif.y < 0) { return make_pair(UP, UP); }
        if (dif.y == 0 && dif.x > 0) { return make_pair(RIGHT, RIGHT); }
        if (dif.y == 0 && dif.x < 0) { return make_pair(LEFT, LEFT); }
        
        double tg = abs(dif.y / dif.x);
        if (tg < tg225) { //horizontal
            if (dif.x > 0 && dif.y > 0) { return make_pair(RIGHT, DOWN_RIGHT); }
            if (dif.x > 0 && dif.y < 0) { return make_pair(RIGHT, UP_RIGHT); }
            if (dif.x < 0 && dif.y > 0) { return make_pair(LEFT, DOWN_LEFT); }
            if (dif.x < 0 && dif.y < 0) { return make_pair(LEFT, UP_LEFT); }
        } else if (tg <= tg675) { //diagonal
            if (dif.x > 0 && dif.y > 0) { 
                if (tg > 1) { return make_pair(DOWN_RIGHT, DOWN); }
                else if (tg < 1) { return make_pair(DOWN_RIGHT, RIGHT); }
                else { return make_pair(DOWN_RIGHT, DOWN_RIGHT); }
            }
            if (dif.x > 0 && dif.y < 0) { 
                if (tg > 1) { return make_pair(UP_RIGHT, UP); }
                else if (tg < 1) { return make_pair(UP_RIGHT, RIGHT); }
                else { return make_pair(UP_RIGHT, UP_RIGHT); }
            }
            if (dif.x < 0 && dif.y > 0) { 
                if (tg > 1) { return make_pair(DOWN_LEFT, DOWN); }
                else if (tg < 1) { return make_pair(DOWN_LEFT, LEFT); }
                else { return make_pair(DOWN_LEFT, DOWN_LEFT); }
            }
            if (dif.x < 0 && dif.y < 0) { 
                if (tg > 1) { return make_pair(UP_LEFT, UP); }
                else if (tg < 1) { return make_pair(UP_LEFT, LEFT); }
                else { return make_pair(UP_LEFT, UP_LEFT); }
            }
        } else { //vertical
            if (dif.x > 0 && dif.y > 0) { return make_pair(DOWN, DOWN_RIGHT); }
            if (dif.x > 0 && dif.y < 0) { return make_pair(UP, UP_RIGHT); }
            if (dif.x < 0 && dif.y > 0) { return make_pair(DOWN, DOWN_LEFT); }
            if (dif.x < 0 && dif.y < 0) { return make_pair(UP, UP_LEFT); }
        }
        return make_pair(NO_DIRECTION, NO_DIRECTION);
    }
    bool IsInside (const Vector<T>& v1, const Vector<T>& v2) const {
        return (x < v2.x && x > v1.x && y < v2.y && y > v1.y);
    }
};



template<class T> Vector<T> operator*(const int x, const Vector<T>& v) {
	return Vector<T>(v.x * x, v.y * x);
}
template<class T> Vector<T> operator*(const Vector<T> &v, const int x) {
	return Vector<T>(v.x * x, v.y * x);
}
template<class T> Vector<T> operator/(const int x, const Vector<T>& v) {
	return Vector<T>(v.x / x, v.y / x);
}
template<class T> Vector<T> operator/(const Vector<T> &v, const int x) {
	return Vector<T>(v.x / x, v.y / x);
}
#define PI 3.14159265
typedef Vector<int> vInt;
typedef Vector<double> vDbl;


enum ReflectType
{
    REFLECT_POINT,
    REFLECT_VERTICAL,
    REFLECT_HORIZONTAL
};

class Angle
{
    int degrees; //0 is at Ox direction; + is clock-wise
public:
    Angle(): degrees(0) {}
    Angle(const vInt& start, const vInt& dest) {
        if (start == dest) {
            throw ("bad vector");
        }

        if (start.x == dest.x) {
            if (dest.x > start.x) {
                degrees = 0;
            } else {
                degrees = 180;
            }
            return;
        }
        if (start.y == dest.y) {
            if (dest.y > start.y) {
                degrees = 90;
            } else {
                degrees = 270;
            }
            return;
        }

        degrees = static_cast<int>(atan2(static_cast<double>(dest.y-start.y), dest.x-start.x) * 180 / PI);
    }
    void Reflect(ReflectType type) {
        switch (type) {
        case REFLECT_POINT:
            degrees = (degrees + 180) % 360;
            break;
        case REFLECT_VERTICAL:
            degrees = (540 - degrees) % 360;
            break;
        case REFLECT_HORIZONTAL:
            degrees = 360 - degrees;
            break;
        };
    }
    Direction ToDirection() const {
        //dividing into quadrants and analyzing an in-quadrant deviation
        int tmp = degrees % 90;
        if (degrees < 90) {
            if (tmp < 22) {
                return RIGHT;
            } else if (tmp < 68) {
                return DOWN_RIGHT;
            } else {
                return DOWN;
            }
        } else if (degrees < 180) {
            if (tmp < 22) {
                return DOWN;
            } else if (tmp < 68) {
                return DOWN_LEFT;
            } else {
                return LEFT;
            }
        } else if (degrees < 270) {
            if (tmp < 22) {
                return LEFT;
            } else if (tmp < 68) {
                return UP_LEFT;
            } else {
                return UP;
            }
        } else {
            if (tmp < 22) {
                return UP;
            } else if (tmp < 68) {
                return UP_RIGHT;
            } else {
                return RIGHT;
            }
        }
    }
    Direction ToMainDirection() const { 
        if (degrees < 45 || degrees > 315) { return RIGHT; }
        if (degrees < 135) { return DOWN; }
        if (degrees < 225) { return LEFT; }
        return UP;
    }
    double Tg() const { 
        return tan(static_cast<double>(degrees) * PI / 180); 
    }
    vDbl ProjectileIncrement() const {
        switch (ToMainDirection()) {
        case RIGHT:
            return vDbl(1.0, Tg());
        case LEFT:
            return vDbl(-1.0, Tg());
        case UP:
            return vDbl(1/Tg(), -1.0);
        case DOWN:
            return vDbl(1/Tg(), 1.0);
        }
        return vDbl();
};
};