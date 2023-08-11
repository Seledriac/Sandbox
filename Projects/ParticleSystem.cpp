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
#include "../Data.hpp"
#include "../Util/Colormap.hpp"
#include "../Util/Vector.hpp"


extern Data D;

enum ParamType
{
  PD_Constrain2D______,
  PD_NbParticles______,
  PD_TimeStep_________,
  PD_NbSubStep________,
  PD_VelocityDecay____,
  PD_FactorConduc_____,
  PD_ForceGravity_____,
  PD_ForceBuoyancy____,
  PD_HeatInput________,
  PD_HeatOutput_______,
};


ParticleSystem::ParticleSystem() {
  D.param.clear();
  D.plotData.clear();
  isActiveProject= false;
  isInitialized= false;
  isRefreshed= false;
}


void ParticleSystem::SetActiveProject() {
  if (!isActiveProject) {
    D.param.push_back(ParamUI("PD_Constrain2D______", 1));
    D.param.push_back(ParamUI("PD_NbParticles______", 1000));
    D.param.push_back(ParamUI("PD_TimeStep_________", 0.05));
    D.param.push_back(ParamUI("PD_NbSubStep________", 8));
    D.param.push_back(ParamUI("PD_VelocityDecay____", 0.1));
    D.param.push_back(ParamUI("PD_FactorConduc_____", 2.0));
    D.param.push_back(ParamUI("PD_ForceGravity_____", -1.0));
    D.param.push_back(ParamUI("PD_ForceBuoyancy____", 4.0));
    D.param.push_back(ParamUI("PD_HeatInput________", 4.0));
    D.param.push_back(ParamUI("PD_HeatOutput_______", 0.1));
  }

  isActiveProject= true;
  isInitialized= false;
  isRefreshed= false;
  Initialize();
}


void ParticleSystem::Initialize() {
  // Check if need to skip
  if (!isActiveProject) return;
  if (D.param[PD_NbParticles______].hasChanged()) isInitialized= false;
  if (isInitialized) return;
  isInitialized= true;

  // Get UI parameters
  NbParticles= std::max((int)std::round(D.param[PD_NbParticles______].Get()), 1);

  // Allocate data
  PosOld= std::vector<Vector::Vec3f>(NbParticles, Vector::Vec3f(0.0f, 0.0f, 0.0f));
  PosCur= std::vector<Vector::Vec3f>(NbParticles, Vector::Vec3f(0.0f, 0.0f, 0.0f));
  PosCur= std::vector<Vector::Vec3f>(NbParticles, Vector::Vec3f(0.0f, 0.0f, 0.0f));
  VelCur= std::vector<Vector::Vec3f>(NbParticles, Vector::Vec3f(0.0f, 0.0f, 0.0f));
  AccCur= std::vector<Vector::Vec3f>(NbParticles, Vector::Vec3f(0.0f, 0.0f, 0.0f));
  ForCur= std::vector<Vector::Vec3f>(NbParticles, Vector::Vec3f(0.0f, 0.0f, 0.0f));
  ColCur= std::vector<Vector::Vec3f>(NbParticles, Vector::Vec3f(0.0f, 0.0f, 0.0f));
  RadCur= std::vector<float>(NbParticles, 0.0f);
  MasCur= std::vector<float>(NbParticles, 0.0f);
  HotCur= std::vector<float>(NbParticles, 0.0f);

  // Force refresh
  isRefreshed= false;
  Refresh();
}


void ParticleSystem::Refresh() {
  if (!isActiveProject) return;
  if (!isInitialized) return;
  if (isRefreshed) return;
  isRefreshed= true;

  // Compute radius based on box size and 2D or 3D
  float BaseRadius= 0.6;
  if (int(std::round(D.param[PD_Constrain2D______].Get())) >= 1)
    BaseRadius/= std::pow(float(NbParticles), 1.0f / 2.0f);
  else
    BaseRadius/= std::pow(float(NbParticles), 1.0f / 3.0f);

  // Initialize with random particle properties
  for (int k= 0; k < NbParticles; k++) {
    for (int dim= 0; dim < 3; dim++) {
      if (int(std::round(D.param[PD_Constrain2D______].Get())) >= 1 && dim == 0) continue;
      PosCur[k][dim]= (float(rand()) / float(RAND_MAX)) - 0.5f;
      ColCur[k][dim]= (float(rand()) / float(RAND_MAX));
    }
    RadCur[k]= BaseRadius;
    MasCur[k]= 1.0f;
    HotCur[k]= (float(rand()) / float(RAND_MAX));
  }

  // Apply collision constraint (Gauss Seidel)
  for (int idxStep= 0; idxStep < int(std::round(D.param[PD_NbSubStep________].Get())); idxStep++) {
    for (int k0= 0; k0 < NbParticles; k0++) {
      for (int k1= k0 + 1; k1 < NbParticles; k1++) {
        if ((PosCur[k1] - PosCur[k0]).normSquared() <= (RadCur[k0] + RadCur[k1]) * (RadCur[k0] + RadCur[k1])) {
          Vector::Vec3f val= (PosCur[k1] - PosCur[k0]).normalized() * 0.5f * ((RadCur[k0] + RadCur[k1]) - (PosCur[k1] - PosCur[k0]).norm());
          PosCur[k0]-= val;
          PosCur[k1]+= val;
        }
      }
    }
  }

  PosOld= PosCur;
}


