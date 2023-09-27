#include "ParticleSystem.hpp"


// Standard lib
#include <cmath>
#include <vector>

// GLUT lib
#include "../freeglut/include/GL/freeglut.h"

// Project lib
#include "../Data.hpp"
#include "../Util/Colormap.hpp"
#include "../Util/Random.hpp"
#include "../Util/Vector.hpp"


// Link to shared sandbox data
extern Data D;

// List of UI parameters for this project
enum ParamType
{
  Constrain2D_,
  NbParticles_,
  TimeStep____,
  NbSubStep___,
  VelDecay____,
  FactorCondu_,
  ForceGrav___,
  ForceBuoy___,
  HeatInput___,
  HeatOutput__,
};


// Constructor
ParticleSystem::ParticleSystem() {
  isActivProj= false;
  isAllocated= false;
  isRefreshed= false;
}


// Initialize Project UI parameters
void ParticleSystem::SetActiveProject() {
  if (!isActivProj) {
    D.param.clear();
    D.param.push_back(ParamUI("Constrain2D_", 1));
    D.param.push_back(ParamUI("NbParticles_", 1000));
    D.param.push_back(ParamUI("TimeStep____", 0.05));
    D.param.push_back(ParamUI("NbSubStep___", 8));
    D.param.push_back(ParamUI("VelDecay____", 0.1));
    D.param.push_back(ParamUI("FactorCondu_", 2.0));
    D.param.push_back(ParamUI("ForceGrav___", -1.0));
    D.param.push_back(ParamUI("ForceBuoy___", 4.0));
    D.param.push_back(ParamUI("HeatInput___", 4.0));
    D.param.push_back(ParamUI("HeatOutput__", 0.1));
  }

  D.boxMin= {0.0, 0.0, 0.0};
  D.boxMax= {1.0, 1.0, 1.0};

  isActivProj= true;
  isAllocated= false;
  isRefreshed= false;
  Allocate();
  Refresh();
}


// Check if parameter changes should trigger an allocation
void ParticleSystem::CheckAlloc() {
  if (D.param[NbParticles_].hasChanged()) isAllocated= false;
}


// Check if parameter changes should trigger a refresh
void ParticleSystem::CheckRefresh() {
}


// Allocate the project data
void ParticleSystem::Allocate() {
  if (!isActivProj) return;
  CheckAlloc();
  if (isAllocated) return;
  isRefreshed= false;
  isAllocated= true;

  // Get UI parameters
  NbParticles= std::max((int)std::round(D.param[NbParticles_].Get()), 1);

  // Allocate data
  PosOld= std::vector<Math::Vec3f>(NbParticles, Math::Vec3f(0.0f, 0.0f, 0.0f));
  PosCur= std::vector<Math::Vec3f>(NbParticles, Math::Vec3f(0.0f, 0.0f, 0.0f));
  PosCur= std::vector<Math::Vec3f>(NbParticles, Math::Vec3f(0.0f, 0.0f, 0.0f));
  VelCur= std::vector<Math::Vec3f>(NbParticles, Math::Vec3f(0.0f, 0.0f, 0.0f));
  AccCur= std::vector<Math::Vec3f>(NbParticles, Math::Vec3f(0.0f, 0.0f, 0.0f));
  ForCur= std::vector<Math::Vec3f>(NbParticles, Math::Vec3f(0.0f, 0.0f, 0.0f));
  ColCur= std::vector<Math::Vec3f>(NbParticles, Math::Vec3f(0.0f, 0.0f, 0.0f));
  RadCur= std::vector<float>(NbParticles, 0.0f);
  MasCur= std::vector<float>(NbParticles, 0.0f);
  HotCur= std::vector<float>(NbParticles, 0.0f);
}


