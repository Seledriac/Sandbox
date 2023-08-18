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


extern Data D;

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


AgentSwarmBoid::AgentSwarmBoid() {
  D.param.clear();
  D.plotData.clear();
  isActiveProject= false;
  isInitialized= false;
  isRefreshed= false;
}


void AgentSwarmBoid::SetActiveProject() {
  if (!isActiveProject) {
    D.param.push_back(ParamUI("NbAgents____", 100));
    D.param.push_back(ParamUI("SizeAgent___", 0.05));
    D.param.push_back(ParamUI("TimeStep____", 0.05));
    D.param.push_back(ParamUI("CoeffSep____", 0.75));
    D.param.push_back(ParamUI("CoeffAli____", 0.015));
    D.param.push_back(ParamUI("CoeffCohes__", 0.45));
    D.param.push_back(ParamUI("CoeffHunger_", 0.04));
    D.param.push_back(ParamUI("CoeffFear___", 0.06));
  }

  isActiveProject= true;
  isInitialized= false;
  isRefreshed= false;
  Initialize();
}


void AgentSwarmBoid::Initialize() {
  // Check if need to skip
  if (!isActiveProject) return;
  if (D.param[NbAgents____].hasChanged()) isInitialized= false;
  if (isInitialized) return;
  isInitialized= true;

  // Get UI parameters
  NbAgents= std::max((int)std::round(D.param[NbAgents____].Get()), 1);

  // Allocate data
  Agents= std::vector<Agent>(NbAgents);

  // Force refresh
  isRefreshed= false;
  Refresh();
}


void AgentSwarmBoid::Refresh() {
  if (!isActiveProject) return;
  if (!isInitialized) return;
  if (isRefreshed) return;
  isRefreshed= true;

  PosFood= Math::Vec3f(0.5f, 0.5f, 0.6f);
  PosPredator= Math::Vec3f(0.5f, 0.5f, 0.4f);
  Math::Vec3f p= Math::Vec3f(0.0f, 0.5f, 0.0f);
  Math::Vec3f dp= Math::Vec3f(0.5f, 0.0f, 0.5f);
  Math::Vec3f v= Math::Vec3f(0.0f, 0.0f, 0.0f);
  Math::Vec3f dv= Math::Vec3f(0.2f, 0.2f, 0.2f);

  for (int k= 0; k < NbAgents; k++) {
    Agents[k].p[0]= 2.0f * dp[0] * (float)rand() / (float)RAND_MAX - dp[0] + p[0];
    Agents[k].p[1]= 2.0f * dp[1] * (float)rand() / (float)RAND_MAX - dp[1] + p[1];
    Agents[k].p[2]= 2.0f * dp[2] * (float)rand() / (float)RAND_MAX - dp[2] + p[2];
    Agents[k].v[0]= 2.0f * dv[0] * (float)rand() / (float)RAND_MAX - dv[0] + v[0];
    Agents[k].v[1]= 2.0f * dv[1] * (float)rand() / (float)RAND_MAX - dv[1] + v[1];
    Agents[k].v[2]= 2.0f * dv[2] * (float)rand() / (float)RAND_MAX - dv[2] + v[2];
    Agents[k].n[0]= 0.0f;
    Agents[k].n[1]= 1.0f;
    Agents[k].n[2]= 0.0f;
  }
}


void AgentSwarmBoid::Animate() {
  if (!isActiveProject) return;
  if (!isInitialized) return;
  if (!isRefreshed) return;

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
      if ((Agents[k0].p - Agents[k1].p).normSquared() > sizeAgent * sizeAgent) continue;
      if (k0 == k1) continue;
      ali= ali + Agents[k1].v;
      coh= coh + Agents[k1].p;
      count++;
    }
    if (count > 0) {
      sep= Agents[k0].p - coh / count;
      ali= ali / count;
      coh= coh / count - Agents[k0].p;
    }
    aim= PosFood - Agents[k0].p;
    if ((Agents[k0].p - PosPredator).normSquared() < (5.0f * sizeAgent) * (5.0f * sizeAgent))
      run= Agents[k0].p - PosPredator;

    velocityChange[k0]= a * sep + b * ali + c * coh + d * aim + e * run;
  }

  // Apply the forces
  for (int k= 0; k < NbAgents; k++) {
    Agents[k].v+= velocityChange[k];
    float l= Agents[k].v.norm();
    if (l > 0.3f) {
      Agents[k].v= 0.3f * Agents[k].v / l;
    }
    Agents[k].p= Agents[k].p + Agents[k].v * D.param[TimeStep____].Get();
  }
}


void AgentSwarmBoid::Draw() {
  if (!isActiveProject) return;
  if (!isInitialized) return;
  if (!isRefreshed) return;

  float sizeAgent= (float)D.param[SizeAgent___].Get();

  glBegin(GL_LINES);
  for (int i= 0; i < NbAgents; i++) {
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3fv(Agents[i].p.array());
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3fv((Agents[i].p + Agents[i].v).array());
  }
  glEnd();

  for (int k= 0; k < NbAgents; k++) {
    Math::Vec3f front= Agents[k].v.normalized();
    Math::Vec3f u(1.0f, 0.0f, 0.0f);
    if (u.dot(front) == 0.0f) u.set(0.0f, 1.0f, 0.0f);
    Math::Vec3f left= front.cross(u).normalized();
    Math::Vec3f up= front.cross(left).normalized();

    Math::Vec3f p1, p2, p3, p4, p5, p6, p7;

    p1= Agents[k].p + sizeAgent * (+0.5f * front + 0.00f * left + 0.0f * up);
    p2= Agents[k].p + sizeAgent * (+0.0f * front - 0.07f * left + 0.1f * up);
    p3= Agents[k].p + sizeAgent * (+0.0f * front + 0.07f * left + 0.1f * up);
    p4= Agents[k].p + sizeAgent * (+0.0f * front + 0.00f * left - 0.1f * up);
    p5= Agents[k].p + sizeAgent * (-0.4f * front + 0.00f * left + 0.0f * up);
    p6= Agents[k].p + sizeAgent * (-0.6f * front + 0.00f * left - 0.2f * up);
    p7= Agents[k].p + sizeAgent * (-0.6f * front + 0.00f * left + 0.2f * up);

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