void ParticleSystem::Animate() {
  if (!isActiveProject) return;
  if (!isInitialized) return;
  if (!isRefreshed) return;

  float domainRad= 1.0f;
  int nbSubstep= int(std::round(D.param[PD_NbSubStep________].Get()));
  float dt= D.param[PD_TimeStep_________].Get() / float(nbSubstep);
  float velocityDecay= (1.0f - D.param[PD_VelocityDecay____].Get() * dt);

  Vector::Vec3f gravity(0.0f, 0.0f, D.param[PD_ForceGravity_____].Get());
  Vector::Vec3f buoyancy(0.0f, 0.0f, D.param[PD_ForceBuoyancy____].Get());

  float conductionFactor= D.param[PD_FactorConduc_____].Get();

  float heatAdd= D.param[PD_HeatInput________].Get();
  float heatRem= D.param[PD_HeatOutput_______].Get();

  for (int idxStep= 0; idxStep < nbSubstep; idxStep++) {
    // Project to 2D
    if (int(std::round(D.param[PD_Constrain2D______].Get())) >= 1) {
      for (int k0= 0; k0 < NbParticles; k0++) {
        PosCur[k0][0]= 0.0f;
        VelCur[k0][0]= 0.0f;
      }
    }

    // Add or remove heat to particles based on position in the domain
    for (int k0= 0; k0 < NbParticles; k0++) {
      if (PosCur[k0][2] < -0.9f * domainRad && PosCur[k0][1] > -0.8f * domainRad && PosCur[k0][1] < 0.8f * domainRad && PosCur[k0][0] > -0.8f * domainRad && PosCur[k0][0] < 0.8f * domainRad)
        HotCur[k0]+= heatAdd * dt;
      HotCur[k0]-= heatRem * dt;
      HotCur[k0]= std::min(std::max(HotCur[k0], 0.0f), 1.0f);
    }

    // Transfer heat between particles (Gauss Seidel)
    std::vector<float> HotOld= HotCur;
    for (int k0= 0; k0 < NbParticles; k0++) {
      for (int k1= k0 + 1; k1 < NbParticles; k1++) {
        if ((PosCur[k1] - PosCur[k0]).normSquared() <= 1.1f * (RadCur[k0] + RadCur[k1]) * (RadCur[k0] + RadCur[k1])) {
          float val= conductionFactor * (HotOld[k1] - HotOld[k0]) * dt;
          HotCur[k0]+= val;
          HotCur[k1]-= val;
        }
      }
      HotCur[k0]= std::min(std::max(HotCur[k0], 0.0f), 1.0f);
    }

    // // Update particle radii based on heat
    // for (int k0= 0; k0 < NbParticles; k0++)
    //   RadCur[k0]= 0.5 * BaseRadius + HotCur[k0] * BaseRadius;

    // Reset forces
    for (int k0= 0; k0 < NbParticles; k0++)
      ForCur[k0].set(0.0f, 0.0f, 0.0f);

    // Add gravity forces
    for (int k0= 0; k0 < NbParticles; k0++)
      ForCur[k0]+= gravity * MasCur[k0];

    // Add boyancy forces
    for (int k0= 0; k0 < NbParticles; k0++)
      ForCur[k0]+= buoyancy * HotCur[k0];

    // Apply boundary constraint
    for (int k0= 0; k0 < NbParticles; k0++) {
      // // Square domain
      // for (int dim= 0; dim < 3; dim++)
      //   PosCur[k0][dim]= std::min(std::max(PosCur[k0][dim], -domainRad), domainRad);
      // Circular domain
      if (PosCur[k0].norm() + RadCur[k0] > domainRad)
        PosCur[k0]= PosCur[k0].normalized() * (domainRad - RadCur[k0]);
    }

    // Apply collision constraint (Gauss Seidel)
    for (int k0= 0; k0 < NbParticles; k0++) {
      for (int k1= k0 + 1; k1 < NbParticles; k1++) {
        if ((PosCur[k1] - PosCur[k0]).normSquared() <= (RadCur[k0] + RadCur[k1]) * (RadCur[k0] + RadCur[k1])) {
          Vector::Vec3f val= (PosCur[k1] - PosCur[k0]).normalized() * 0.5f * ((RadCur[k0] + RadCur[k1]) - (PosCur[k1] - PosCur[k0]).norm());
          PosCur[k0]-= val;
          PosCur[k1]+= val;
        }
      }
    }

    // Deduce velocities
    for (int k0= 0; k0 < NbParticles; k0++)
      VelCur[k0]= (PosCur[k0] - PosOld[k0]) / dt;

    // Apply explicit velocity damping
    for (int k0= 0; k0 < NbParticles; k0++)
      VelCur[k0]= VelCur[k0] * velocityDecay;

    // Update positions
    PosOld= PosCur;
    for (int k0= 0; k0 < NbParticles; k0++) {
      AccCur[k0]= ForCur[k0] / MasCur[k0];
      PosCur[k0]= PosCur[k0] + VelCur[k0] * dt + AccCur[k0] * dt * dt;
    }
  }
}


void ParticleSystem::Draw() {
  if (!isActiveProject) return;
  if (!isInitialized) return;
  if (!isRefreshed) return;

  for (int k= 0; k < NbParticles; k++) {
    glPushMatrix();
    glTranslatef(PosCur[k][0], PosCur[k][1], PosCur[k][2]);
    glScalef(RadCur[k], RadCur[k], RadCur[k]);
    // glScalef(RadCur[k] * (HotCur[k] + 0.5), RadCur[k] * (HotCur[k] + 0.5), RadCur[k] * (HotCur[k] + 0.5));
    float r, g, b;
    // Colormap::RatioToJetSmooth(VelCur[k].norm(), r, g, b);
    Colormap::RatioToJetSmooth(HotCur[k], r, g, b);
    // Colormap::RatioToBlackBody(HotCur[k], r, g, b);
    glColor3f(r, g, b);
    glutSolidSphere(1.0, 32, 16);
    glPopMatrix();
  }
}
