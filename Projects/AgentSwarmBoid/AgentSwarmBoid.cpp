#include "AgentSwarmBoid.hpp"


// Standard lib
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
  Constrain2D_,
  PopSize_____,
  PopTypes____,
  TimeStep____,
  SizeView____,
  SizeBody____,
  CoeffSep____,
  CoeffAli____,
  CoeffCoh____,
  CoeffEat____,
  CoeffRun____,
  CoeffOri____,
};


// Constructor
AgentSwarmBoid::AgentSwarmBoid() {
  isActivProj= false;
  isAllocated= false;
  isRefreshed= false;
}


// Initialize Project UI parameters
void AgentSwarmBoid::SetActiveProject() {
  if (!isActivProj) {
    D.UI.clear();
    D.UI.push_back(ParamUI("Constrain2D_", 0));
    D.UI.push_back(ParamUI("PopSize_____", 300));
    D.UI.push_back(ParamUI("PopTypes____", 3));
    D.UI.push_back(ParamUI("TimeStep____", 0.05));
    D.UI.push_back(ParamUI("SizeView____", 0.15));
    D.UI.push_back(ParamUI("SizeBody____", 0.05));
    D.UI.push_back(ParamUI("CoeffSep____", 0.15));
    D.UI.push_back(ParamUI("CoeffAli____", 0.05));
    D.UI.push_back(ParamUI("CoeffCoh____", 0.10));
    D.UI.push_back(ParamUI("CoeffEat____", 0.10));
    D.UI.push_back(ParamUI("CoeffRun____", 0.15));
    D.UI.push_back(ParamUI("CoeffOri____", 0.02));
  }

  if (D.UI.size() != CoeffOri____ + 1) {
    printf("[ERROR] Invalid parameter count in UI\n");
  }

  D.boxMin= {0.0, 0.0, 0.0};
  D.boxMax= {1.0, 1.0, 1.0};

  isActivProj= true;
  isAllocated= false;
  isRefreshed= false;
}


// Check if parameter changes should trigger an allocation
void AgentSwarmBoid::CheckAlloc() {
  if (D.UI[PopSize_____].hasChanged()) isAllocated= false;
  if (D.UI[PopTypes____].hasChanged()) isAllocated= false;
}


// Check if parameter changes should trigger a refresh
void AgentSwarmBoid::CheckRefresh() {
}


// Allocate the project data
void AgentSwarmBoid::Allocate() {
  if (!isActivProj) return;
  CheckAlloc();
  if (isAllocated) return;
  isRefreshed= false;
  isAllocated= true;

  // Get UI parameters
  NbAgents= std::max(D.UI[PopSize_____].GetI(), 1);
  NbTypes= std::max(D.UI[PopTypes____].GetI(), 1);

  // Allocate data
  Pos= std::vector<Vec::Vec3f>(NbAgents);
  Vel= std::vector<Vec::Vec3f>(NbAgents);
  Typ= std::vector<int>(NbAgents);
}


// Refresh the project
void AgentSwarmBoid::Refresh() {
  if (!isActivProj) return;
  CheckAlloc();
  if (!isAllocated) Allocate();
  CheckRefresh();
  if (isRefreshed) return;
  isRefreshed= true;

  // Initialize population with random positions, velocities and types
  for (int k= 0; k < NbAgents; k++) {
    Pos[k].set(Random::Val(0.0f, 1.0f), Random::Val(0.0f, 1.0f), Random::Val(0.0f, 1.0f));
    Vel[k].set(Random::Val(-0.2f, 0.2f), Random::Val(-0.2f, 0.2f), Random::Val(-0.2f, 0.2f));
    Typ[k]= Random::Val(0, NbTypes - 1);
  }

  // Optionally constrain to 2D
  if (D.UI[Constrain2D_].GetB()) {
    for (int k0= 0; k0 < NbAgents; k0++) {
      Pos[k0][0]= 0.5;
      Vel[k0][0]= 0.0;
    }
  }
}


