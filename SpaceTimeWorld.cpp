#include "SpaceTimeWorld.hpp"


// Standard lib
#include <array>
#include <cmath>
#include <cstdio>
#include <ctime>
#include <vector>

// GLUT lib
#include <GL/freeglut.h>

// Project lib
#include "Data.hpp"
#include "SrtFileInput.hpp"
#include "math/Util.hpp"
#include "math/Vectors.hpp"


extern Data D;


inline void myVertex3f(Math::Vec3 vec) { glVertex3f(float(vec[0]), float(vec[1]), float(vec[2])); }
inline void myColor3f(Math::Vec3 vec) { glColor3f(float(vec[0]), float(vec[1]), float(vec[2])); }


std::vector<std::array<int, 3>> Bresenham3D(int x0, int y0, int z0, int x1, int y1, int z1) {
  std::vector<std::array<int, 3>> listVoxels;
  listVoxels.push_back({x0, y0, z0});

  int dx= std::abs(x1 - x0);
  int dy= std::abs(y1 - y0);
  int dz= std::abs(z1 - z0);
  int xs= (x1 > x0) ? 1 : -1;
  int ys= (y1 > y0) ? 1 : -1;
  int zs= (z1 > z0) ? 1 : -1;

  // Driving axis is X-axis
  if (dx >= dy and dx >= dz) {
    int p1= 2 * dy - dx;
    int p2= 2 * dz - dx;
    while (x0 != x1) {
      x0+= xs;
      if (p1 >= 0) {
        y0+= ys;
        p1-= 2 * dx;
      }
      if (p2 >= 0) {
        z0+= zs;
        p2-= 2 * dx;
      }
      p1+= 2 * dy;
      p2+= 2 * dz;
      listVoxels.push_back({x0, y0, z0});
    }
  }
  // Driving axis is Y-axis
  else if (dy >= dx and dy >= dz) {
    int p1= 2 * dx - dy;
    int p2= 2 * dz - dy;
    while (y0 != y1) {
      y0+= ys;
      if (p1 >= 0) {
        x0+= xs;
        p1-= 2 * dy;
      }
      if (p2 >= 0) {
        z0+= zs;
        p2-= 2 * dy;
      }
      p1+= 2 * dx;
      p2+= 2 * dz;
      listVoxels.push_back({x0, y0, z0});
    }
  }
  // Driving axis is Z-axis
  else {
    int p1= 2 * dy - dz;
    int p2= 2 * dx - dz;
    while (z0 != z1) {
      z0+= zs;
      if (p1 >= 0) {
        y0+= ys;
        p1-= 2 * dz;
      }
      if (p2 >= 0) {
        x0+= xs;
        p2-= 2 * dz;
      }
      p1+= 2 * dy;
      p2+= 2 * dx;
      listVoxels.push_back({x0, y0, z0});
    }
  }
  return listVoxels;
}


class Ball
{
  public:
  Math::Vec3 pos;
  Math::Vec3 col;
  Math::Vec3 spin;
  double rad;

  Ball(
      Math::Vec3 const iPos,
      Math::Vec3 const iCol,
      Math::Vec3 const iSpin,
      double const iRad) {
    pos= iPos;
    col= iCol;
    spin= iSpin / iSpin.norm2();
    rad= iRad;
  }
};


SpaceTimeWorld::SpaceTimeWorld() {
  isInitialized= false;
}


