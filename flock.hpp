#pragma once


// Standard lib
#include <vector>

// Project lib
#include "math/Vectors.hpp"


class Boid
{
  public:
  Math::Vec3 p, v, n;
  float size;
};


class Flock
{
  public:
  Math::Vec3 posFood;
  Math::Vec3 posPredator;

  private:
  int nb_boids;
  float size;

  std::vector<Boid> boids;

  public:
  Flock(int n, float size);

  void draw();
  void animate(float t);
};
