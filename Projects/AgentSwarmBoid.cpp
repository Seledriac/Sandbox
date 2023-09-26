#include "AgentSwarmBoid.hpp"


// Standard lib
#include <cmath>
#include <cstdio>
#include <ctime>
#include <vector>

// GLUT lib
#include <GL/freeglut.h>

// Project lib
#include "../Data.hpp"
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
};


// Constructor
AgentSwarmBoid::AgentSwarmBoid() {
  D.param.clear();
  D.plotData.clear();
  isActivProj= false;
  isAllocated= false;
  isRefreshed= false;
}


// Initialize Project UI parameters
void AgentSwarmBoid::SetActiveProject() {
  if (!isActivProj) {
    D.param.push_back(ParamUI("NbAgents____", 100));
    D.param.push_back(ParamUI("SizeAgent___", 0.05));
    D.param.push_back(ParamUI("TimeStep____", 0.05));
    D.param.push_back(ParamUI("CoeffSep____", 0.75));
    D.param.push_back(ParamUI("CoeffAli____", 0.015));
    D.param.push_back(ParamUI("CoeffCohes__", 0.45));
    D.param.push_back(ParamUI("CoeffHunger_", 0.04));
    D.param.push_back(ParamUI("CoeffFear___", 0.06));
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

  PosFood= Math::Vec3f(0.5f, 0.5f, 0.6f);
  PosPredator= Math::Vec3f(0.5f, 0.5f, 0.4f);
  Math::Vec3f p= Math::Vec3f(0.0f, 0.5f, 0.0f);
  Math::Vec3f dp= Math::Vec3f(0.5f, 0.0f, 0.5f);
  Math::Vec3f v= Math::Vec3f(0.0f, 0.0f, 0.0f);
  Math::Vec3f dv= Math::Vec3f(0.2f, 0.2f, 0.2f);

  for (int k= 0; k < NbAgents; k++) {
    Pos[k][0]= 2.0f * dp[0] * (float)rand() / (float)RAND_MAX - dp[0] + p[0];
    Pos[k][1]= 2.0f * dp[1] * (float)rand() / (float)RAND_MAX - dp[1] + p[1];
    Pos[k][2]= 2.0f * dp[2] * (float)rand() / (float)RAND_MAX - dp[2] + p[2];
    Vel[k][0]= 2.0f * dv[0] * (float)rand() / (float)RAND_MAX - dv[0] + v[0];
    Vel[k][1]= 2.0f * dv[1] * (float)rand() / (float)RAND_MAX - dv[1] + v[1];
    Vel[k][2]= 2.0f * dv[2] * (float)rand() / (float)RAND_MAX - dv[2] + v[2];
    Nor[k][0]= 0.0f;
    Nor[k][1]= 1.0f;
    Nor[k][2]= 0.0f;
  }
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
    if ((Pos[k0] - PosPredator).normSquared() < (5.0f * sizeAgent) * (5.0f * sizeAgent))
      run= Pos[k0] - PosPredator;

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

    p1= Pos[k] + sizeAgent * (+0.5f * front + 0.00f * left + 0.0f * up);
    p2= Pos[k] + sizeAgent * (+0.0f * front - 0.07f * left + 0.1f * up);
    p3= Pos[k] + sizeAgent * (+0.0f * front + 0.07f * left + 0.1f * up);
    p4= Pos[k] + sizeAgent * (+0.0f * front + 0.00f * left - 0.1f * up);
    p5= Pos[k] + sizeAgent * (-0.4f * front + 0.00f * left + 0.0f * up);
    p6= Pos[k] + sizeAgent * (-0.6f * front + 0.00f * left - 0.2f * up);
    p7= Pos[k] + sizeAgent * (-0.6f * front + 0.00f * left + 0.2f * up);

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
  glVertex3fv(PosPredator.array());
  glEnd();
  glPointSize(1);
}