void SpaceTimeWorld::Init() {
  isInitialized= true;

  worldNbT= int(std::round(D.param[worldNbT____________].val));
  worldNbX= int(std::round(D.param[worldNbX____________].val));
  worldNbY= int(std::round(D.param[worldNbY____________].val));
  worldNbZ= int(std::round(D.param[worldNbZ____________].val));
  screenNbH= int(std::round(D.param[screenNbH___________].val));
  screenNbV= int(std::round(D.param[screenNbV___________].val));
  screenNbS= int(std::round(D.param[screenNbS___________].val));

  worldSolid= Util::AllocField4D(worldNbT, worldNbX, worldNbY, worldNbZ, false);
  worldColor= Util::AllocField4D(worldNbT, worldNbX, worldNbY, worldNbZ, Math::Vec3(0.0, 0.0, 0.0));
  worldFlows= Util::AllocField4D(worldNbT, worldNbX, worldNbY, worldNbZ, Math::Vec3(0.0, 0.0, 0.0));

  worldBBoxMin= {0.0, 0.0, 0.0};
  worldBBoxMax= {1.0, 1.0, 1.0};

  // Create balls
  std::vector<Ball> balls;
  // balls.push_back(Ball(Math::Vec3(0.7, 0.4, 0.5), Math::Vec3(0.6, 0.0, 0.0), Math::Vec3(0.0, 0.0, 1.0), 0.1));
  // balls.push_back(Ball(Math::Vec3(0.5, 0.7, 0.3), Math::Vec3(0.0, 0.6, 0.0), Math::Vec3(1.0, 0.0, 0.0), 0.05));
  // balls.push_back(Ball(Math::Vec3(0.2, 0.5, 0.6), Math::Vec3(0.0, 0.0, 0.6), Math::Vec3(0.0, 0.0, 0.0), 0.1));

  Math::Vec3 tempPos(D.param[ParamType::testVar0____________].val, D.param[ParamType::testVar1____________].val, D.param[ParamType::testVar2____________].val);
  balls.push_back(Ball(tempPos, Math::Vec3(0.0, 0.6, 0.0), Math::Vec3(0.0, 0.0, 1.0), 0.1));

  for (int t= 0; t < worldNbT; t++) {
    for (int x= 0; x < worldNbX; x++) {
      for (int y= 0; y < worldNbY; y++) {
        for (int z= 0; z < worldNbZ; z++) {
          Math::Vec3 posCell((double(x) + 0.5) / double(worldNbX), (double(y) + 0.5) / double(worldNbY), (double(z) + 0.5) / double(worldNbZ));
          // Add background layer
          if (x == 0) {
            worldSolid[t][x][y][z]= true;
            if (y % 10 == 0 || y % 10 == 9 || z % 10 == 0 || z % 10 == 9)
              worldColor[t][x][y][z].set(0.4, 0.4, 0.4);
            else
              worldColor[t][x][y][z].set(0.6, 0.6, 0.6);
          }
          // Add balls
          for (Ball ball : balls) {
            // Set voxel presence and colors
            if ((posCell - ball.pos).norm2Squared() < ball.rad * ball.rad) {
              worldSolid[t][x][y][z]= true;
              worldColor[t][x][y][z]= ((x + y + z) % 2 == 0) ? ball.col : 0.8 * ball.col;
            }

            // Compute inward gravitational pull
            if (!worldSolid[t][x][y][z]) {
              worldFlows[t][x][y][z]+= D.param[ParamType::gravStrength________].val * (ball.pos - posCell).normalized() / (ball.pos - posCell).norm2Squared();
            }

            // Compute frame dragging
            if (!worldSolid[t][x][y][z]) {
              if (ball.spin.norm2Squared() > 0.0) {
                Math::Vec3 vec= (posCell - ball.pos).normalized();
                Math::Vec3 dir= ball.spin.cross(vec).normalized();
                worldFlows[t][x][y][z]+= D.param[ParamType::dragStrength________].val * (1.0 - std::abs(vec.dot(ball.spin))) * dir / (ball.pos - posCell).norm2Squared();
              }
            }
          }
        }
      }
    }
  }


  // Load PNG image for the background
  static std::vector<std::vector<std::array<double, 4>>> loadedImage;
  if (loadedImage.empty()) {
    try {
      SrtFileInput::LoadImagePNGFile("HubbleDeepField.png", loadedImage, true);
    } catch (...) {
    }
  }
  for (int y= 0; y < worldNbY; y++) {
    for (int z= 0; z < worldNbZ; z++) {
      int imgY= y * int(loadedImage.size()) / worldNbY;
      int imgZ= z * int(loadedImage[0].size()) / worldNbZ;
      worldColor[0][0][y][z].set(loadedImage[imgY][imgZ][0], loadedImage[imgY][imgZ][1], loadedImage[imgY][imgZ][2]);
    }
  }


  photonPos= Util::AllocField3D(screenNbH, screenNbV, screenNbS, Math::Vec3(-1.0, -1.0, -1.0));
  photonVel= Util::AllocField3D(screenNbH, screenNbV, screenNbS, Math::Vec3(0.0, 0.0, 0.0));
  // photonPos= vector<vector<vector<Math::Vec3>>>(screenNbH, vector<vector<Math::Vec3>>(screenNbV, vector<Math::Vec3>(screenNbS, Math::Vec3(-1.0, -1.0, -1.0))));
  // photonVel= vector<vector<vector<Math::Vec3>>>(screenNbH, vector<vector<Math::Vec3>>(screenNbV, vector<Math::Vec3>(screenNbS, Math::Vec3(0.0, 0.0, 0.0))));
  for (int h= 0; h < screenNbH; h++) {
    for (int v= 0; v < screenNbV; v++) {
      photonPos[h][v][0]= Math::Vec3(1.0, (0.5 + double(h)) / double(screenNbH), (0.5 + double(v)) / double(screenNbV));
      photonVel[h][v][0]= Math::Vec3(-2.0, 0.0, 0.0);
    }
  }

  screenCount= Util::AllocField2D(screenNbH, screenNbV, 1);
  screenColor= Util::AllocField2D(screenNbH, screenNbV, Math::Vec3(0.0, 0.0, 0.0));
#pragma omp parallel for
  for (int h= 0; h < screenNbH; h++) {
    for (int v= 0; v < screenNbV; v++) {
      for (int s= 0; s < screenNbS - 1; s++) {
        int begX= std::min(std::max(int(std::floor(photonPos[h][v][s][0] * worldNbX)), 0), worldNbX - 1);
        int begY= std::min(std::max(int(std::floor(photonPos[h][v][s][1] * worldNbY)), 0), worldNbY - 1);
        int begZ= std::min(std::max(int(std::floor(photonPos[h][v][s][2] * worldNbZ)), 0), worldNbZ - 1);
        photonPos[h][v][s + 1]= photonPos[h][v][s] + photonVel[h][v][s] / double(screenNbS);
        photonVel[h][v][s + 1]= photonVel[h][v][s] + worldFlows[0][begX][begY][begZ] / double(screenNbS);
        screenCount[h][v]++;
        int endX= std::min(std::max(int(std::floor(photonPos[h][v][s + 1][0] * worldNbX)), 0), worldNbX - 1);
        int endY= std::min(std::max(int(std::floor(photonPos[h][v][s + 1][1] * worldNbY)), 0), worldNbY - 1);
        int endZ= std::min(std::max(int(std::floor(photonPos[h][v][s + 1][2] * worldNbZ)), 0), worldNbZ - 1);

        std::vector<std::array<int, 3>> listVox= Bresenham3D(begX, begY, begZ, endX, endY, endZ);
        bool foundColision= false;
        for (std::array<int, 3> voxIdx : listVox) {
          if (worldSolid[0][voxIdx[0]][voxIdx[1]][voxIdx[2]]) {
            double velDif= D.param[ParamType::dopplerShift________].val * (photonVel[h][v][0].norm2() - photonVel[h][v][s].norm2());
            screenColor[h][v]= worldColor[0][voxIdx[0]][voxIdx[1]][voxIdx[2]] * (1 + velDif);
            foundColision= true;
            break;
          }
        }
        if (foundColision) break;
      }
    }
  }
}


