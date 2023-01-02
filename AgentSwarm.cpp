#include "AgentSwarm.hpp"


// Standard lib
#include <cmath>
#include <cstdio>
#include <ctime>
#include <vector>

// GLUT lib
#include <GL/freeglut.h>

// Project lib
#include "Data.hpp"
#include "math/Vectors.hpp"

extern Data D;


AgentSwarm::AgentSwarm() {
  isInitialized= false;
}


void AgentSwarm::Init() {
  isInitialized= true;

  NbAgents= int(std::round(D.param[AS_NbAgents_________].val));
  SizeAgent= D.param[AS_SizeAgent________].val;

  PosFood= Math::Vec3(0.5, 0.5, 0.6);
  PosPredator= Math::Vec3(0.5, 0.5, 0.4);

  Math::Vec3 p= Math::Vec3(0.0, 0.5, 0.0);
  Math::Vec3 dp= Math::Vec3(0.5, 0.0, 0.5);
  Math::Vec3 v= Math::Vec3(0.0, 0.0, 0.0);
  Math::Vec3 dv= Math::Vec3(0.2, 0.2, 0.2);

  Agents= std::vector<Agent>(NbAgents);
  for (int k= 0; k < NbAgents; k++) {
    Agents[k].p[0]= 2.0f * dp[0] * float(rand()) / float(RAND_MAX) - dp[0] + p[0];
    Agents[k].p[1]= 2.0f * dp[1] * float(rand()) / float(RAND_MAX) - dp[1] + p[1];
    Agents[k].p[2]= 2.0f * dp[2] * float(rand()) / float(RAND_MAX) - dp[2] + p[2];
    Agents[k].v[0]= 2.0f * dv[0] * float(rand()) / float(RAND_MAX) - dv[0] + v[0];
    Agents[k].v[1]= 2.0f * dv[1] * float(rand()) / float(RAND_MAX) - dv[1] + v[1];
    Agents[k].v[2]= 2.0f * dv[2] * float(rand()) / float(RAND_MAX) - dv[2] + v[2];
    Agents[k].n[0]= 0.0f;
    Agents[k].n[1]= 1.0f;
    Agents[k].n[2]= 0.0f;
  }
}


