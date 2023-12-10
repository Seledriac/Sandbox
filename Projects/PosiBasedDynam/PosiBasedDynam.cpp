#include "PosiBasedDynam.hpp"


// Standard lib
#include <cmath>
#include <cstring>
#include <vector>

// GLUT lib
#include "../../Libs/freeglut/include/GL/freeglut.h"

// Project lib
#include "../../Data.hpp"
#include "../../Util/Colormap.hpp"
#include "../../Util/Random.hpp"
#include "../../Util/Vec.hpp"


// Link to shared sandbox data
extern Data D;

// List of UI parameters for this project
enum ParamType
{
  NumParticl__,
  RadParticl__,
  DomainX_____,
  DomainY_____,
  DomainZ_____,
  TimeStep____,
  VelDecay____,
  FactorCondu_,
  ForceGrav___,
  ForceBuoy___,
  HeatInput___,
  HeatOutput__,
};


// Constructor
PosiBasedDynam::PosiBasedDynam() {
  isActivProj= false;
  isAllocated= false;
  isRefreshed= false;
}


// Initialize Project UI parameters
void PosiBasedDynam::SetActiveProject() {
  if (!isActivProj) {
    D.UI.clear();
    D.UI.push_back(ParamUI("NumParticl__", 1000));
    D.UI.push_back(ParamUI("RadParticl__", 0.02));
    D.UI.push_back(ParamUI("DomainX_____", 0.5));
    D.UI.push_back(ParamUI("DomainY_____", 0.5));
    D.UI.push_back(ParamUI("DomainZ_____", 0.3));
    D.UI.push_back(ParamUI("TimeStep____", 0.02));
    D.UI.push_back(ParamUI("VelDecay____", 0.1));
    D.UI.push_back(ParamUI("FactorCondu_", 2.0));
    D.UI.push_back(ParamUI("ForceGrav___", -1.0));
    D.UI.push_back(ParamUI("ForceBuoy___", 2.0));
    D.UI.push_back(ParamUI("HeatInput___", 0.2));
    D.UI.push_back(ParamUI("HeatOutput__", 0.1));
  }

  if (D.UI.size() != HeatOutput__ + 1) {
    printf("[ERROR] Invalid parameter count in UI\n");
  }

  D.boxMin= {0.0, 0.0, 0.0};
  D.boxMax= {1.0, 1.0, 1.0};

  isActivProj= true;
  isAllocated= false;
  isRefreshed= false;
}


// Check if parameter changes should trigger an allocation
bool PosiBasedDynam::CheckAlloc() {
  if (D.UI[NumParticl__].hasChanged()) isAllocated= false;
  return isAllocated;
}


// Check if parameter changes should trigger a refresh
bool PosiBasedDynam::CheckRefresh() {
  if (D.UI[RadParticl__].hasChanged()) isRefreshed= false;
  if (D.UI[DomainX_____].hasChanged()) isRefreshed= false;
  if (D.UI[DomainY_____].hasChanged()) isRefreshed= false;
  if (D.UI[DomainZ_____].hasChanged()) isRefreshed= false;
  return isRefreshed;
}


// Allocate the project data
void PosiBasedDynam::Allocate() {
  if (!isActivProj) return;
  if (CheckAlloc()) return;
  isRefreshed= false;
  isAllocated= true;

  // Get UI parameters
  N= std::max(D.UI[NumParticl__].GetI(), 1);

  // Allocate data
  PosOld= std::vector<Vec::Vec3f>(N, Vec::Vec3f(0.0f, 0.0f, 0.0f));
  PosCur= std::vector<Vec::Vec3f>(N, Vec::Vec3f(0.0f, 0.0f, 0.0f));
  VelCur= std::vector<Vec::Vec3f>(N, Vec::Vec3f(0.0f, 0.0f, 0.0f));
  AccCur= std::vector<Vec::Vec3f>(N, Vec::Vec3f(0.0f, 0.0f, 0.0f));
  ForCur= std::vector<Vec::Vec3f>(N, Vec::Vec3f(0.0f, 0.0f, 0.0f));
  ColCur= std::vector<Vec::Vec3f>(N, Vec::Vec3f(0.0f, 0.0f, 0.0f));
  RadCur= std::vector<float>(N, 0.0f);
  MasCur= std::vector<float>(N, 0.0f);
  HotCur= std::vector<float>(N, 0.0f);
}


// Refresh the project
void PosiBasedDynam::Refresh() {
  if (!isActivProj) return;
  if (!CheckAlloc()) Allocate();
  if (CheckRefresh()) return;
  isRefreshed= true;

  // Get domain dimensions
  D.boxMin= {0.5f - D.UI[DomainX_____].GetF(), 0.5f - D.UI[DomainY_____].GetF(), 0.5f - D.UI[DomainZ_____].GetF()};
  D.boxMax= {0.5f + D.UI[DomainX_____].GetF(), 0.5f + D.UI[DomainY_____].GetF(), 0.5f + D.UI[DomainZ_____].GetF()};

  // Initialize with random particle properties
  for (int k= 0; k < N; k++) {
    for (int dim= 0; dim < 3; dim++) {
      PosCur[k][dim]= Random::Val((float)D.boxMin[dim], (float)D.boxMax[dim]);
      ColCur[k][dim]= Random::Val(0.0f, 1.0f);
    }
    RadCur[k]= D.UI[RadParticl__].GetF();
    MasCur[k]= 1.0f;
    HotCur[k]= Random::Val(0.0f, 1.0f);
  }
  PosOld= PosCur;
}


