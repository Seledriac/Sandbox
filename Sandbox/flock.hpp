#pragma once

// Project lib
#include "boid.hpp"


class Flock {

public:
  Vector food, predator;
private:
  int nb_boids;
  float size;
  
  Boid *boids;

public:
  Flock(int n, float size);
  
  void draw();
  void animate(float t);
};
