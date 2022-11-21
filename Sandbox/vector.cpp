#include "vector.hpp"

// Standard lib
#include <math.h>
#include <stdio.h>


float Vector::operator % ( const Vector& v ) const {
  return x*v.x + y*v.y + z*v.z;
}

Vector Vector::operator ^ ( const Vector& v ) const {
  return Vector( y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x );
}

void Vector::normalize() {
  float  len = (float)sqrt( x*x + y*y + z*z );

  if( len == 0 )
  {
    x=0;
    y=0;
    z=0;
  }
  else
  {
    x/=len;
    y/=len;
    z/=len;
  }
}

Vector Vector::operator + ( const Vector& v ) const {
  return Vector( x+v.x, y+v.y, z+v.z );
}

Vector Vector::operator + ( float d ) const {
  return Vector( x+d, y+d, z+d );
}

Vector operator + ( const Vector& v1, const Vector& v2 ) {
  return Vector( v1.x+v2.x, v1.y+v2.y, v1.z+v2.z );
}

Vector Vector::operator - ( const Vector& v ) const {
  return Vector( x-v.x, y-v.y, z-v.z );
}

Vector Vector::operator - ( float d ) const {
  return Vector( x-d, y-d, z-d );
}

Vector operator - ( const Vector& v1, const Vector& v2 ) {
  return Vector( v1.x-v2.x, v1.y-v2.y, v1.z-v2.z );
}

Vector Vector::operator - () const {
  return Vector( -x, -y, -z );
}

Vector Vector::operator * ( const Vector& v ) const {
  return Vector( x*v.x, y*v.y, z*v.z );
}

Vector operator * ( const Vector& v1, const Vector& v2 ) {
  return Vector( v1.x*v2.x, v1.y*v2.y, v1.z*v2.z );
}

Vector Vector::operator * ( float d ) const {
  return Vector( x*d, y*d, z*d );
}

Vector Vector::operator / ( float d ) const {
  return Vector( x/d, y/d, z/d );
}

Vector Vector::operator / ( const Vector& v ) const {
  return Vector( x/v.x, y/v.y, z/v.z );
}

Vector operator / ( const Vector& v1, const Vector& v2 ) {
  return Vector( v1.x/v2.x, v1.y/v2.y, v1.z/v2.z );
}

int Vector::operator == ( const Vector& v ) {
  return( (x == v.x) && (y == v.y) && (z == v.z) );
}

int Vector::operator != ( const Vector& v ) {
  return( (x != v.x) || (y != v.y) || (z != v.z) );
}

int Vector::operator > ( const Vector& v ) {
  return( (x > v.x) || (y > v.y) || (z > v.z) );
}

int Vector::operator < ( const Vector& v ) {
  return( (x < v.x) || (y < v.y) || (z < v.z) );
}

Vector& Vector::operator = ( const Vector& v ) {
  x = v.x;
  y = v.y;
  z = v.z;
  return *this;
}

Vector& Vector::operator += ( const Vector& v ) {
  x += v.x;
  y += v.y;
  z += v.z;
  return *this;
}

Vector& Vector::operator -= ( const Vector& v ) {
  x -= v.x;
  y -= v.y;
  z -= v.z;
  return *this;
}

Vector& Vector::operator += ( float d ) {
  x += d;
  y += d;
  z += d;
  return *this;
}

Vector& Vector::operator -= ( float d ) {
  x -= d;
  y -= d;
  z -= d;
  return *this;
}

Vector& Vector::operator *= ( float d ) {
  x *= d;
  y *= d;
  z *= d;
  return *this;
}

Vector& Vector::operator /= ( float d ) {
  x /= d;
  y /= d;
  z /= d;
  return *this;
}

float Vector::length() const {
  return (float)sqrt( x*x + y*y + z*z );
}

float Vector::distance( const Vector& v ) const {
  return (float)sqrt( (x-v.x)*(x-v.x) + (y-v.y)*(y-v.y) + (z-v.z)*(z-v.z) );
}

float Vector::distance( float _x, float _y, float _z ) const {
  return (float)sqrt( (x-_x)*(x-_x) + (y-_y)*(y-_y) + (z-_z)*(z-_z) );
}

float Vector::distance_squared( const Vector& v ) const {
  return (x-v.x)*(x-v.x) + (y-v.y)*(y-v.y) + (z-v.z)*(z-v.z);
}

float Vector::distance_squared( float _x, float _y, float _z ) const {
  return (x-_x)*(x-_x) + (y-_y)*(y-_y) + (z-_z)*(z-_z);
}
