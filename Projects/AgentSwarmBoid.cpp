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
#include "../util/Vector.hpp"


extern Data D;

enum ParamType
{
  AS_NbAgents_________,
  AS_SizeAgent________,
  AS_TimeStep_________,
  AS_CoeffSep_________,
  AS_CoeffAli_________,
  AS_CoeffCohes_______,
  AS_CoeffHunger______,
  AS_CoeffFear________,
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
    D.param.push_back(ParamUI("AS_NbAgents_________", 100));
    D.param.push_back(ParamUI("AS_SizeAgent________", 0.05));
    D.param.push_back(ParamUI("AS_TimeStep_________", 0.05));
    D.param.push_back(ParamUI("AS_CoeffSep_________", 0.75));
    D.param.push_back(ParamUI("AS_CoeffAli_________", 0.015));
    D.param.push_back(ParamUI("AS_CoeffCohes_______", 0.45));
    D.param.push_back(ParamUI("AS_CoeffHunger______", 0.04));
    D.param.push_back(ParamUI("AS_CoeffFear________", 0.06));
  }

  isActiveProject= true;
  isInitialized= false;
  isRefreshed= false;
  Initialize();
}


void AgentSwarmBoid::Initialize() {
  // Check if need to skip
  if (!isActiveProject) return;
  if (D.param[AS_NbAgents_________].hasChanged()) isInitialized= false;
  if (isInitialized) return;
  isInitialized= true;

  // Get UI parameters
  NbAgents= std::max((int)std::round(D.param[AS_NbAgents_________].Get()), 1);

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

  PosFood= Vector::Vec3f(0.5f, 0.5f, 0.6f);
  PosPredator= Vector::Vec3f(0.5f, 0.5f, 0.4f);
  Vector::Vec3f p= Vector::Vec3f(0.0f, 0.5f, 0.0f);
  Vector::Vec3f dp= Vector::Vec3f(0.5f, 0.0f, 0.5f);
  Vector::Vec3f v= Vector::Vec3f(0.0f, 0.0f, 0.0f);
  Vector::Vec3f dv= Vector::Vec3f(0.2f, 0.2f, 0.2f);

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

  float sizeAgent= (float)D.param[AS_SizeAgent________].Get();
  float a= (float)D.param[AS_CoeffSep_________].Get();
  float b= (float)D.param[AS_CoeffAli_________].Get();
  float c= (float)D.param[AS_CoeffCohes_______].Get();
  float d= (float)D.param[AS_CoeffHunger______].Get();
  float e= (float)D.param[AS_CoeffFear________].Get();
  std::vector<Vector::Vec3f> velocityChange(NbAgents);

#pragma omp parallel for
  // Compute the forces
  for (int k0= 0; k0 < NbAgents; k0++) {
    int count= 0;
    Vector::Vec3f sep, ali, coh, aim, run;
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
    Agents[k].p= Agents[k].p + Agents[k].v * D.param[AS_TimeStep_________].Get();
  }
}


void AgentSwarmBoid::Draw() {
  if (!isActiveProject) return;
  if (!isInitialized) return;
  if (!isRefreshed) return;

  float sizeAgent= (float)D.param[AS_SizeAgent________].Get();

  glBegin(GL_LINES);
  for (int i= 0; i < NbAgents; i++) {
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3fv(Agents[i].p.array());
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3fv((Agents[i].p + Agents[i].v).array());
  }
  glEnd();

  for (int k= 0; k < NbAgents; k++) {
    Vector::Vec3f front= Agents[k].v.normalized();
    Vector::Vec3f u(1.0f, 0.0f, 0.0f);
    if (u.dot(front) == 0.0f) u.set(0.0f, 1.0f, 0.0f);
    Vector::Vec3f left= front.cross(u).normalized();
    Vector::Vec3f up= front.cross(left).normalized();

    Vector::Vec3f p1, p2, p3, p4, p5, p6, p7;

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
