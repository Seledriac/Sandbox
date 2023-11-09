#pragma once

// Standard lib
#include <cmath>


namespace Math {
  class Vec2f
  {
public:
    // Constructors
    Vec2f(float a= 0.0, float b= 0.0) {
      this->x[0]= a;
      this->x[1]= b;
    }
    Vec2f(const Vec2f& v) { *this= v; }

    // Methods
    inline void set(float a, float b) {
      x[0]= a;
      x[1]= b;
    }
    inline float dot(const Vec2f& v) const { return x[0] * v.x[0] + x[1] * v.x[1]; }
    inline Vec2f coeffMul(const Vec2f& v) const { return Vec2f(x[0] * v.x[0], x[1] * v.x[1]); }
    inline Vec2f coeffDiv(const Vec2f& v) const { return Vec2f(x[0] / v.x[0], x[1] / v.x[1]); }
    inline float normSquared() const { return x[0] * x[0] + x[1] * x[1]; }
    inline float norm() const { return sqrt(normSquared()); }
    inline Vec2f& normalize(float len= 1.0) {
      float l= norm();
      return (l != 0.0) ? (*this*= (len / l)) : (*this);
    }
    inline Vec2f normalized(float len= 1.0) {
      float l= norm();
      return (l != 0.0) ? (*this * len / l) : (*this);
    }
    inline Vec2f abs() {
      return Vec2f(std::abs(x[0]), std::abs(x[1]));
    }
    inline float sum() {
      return x[0] + x[1];
    }
    inline float max() const { return std::max(x[0], x[1]); }
    inline float min() const { return std::min(x[0], x[1]); }

    // Operators
    inline float operator[](int idx) const { return x[idx]; }
    inline float& operator[](int idx) { return x[idx]; }
    inline operator const float*(void) const { return x; }
    inline Vec2f& operator=(const Vec2f& v) {
      x[0]= v.x[0];
      x[1]= v.x[1];
      return *this;
    }
    inline bool operator==(const Vec2f& v) { return ((x[0] == v.x[0]) && (x[1] == v.x[1])); }
    inline Vec2f& operator+=(const Vec2f& v) {
      x[0]+= v.x[0];
      x[1]+= v.x[1];
      return *this;
    }
    inline Vec2f& operator-=(const Vec2f& v) {
      x[0]-= v.x[0];
      x[1]-= v.x[1];
      return *this;
    }
    inline Vec2f& operator*=(float f) {
      x[0]*= f;
      x[1]*= f;
      return *this;
    }
    inline Vec2f& operator/=(float f) {
      x[0]/= f;
      x[1]/= f;
      return *this;
    }
    friend Vec2f operator+(const Vec2f& w, const Vec2f& v) { return Vec2f(w)+= v; }
    friend Vec2f operator-(const Vec2f& w, const Vec2f& v) { return Vec2f(w)-= v; }
    friend Vec2f operator*(const Vec2f& v, float f) { return Vec2f(v)*= f; }
    friend Vec2f operator*(float f, const Vec2f& v) { return Vec2f(v)*= f; }
    friend Vec2f operator/(const Vec2f& v, float f) { return Vec2f(v)/= f; }
    friend Vec2f operator/(float f, const Vec2f& v) { return Vec2f(v)/= f; }

    inline float* array() { return x; }

private:
    float x[2];
  };

  class Vec2d
  {
public:
    // Constructors
    Vec2d(double a= 0.0, double b= 0.0) {
      this->x[0]= a;
      this->x[1]= b;
    }
    Vec2d(const Vec2d& v) { *this= v; }

    // Methods
    inline void set(double a, double b) {
      x[0]= a;
      x[1]= b;
    }
    inline double dot(const Vec2d& v) const { return x[0] * v.x[0] + x[1] * v.x[1]; }
    inline double normSquared() const { return x[0] * x[0] + x[1] * x[1]; }
    inline double norm() const { return sqrt(normSquared()); }
    inline Vec2d& normalize(double len= 1.0) {
      double l= norm();
      return (l != 0.0) ? (*this*= (len / l)) : (*this);
    }
    inline Vec2d normalized(double len= 1.0) {
      double l= norm();
      return (l != 0.0) ? (*this * len / l) : (*this);
    }
    inline Vec2d abs() {
      return Vec2d(std::abs(x[0]), std::abs(x[1]));
    }
    inline double sum() {
      return x[0] + x[1];
    }
    inline double max() const { return std::max(x[0], x[1]); }
    inline double min() const { return std::min(x[0], x[1]); }

