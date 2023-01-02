#pragma once


#include <cmath>


namespace Math {

  class Vec3
  {
public:
    // Constructors
    Vec3(double x= 0.0, double y= 0.0, double z= 0.0) {
      this->x[0]= x;
      this->x[1]= y;
      this->x[2]= z;
    }
    Vec3(const Vec3& v) { *this= v; }

    // Methods
    inline void set(double a, double b, double c) {
      x[0]= a;
      x[1]= b;
      x[2]= c;
    }
    inline double dot(const Vec3& v) const { return x[0] * v.x[0] + x[1] * v.x[1] + x[2] * v.x[2]; }
    inline Vec3 cross(const Vec3& v) const {
      return Vec3(
          x[1] * v.x[2] - x[2] * v.x[1],
          x[2] * v.x[0] - x[0] * v.x[2],
          x[0] * v.x[1] - x[1] * v.x[0]);
    }
    inline double normSquared() const { return x[0] * x[0] + x[1] * x[1] + x[2] * x[2]; }
    inline double norm() const { return sqrt(normSquared()); }
    inline Vec3& normalize(double len= 1.0) {
      double l= norm();
      return (l != 0.0) ? (*this*= (len / l)) : (*this);
    }
    inline Vec3 normalized(double len= 1.0) {
      double l= norm();
      return (l != 0.0) ? (*this * len / l) : (*this);
    }
    inline Vec3 abs() {
      return Vec3(std::abs(x[0]), std::abs(x[1]), std::abs(x[2]));
    }
    inline double max() const { return std::max(std::max(x[0], x[1]), x[2]); }
    inline double min() const { return std::min(std::min(x[0], x[1]), x[2]); }

    // Operators
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

    inline double* array() { return x; }

private:
    double x[3];
  };

}  // namespace Math
