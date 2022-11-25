#include "flock.hpp"


// Standard lib
#include <cmath>
#include <cstdio>
#include <ctime>
#include <vector>

// GLUT lib
#include <GL/freeglut.h>

// Project lib
#include "Data.hpp"
#include "math/Vec.hpp"

extern Data D;

Flock::Flock(int n, float size) {
  this->nb_boids= n;
  this->size= size;

  this->posFood= math::Vec3(0.5, 0.5, 0.5);
  this->posPredator= math::Vec3(0.5, 0.5, 0.3);

  math::Vec3 p= math::Vec3(0.0, 0.5, 0.0);
  math::Vec3 dp= math::Vec3(0.5, 0.0, 0.5);
  math::Vec3 v= math::Vec3(0.0, 0.0, 0.0);
  math::Vec3 dv= math::Vec3(0.2, 0.2, 0.2);

  this->boids= std::vector<Boid>(n);
  for (int i= 0; i < n; i++) {
    boids[i].p[0]= 2.0f * dp[0] * float(rand()) / float(RAND_MAX) - dp[0] + p[0];
    boids[i].p[1]= 2.0f * dp[1] * float(rand()) / float(RAND_MAX) - dp[1] + p[1];
    boids[i].p[2]= 2.0f * dp[2] * float(rand()) / float(RAND_MAX) - dp[2] + p[2];
    boids[i].v[0]= 2.0f * dv[0] * float(rand()) / float(RAND_MAX) - dv[0] + v[0];
    boids[i].v[1]= 2.0f * dv[1] * float(rand()) / float(RAND_MAX) - dv[1] + v[1];
    boids[i].v[2]= 2.0f * dv[2] * float(rand()) / float(RAND_MAX) - dv[2] + v[2];
    boids[i].n[0]= 0.0f;
    boids[i].n[1]= 1.0f;
    boids[i].n[2]= 0.0f;
    boids[i].size= size;
  }
}


void Flock::draw() {
  glBegin(GL_LINES);
  for (int i= 0; i < nb_boids; i++) {
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(boids[i].p[0], boids[i].p[1], boids[i].p[2]);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(boids[i].p[0] + boids[i].v[0], boids[i].p[1] + boids[i].v[1], boids[i].p[2] + boids[i].v[2]);
  }
  glEnd();

  glPointSize(10.0f);
  glBegin(GL_POINTS);
  glColor3f(0.0f, 0.8f, 0.0f);
  glVertex3f(posFood[0], posFood[1], posFood[2]);
  glColor3f(1.0f, 0.0f, 0.0f);
  glVertex3f(posPredator[0], posPredator[1], posPredator[2]);
  glEnd();
  glPointSize(1);
}


void Flock::animate(float t) {
  float a= 0.75, b= 0.015, c= 0.45, d= 0.04, e= 0.06;
  std::vector<math::Vec3> velocityChange(this->nb_boids);

  // Compute the forces
  // #pragma omp parallel for
  for (int i= 0; i < nb_boids; i++) {
    int count= 0;
    math::Vec3 sep, ali, coh, aim, run;
    for (int j= 0; j < nb_boids; j++) {
      if ((boids[i].p - boids[j].p).length2() > size * size) continue;
      if (i == j) continue;
      ali= ali + boids[j].v;
      coh= coh + boids[j].p;
      count++;
    }
    if (count > 0) {
      sep= boids[i].p - coh / count;
      ali= ali / count;
      coh= coh / count - boids[i].p;
    }
    aim= posFood - boids[i].p;
    if ((boids[i].p - posPredator).length2() < (5.0 * size) * (5.0 * size))
      run= boids[i].p - posPredator;

    velocityChange[i]= a * sep + b * ali + c * coh + d * aim + e * run;
  }

  // Apply the forces
  for (int i= 0; i < nb_boids; i++) {
    boids[i].v+= velocityChange[i];
    float l= boids[i].v.length();
    if (l > 0.3) {
      boids[i].v= 0.3 * boids[i].v / l;
    }
    boids[i].p= boids[i].p + boids[i].v * t;
  }
}
