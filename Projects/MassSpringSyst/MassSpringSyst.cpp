#include "MassSpringSyst.hpp"


// Standard lib
#include <cstdio>
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
  Scenario____,
  InputFile___,
  DomainX_____,
  DomainY_____,
  DomainZ_____,
  Distrib_____,
  NbNodes_____,
  LinkDist____,
  TimeStep____,
  IntegMode___,
  SolvMaxIter_,
  CoeffExt____,
  CoeffGravi__,
  CoeffSpring_,
  CoeffDamp___,
  ColorFactor_,
  ColorMode___,
  Verbose_____,
  VerboseSolv_,
};


// Constructor
MassSpringSyst::MassSpringSyst() {
  isActivProj= false;
  isAllocated= false;
  isRefreshed= false;
}


// Initialize Project UI parameters
void MassSpringSyst::SetActiveProject() {
  if (!isActivProj) {
    D.UI.clear();
    D.UI.push_back(ParamUI("Scenario____", 1));
    D.UI.push_back(ParamUI("InputFile___", 0));
    D.UI.push_back(ParamUI("DomainX_____", 0.5));
    D.UI.push_back(ParamUI("DomainY_____", 0.5));
    D.UI.push_back(ParamUI("DomainZ_____", 0.5));
    D.UI.push_back(ParamUI("Distrib_____", 0));
    D.UI.push_back(ParamUI("NbNodes_____", 500));
    D.UI.push_back(ParamUI("LinkDist____", 0.3));
    D.UI.push_back(ParamUI("TimeStep____", 0.02));
    D.UI.push_back(ParamUI("IntegMode___", 0));
    D.UI.push_back(ParamUI("SolvMaxIter_", 10));
    D.UI.push_back(ParamUI("CoeffExt____", 1.0));
    D.UI.push_back(ParamUI("CoeffGravi__", 0.1));
    D.UI.push_back(ParamUI("CoeffSpring_", 40.0));
    D.UI.push_back(ParamUI("CoeffDamp___", 0.2));
    D.UI.push_back(ParamUI("ColorFactor_", 1.0));
    D.UI.push_back(ParamUI("ColorMode___", 1.0));
    D.UI.push_back(ParamUI("Verbose_____", -0.5));
    D.UI.push_back(ParamUI("VerboseSolv_", -0.5));
  }

  if (D.UI.size() != VerboseSolv_ + 1) {
    printf("[ERROR] Invalid parameter count in UI\n");
  }

  D.boxMin= {0.0, 0.0, 0.0};
  D.boxMax= {1.0, 1.0, 1.0};

  isActivProj= true;
  isAllocated= false;
  isRefreshed= false;
}


// Check if parameter changes should trigger an allocation
bool MassSpringSyst::CheckAlloc() {
  if (D.UI[Scenario____].hasChanged()) isAllocated= false;
  if (D.UI[InputFile___].hasChanged()) isAllocated= false;
  if (D.UI[DomainX_____].hasChanged()) isAllocated= false;
  if (D.UI[DomainY_____].hasChanged()) isAllocated= false;
  if (D.UI[DomainZ_____].hasChanged()) isAllocated= false;
  if (D.UI[Distrib_____].hasChanged()) isAllocated= false;
  if (D.UI[NbNodes_____].hasChanged()) isAllocated= false;
  if (D.UI[LinkDist____].hasChanged()) isAllocated= false;
  return isAllocated;
}


// Check if parameter changes should trigger a refresh
bool MassSpringSyst::CheckRefresh() {
  return isRefreshed;
}