    // Operators
    inline double operator[](int idx) const { return x[idx]; }
    inline double& operator[](int idx) { return x[idx]; }
    inline operator const double*(void) const { return x; }
    inline Vec2d& operator=(const Vec2d& v) {
      x[0]= v.x[0];
      x[1]= v.x[1];
      return *this;
    }
    inline bool operator==(const Vec2d& v) { return ((x[0] == v.x[0]) && (x[1] == v.x[1])); }
    inline Vec2d& operator+=(const Vec2d& v) {
      x[0]+= v.x[0];
      x[1]+= v.x[1];
      return *this;
    }
    inline Vec2d& operator-=(const Vec2d& v) {
      x[0]-= v.x[0];
      x[1]-= v.x[1];
      return *this;
    }
    inline Vec2d& operator*=(double f) {
      x[0]*= f;
      x[1]*= f;
      return *this;
    }
    inline Vec2d& operator/=(double f) {
      x[0]/= f;
      x[1]/= f;
      return *this;
    }
    friend Vec2d operator+(const Vec2d& w, const Vec2d& v) { return Vec2d(w)+= v; }
    friend Vec2d operator-(const Vec2d& w, const Vec2d& v) { return Vec2d(w)-= v; }
    friend Vec2d operator*(const Vec2d& v, double f) { return Vec2d(v)*= f; }
    friend Vec2d operator*(double f, const Vec2d& v) { return Vec2d(v)*= f; }
    friend Vec2d operator/(const Vec2d& v, double f) { return Vec2d(v)/= f; }
    friend Vec2d operator/(double f, const Vec2d& v) { return Vec2d(v)/= f; }

    inline double* array() { return x; }

private:
    double x[2];
  };

  class Vec3f
  {
public:
    // Constructors
    Vec3f(float a= 0.0, float b= 0.0, float c= 0.0) {
      this->x[0]= a;
      this->x[1]= b;
      this->x[2]= c;
    }
    Vec3f(const Vec3f& v) { *this= v; }

    // Methods
    inline void set(float a, float b, float c) {
      x[0]= a;
      x[1]= b;
      x[2]= c;
    }
    inline float dot(const Vec3f& v) const { return x[0] * v.x[0] + x[1] * v.x[1] + x[2] * v.x[2]; }
    inline Vec3f cross(const Vec3f& v) const {
      return Vec3f(
          x[1] * v.x[2] - x[2] * v.x[1],
          x[2] * v.x[0] - x[0] * v.x[2],
          x[0] * v.x[1] - x[1] * v.x[0]);
    }
    inline Vec3f coeffMul(const Vec3f& v) const { return Vec3f(x[0] * v.x[0], x[1] * v.x[1], x[2] * v.x[2]); }
    inline Vec3f coeffDiv(const Vec3f& v) const { return Vec3f(x[0] / v.x[0], x[1] / v.x[1], x[2] / v.x[2]); }
    inline float normSquared() const { return x[0] * x[0] + x[1] * x[1] + x[2] * x[2]; }
    inline float norm() const { return sqrt(normSquared()); }
    inline Vec3f& normalize(float len= 1.0) {
      float l= norm();
      return (l != 0.0) ? (*this*= (len / l)) : (*this);
    }
    inline Vec3f normalized(float len= 1.0) {
      float l= norm();
      return (l != 0.0) ? (*this * len / l) : (*this);
    }
    inline Vec3f abs() {
      return Vec3f(std::abs(x[0]), std::abs(x[1]), std::abs(x[2]));
    }
    inline float sum() {
      return x[0] + x[1] + x[2];
    }
    inline float max() const { return std::max(std::max(x[0], x[1]), x[2]); }
    inline float min() const { return std::min(std::min(x[0], x[1]), x[2]); }

    // Operators
    inline float operator[](int idx) const { return x[idx]; }
    inline float& operator[](int idx) { return x[idx]; }
    inline operator const float*(void) const { return x; }
    inline Vec3f& operator=(const Vec3f& v) {
      x[0]= v.x[0];
      x[1]= v.x[1];
      x[2]= v.x[2];
      return *this;
    }
    inline bool operator==(const Vec3f& v) { return ((x[0] == v.x[0]) && (x[1] == v.x[1]) && (x[2] == v.x[2])); }
    inline Vec3f& operator+=(const Vec3f& v) {
      x[0]+= v.x[0];
      x[1]+= v.x[1];
      x[2]+= v.x[2];
      return *this;
    }
    inline Vec3f& operator-=(const Vec3f& v) {
      x[0]-= v.x[0];
      x[1]-= v.x[1];
      x[2]-= v.x[2];
      return *this;
    }
    inline Vec3f& operator*=(float f) {
      x[0]*= f;
      x[1]*= f;
      x[2]*= f;
      return *this;
    }
    inline Vec3f& operator/=(float f) {
      x[0]/= f;
      x[1]/= f;
      x[2]/= f;
      return *this;
    }
    friend Vec3f operator+(const Vec3f& w, const Vec3f& v) { return Vec3f(w)+= v; }
    friend Vec3f operator-(const Vec3f& w, const Vec3f& v) { return Vec3f(w)-= v; }
    friend Vec3f operator*(const Vec3f& v, float f) { return Vec3f(v)*= f; }
    friend Vec3f operator*(float f, const Vec3f& v) { return Vec3f(v)*= f; }
    friend Vec3f operator/(const Vec3f& v, float f) { return Vec3f(v)/= f; }
    friend Vec3f operator/(float f, const Vec3f& v) { return Vec3f(v)/= f; }

