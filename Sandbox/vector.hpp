#pragma once

class Vector
{
public:
  Vector():
       x(0), y(0), z(0) {}
  Vector (float x1, float y1, float z1):
       x(x1), y(y1), z(z1) {}
  Vector ( const Vector &v ):
       x(v.x),
       y(v.y),
       z(v.z) {}

  float distance( const Vector& v ) const;
  float distance( float _x, float _y, float _z ) const;

  float distance_squared( const Vector& v ) const;
  float distance_squared( float _x, float _y, float _z ) const;

  float  operator %  ( const Vector& v ) const;    // produit scalaire
  Vector operator ^  ( const Vector& v ) const;    // produit vectoriel
  void normalize();

  Vector operator + ( const Vector& v ) const;
  Vector operator + ( float d ) const;
  Vector operator - ( const Vector& v ) const;
  Vector operator - ( float d ) const;
  Vector operator - () const;

  Vector operator *  ( const Vector& v ) const;
  Vector operator *  ( float d ) const;
  Vector operator /  ( const Vector& v ) const;
  Vector operator /  ( float d ) const;

  int    operator >  ( const Vector& v );
  int    operator <  ( const Vector& v );
  int    operator == ( const Vector& v );
  int    operator != ( const Vector& v );

  Vector& operator =  ( const Vector& v );
  Vector& operator += ( const Vector& v );
  Vector& operator -= ( const Vector& v );
  Vector& operator += ( float d );
  Vector& operator -= ( float d );
  Vector& operator *= ( float d );
  Vector& operator /= ( float d );
  
  friend Vector operator + ( float d, const Vector& v ) {
    return Vector( v.x+d, v.y+d, v.z+d );
  }

  friend Vector operator * ( float d, const Vector& v ) {
    return Vector( v.x*d, v.y*d, v.z*d );
  }

  float length() const;

  float x, y, z;
};
