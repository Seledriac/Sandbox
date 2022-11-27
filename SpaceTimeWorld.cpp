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


inline void myVertex3f(math::Vec3 vec) {
  glVertex3f(float(vec[0]), float(vec[1]), float(vec[2]));
}


inline void myColor3f(math::Vec3 vec) {
  glColor3f(float(vec[0]), float(vec[1]), float(vec[2]));
}


class Ball
{
  public:
  math::Vec3 pos;
  math::Vec3 col;
  math::Vec3 spin;
  double rad;

  Ball(
      math::Vec3 const iPos,
      math::Vec3 const iCol,
      math::Vec3 const iSpin,
      double const iRad) {
    pos= iPos;
    col= iCol;
    spin= iSpin / iSpin.length();
    rad= iRad;
  }
};


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
  worldFlow= std::vector<std::vector<std::vector<std::vector<math::Vec3>>>>(worldNbT, std::vector<std::vector<std::vector<math::Vec3>>>(worldNbX, std::vector<std::vector<math::Vec3>>(worldNbY, std::vector<math::Vec3>(worldNbZ, math::Vec3(0.0, 0.0, 0.0)))));

  std::vector<Ball> balls;
  balls.push_back(Ball(math::Vec3(0.5, 0.5, 0.5), math::Vec3(0.6, 0.0, 0.0), math::Vec3(0.0, 0.0, 1.0), 0.1));

  for (int t= 0; t < worldNbT; t++) {
    for (int x= 0; x < worldNbX; x++) {
      for (int y= 0; y < worldNbY; y++) {
        for (int z= 0; z < worldNbZ; z++) {
          math::Vec3 posCell((double(x) + 0.5) / double(worldNbX), (double(y) + 0.5) / double(worldNbY), (double(z) + 0.5) / double(worldNbZ));
          // Add background layer
          if (x == 0) {
            worldSolid[t][x][y][z]= true;
            if (y % 10 == 0 || y % 10 == 9 || z % 10 == 0 || z % 10 == 9)
              worldColor[t][x][y][z].set(0.2, 0.2, 0.2);
            else
              worldColor[t][x][y][z].set(0.4, 0.4, 0.4);
          }
          // Add balls
          for (Ball ball : balls) {
            // Set voxel presence and colors
            if ((posCell - ball.pos).length2() < ball.rad * ball.rad) {
              worldSolid[t][x][y][z]= true;
              worldColor[t][x][y][z]= ball.col;
            }

            // Compute inward gravitational pull
            if (!worldSolid[t][x][y][z]) {
              worldFlow[t][x][y][z]+= D.param[ParamType::gravStrength________].val * (ball.pos - posCell).normalized() / (ball.pos - posCell).length2();
            }

            // Compute frame dragging
            if (!worldSolid[t][x][y][z]) {
              math::Vec3 vec= (ball.pos - posCell).normalized();
              math::Vec3 dir= vec.cross(ball.spin).normalized();
              worldFlow[t][x][y][z]+= D.param[ParamType::dragStrength________].val * (1.0 - std::abs(vec.dot(ball.spin))) * dir / (ball.pos - posCell).length2();
            }
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
      photonVel[h][v][0]= math::Vec3(-3.0, 0.0, 0.0);
    }
  }

  screenSet= std::vector<std::vector<bool>>(screenNbH, std::vector<bool>(screenNbV, false));
  screenCount= std::vector<std::vector<int>>(screenNbH, std::vector<int>(screenNbV, 1));
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
          double velDif= D.param[ParamType::dopplerStrength_____].val * (photonVel[h][v][0].length() - photonVel[h][v][s].length());
          screenCol[h][v]= worldColor[0][worldPosX][worldPosY][worldPosZ] * (1 + velDif);
          screenSet[h][v]= true;
          continue;
        }
        photonPos[h][v][s + 1]= photonPos[h][v][s] + photonVel[h][v][s] / double(screenNbS);
        photonVel[h][v][s + 1]= photonVel[h][v][s] + worldFlow[0][worldPosX][worldPosY][worldPosZ] / double(screenNbS);
        screenCount[h][v]++;
      }
    }
  }
}


void SpaceTimeWorld::draw() {
  // Draw the solid voxels
  if (D.showWorld) {
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
          glPopMatrix();
        }
      }
    }
    glPopMatrix();
  }

  // Draw the gravitational field
  if (D.showGravity) {
    glBegin(GL_LINES);

    int displaySkipsize= std::pow((worldNbX * worldNbY * worldNbZ) / 1000, 1.0 / 3.0);
    for (int x= displaySkipsize / 2; x < worldNbX; x+= displaySkipsize) {
      for (int y= displaySkipsize / 2; y < worldNbY; y+= displaySkipsize) {
        for (int z= displaySkipsize / 2; z < worldNbZ; z+= displaySkipsize) {
          if (!worldSolid[0][x][y][z]) {
            math::Vec3 pos((double(x) + 0.5) / double(worldNbX), (double(y) + 0.5) / double(worldNbY), (double(z) + 0.5) / double(worldNbZ));
            glColor3f(0.0f, 0.8f, 0.0f);
            myVertex3f(pos);
            glColor3f(1.0f, 0.0f, 0.0f);
            myVertex3f(pos + worldFlow[0][x][y][z]);
          }
        }
      }
    }
    glEnd();
  }

  // Draw the screen
  if (D.showScreen) {
    glPushMatrix();
    glTranslatef(1.0f, 0.0f, 0.0f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    for (int h= 0; h < screenNbH; h++) {
      for (int v= 0; v < screenNbV; v++) {
        myColor3f(screenCol[h][v]);
        glRectf(float(h) / float(screenNbH), float(v) / float(screenNbV), float(h + 1) / float(screenNbH), float(v + 1) / float(screenNbV));
      }
    }
    glPopMatrix();
  }

  // Draw the photon paths
  if (D.showPhotonPath) {
    int displaySkipsize= std::sqrt((screenNbH * screenNbV) / 100);
    glBegin(GL_LINES);
    for (int h= displaySkipsize / 2; h < screenNbH; h+= displaySkipsize) {
      for (int v= displaySkipsize / 2; v < screenNbV; v+= displaySkipsize) {
        for (int s= 0; s < screenCount[h][v]-1; s++) {
          myColor3f(screenCol[h][v]);
          myVertex3f(photonPos[h][v][s]);
          myColor3f(screenCol[h][v]);
          myVertex3f(photonPos[h][v][s+1]);
        }
      }
    }
    glEnd();
    // glPointSize(3.0f);
    // glBegin(GL_POINTS);
    // for (int h= displaySkipsize / 2; h < screenNbH; h+= displaySkipsize) {
    //   for (int v= displaySkipsize / 2; v < screenNbV; v+= displaySkipsize) {
    //     for (int s= 0; s < screenCount[h][v]; s++) {
    //       myColor3f(screenCol[h][v]);
    //       myVertex3f(photonPos[h][v][s]);
    //     }
    //   }
    // }
    // glEnd();
    // glPointSize(1.0f);
  }
}


void SpaceTimeWorld::animate(double const iTimestep) {
  (void)iTimestep;
}
