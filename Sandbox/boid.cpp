#include "boid.hpp"

// Standard lib
#include <math.h>

// Project lib
#include "draw.hpp"


Boid::Boid() {}


Boid::Boid(Vector p, Vector v, Vector n, float size= 0.05) {
  this->p= p;
  this->v= v;
  this->n= n;
  this->size= size;
}


void Boid::draw() {
  Vector front, left, up, u(1, 0, 0);
  front= v;
  front.normalize();

  if (u % front < 100) u= Vector(0, 1, 0);
  left = front ^ u;
  up = front ^ left;
  
  left.normalize();
  up.normalize();
  
  draw_boid(front, left, up, p, size);
  //draw_line(p, p+v*size*2.0);
}
