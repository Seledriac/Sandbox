#include "SpaceTimeWorld.hpp"


// Standard lib
#include <cmath>
#include <cstdio>
#include <ctime>
#include <vector>

// GLUT lib
#include <GL/freeglut.h>

// Project lib
#include "Data.hpp"
#include "math/Fields.hpp"
#include "math/Vectors.hpp"

extern Data D;

SpaceTimeWorld::SpaceTimeWorld(int const iWorldNbT, int const iWorldNbX, int const iWorldNbY, int const iWorldNbZ,
                               int const iScreenNbH, int const iScreenNbV, int const iScreenNbS) {
  worldNbT= iWorldNbT;
  worldNbX= iWorldNbX;
  worldNbY= iWorldNbY;
  worldNbZ= iWorldNbZ;

  screenNbH= iScreenNbH;
  screenNbV= iScreenNbV;
  screenNbS= iScreenNbS;

  fieldSolid= math::Field4D<bool>(worldNbT, worldNbX, worldNbY, worldNbZ, false);
  fieldColor= math::Field4D<math::Vec3>(worldNbT, worldNbX, worldNbY, worldNbZ, math::Vec3(0.0, 0.0, 0.0));
  fieldGravi= math::Field4D<math::Vec3>(worldNbT, worldNbX, worldNbY, worldNbZ, math::Vec3(0.0, 0.0, 0.0));

  for (int t= 0; t < worldNbT; t++) {
    for (int x= 0; x < worldNbX; x++) {
      for (int y= 0; y < worldNbY; y++) {
        for (int z= 0; z < worldNbZ; z++) {
          math::Vec3 posCell(double(x) / double(worldNbX), double(y) / double(worldNbY), double(z) / double(worldNbZ));
          // Add back checkerboard layer
          if (x == 0) {
            fieldSolid(t, x, y, z)= true;
            if (y % 10 == 0 || y % 10 == 9 || z % 10 == 0 || z % 10 == 9)
              fieldColor(t, x, y, z).set(0.2, 0.2, 0.2);
            else
              fieldColor(t, x, y, z).set(0.5, 0.5, 0.5);
          }
          // Add balls
          double radBall= 0.15;
          math::Vec3 posBallR(0.2, 0.5, 0.6);
          if ((posCell - posBallR).length2() < radBall * radBall) {
            fieldSolid(t, x, y, z)= true;
            fieldColor(t, x, y, z).set(4.0*(posCell[0]-posBallR[0]+radBall), 0.0, 0.0);
          }
          math::Vec3 posBallG(0.6, 0.3, 0.3);
          if ((posCell - posBallG).length2() < radBall * radBall) {
            fieldSolid(t, x, y, z)= true;
            fieldColor(t, x, y, z).set(0.0, 3.0*(posCell[0]-posBallG[0]+radBall), 0.0);
          }
          math::Vec3 posBallB(0.4, 0.8, 0.3);
          if ((posCell - posBallB).length2() < radBall * radBall) {
            fieldSolid(t, x, y, z)= true;
            fieldColor(t, x, y, z).set(0.0, 0.0, 3.0*(posCell[0]-posBallB[0]+radBall));
          }
          // Compute grav field
          if (!fieldSolid(t, x, y, z)) {
            fieldGravi(t, x, y, z)+= D.timeVal * (posBallR - posCell).normalized() / (posBallR - posCell).length2();
            fieldGravi(t, x, y, z)+= D.timeVal * (posBallG - posCell).normalized() / (posBallG - posCell).length2();
            fieldGravi(t, x, y, z)+= D.timeVal * (posBallB - posCell).normalized() / (posBallB - posCell).length2();
          }
        }
      }
    }
  }

  screenSet= math::Field2D<bool>(screenNbH, screenNbV, false);
  screenCol= math::Field2D<math::Vec3>(screenNbH, screenNbV, math::Vec3(0.0, 0.0, 0.0));
  photonPos= math::Field3D<math::Vec3>(screenNbH, screenNbV, screenNbS, math::Vec3(-1.0, -1.0, -1.0));
  photonVel= math::Field3D<math::Vec3>(screenNbH, screenNbV, screenNbS, math::Vec3(0.0, 0.0, 0.0));
  for (int h= 0; h < screenNbH; h++) {
    for (int v= 0; v < screenNbV; v++) {
      photonPos(h, v, 0)= math::Vec3(1.0, (0.5 + double(h)) / double(screenNbH), (0.5 + double(v)) / double(screenNbV));
      photonVel(h, v, 0)= math::Vec3(-2.0 / double(screenNbS), 0.0, 0.0);
    }
  }
  #pragma omp parallel for
  for (int h= 0; h < screenNbH; h++) {
    for (int v= 0; v < screenNbV; v++) {
      for (int s= 0; s < screenNbS - 1; s++) {
        if (screenSet(h, v)) continue;
        int worldPosX= std::min(std::max(int(std::floor(photonPos(h, v, s)[0] * worldNbX)), 0), worldNbX - 1);
        int worldPosY= std::min(std::max(int(std::floor(photonPos(h, v, s)[1] * worldNbY)), 0), worldNbY - 1);
        int worldPosZ= std::min(std::max(int(std::floor(photonPos(h, v, s)[2] * worldNbZ)), 0), worldNbZ - 1);
        if (fieldSolid(0, worldPosX, worldPosY, worldPosZ)) {
          screenCol(h, v)= fieldColor(0, worldPosX, worldPosY, worldPosZ);
          screenSet(h, v)= true;
          continue;
        }
        photonPos(h, v, s + 1)= photonPos(h, v, s) + photonVel(h, v, s);
        photonVel(h, v, s + 1)= photonVel(h, v, s) + fieldGravi(0, worldPosX, worldPosY, worldPosZ);
        photonVel(h, v, s + 1).normalize(2.0 / double(screenNbS));
      }
    }
  }
}