    inline float* array() { return x; }

private:
    float x[3];
  };

  class Vec4f
  {
public:
    // Constructors
    Vec4f(float a= 0.0, float b= 0.0, float c= 0.0, float d= 0.0) {
      this->x[0]= a;
      this->x[1]= b;
      this->x[2]= c;
      this->x[3]= d;
    }
    Vec4f(const Vec4f& v) { *this= v; }

    // Methods
    inline void set(float a, float b, float c, float d) {
      x[0]= a;
      x[1]= b;
      x[2]= c;
      x[3]= d;
    }
    inline float dot(const Vec4f& v) const { return x[0] * v.x[0] + x[1] * v.x[1] + x[2] * v.x[2] + x[3] * v.x[3]; }
    inline Vec4f coeffMul(const Vec4f& v) const { return Vec4f(x[0] * v.x[0], x[1] * v.x[1], x[2] * v.x[2], x[3] * v.x[3]); }
    inline Vec4f coeffDiv(const Vec4f& v) const { return Vec4f(x[0] / v.x[0], x[1] / v.x[1], x[2] / v.x[2], x[3] / v.x[3]); }
    inline float normSquared() const { return x[0] * x[0] + x[1] * x[1] + x[2] * x[2] + x[3] * x[3]; }
    inline float norm() const { return sqrt(normSquared()); }
    inline Vec4f& normalize(float len= 1.0) {
      float l= norm();
      return (l != 0.0) ? (*this*= (len / l)) : (*this);
    }
    inline Vec4f normalized(float len= 1.0) {
      float l= norm();
      return (l != 0.0) ? (*this * len / l) : (*this);
    }
    inline Vec4f abs() {
      return Vec4f(std::abs(x[0]), std::abs(x[1]), std::abs(x[2]), std::abs(x[3]));
    }
    inline float sum() {
      return x[0] + x[1] + x[2] + x[3];
    }
    inline float max() const { return std::max(std::max(std::max(x[0], x[1]), x[2]), x[3]); }
    inline float min() const { return std::min(std::min(std::min(x[0], x[1]), x[2]), x[3]); }

    // Operators
    inline float operator[](int idx) const { return x[idx]; }
    inline float& operator[](int idx) { return x[idx]; }
    inline operator const float*(void) const { return x; }
    inline Vec4f& operator=(const Vec4f& v) {
      x[0]= v.x[0];
      x[1]= v.x[1];
      x[2]= v.x[2];
      x[3]= v.x[3];
      return *this;
    }
    inline bool operator==(const Vec4f& v) { return ((x[0] == v.x[0]) && (x[1] == v.x[1]) && (x[2] == v.x[2]) && (x[3] == v.x[3])); }
    inline Vec4f& operator+=(const Vec4f& v) {
      x[0]+= v.x[0];
      x[1]+= v.x[1];
      x[2]+= v.x[2];
      x[3]+= v.x[3];
      return *this;
    }
    inline Vec4f& operator-=(const Vec4f& v) {
      x[0]-= v.x[0];
      x[1]-= v.x[1];
      x[2]-= v.x[2];
      x[3]-= v.x[3];
      return *this;
    }
    inline Vec4f& operator*=(float f) {
      x[0]*= f;
      x[1]*= f;
      x[2]*= f;
      x[3]*= f;
      return *this;
    }
    inline Vec4f& operator/=(float f) {
      x[0]/= f;
      x[1]/= f;
      x[2]/= f;
      x[3]/= f;
      return *this;
    }
    friend Vec4f operator+(const Vec4f& w, const Vec4f& v) { return Vec4f(w)+= v; }
    friend Vec4f operator-(const Vec4f& w, const Vec4f& v) { return Vec4f(w)-= v; }
    friend Vec4f operator*(const Vec4f& v, float f) { return Vec4f(v)*= f; }
    friend Vec4f operator*(float f, const Vec4f& v) { return Vec4f(v)*= f; }
    friend Vec4f operator/(const Vec4f& v, float f) { return Vec4f(v)/= f; }
    friend Vec4f operator/(float f, const Vec4f& v) { return Vec4f(v)/= f; }

    inline float* array() { return x; }

private:
    float x[4];
  };

}  // namespace Math