// Animate the project
void AgentSwarmBoid::Animate() {
  if (!isActivProj) return;
  CheckAlloc();
  if (!isAllocated) Allocate();
  CheckRefresh();
  if (!isRefreshed) Refresh();

  // Optionally constrain to 2D
  if (D.UI[Constrain2D_].GetB()) {
    for (int k0= 0; k0 < NbAgents; k0++) {
      Pos[k0][0]= 0.5;
      Vel[k0][0]= 0.0;
    }
  }

  // Compute the forces
  std::vector<Vec::Vec3f> velocityChange(NbAgents);
#pragma omp parallel for
  for (int k0= 0; k0 < NbAgents; k0++) {
    int countSep= 0;
    int countAli= 0;
    int countEat= 0;
    int countRun= 0;
    Vec::Vec3f sep, ali, coh, eat, run, ori;
    for (int k1= 0; k1 < NbAgents; k1++) {
      if ((Pos[k0] - Pos[k1]).normSquared() < D.UI[SizeView____].GetF() * D.UI[SizeView____].GetF()) {
        if (k0 != k1) {
          if ((Pos[k0] - Pos[k1]).normSquared() < D.UI[SizeBody____].GetF() * D.UI[SizeBody____].GetF()) {
            sep+= Pos[k0] - Pos[k1];
            countSep++;
          }
          if (Typ[k0] == Typ[k1]) {
            ali+= Vel[k1];
            coh= Pos[k1] - Pos[k0];
            countAli++;
          }
          if (Typ[k0] == (Typ[k1] + NbTypes - 1) % NbTypes) {
            eat= Pos[k1] - Pos[k0];
            countEat++;
          }
          if (Typ[k0] == (Typ[k1] + 1) % NbTypes) {
            run= Pos[k0] - Pos[k1];
            countRun++;
          }
        }
      }
    }
    if (countSep > 0) sep/= countSep;
    if (countAli > 0) ali/= countAli;
    if (countAli > 0) coh/= countAli;
    if (countEat > 0) eat/= countEat;
    if (countRun > 0) run/= countRun;

    Vec::Vec3f center(0.5f, 0.5f, 0.5f);
    ori= center - Pos[k0];

    velocityChange[k0].set(0.0f, 0.0f, 0.0f);
    velocityChange[k0]+= D.UI[CoeffSep____].GetF() * sep;
    velocityChange[k0]+= D.UI[CoeffAli____].GetF() * ali;
    velocityChange[k0]+= D.UI[CoeffCoh____].GetF() * coh;
    velocityChange[k0]+= D.UI[CoeffEat____].GetF() * eat;
    velocityChange[k0]+= D.UI[CoeffRun____].GetF() * run;
    velocityChange[k0]+= D.UI[CoeffOri____].GetF() * ori;
  }

  // Apply the forces
  for (int k= 0; k < NbAgents; k++) {
    Vel[k]+= velocityChange[k];
    float l= Vel[k].norm();
    if (l > 0.3f) {
      Vel[k]= 0.3f * Vel[k] / l;
    }
    Pos[k]= Pos[k] + Vel[k] * D.UI[TimeStep____].GetF();
  }
}


// Draw the project
void AgentSwarmBoid::Draw() {
  if (!isActivProj) return;
  if (!isAllocated) return;
  if (!isRefreshed) return;

  // Draw the agents
  if (D.displayMode1) {
    for (int k= 0; k < NbAgents; k++) {
      Vec::Vec3f front= Vel[k].normalized();
      Vec::Vec3f u(1.0f, 0.0f, 0.0f);
      if (u.dot(front) == 0.0f) u.set(0.0f, 1.0f, 0.0f);
      Vec::Vec3f left= front.cross(u).normalized();
      Vec::Vec3f up= front.cross(left).normalized();

      Vec::Vec3f p1= Pos[k] + 0.05f * 2.0f * (+0.5f * front + 0.00f * left + 0.0f * up);
      Vec::Vec3f p2= Pos[k] + 0.05f * 2.0f * (+0.0f * front - 0.07f * left + 0.1f * up);
      Vec::Vec3f p3= Pos[k] + 0.05f * 2.0f * (+0.0f * front + 0.07f * left + 0.1f * up);
      Vec::Vec3f p4= Pos[k] + 0.05f * 2.0f * (+0.0f * front + 0.00f * left - 0.1f * up);
      Vec::Vec3f p5= Pos[k] + 0.05f * 2.0f * (-0.4f * front + 0.00f * left + 0.0f * up);
      Vec::Vec3f p6= Pos[k] + 0.05f * 2.0f * (-0.6f * front + 0.00f * left - 0.2f * up);
      Vec::Vec3f p7= Pos[k] + 0.05f * 2.0f * (-0.6f * front + 0.00f * left + 0.2f * up);

      float r= 0.5f, g= 0.5f, b= 0.5f;
      if (NbTypes > 1)
        Colormap::RatioToRainbow((float)Typ[k] / (float)(NbTypes - 1), r, g, b);

      glBegin(GL_TRIANGLES);
      glColor3f(r, g, b);
      glVertex3fv(p1.array());
      glVertex3fv(p2.array());
      glVertex3fv(p3.array());
      glVertex3fv(p1.array());
      glVertex3fv(p4.array());
      glVertex3fv(p2.array());
      glVertex3fv(p1.array());
      glVertex3fv(p3.array());
      glVertex3fv(p4.array());
      glVertex3fv(p2.array());
      glVertex3fv(p4.array());
      glVertex3fv(p5.array());
      glVertex3fv(p3.array());
      glVertex3fv(p2.array());
      glVertex3fv(p5.array());
      glVertex3fv(p3.array());
      glVertex3fv(p5.array());
      glVertex3fv(p4.array());
      glVertex3fv(p5.array());
      glVertex3fv(p6.array());
      glVertex3fv(p7.array());
      glEnd();
    }
  }

  // Draw the agents velocity vectors
  if (D.displayMode2) {
    glBegin(GL_LINES);
    for (int k= 0; k < NbAgents; k++) {
      glColor3f(0.0f, 0.0f, 1.0f);
      glVertex3fv(Pos[k].array());
      glColor3f(1.0f, 0.0f, 0.0f);
      glVertex3fv((Pos[k] + Vel[k]).array());
    }
    glEnd();
  }
}