inline void myVertex3f(math::Vec3 vec) {
  glVertex3f(float(vec[0]), float(vec[1]), float(vec[2]));
}
inline void myColor3f(math::Vec3 vec) {
  glColor3f(float(vec[0]), float(vec[1]), float(vec[2]));
}

void SpaceTimeWorld::draw() {
  glPushMatrix();
  glScalef(1.0f / float(worldNbX), 1.0f / float(worldNbY), 1.0f / float(worldNbZ));
  glTranslatef(0.5f, 0.5f, 0.5f);
  for (int x= 0; x < worldNbX; x++) {
    for (int y= 0; y < worldNbY; y++) {
      for (int z= 0; z < worldNbZ; z++) {
        glPushMatrix();
        if (fieldSolid(0, x, y, z)) {
          glTranslatef(float(x), float(y), float(z));
          myColor3f(fieldColor(0, x, y, z));
          glutSolidCube(1.0);
        }
        // else {
        //   glTranslatef(float(x), float(y), float(z));
        //   glBegin(GL_LINES);
        //   glColor3f(0.0f, 0.8f, 0.0f);
        //   glVertex3f(0.0f, 0.0f, 0.0f);
        //   glColor3f(1.0f, 0.0f, 0.0f);
        //   myVertex3f(fieldGravi(0, x, y, z));
        //   glEnd();
        // }
        glPopMatrix();
      }
    }
  }
  glPopMatrix();

  // glPushMatrix();
  // glScalef(1.0f / float(worldNbX), 1.0f / float(screenNbH), 1.0f / float(screenNbV));
  // glTranslatef(0.5f, 0.5f, 0.5f);
  // glTranslatef(float(worldNbX), 0.0f, 0.0f);
  // for (int h= 0; h < screenNbH; h++) {
  //   for (int v= 0; v < screenNbV; v++) {
  //     glPushMatrix();
  //     glTranslatef(0.0f, float(h), float(v));
  //     myColor3f(screenCol(h, v));
  //     glutSolidCube(1.0);
  //     glPopMatrix();
  //   }
  // }
  // glPopMatrix();

  glPointSize(5.0f);
  glBegin(GL_POINTS);
  for (int h= 0; h < screenNbH; h++) {
    for (int v= 0; v < screenNbV; v++) {
      myColor3f(screenCol(h, v));
      glVertex3f(1.0, float(h)/float(screenNbH), float(v)/float(screenNbV));
    }
  }
  glEnd();
  glPointSize(1.0f);

  glBegin(GL_LINES);
  for (int h= 0; h < screenNbH; h+= 20) {
    for (int v= 0; v < screenNbV; v+= 20) {
      for (int s= 0; s < screenNbS; s++) {
        myColor3f(screenCol(h, v));
        myVertex3f(photonPos(h, v, s));
        myColor3f(screenCol(h, v));
        myVertex3f(photonPos(h, v, s) + photonVel(h, v, s));
      }
    }
  }
  glEnd();
}


void SpaceTimeWorld::animate(double const iTimestep) {
  (void)iTimestep;
}
