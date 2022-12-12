#include "ParticleSystem.hpp"


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


ParticleSystem::ParticleSystem() {
  isInitialized= false;
}


void ParticleSystem::Init() {
  isInitialized= true;

  NbParticles= int(std::round(D.param[PS_NbParticles______].val));

  PosOld= std::vector<Math::Vec3>(NbParticles, Math::Vec3(0.0, 0.0, 0.0));
  PosCur= std::vector<Math::Vec3>(NbParticles, Math::Vec3(0.0, 0.0, 0.0));
  PosCur= std::vector<Math::Vec3>(NbParticles, Math::Vec3(0.0, 0.0, 0.0));
  VelCur= std::vector<Math::Vec3>(NbParticles, Math::Vec3(0.0, 0.0, 0.0));
  AccCur= std::vector<Math::Vec3>(NbParticles, Math::Vec3(0.0, 0.0, 0.0));
  ForCur= std::vector<Math::Vec3>(NbParticles, Math::Vec3(0.0, 0.0, 0.0));
  ColCur= std::vector<Math::Vec3>(NbParticles, Math::Vec3(0.0, 0.0, 0.0));
  RadCur= std::vector<double>(NbParticles, 0.0);
  MasCur= std::vector<double>(NbParticles, 0.0);
  HotCur= std::vector<double>(NbParticles, 0.0);


  double minRadius= 1.0;
  if (int(std::round(D.param[PS_Contrain2D_______].val)) >= 1)
    minRadius= 0.4 / std::pow(double(NbParticles), 1.0 / 2.0);
  else
    minRadius= 0.4 / std::pow(double(NbParticles), 1.0 / 3.0);

  for (int k= 0; k < NbParticles; k++) {
    for (int dim= 0; dim < 3; dim++) {
      PosCur[k][dim]= (double(rand()) / double(RAND_MAX)) - 0.5;
      ColCur[k][dim]= (double(rand()) / double(RAND_MAX));
    }
    RadCur[k]= minRadius + minRadius * (double(rand()) / double(RAND_MAX));
    MasCur[k]= 1.0;
    HotCur[k]= (double(rand()) / double(RAND_MAX));
  }
  PosOld= PosCur;
}


void ParticleSystem::Draw() {
  if (!isInitialized) return;

  for (int k= 0; k < NbParticles; k++) {
    glPushMatrix();
    glTranslatef(PosCur[k][0], PosCur[k][1], PosCur[k][2]);
    glScalef(RadCur[k] * (HotCur[k] + 0.5), RadCur[k] * (HotCur[k] + 0.5), RadCur[k] * (HotCur[k] + 0.5));
    double r, g, b;
    // SrtColormap::RatioToJetSmooth(VelCur[k].norm(), r, g, b);
    SrtColormap::RatioToBlackBody(HotCur[k], r, g, b);
    glColor3f(r, g, b);
    glutSolidSphere(1.0, 32, 16);
    glPopMatrix();
  }
}


void ParticleSystem::Animate() {
  if (!isInitialized) return;

  double domainRad= 1.0;
  int nbSubstep= int(std::round(D.param[PS_NbSubStep________].val));
  double dt= D.param[PS_TimeStep_________].val / double(nbSubstep);

  Math::Vec3 gravity(0.0, 0.0, D.param[PS_ForceGravity_____].val);
  Math::Vec3 buoyancy(0.0, 0.0, D.param[PS_ForceBuoyancy____].val);

  double conductionFactor= D.param[PS_FactorConduction_].val;

  double heatAdd= D.param[PS_HeatInput________].val;
  double heatRem= D.param[PS_HeatOutput_______].val;


  for (int idxStep= 0; idxStep < nbSubstep; idxStep++) {
    // Project to 2D
    if (int(std::round(D.param[PS_Contrain2D_______].val)) >= 1) {
      for (int k0= 0; k0 < NbParticles; k0++) {
        PosCur[k0][0]= 0.0;
        VelCur[k0][0]= 0.0;
      }
    }

    // Add or remove heat to particles based on position in the domain
    for (int k0= 0; k0 < NbParticles; k0++) {
      if (PosCur[k0][2] < -0.9 * domainRad && PosCur[k0][1] > -0.8 * domainRad && PosCur[k0][1] < 0.8 * domainRad  && PosCur[k0][0] > -0.8 * domainRad && PosCur[k0][0] < 0.8 * domainRad)
        HotCur[k0]+= heatAdd * dt;
      else
        HotCur[k0]-= heatRem * dt;
      HotCur[k0]= std::min(std::max(HotCur[k0], 0.0), 1.0);
    }

    // Transfer heat between particles
    std::vector<double> HotOld= HotCur;
    for (int k0= 0; k0 < NbParticles; k0++) {
      for (int k1= k0 + 1; k1 < NbParticles; k1++) {
        if ((PosCur[k1] - PosCur[k0]).normSquared() <= (RadCur[k0] + RadCur[k1]) * (RadCur[k0] + RadCur[k1])) {
          double val= conductionFactor * (HotOld[k1] - HotOld[k0]) * dt;
          HotCur[k0]+= val;
          HotCur[k1]-= val;
        }
      }
      HotCur[k0]= std::min(std::max(HotCur[k0], 0.0), 1.0);
    }

    // Reset forces
    for (int k0= 0; k0 < NbParticles; k0++)
      ForCur[k0].set(0.0, 0.0, 0.0);

    // Add gravity forces
    for (int k0= 0; k0 < NbParticles; k0++)
      ForCur[k0]+= gravity * MasCur[k0];

    // Add boyancy forces
    for (int k0= 0; k0 < NbParticles; k0++)
      ForCur[k0]+= buoyancy * HotCur[k0];

    // Apply boundary constraint
    for (int k0= 0; k0 < NbParticles; k0++) {
      // Square domain
      for (int dim= 0; dim < 3; dim++)
        PosCur[k0][dim]= std::min(std::max(PosCur[k0][dim], -domainRad), domainRad);
      // // Circular domain
      // if (PosCur[k0].norm() + RadCur[k0] > domainRad)
      //   PosCur[k0]= PosCur[k0].normalized() * (domainRad - RadCur[k0]);
    }

    // Apply collision constraint
    for (int k0= 0; k0 < NbParticles; k0++) {
      for (int k1= k0 + 1; k1 < NbParticles; k1++) {
        if ((PosCur[k1] - PosCur[k0]).normSquared() <= (RadCur[k0] + RadCur[k1]) * (RadCur[k0] + RadCur[k1])) {
          Math::Vec3 val= (PosCur[k1] - PosCur[k0]).normalized() * 0.5 * ((RadCur[k0] + RadCur[k1]) - (PosCur[k1] - PosCur[k0]).norm());
          PosCur[k0]-= val;
          PosCur[k1]+= val;
        }
      }
    }

    // Deduce velocities
    for (int k0= 0; k0 < NbParticles; k0++)
      VelCur[k0]= (PosCur[k0] - PosOld[k0]) / dt;

    // Update positions
    PosOld= PosCur;
    for (int k0= 0; k0 < NbParticles; k0++) {
      AccCur[k0]= ForCur[k0] / MasCur[k0];
      PosCur[k0]= PosCur[k0] + VelCur[k0] * dt + AccCur[k0] * dt * dt;
    }
  }
}