// Allocate the project data
void MassSpringSyst::Allocate() {
  if (!isActivProj) return;
  if (CheckAlloc()) return;
  isRefreshed= false;
  isAllocated= true;
  if (D.UI[Verbose_____].GetB()) printf("MassSpringSyst::Allocate()\n");

  // Get UI parameters
  N= std::max(D.UI[NbNodes_____].GetI(), 1);

  // Allocate
  Adj= std::vector<std::vector<int>>(N, std::vector<int>());
  Ref= std::vector<Vec::Vec3f>(N, Vec::Vec3f(0.0f, 0.0f, 0.0f));
  Pos= std::vector<Vec::Vec3f>(N, Vec::Vec3f(0.0f, 0.0f, 0.0f));
  Vel= std::vector<Vec::Vec3f>(N, Vec::Vec3f(0.0f, 0.0f, 0.0f));
  Acc= std::vector<Vec::Vec3f>(N, Vec::Vec3f(0.0f, 0.0f, 0.0f));
  For= std::vector<Vec::Vec3f>(N, Vec::Vec3f(0.0f, 0.0f, 0.0f));
  Ext= std::vector<Vec::Vec3f>(N, Vec::Vec3f(0.0f, 0.0f, 0.0f));
  Fix= std::vector<Vec::Vec3f>(N, Vec::Vec3f(0.0f, 0.0f, 0.0f));
  Mas= std::vector<float>(N, 0.0f);
}


// Refresh the project
void MassSpringSyst::Refresh() {
  if (!isActivProj) return;
  if (!CheckAlloc()) Allocate();
  if (CheckRefresh()) return;
  isRefreshed= true;
  if (D.UI[Verbose_____].GetB()) printf("MassSpringSyst::Refresh()\n");

  // Get domain dimensions
  D.boxMin= {0.5f - D.UI[DomainX_____].GetF(), 0.5f - D.UI[DomainY_____].GetF(), 0.5f - D.UI[DomainZ_____].GetF()};
  D.boxMax= {0.5f + D.UI[DomainX_____].GetF(), 0.5f + D.UI[DomainY_____].GetF(), 0.5f + D.UI[DomainZ_____].GetF()};

  // Initialize nodes properties
  for (int k0= 0; k0 < N; k0++) {
    Mas[k0]= 1.0f;
    for (int dim= 0; dim < 3; dim++)
      Pos[k0][dim]= Random::Val(D.boxMin[dim], D.boxMax[dim]);
  }
  Ref= Pos;

  // Initialize the links
  for (int k0= 0; k0 < N; k0++) {
    for (int k1= k0 + 1; k1 < N; k1++) {
      if ((Pos[k0] - Pos[k1]).normSquared() < std::pow(D.UI[LinkDist____].GetF(), 2.0f)) {
        Adj[k0].push_back(k1);
        Adj[k1].push_back(k0);
      }
    }
  }
}


// Handle keypress
void MassSpringSyst::KeyPress(const unsigned char key) {
  if (!isActivProj) return;
  if (!CheckAlloc()) Allocate();
  (void)key;  // Disable warning unused variable
  if (D.UI[Verbose_____].GetB()) printf("MassSpringSyst::KeyPress()\n");
}


// Animate the project
void MassSpringSyst::Animate() {
  if (!isActivProj) return;
  if (!CheckAlloc()) Allocate();
  if (!CheckRefresh()) Refresh();
  if (D.UI[Verbose_____].GetB()) printf("MassSpringSyst::Animate()\n");

  StepForwardInTime();
}


// Draw the project
void MassSpringSyst::Draw() {
  if (!isActivProj) return;
  if (!isAllocated) return;
  if (!isRefreshed) return;
  if (D.UI[Verbose_____].GetB()) printf("MassSpringSyst::Draw()\n");

  // Draw the nodes
  if (D.displayMode1) {
    const float rad= 0.1f * D.UI[LinkDist____].GetF();
    for (int k0= 0; k0 < N; k0++) {
      glPushMatrix();
      glTranslatef(Pos[k0][0], Pos[k0][1], Pos[k0][2]);
      glScalef(rad, rad, rad);
      float r, g, b;
      Colormap::RatioToJetSmooth(Vel[k0].norm(), r, g, b);
      glColor3f(r, g, b);
      glutSolidSphere(1.0, 32, 16);
      glPopMatrix();
    }
  }

  // Draw the springs
  if (D.displayMode2) {
    glBegin(GL_LINES);
    for (int k0= 0; k0 < N; k0++) {
      for (int k1 : Adj[k0]) {
        glColor3f(0.5f, 0.5f, 0.5f);
        glVertex3fv(Pos[k0].array());
        glVertex3fv(Pos[k1]);
      }
    }
    glEnd();
  }
}


