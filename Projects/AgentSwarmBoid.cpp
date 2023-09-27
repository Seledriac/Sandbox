#include "AgentSwarmBoid.hpp"


// Standard lib
#include <vector>

// GLUT lib
#include "../freeglut/include/GL/freeglut.h"

// Project lib
#include "../Data.hpp"
#include "../Util/Random.hpp"
#include "../Util/Vector.hpp"


// Link to shared sandbox data
extern Data D;

// List of UI parameters for this project
enum ParamType
{
  NbAgents____,
  SizeAgent___,
  TimeStep____,
  CoeffSep____,
  CoeffAli____,
  CoeffCohes__,
  CoeffHunger_,
  CoeffFear___,
  PosFoodX____,
  PosFoodY____,
  PosFoodZ____,
  PosPredX____,
  PosPredY____,
  PosPredZ____,
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
    D.param.clear();
    D.param.push_back(ParamUI("NbAgents____", 200));
    D.param.push_back(ParamUI("SizeAgent___", 0.05));
    D.param.push_back(ParamUI("TimeStep____", 0.05));
    D.param.push_back(ParamUI("CoeffSep____", 0.75));
    D.param.push_back(ParamUI("CoeffAli____", 0.015));
    D.param.push_back(ParamUI("CoeffCohes__", 0.45));
    D.param.push_back(ParamUI("CoeffHunger_", 0.04));
    D.param.push_back(ParamUI("CoeffFear___", 0.1));
    D.param.push_back(ParamUI("PosFoodX____", 0.5));
    D.param.push_back(ParamUI("PosFoodY____", 0.5));
    D.param.push_back(ParamUI("PosFoodZ____", 0.6));
    D.param.push_back(ParamUI("PosPredX____", 0.5));
    D.param.push_back(ParamUI("PosPredY____", 0.5));
    D.param.push_back(ParamUI("PosPredZ____", 0.4));
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
void AgentSwarmBoid::CheckAlloc() {
  if (D.param[NbAgents____].hasChanged()) isAllocated= false;
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
  NbAgents= std::max((int)std::round(D.param[NbAgents____].Get()), 1);

  // Allocate data
  Pos= std::vector<Math::Vec3f>(NbAgents);
  Vel= std::vector<Math::Vec3f>(NbAgents);
  Nor= std::vector<Math::Vec3f>(NbAgents);
}


// Refresh the project
void AgentSwarmBoid::Refresh() {
  if (!isActivProj) return;
  CheckAlloc();
  if (!isAllocated) Allocate();
  CheckRefresh();
  if (isRefreshed) return;
  isRefreshed= true;

  for (int k= 0; k < NbAgents; k++) {
    Pos[k].set(Random::Val(0.0f, 1.0f), Random::Val(0.0f, 1.0f), Random::Val(0.0f, 1.0f));
    Vel[k].set(Random::Val(-0.2f, 0.2f), Random::Val(-0.2f, 0.2f), Random::Val(-0.2f, 0.2f));
    Nor[k].set(0.0f, 1.0f, 0.0f);
  }
  PosFood.set((float)D.param[PosFoodX____].Get(), (float)D.param[PosFoodY____].Get(), (float)D.param[PosFoodZ____].Get());
  PosPred.set((float)D.param[PosPredX____].Get(), (float)D.param[PosPredY____].Get(), (float)D.param[PosPredZ____].Get());
}


// Animate the project
void AgentSwarmBoid::Animate() {
  if (!isActivProj) return;
  CheckAlloc();
  if (!isAllocated) Allocate();
  CheckRefresh();
  if (!isRefreshed) Refresh();

  float sizeAgent= (float)D.param[SizeAgent___].Get();
  float a= (float)D.param[CoeffSep____].Get();
  float b= (float)D.param[CoeffAli____].Get();
  float c= (float)D.param[CoeffCohes__].Get();
  float d= (float)D.param[CoeffHunger_].Get();
  float e= (float)D.param[CoeffFear___].Get();
  std::vector<Math::Vec3f> velocityChange(NbAgents);

  PosFood.set((float)D.param[PosFoodX____].Get(), (float)D.param[PosFoodY____].Get(), (float)D.param[PosFoodZ____].Get());
  PosPred.set((float)D.param[PosPredX____].Get(), (float)D.param[PosPredY____].Get(), (float)D.param[PosPredZ____].Get());

#pragma omp parallel for
  // Compute the forces
  for (int k0= 0; k0 < NbAgents; k0++) {
    int count= 0;
    Math::Vec3f sep, ali, coh, aim, run;
    for (int k1= 0; k1 < NbAgents; k1++) {
      if ((Pos[k0] - Pos[k1]).normSquared() > sizeAgent * sizeAgent) continue;
      if (k0 == k1) continue;
      ali= ali + Vel[k1];
      coh= coh + Pos[k1];
      count++;
    }
    if (count > 0) {
      sep= Pos[k0] - coh / count;
      ali= ali / count;
      coh= coh / count - Pos[k0];
    }
    aim= PosFood - Pos[k0];
    if ((Pos[k0] - PosPred).normSquared() < (5.0f * sizeAgent) * (5.0f * sizeAgent))
      run= Pos[k0] - PosPred;

    velocityChange[k0]= a * sep + b * ali + c * coh + d * aim + e * run;
  }

  // Apply the forces
  for (int k= 0; k < NbAgents; k++) {
    Vel[k]+= velocityChange[k];
    float l= Vel[k].norm();
    if (l > 0.3f) {
      Vel[k]= 0.3f * Vel[k] / l;
    }
    Pos[k]= Pos[k] + Vel[k] * D.param[TimeStep____].Get();
  }
}


// Draw the project
void AgentSwarmBoid::Draw() {
  if (!isActivProj) return;
  if (!isAllocated) return;
  if (!isRefreshed) return;

  float sizeAgent= (float)D.param[SizeAgent___].Get();

  glBegin(GL_LINES);
  for (int k= 0; k < NbAgents; k++) {
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3fv(Pos[k].array());
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3fv((Pos[k] + Vel[k]).array());
  }
  glEnd();

  for (int k= 0; k < NbAgents; k++) {
    Math::Vec3f front= Vel[k].normalized();
    Math::Vec3f u(1.0f, 0.0f, 0.0f);
    if (u.dot(front) == 0.0f) u.set(0.0f, 1.0f, 0.0f);
    Math::Vec3f left= front.cross(u).normalized();
    Math::Vec3f up= front.cross(left).normalized();

    Math::Vec3f p1, p2, p3, p4, p5, p6, p7;

    p1= Pos[k] + sizeAgent * 2.0f * (+0.5f * front + 0.00f * left + 0.0f * up);
    p2= Pos[k] + sizeAgent * 2.0f * (+0.0f * front - 0.07f * left + 0.1f * up);
    p3= Pos[k] + sizeAgent * 2.0f * (+0.0f * front + 0.07f * left + 0.1f * up);
    p4= Pos[k] + sizeAgent * 2.0f * (+0.0f * front + 0.00f * left - 0.1f * up);
    p5= Pos[k] + sizeAgent * 2.0f * (-0.4f * front + 0.00f * left + 0.0f * up);
    p6= Pos[k] + sizeAgent * 2.0f * (-0.6f * front + 0.00f * left - 0.2f * up);
    p7= Pos[k] + sizeAgent * 2.0f * (-0.6f * front + 0.00f * left + 0.2f * up);

    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 0.0f, 1.0f);
    glVertex3fv(p1.array());
    glColor3f(1.0f, 0.0f, 0.5f);
    glVertex3fv(p2.array());
    glColor3f(1.0f, 0.0f, 0.5f);
    glVertex3fv(p3.array());
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3fv(p1.array());
    glColor3f(0.0f, 0.0f, 0.5f);
    glVertex3fv(p4.array());
    glColor3f(0.0f, 0.0f, 0.5f);
    glVertex3fv(p2.array());
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3fv(p1.array());
    glColor3f(0.0f, 0.0f, 0.5f);
    glVertex3fv(p3.array());
    glColor3f(0.0f, 0.0f, 0.5f);
    glVertex3fv(p4.array());
    glColor3f(0.0f, 0.0f, 0.2f);
    glVertex3fv(p2.array());
    glColor3f(0.0f, 0.0f, 0.2f);
    glVertex3fv(p4.array());
    glColor3f(0.0f, 0.0f, 0.2f);
    glVertex3fv(p5.array());
    glColor3f(0.0f, 0.0f, 0.2f);
    glVertex3fv(p3.array());
    glColor3f(0.0f, 0.0f, 0.2f);
    glVertex3fv(p2.array());
    glColor3f(0.0f, 0.0f, 0.2f);
    glVertex3fv(p5.array());
    glColor3f(0.0f, 0.0f, 0.2f);
    glVertex3fv(p3.array());
    glColor3f(0.0f, 0.0f, 0.2f);
    glVertex3fv(p5.array());
    glColor3f(0.0f, 0.0f, 0.2f);
    glVertex3fv(p4.array());
    glColor3f(0.0f, 0.0f, 0.2f);
    glVertex3fv(p5.array());
    glColor3f(0.0f, 1.0f, 1.0f);
    glVertex3fv(p6.array());
    glColor3f(0.0f, 1.0f, 1.0f);
    glVertex3fv(p7.array());
    glEnd();
  }

  glPointSize(10.0f);
  glBegin(GL_POINTS);
  glColor3f(0.0f, 0.8f, 0.0f);
  glVertex3fv(PosFood.array());
  glColor3f(1.0f, 0.0f, 0.0f);
  glVertex3fv(PosPred.array());
  glEnd();
  glPointSize(1);
}
