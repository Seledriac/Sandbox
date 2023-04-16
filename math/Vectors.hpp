#pragma once


#include <cmath>


namespace Math {

  class Vec3
  {
public:
    // Constructors
    Vec3(double a= 0.0, double b= 0.0, double c= 0.0) {
      this->x[0]= a;
      this->x[1]= b;
      this->x[2]= c;
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

  /**/
  class Vec4
  {
public:
    // Constructors
    Vec4(double a= 0.0, double b= 0.0, double c= 0.0, double d= 0.0) {
      this->x[0]= a;
      this->x[1]= b;
      this->x[2]= c;
      this->x[3]= d;
    }
    Vec4(const Vec4& v) { *this= v; }

    // Methods
    inline void set(double a, double b, double c, double d) {
      x[0]= a;
      x[1]= b;
      x[2]= c;
      x[3]= d;
    }
    inline double dot(const Vec4& v) const { return x[0] * v.x[0] + x[1] * v.x[1] + x[2] * v.x[2] + x[3] * v.x[3]; }
    inline double normSquared() const { return x[0] * x[0] + x[1] * x[1] + x[2] * x[2] + x[3] * x[3]; }
    inline double norm() const { return sqrt(normSquared()); }
    inline Vec4& normalize(double len= 1.0) {
      double l= norm();
      return (l != 0.0) ? (*this*= (len / l)) : (*this);
    }
    inline Vec4 normalized(double len= 1.0) {
      double l= norm();
      return (l != 0.0) ? (*this * len / l) : (*this);
    }
    inline Vec4 abs() {
      return Vec4(std::abs(x[0]), std::abs(x[1]), std::abs(x[2]), std::abs(x[3]));
    }
    inline double max() const { return std::max(std::max(std::max(x[0], x[1]), x[2]), x[3]); }
    inline double min() const { return std::min(std::min(std::min(x[0], x[1]), x[2]), x[3]); }

    // Operators
    inline double operator[](int idx) const { return x[idx]; }
    inline double& operator[](int idx) { return x[idx]; }
    inline operator const double*(void) const { return x; }
    inline Vec4& operator=(const Vec4& v) {
      x[0]= v.x[0];
      x[1]= v.x[1];
      x[2]= v.x[2];
      x[3]= v.x[3];
      return *this;
    }
    inline bool operator==(const Vec4& v) { return ((x[0] == v.x[0]) && (x[1] == v.x[1]) && (x[2] == v.x[2]) && (x[3] == v.x[3])); }
    inline Vec4& operator+=(const Vec4& v) {
      x[0]+= v.x[0];
      x[1]+= v.x[1];
      x[2]+= v.x[2];
      x[3]+= v.x[3];
      return *this;
    }
    inline Vec4& operator-=(const Vec4& v) {
      x[0]-= v.x[0];
      x[1]-= v.x[1];
      x[2]-= v.x[2];
      x[3]-= v.x[3];
      return *this;
    }
    inline Vec4& operator*=(double f) {
      x[0]*= f;
      x[1]*= f;
      x[2]*= f;
      x[3]*= f;
      return *this;
    }
    inline Vec4& operator/=(double f) {
      x[0]/= f;
      x[1]/= f;
      x[2]/= f;
      x[3]/= f;
      return *this;
    }
    friend Vec4 operator+(const Vec4& w, const Vec4& v) { return Vec4(w)+= v; }
    friend Vec4 operator-(const Vec4& w, const Vec4& v) { return Vec4(w)-= v; }
    friend Vec4 operator*(const Vec4& v, double f) { return Vec4(v)*= f; }
    friend Vec4 operator*(double f, const Vec4& v) { return Vec4(v)*= f; }
    friend Vec4 operator/(const Vec4& v, double f) { return Vec4(v)/= f; }
    friend Vec4 operator/(double f, const Vec4& v) { return Vec4(v)/= f; }
    friend double operator*(const Vec4& a, const Vec4& b) { return a.dot(b); }

    inline double* array() { return x; }

private:
    double x[4];
  };
  /**/

}  // namespace Math
