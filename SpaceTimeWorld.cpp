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
#include "math/Vec.hpp"

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

  worldSolid= std::vector<std::vector<std::vector<std::vector<bool>>>>(worldNbT, std::vector<std::vector<std::vector<bool>>>(worldNbX, std::vector<std::vector<bool>>(worldNbY, std::vector<bool>(worldNbZ, false))));
  worldColor= std::vector<std::vector<std::vector<std::vector<math::Vec3>>>>(worldNbT, std::vector<std::vector<std::vector<math::Vec3>>>(worldNbX, std::vector<std::vector<math::Vec3>>(worldNbY, std::vector<math::Vec3>(worldNbZ, math::Vec3(0.0, 0.0, 0.0)))));
  worldGravi= std::vector<std::vector<std::vector<std::vector<math::Vec3>>>>(worldNbT, std::vector<std::vector<std::vector<math::Vec3>>>(worldNbX, std::vector<std::vector<math::Vec3>>(worldNbY, std::vector<math::Vec3>(worldNbZ, math::Vec3(0.0, 0.0, 0.0)))));
  for (int t= 0; t < worldNbT; t++) {
    for (int x= 0; x < worldNbX; x++) {
      for (int y= 0; y < worldNbY; y++) {
        for (int z= 0; z < worldNbZ; z++) {
          math::Vec3 posCell(double(x) / double(worldNbX), double(y) / double(worldNbY), double(z) / double(worldNbZ));
          // Add back checkerboard layer
          if (x == 0) {
            worldSolid[t][x][y][z]= true;
            if (y % 10 == 0 || y % 10 == 9 || z % 10 == 0 || z % 10 == 9)
              worldColor[t][x][y][z].set(0.2, 0.2, 0.2);
            else
              worldColor[t][x][y][z].set(0.5, 0.5, 0.5);
          }
          // Add balls
          double radBall= 0.15;
          math::Vec3 posBallR(0.2, 0.5, 0.6);
          if ((posCell - posBallR).length2() < radBall * radBall) {
            worldSolid[t][x][y][z]= true;
            worldColor[t][x][y][z].set(4.0 * (posCell[0] - posBallR[0] + radBall), 0.0, 0.0);
          }
          math::Vec3 posBallG(0.6, 0.3, 0.2);
          if ((posCell - posBallG).length2() < radBall * radBall) {
            worldSolid[t][x][y][z]= true;
            worldColor[t][x][y][z].set(0.0, 3.0 * (posCell[0] - posBallG[0] + radBall), 0.0);
          }
          math::Vec3 posBallB(0.4, 0.8, 0.3);
          if ((posCell - posBallB).length2() < radBall * radBall) {
            worldSolid[t][x][y][z]= true;
            worldColor[t][x][y][z].set(0.0, 0.0, 3.0 * (posCell[0] - posBallB[0] + radBall));
          }
          // Compute grav field
          if (!worldSolid[t][x][y][z]) {
            worldGravi[t][x][y][z]+= D.timeVal * 0.01 * (posBallR - posCell).normalized() / (posBallR - posCell).length2();
            worldGravi[t][x][y][z]+= D.timeVal * 0.01 * (posBallG - posCell).normalized() / (posBallG - posCell).length2();
            worldGravi[t][x][y][z]+= D.timeVal * 0.01 * (posBallB - posCell).normalized() / (posBallB - posCell).length2();
          }
        }
      }
    }
  }

  photonPos= std::vector<std::vector<std::vector<math::Vec3>>>(screenNbH, std::vector<std::vector<math::Vec3>>(screenNbV, std::vector<math::Vec3>(screenNbS, math::Vec3(-1.0, -1.0, -1.0))));
  photonVel= std::vector<std::vector<std::vector<math::Vec3>>>(screenNbH, std::vector<std::vector<math::Vec3>>(screenNbV, std::vector<math::Vec3>(screenNbS, math::Vec3(0.0, 0.0, 0.0))));
  for (int h= 0; h < screenNbH; h++) {
    for (int v= 0; v < screenNbV; v++) {
      photonPos[h][v][0]= math::Vec3(1.0, (0.5 + double(h)) / double(screenNbH), (0.5 + double(v)) / double(screenNbV));
      photonVel[h][v][0]= math::Vec3(-2.0 / double(screenNbS), 0.0, 0.0);
    }
  }

  screenSet= std::vector<std::vector<bool>>(screenNbH, std::vector<bool>(screenNbV, false));
  screenCol= std::vector<std::vector<math::Vec3>>(screenNbH, std::vector<math::Vec3>(screenNbV, math::Vec3(0.0, 0.0, 0.0)));
#pragma omp parallel for
  for (int h= 0; h < screenNbH; h++) {
    for (int v= 0; v < screenNbV; v++) {
      for (int s= 0; s < screenNbS - 1; s++) {
        if (screenSet[h][v]) continue;
        int worldPosX= std::min(std::max(int(std::floor(photonPos[h][v][s][0] * worldNbX)), 0), worldNbX - 1);
        int worldPosY= std::min(std::max(int(std::floor(photonPos[h][v][s][1] * worldNbY)), 0), worldNbY - 1);
        int worldPosZ= std::min(std::max(int(std::floor(photonPos[h][v][s][2] * worldNbZ)), 0), worldNbZ - 1);
        if (worldSolid[0][worldPosX][worldPosY][worldPosZ]) {
          screenCol[h][v]= worldColor[0][worldPosX][worldPosY][worldPosZ];
          screenSet[h][v]= true;
          continue;
        }
        photonPos[h][v][s + 1]= photonPos[h][v][s] + photonVel[h][v][s];
        photonVel[h][v][s + 1]= photonVel[h][v][s] + worldGravi[0][worldPosX][worldPosY][worldPosZ];
        photonVel[h][v][s + 1].normalize(2.0 / double(screenNbS));
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
        if (worldSolid[0][x][y][z]) {
          glTranslatef(float(x), float(y), float(z));
          myColor3f(worldColor[0][x][y][z]);
          glutSolidCube(1.0);
        }
        // else {
        //   glTranslatef(float(x), float(y), float(z));
        //   glBegin(GL_LINES);
        //   glColor3f(0.0f, 0.8f, 0.0f);
        //   glVertex3f(0.0f, 0.0f, 0.0f);
        //   glColor3f(1.0f, 0.0f, 0.0f);
        //   myVertex3f(worldGravi(0, x, y, z));
        //   glEnd();
        // }
        glPopMatrix();
      }
    }
  }
  glPopMatrix();

  glPointSize(2.0f);
  glBegin(GL_POINTS);
  for (int h= 0; h < screenNbH; h++) {
    for (int v= 0; v < screenNbV; v++) {
      myColor3f(screenCol[h][v]);
      glVertex3f(1.0, float(h) / float(screenNbH), float(v) / float(screenNbV));
    }
  }
  glEnd();
  glPointSize(1.0f);

  glBegin(GL_LINES);
  for (int h= 0; h < screenNbH; h+= 30) {
    for (int v= 0; v < screenNbV; v+= 30) {
      if (!screenSet[h][v]) continue;
      for (int s= 0; s < screenNbS; s++) {
        myColor3f(screenCol[h][v]);
        myVertex3f(photonPos[h][v][s]);
        myColor3f(screenCol[h][v]);
        myVertex3f(photonPos[h][v][s] + photonVel[h][v][s]);
      }
    }
  }
  glEnd();
}


void SpaceTimeWorld::animate(double const iTimestep) {
  (void)iTimestep;
}