void SpaceTimeWorld::Draw() {
  if (!isInitialized) return;

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
            Math::Vec3 pos((double(x) + 0.5) / double(worldNbX), (double(y) + 0.5) / double(worldNbY), (double(z) + 0.5) / double(worldNbZ));
            glColor3f(0.0f, 0.8f, 0.0f);
            myVertex3f(pos);
            glColor3f(1.0f, 0.0f, 0.0f);
            myVertex3f(pos + worldFlows[0][x][y][z]);
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
        myColor3f(screenColor[h][v]);
        glRectf(float(h) / float(screenNbH), float(v) / float(screenNbV), float(h + 1) / float(screenNbH), float(v + 1) / float(screenNbV));
      }
    }
    glPopMatrix();
  }

  // Draw the photon paths
  if (D.showPhotonPath) {
    int displaySkipsize= std::sqrt((screenNbH * screenNbV) / 400);
    glBegin(GL_LINES);
    for (int h= displaySkipsize / 2; h < screenNbH; h+= displaySkipsize) {
      for (int v= displaySkipsize / 2; v < screenNbV; v+= displaySkipsize) {
        for (int s= 0; s < screenCount[h][v] - 1; s++) {
          myColor3f(screenColor[h][v]);
          myVertex3f(photonPos[h][v][s]);
          myColor3f(screenColor[h][v]);
          myVertex3f(photonPos[h][v][s + 1]);
        }
      }
    }
    glEnd();
    glPointSize(3.0f);
    glBegin(GL_POINTS);
    for (int h= displaySkipsize / 2; h < screenNbH; h+= displaySkipsize) {
      for (int v= displaySkipsize / 2; v < screenNbV; v+= displaySkipsize) {
        for (int s= 0; s < screenCount[h][v]; s++) {
          myColor3f(screenColor[h][v]);
          myVertex3f(photonPos[h][v][s]);
        }
      }
    }
    glEnd();
    glPointSize(1.0f);
  }

  // Draw photon path for pixel selected by cursor
  if (D.showCursor) {
    int h= std::min(std::max(int(D.param[ParamType::cursorPosY__________].val), 0), screenNbH - 1);
    int v= std::min(std::max(int(D.param[ParamType::cursorPosZ__________].val), 0), screenNbV - 1);
    glBegin(GL_LINES);
    for (int s= 0; s < screenCount[h][v] - 1; s++) {
      myColor3f(screenColor[h][v]);
      myVertex3f(photonPos[h][v][s]);
      myColor3f(screenColor[h][v]);
      myVertex3f(photonPos[h][v][s + 1]);
    }
    glEnd();
    glPointSize(3.0f);
    glBegin(GL_POINTS);
    for (int s= 0; s < screenCount[h][v]; s++) {
      myColor3f(screenColor[h][v]);
      myVertex3f(photonPos[h][v][s]);
    }
    glEnd();
    glPointSize(1.0f);
  }
}