// Refresh the project
void ParticleSystem::Refresh() {
  if (!isActivProj) return;
  CheckAlloc();
  if (!isAllocated) Allocate();
  CheckRefresh();
  if (isRefreshed) return;
  isRefreshed= true;

  // Compute radius based on box size and 2D or 3D
  float BaseRadius= 0.6;
  if (int(std::round(D.param[Constrain2D_].Get())) >= 1)
    BaseRadius/= std::pow(float(NbParticles), 1.0f / 2.0f);
  else
    BaseRadius/= std::pow(float(NbParticles), 1.0f / 3.0f);

  // Initialize with random particle properties
  for (int k= 0; k < NbParticles; k++) {
    for (int dim= 0; dim < 3; dim++) {
      if (int(std::round(D.param[Constrain2D_].Get())) >= 1 && dim == 0) continue;
      PosCur[k][dim]= Random::Val(-0.5f, 0.5f);
      ColCur[k][dim]= Random::Val(0.0f, 1.0f);
    }
    RadCur[k]= BaseRadius;
    MasCur[k]= 1.0f;
    HotCur[k]= Random::Val(0.0f, 1.0f);
  }

  // Apply collision constraint (Gauss Seidel)
  for (int idxStep= 0; idxStep < int(std::round(D.param[NbSubStep___].Get())); idxStep++) {
    for (int k0= 0; k0 < NbParticles; k0++) {
      for (int k1= k0 + 1; k1 < NbParticles; k1++) {
        if ((PosCur[k1] - PosCur[k0]).normSquared() <= (RadCur[k0] + RadCur[k1]) * (RadCur[k0] + RadCur[k1])) {
          Math::Vec3f val= (PosCur[k1] - PosCur[k0]).normalized() * 0.5f * ((RadCur[k0] + RadCur[k1]) - (PosCur[k1] - PosCur[k0]).norm());
          PosCur[k0]-= val;
          PosCur[k1]+= val;
        }
      }
    }
  }

  PosOld= PosCur;
}


// Animate the project
void ParticleSystem::Animate() {
  if (!isActivProj) return;
  CheckAlloc();
  if (!isAllocated) Allocate();
  CheckRefresh();
  if (!isRefreshed) Refresh();

  float domainRad= 1.0f;
  int nbSubstep= int(std::round(D.param[NbSubStep___].Get()));
  float dt= D.param[TimeStep____].Get() / float(nbSubstep);
  float velocityDecay= (1.0f - D.param[VelDecay____].Get() * dt);

  Math::Vec3f gravity(0.0f, 0.0f, D.param[ForceGrav___].Get());
  Math::Vec3f buoyancy(0.0f, 0.0f, D.param[ForceBuoy___].Get());

  float conductionFactor= D.param[FactorCondu_].Get();

  float heatAdd= D.param[HeatInput___].Get();
  float heatRem= D.param[HeatOutput__].Get();

  for (int idxStep= 0; idxStep < nbSubstep; idxStep++) {
    // Project to 2D
    if (int(std::round(D.param[Constrain2D_].Get())) >= 1) {
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
      // Square domain
      for (int dim= 0; dim < 3; dim++)
        PosCur[k0][dim]= std::min(std::max(PosCur[k0][dim], -domainRad), domainRad);
      // // Circular domain
      // if (PosCur[k0].norm() + RadCur[k0] > domainRad)
      //   PosCur[k0]= PosCur[k0].normalized() * (domainRad - RadCur[k0]);
    }

    // Apply collision constraint (Gauss Seidel)
    for (int k0= 0; k0 < NbParticles; k0++) {
      for (int k1= k0 + 1; k1 < NbParticles; k1++) {
        if ((PosCur[k1] - PosCur[k0]).normSquared() <= (RadCur[k0] + RadCur[k1]) * (RadCur[k0] + RadCur[k1])) {
          Math::Vec3f val= (PosCur[k1] - PosCur[k0]).normalized() * 0.5f * ((RadCur[k0] + RadCur[k1]) - (PosCur[k1] - PosCur[k0]).norm());
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


// Draw the project
void ParticleSystem::Draw() {
  if (!isActivProj) return;
  if (!isAllocated) return;
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
