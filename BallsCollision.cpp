#include "BallsCollision.hpp"


// Standard lib
#include <array>
#include <cmath>
#include <cstdio>
#include <ctime>
#include <vector>

// GLUT lib
#include <GL/freeglut.h>

// Project lib
#include "Data.hpp"
#include "SrtColormap.hpp"
#include "math/Vectors.hpp"


extern Data D;


BallsCollision::BallsCollision() {
  isInitialized= false;
}


void BallsCollision::Init() {
  isInitialized= true;

  nbBalls= int(std::round(D.param[nbParticles_________].val));
  Pos= std::vector<Math::Vec3>(nbBalls);
  Vel= std::vector<Math::Vec3>(nbBalls);
  Acc= std::vector<Math::Vec3>(nbBalls);
  For= std::vector<Math::Vec3>(nbBalls);
  Col= std::vector<Math::Vec3>(nbBalls);
  Rad= std::vector<double>(nbBalls);
  Hot= std::vector<double>(nbBalls);

  for (int k= 0; k < nbBalls; k++) {
    for (int dim= 0; dim < 3; dim++) {
      Pos[k][dim]= (double(rand()) / double(RAND_MAX)) - 0.5;
      Vel[k][dim]= (double(rand()) / double(RAND_MAX)) - 0.5;
      Acc[k][dim]= 0.0;
      For[k][dim]= 0.0;
      Col[k][dim]= (double(rand()) / double(RAND_MAX));
    }
    Rad[k]= 0.05 + 0.1 * (double(rand()) / double(RAND_MAX));
    Hot[k]= (double(rand()) / double(RAND_MAX));
  }
}


void BallsCollision::Draw() {
  if (!isInitialized) return;

  for (int k= 0; k < nbBalls; k++) {
    glPushMatrix();
    glTranslatef(Pos[k][0], Pos[k][1], Pos[k][2]);
    glScalef(Rad[k], Rad[k], Rad[k]);
    double r, g, b;
    SrtColormap::RatioToBlackBody(Hot[k], r, g, b);
    glColor3f(r, g, b);
    glutSolidSphere(1.0, 32, 16);
    // glutSolidIcosahedron();
    glPopMatrix();
  }
}


void BallsCollision::Animate() {
  if (!isInitialized) return;

  double domainRad= 1.0;
  Math::Vec3 gravity(0.0, 0.0, -0.1 * D.param[forceGravity________].val);
  double dt= 0.1 * D.param[timeStep____________].val;
  double boundaryForce= D.param[forceBoundary_______].val;
  double collisionForce= D.param[forceCollision______].val;
  double dampingForce= D.param[ratioDamping________].val;
  double conductionRatio= D.param[ratioConduction_____].val;

  // Project to 2D
  for (int k0= 0; k0 < nbBalls; k0++) {
    Pos[k0][0]= 0.0;
    Vel[k0][0]= 0.0;
  }

  // Calculate heat transfer
  std::vector<double> HotOld= Hot;
  for (int k0= 0; k0 < nbBalls; k0++) {
    for (int k1= 0; k1 < nbBalls; k1++) {
      if (k0 == k1) continue;
      if ((Pos[k0] - Pos[k1]).norm2() > Rad[k0] + Rad[k1]) continue;
      Hot[k0]+= conductionRatio * (HotOld[k1]-HotOld[k0]);
    }
  }

  // Calculate forces
  for (int k0= 0; k0 < nbBalls; k0++) {
    double mass= (4.0 / 3.0) * M_PI * Rad[k0] * Rad[k0] * Rad[k0];  // m= (4/3) * PI * r^3
    For[k0]= gravity * mass;                                        // fg= g * m

    double distExcess= Pos[k0].norm2() + Rad[k0] - domainRad;
    if (distExcess > 0.0)
      For[k0]-= Pos[k0].normalized() * boundaryForce * distExcess * distExcess;

    for (int k1= 0; k1 < nbBalls; k1++) {
      if (k0 == k1) continue;
      Math::Vec3 collAxis= Pos[k1] - Pos[k0];
      double dist= collAxis.norm2();
      double overlapSize= (Rad[k0] + Rad[k1]) - dist;
      if (overlapSize > 0.0) {
        For[k0]-= collAxis * collisionForce * overlapSize * overlapSize;
      }
    }
  }

  // Euler integration
  for (int k0= 0; k0 < nbBalls; k0++) {
    double mass= (4.0 / 3.0) * M_PI * Rad[k0] * Rad[k0] * Rad[k0];  // m= (4/3) * PI * r^3
    Acc[k0]= For[k0] / mass;                                        // at+1 = ft+1 / m
    Vel[k0]= (1.0 - dampingForce) * Vel[k0] + Acc[k0] * dt;         // vt+1 = vt + at+1 * dt
    Pos[k0]= Pos[k0] + Vel[k0] * dt;                                // xt+1 = xt + vt * dt
  }
}
