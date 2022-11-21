#include "flock.hpp"

// Standard lib
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

// Project lib
#include "draw.hpp"


Flock::Flock(int n, float size) {
  this->nb_boids = n;
  this->size = size;

  this->food = Vector(0.4, 0.0, 0.4);
  this->predator = Vector(-0.4, 0.0, -0.4);

  Vector p = Vector(0.0, 0.5, 0.0);
  Vector dp = Vector(0.5, 0.0, 0.5);
  Vector v = Vector(0.0, 0.0, 0.0);
  Vector dv = Vector(0.2, 0.2, 0.2);

  this->boids = new Boid[n];
  for (int i = 0; i < n; i++) {
    float px = (float)2 * dp.x * rand() / RAND_MAX - dp.x + p.x;
    float py = (float)2 * dp.y * rand() / RAND_MAX - dp.y + p.y;
    float pz = (float)2 * dp.z * rand() / RAND_MAX - dp.z + p.z;
    float vx = (float)2 * dv.x * rand() / RAND_MAX - dv.x + v.x;
    float vy = (float)2 * dv.y * rand() / RAND_MAX - dv.y + v.y;
    float vz = (float)2 * dv.z * rand() / RAND_MAX - dv.z + v.z;
    boids[i] = Boid(Vector(px, py, pz), Vector(vx, vy, vz), Vector(0, 1, 0), size);
  }
}


void Flock::draw() {
  for (int i = 0; i < nb_boids; i++) {
    boids[i].draw();
  }
  draw_point(food, 0.0, 1.0, 0.0);
  draw_point(predator, 1.0, 0.0, 0.0);

  draw_box(food - 0.05, food + 0.05, 0.0, 1.0, 0.0);
  draw_box(predator - 0.05, predator + 0.05, 1.0, 0.0, 0.0);
}


void Flock::animate(float t) {
  float a = 0.75, b = 0.015, c = 0.45, d = 0.04, e = 0.06;
  std::vector<Vector> velocityChange(this->nb_boids);

  // Compute the forces
#pragma omp parallel for
  for (int i = 0; i < nb_boids; i++) {
    int count = 0;
    Vector sep, ali, coh, aim, run;
    for (int j = 0; j < nb_boids; j++) {
      if (boids[i].p.distance(boids[j].p) > size) continue;
      if (i == j) continue;
      ali = ali + boids[j].v;
      coh = coh + boids[j].p;
      count++;
    }
    if (count > 0) {
      sep = boids[i].p - coh / count;
      ali = ali / count;
      coh = coh / count - boids[i].p;
    }
    aim = food - boids[i].p;
    if (boids[i].p.distance(predator) < 5.0 * size)
      run = boids[i].p - predator;

    velocityChange[i]= a * sep + b * ali + c * coh + d * aim + e * run;
  }

  // Apply the forces
  for (int i = 0; i < nb_boids; i++) {
    boids[i].v += velocityChange[i];
    float l = boids[i].v.length();
    if (l > 0.3) {
      boids[i].v = 0.3 * boids[i].v / l;
    }
    boids[i].p = boids[i].p + boids[i].v * t;
  }

}