void AgentSwarm::Draw() {
  if (!isInitialized) return;

  // glBegin(GL_LINES);
  // for (int i= 0; i < NbAgents; i++) {
  //   glColor3f(0.0, 0.0, 1.0);
  //   glVertex3f(Agents[i].p[0], Agents[i].p[1], Agents[i].p[2]);
  //   glColor3f(1.0, 0.0, 0.0);
  //   glVertex3f(Agents[i].p[0] + Agents[i].v[0], Agents[i].p[1] + Agents[i].v[1], Agents[i].p[2] + Agents[i].v[2]);
  // }
  // glEnd();

  for (int k= 0; k < NbAgents; k++) {
    Math::Vec3 front= Agents[k].v.normalized();
    Math::Vec3 u(1.0, 0.0, 0.0);
    if (u.dot(front) == 0.0) u.set(0.0, 1.0, 0.0);
    Math::Vec3 left= front.cross(u).normalized();
    Math::Vec3 up= front.cross(left).normalized();

    Math::Vec3 p1, p2, p3, p4, p5, p6, p7;

    p1= Agents[k].p + SizeAgent * (0.5 * front + 0.0 * left + 0.0 * up);
    p2= Agents[k].p + SizeAgent * (0.0 * front - 0.07 * left + 0.1 * up);
    p3= Agents[k].p + SizeAgent * (0.0 * front + 0.07 * left + 0.1 * up);
    p4= Agents[k].p + SizeAgent * (0.0 * front + 0.0 * left - 0.1 * up);
    p5= Agents[k].p + SizeAgent * (-0.4 * front + 0.0 * left + 0.0 * up);
    p6= Agents[k].p + SizeAgent * (-0.6 * front + 0.0 * left - 0.2 * up);
    p7= Agents[k].p + SizeAgent * (-0.6 * front + 0.0 * left + 0.2 * up);

    glBegin(GL_TRIANGLES);
    glColor3f(1.0, 0.0, 1.0); glVertex3f(p1[0], p1[1], p1[2]);
    glColor3f(1.0, 0.0, 0.5); glVertex3f(p2[0], p2[1], p2[2]);
    glColor3f(1.0, 0.0, 0.5); glVertex3f(p3[0], p3[1], p3[2]);
    glColor3f(0.0, 0.0, 1.0); glVertex3f(p1[0], p1[1], p1[2]);
    glColor3f(0.0, 0.0, 0.5); glVertex3f(p4[0], p4[1], p4[2]);
    glColor3f(0.0, 0.0, 0.5); glVertex3f(p2[0], p2[1], p2[2]);
    glColor3f(0.0, 0.0, 1.0); glVertex3f(p1[0], p1[1], p1[2]);
    glColor3f(0.0, 0.0, 0.5); glVertex3f(p3[0], p3[1], p3[2]);
    glColor3f(0.0, 0.0, 0.5); glVertex3f(p4[0], p4[1], p4[2]);
    glColor3f(0.0, 0.0, 0.2); glVertex3f(p2[0], p2[1], p2[2]);
    glColor3f(0.0, 0.0, 0.2); glVertex3f(p4[0], p4[1], p4[2]);
    glColor3f(0.0, 0.0, 0.2); glVertex3f(p5[0], p5[1], p5[2]);
    glColor3f(0.0, 0.0, 0.2); glVertex3f(p3[0], p3[1], p3[2]);
    glColor3f(0.0, 0.0, 0.2); glVertex3f(p2[0], p2[1], p2[2]);
    glColor3f(0.0, 0.0, 0.2); glVertex3f(p5[0], p5[1], p5[2]);
    glColor3f(0.0, 0.0, 0.2); glVertex3f(p3[0], p3[1], p3[2]);
    glColor3f(0.0, 0.0, 0.2); glVertex3f(p5[0], p5[1], p5[2]);
    glColor3f(0.0, 0.0, 0.2); glVertex3f(p4[0], p4[1], p4[2]);
    glColor3f(0.0, 0.0, 0.2); glVertex3f(p5[0], p5[1], p5[2]);
    glColor3f(0.0, 1.0, 1.0); glVertex3f(p6[0], p6[1], p6[2]);
    glColor3f(0.0, 1.0, 1.0); glVertex3f(p7[0], p7[1], p7[2]);
    glEnd();
  }

  glPointSize(10.0f);
  glBegin(GL_POINTS);
  glColor3f(0.0f, 0.8f, 0.0f);
  glVertex3f(PosFood[0], PosFood[1], PosFood[2]);
  glColor3f(1.0f, 0.0f, 0.0f);
  glVertex3f(PosPredator[0], PosPredator[1], PosPredator[2]);
  glEnd();
  glPointSize(1);
}


void AgentSwarm::Animate() {
    if (!isInitialized) return;

  float a= float(D.param[AS_CoeffSeparation__].val);
  float b= float(D.param[AS_CoeffAlignment___].val);
  float c= float(D.param[AS_CoeffCohesion____].val);
  float d= float(D.param[AS_CoeffHunger______].val);
  float e= float(D.param[AS_CoeffFear________].val);
  std::vector<Math::Vec3> velocityChange(NbAgents);

  // Compute the forces
  // #pragma omp parallel for
  for (int k0= 0; k0 < NbAgents; k0++) {
    int count= 0;
    Math::Vec3 sep, ali, coh, aim, run;
    for (int k1= 0; k1 < NbAgents; k1++) {
      if ((Agents[k0].p - Agents[k1].p).normSquared() > SizeAgent * SizeAgent) continue;
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
    if ((Agents[k0].p - PosPredator).normSquared() < (5.0 * SizeAgent) * (5.0 * SizeAgent))
      run= Agents[k0].p - PosPredator;

    velocityChange[k0]= a * sep + b * ali + c * coh + d * aim + e * run;
  }

  // Apply the forces
  for (int k= 0; k < NbAgents; k++) {
    Agents[k].v+= velocityChange[k];
    float l= Agents[k].v.norm();
    if (l > 0.3) {
      Agents[k].v= 0.3 * Agents[k].v / l;
    }
    Agents[k].p= Agents[k].p + Agents[k].v * D.param[AS_TimeStep_________].val;
  }
}