// Handle keypress
void PosiBasedDynam::KeyPress(const unsigned char key) {
  if (!isActivProj) return;
  if (!CheckAlloc()) Allocate();
  (void)key;  // Disable warning unused variable
}


// Animate the project
void PosiBasedDynam::Animate() {
  if (!isActivProj) return;
  if (!CheckAlloc()) Allocate();
  if (!CheckRefresh()) Refresh();

  // Get UI parameters
  const float dt= D.UI[TimeStep____].GetF();
  const float velocityDecay= 1.0f - D.UI[VelDecay____].GetF() * dt;
  const Vec::Vec3f vecGrav(0.0f, 0.0f, D.UI[ForceGrav___].GetF());
  const Vec::Vec3f vecBuoy(0.0f, 0.0f, D.UI[ForceBuoy___].GetF());
  const float conductionFactor= D.UI[FactorCondu_].GetF();
  const float heatAdd= D.UI[HeatInput___].GetF();
  const float heatRem= D.UI[HeatOutput__].GetF();

  // Add or remove heat to particles based on position in the domain
  for (int k0= 0; k0 < N; k0++) {
    const Vec::Vec3f posSource(0.5f * (D.boxMin[0] + D.boxMax[0]), 0.5f * (D.boxMin[1] + D.boxMax[1]), D.boxMin[2]);
    const float radSource= 0.1f * ((D.boxMax[0] - D.boxMin[0]) + (D.boxMax[1] - D.boxMin[1]) + (D.boxMax[2] - D.boxMin[2]));
    if ((posSource - PosCur[k0]).normSquared() < radSource)
      HotCur[k0]+= heatAdd * dt;
    HotCur[k0]-= heatRem * dt;
    HotCur[k0]= std::min(std::max(HotCur[k0], 0.0f), 1.0f);
  }

  // Transfer heat between particles (Gauss Seidel)
  std::vector<float> HotOld= HotCur;
  for (int k0= 0; k0 < N; k0++) {
    for (int k1= k0 + 1; k1 < N; k1++) {
      if ((PosCur[k1] - PosCur[k0]).normSquared() <= 1.1f * (RadCur[k0] + RadCur[k1]) * (RadCur[k0] + RadCur[k1])) {
        float val= conductionFactor * (HotOld[k1] - HotOld[k0]) * dt;
        HotCur[k0]+= val;
        HotCur[k1]-= val;
      }
    }
    HotCur[k0]= std::min(std::max(HotCur[k0], 0.0f), 1.0f);
  }

  // Reset forces
  for (int k0= 0; k0 < N; k0++)
    ForCur[k0].set(0.0f, 0.0f, 0.0f);

  // Add gravity forces
  for (int k0= 0; k0 < N; k0++)
    ForCur[k0]+= vecGrav * MasCur[k0];

  // Add boyancy forces
  for (int k0= 0; k0 < N; k0++)
    ForCur[k0]+= vecBuoy * HotCur[k0];

  // Apply boundary constraint
  for (int k0= 0; k0 < N; k0++)
    for (int dim= 0; dim < 3; dim++)
      PosCur[k0][dim]= std::min(std::max(PosCur[k0][dim], (float)D.boxMin[dim]), (float)D.boxMax[dim]);

  // Apply collision constraint (Gauss Seidel)
  for (int k0= 0; k0 < N; k0++) {
    for (int k1= k0 + 1; k1 < N; k1++) {
      if ((PosCur[k1] - PosCur[k0]).normSquared() <= (RadCur[k0] + RadCur[k1]) * (RadCur[k0] + RadCur[k1])) {
        Vec::Vec3f val= (PosCur[k1] - PosCur[k0]).normalized() * 0.5f * ((RadCur[k0] + RadCur[k1]) - (PosCur[k1] - PosCur[k0]).norm());
        PosCur[k0]-= val;
        PosCur[k1]+= val;
      }
    }
  }

  // Deduce velocities
  for (int k0= 0; k0 < N; k0++)
    VelCur[k0]= (PosCur[k0] - PosOld[k0]) / dt;

  // Apply explicit velocity damping
  for (int k0= 0; k0 < N; k0++)
    VelCur[k0]= VelCur[k0] * velocityDecay;

  // Update positions
  PosOld= PosCur;
  for (int k0= 0; k0 < N; k0++) {
    AccCur[k0]= ForCur[k0] / MasCur[k0];
    PosCur[k0]= PosCur[k0] + VelCur[k0] * dt + AccCur[k0] * dt * dt;
  }
}


// Draw the project
void PosiBasedDynam::Draw() {
  if (!isActivProj) return;
  if (!isAllocated) return;
  if (!isRefreshed) return;

  for (int k= 0; k < N; k++) {
    glPushMatrix();
    glTranslatef(PosCur[k][0], PosCur[k][1], PosCur[k][2]);
    glScalef(RadCur[k], RadCur[k], RadCur[k]);
    float r, g, b;
    if (D.displayMode1)
      Colormap::RatioToJetSmooth(VelCur[k].norm(), r, g, b);
    else if (D.displayMode2)
      Colormap::RatioToBlackBody(HotCur[k], r, g, b);
    else {
      r= VelCur[k][0];
      g= VelCur[k][1];
      b= VelCur[k][2];
    }
    glColor3f(r, g, b);
    glutSolidSphere(1.0, 32, 16);
    glPopMatrix();
  }
}