void MassSpringSyst::ComputeForces() {
  // Reset forces
  std::fill(For.begin(), For.end(), Vec::Vec3f(0.0f, 0.0f, 0.0f));

  // Accumulate forces
  for (int k0= 0; k0 < N; k0++) {
    For[k0]+= D.UI[CoeffExt____].GetF() * Ext[k0];                        // External forces
    For[k0]+= D.UI[CoeffGravi__].GetF() * Vec::Vec3f(0.0f, 0.0f, -1.0f);  // Gravity forces
    for (int k1 : Adj[k0]) {                                              // Spring forces
      const float lenCur= (Pos[k1] - Pos[k0]).norm();
      const float lenRef= (Ref[k1] - Ref[k0]).norm();
      For[k0]-= D.UI[CoeffSpring_].GetF() * (lenRef - lenCur) * (Pos[k1] - Pos[k0]) / lenCur;
    }
  }
}


void MassSpringSyst::StepForwardInTime() {
  const float dt= D.UI[TimeStep____].GetF();
  const float damping= D.UI[CoeffDamp___].GetF();

  // Euler integration
  if (D.UI[IntegMode___].GetI() == 0) {
    ComputeForces();  // ft (xt)
    ApplyBCFor();
    for (int k0= 0; k0 < N; k0++) {
      Acc[k0]= For[k0] / Mas[k0];                              // at+1 = ft / m
      Vel[k0]= (1.0 - damping * dt) * Vel[k0] + Acc[k0] * dt;  // vt+1 = vt + at+1 * dt
      Pos[k0]= Pos[k0] + Vel[k0] * dt;                         // xt+1 = xt + vt+1 * dt
    }
    ApplyBCPos();
  }

  // Velocity Verlet integration
  if (D.UI[IntegMode___].GetI() == 1) {
    for (int k0= 0; k0 < N; k0++) {
      Pos[k0]= Pos[k0] + Vel[k0] * dt + 0.5 * Acc[k0] * dt * dt;  // xt+1 = xt + vt * dt + 0.5 * at * dt * dt
      ApplyBCPos();
    }
    ComputeForces();  // ft+1 (xt+1)
    ApplyBCFor();
    for (int k0= 0; k0 < N; k0++) {
      Vec::Vec3f oldAcc= Acc[k0];
      Acc[k0]= For[k0] / Mas[k0];                                               // at+1 = ft+1 / m
      Vel[k0]= (1.0 - damping * dt) * Vel[k0] + 0.5 * (oldAcc + Acc[k0]) * dt;  // vt+1 = vt + 0.5 * (at + at+1) * dt
    }
  }
}


void MassSpringSyst::ApplyBCPos() {
  for (int k0= 0; k0 < N; k0++) {
    for (int dim= 0; dim < 3; dim++) {
      if (Fix[k0][dim] > 0.0f)
        Pos[k0][dim]= Ref[k0][dim];
      else
        Pos[k0][dim]= std::min(std::max(Pos[k0][dim], (float)D.boxMin[dim]), (float)D.boxMax[dim]);
    }
  }
}


void MassSpringSyst::ApplyBCFor() {
  for (int k0= 0; k0 < N; k0++)
    for (int dim= 0; dim < 3; dim++)
      if (Fix[k0][dim] > 0.0f)
        For[k0][dim]= 0.0f;
}
