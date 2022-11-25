#pragma once


#include <cmath>


namespace math {

  class Vec2
  {
public:
    Vec2(double x= 0.0, double y= 0.0) {
      this->x[0]= x;
      this->x[1]= y;
    }
    Vec2(const Vec2& v) { *this= v; }
    Vec2(double* v) {
      x[0]= v[0];
      x[1]= v[1];
    }
    inline Vec2& set(double a, double b) {
      x[0]= a;
      x[1]= b;
      return *this;
    }
    inline double dot(const Vec2& v) const { return x[0] * v.x[0] + x[1] * v.x[1]; }
    inline double length() const { return sqrt(length2()); }
    inline double length2() const { return x[0] * x[0] + x[1] * x[1]; }
    inline Vec2& normalize(double len= 1.0) {
      double l= length();
      return (l != 0.0) ? (*this*= (len / l)) : (*this);
    }
    inline double operator[](int idx) const { return x[idx]; }
    inline Vec2& operator=(const Vec2& v) {
      x[0]= v.x[0];
      x[1]= v.x[1];
      return *this;
    }
    inline bool operator==(const Vec2& v) { return ((x[0] == v.x[0]) && (x[1] == v.x[1])); }
    inline Vec2& operator+=(const Vec2& v) {
      x[0]+= v.x[0];
      x[1]+= v.x[1];
      return *this;
    }
    inline Vec2& operator-=(const Vec2& v) {
      x[0]-= v.x[0];
      x[1]-= v.x[1];
      return *this;
    }
    inline Vec2& operator*=(double f) {
      x[0]*= f;
      x[1]*= f;
      return *this;
    }
    inline Vec2& operator/=(double f) {
      x[0]/= f;
      x[1]/= f;
      return *this;
    }
    friend Vec2 operator+(const Vec2& w, const Vec2& v) { return Vec2(w)+= v; }
    friend Vec2 operator-(const Vec2& w, const Vec2& v) { return Vec2(w)-= v; }
    friend Vec2 operator*(const Vec2& v, double f) { return Vec2(v)*= f; }
    friend Vec2 operator*(double f, const Vec2& v) { return Vec2(v)*= f; }
    friend Vec2 operator/(const Vec2& v, double f) { return Vec2(v)/= f; }
    friend Vec2 operator/(double f, const Vec2& v) { return Vec2(v)/= f; }
    friend double operator*(const Vec2& a, const Vec2& b) { return a.dot(b); }

private:
    double x[2];
  };

  class Vec3
  {
public:
    Vec3(double x= 0.0, double y= 0.0, double z= 0.0) {
      this->x[0]= x;
      this->x[1]= y;
      this->x[2]= z;
    }
    Vec3(const Vec3& v) { *this= v; }
    Vec3(double* v) {
      x[0]= v[0];
      x[1]= v[1];
      x[2]= v[2];
    }
    inline Vec3& set(double a, double b, double c) {
      x[0]= a;
      x[1]= b;
      x[2]= c;
      return *this;
    }
    inline double dot(const Vec3& v) const { return x[0] * v.x[0] + x[1] * v.x[1] + x[2] * v.x[2]; }
    inline Vec3 cross(const Vec3& v) const {
      return Vec3(
          x[1] * v.x[2] - x[2] * v.x[1],
          x[2] * v.x[0] - x[0] * v.x[2],
          x[0] * v.x[1] - x[1] * v.x[0]);
    }
    inline double length() const { return sqrt(length2()); }
    inline double length2() const { return x[0] * x[0] + x[1] * x[1] + x[2] * x[2]; }
    inline Vec3& normalize(double len= 1.0) {
      double l= length();
      if ((l < 1.0e-7) && (l > -1.0e-7)) return *this;
      l= len / l;
      x[0]*= l;
      x[1]*= l;
      x[2]*= l;
      return *this;
    }
    inline Vec3 normalized(double len= 1.0) {
      double l= length();
      if ((l < 1.0e-7) && (l > -1.0e-7)) return *this;
      l= len / l;
      x[0]*= l;
      x[1]*= l;
      x[2]*= l;
      return *this/l;
    }
    inline double operator[](int idx) const { return x[idx]; }
    inline double& operator[](int idx) { return x[idx]; }
    inline operator const double*(void) const { return x; }
    inline Vec3& operator=(const Vec3& v) {
      x[0]= v.x[0];
      x[1]= v.x[1];
      x[2]= v.x[2];
      return *this;
    }
    inline bool operator==(const Vec3& v) { return ((x[0] == v.x[0]) && (x[1] == v.x[1]) && (x[2] == v.x[2])); }
    inline Vec3& operator+=(const Vec3& v) {
      x[0]+= v.x[0];
      x[1]+= v.x[1];
      x[2]+= v.x[2];
      return *this;
    }
    inline Vec3& operator-=(const Vec3& v) {
      x[0]-= v.x[0];
      x[1]-= v.x[1];
      x[2]-= v.x[2];
      return *this;
    }
    inline Vec3& operator*=(double f) {
      x[0]*= f;
      x[1]*= f;
      x[2]*= f;
      return *this;
    }
    inline Vec3& operator/=(double f) {
      x[0]/= f;
      x[1]/= f;
      x[2]/= f;
      return *this;
    }
    friend Vec3 operator+(const Vec3& w, const Vec3& v) { return Vec3(w)+= v; }
    friend Vec3 operator-(const Vec3& w, const Vec3& v) { return Vec3(w)-= v; }
    friend Vec3 operator*(const Vec3& v, double f) { return Vec3(v)*= f; }
    friend Vec3 operator*(double f, const Vec3& v) { return Vec3(v)*= f; }
    friend Vec3 operator/(const Vec3& v, double f) { return Vec3(v)/= f; }
    friend Vec3 operator/(double f, const Vec3& v) { return Vec3(v)/= f; }
    friend double operator*(const Vec3& a, const Vec3& b) { return a.dot(b); }

private:
    double x[3];
  };

}  // namespace math
