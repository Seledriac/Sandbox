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
#include "SrtColormap.hpp"
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
  Math::Vec3 posBeg;
  Math::Vec3 posEnd;
  Math::Vec3 col;
  Math::Vec3 spin;
  double rad;
  double mass;

  Ball(
      Math::Vec3 const iPosBeg,
      Math::Vec3 const iPosEnd,
      Math::Vec3 const iCol,
      Math::Vec3 const iSpin,
      double const iRad,
      double const iMass) {
    posBeg= iPosBeg;
    posEnd= iPosEnd;
    col= iCol;
    spin= iSpin / iSpin.norm();
    rad= iRad;
    mass= iMass;
  }
};


SpaceTimeWorld::SpaceTimeWorld() {
  isInitialized= false;
}


void SpaceTimeWorld::Init() {
  isInitialized= true;

  worldNbT= int(std::round(D.param[GR_WorldNbT_________].val));
  worldNbX= int(std::round(D.param[GR_WorldNbX_________].val));
  worldNbY= int(std::round(D.param[GR_WorldNbY_________].val));
  worldNbZ= int(std::round(D.param[GR_WorldNbZ_________].val));
  screenNbH= int(std::round(D.param[GR_ScreenNbH________].val));
  screenNbV= int(std::round(D.param[GR_ScreenNbV________].val));
  screenNbS= int(std::round(D.param[GR_ScreenNbS________].val));

  worldSolid= Util::AllocField4D(worldNbT, worldNbX, worldNbY, worldNbZ, false);
  worldColor= Util::AllocField4D(worldNbT, worldNbX, worldNbY, worldNbZ, Math::Vec3(0.0, 0.0, 0.0));
  worldFlows= Util::AllocField4D(worldNbT, worldNbX, worldNbY, worldNbZ, Math::Vec3(0.0, 0.0, 0.0));

  // Create balls
  std::vector<Ball> balls;
  balls.push_back(Ball(Math::Vec3(0.6, 0.20, 0.20), Math::Vec3(0.6, 0.80, 0.80), Math::Vec3(0.2, 0.6, 0.2), Math::Vec3(0.0, 0.0, 0.2), 0.08, 0.2));
  balls.push_back(Ball(Math::Vec3(0.25, 0.80, 0.20), Math::Vec3(0.25, 0.20, 0.80), Math::Vec3(0.6, 0.2, 0.2), Math::Vec3(0.0, 0.0, 0.2), 0.08, -0.2));

  // Compute the world fields
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
            Math::Vec3 BallToCell= posCell - ball.posBeg;
            if (worldNbT > 1)
              BallToCell-= (ball.posEnd - ball.posBeg) * (double(t) / double(worldNbT - 1));

            // Set voxel presence and colors
            if (BallToCell.abs().max() < ball.rad) {
              worldSolid[t][x][y][z]= true;
              worldColor[t][x][y][z]= ball.col * BallToCell.norm() / ball.rad;
            }
            // if (BallToCell.normSquared() < ball.rad * ball.rad) {
            //   worldSolid[t][x][y][z]= true;
            //   worldColor[t][x][y][z]= ((x + y + z) % 2 == 0) ? ball.col : 0.8 * ball.col;
            // }

            // Compute radial gravitational effect
            if (!worldSolid[t][x][y][z]) {
              worldFlows[t][x][y][z]-= D.param[GR_GravStrength_____].val * ball.mass * BallToCell.normalized() / BallToCell.normSquared();
            }

            // Compute frame dragging
            if (!worldSolid[t][x][y][z]) {
              if (ball.spin[0] != 0.0 || ball.spin[1] != 0.0 || ball.spin[2] != 0.0) {
                Math::Vec3 tmpVec= BallToCell.normalized();
                Math::Vec3 tmpDir= ball.spin.cross(tmpVec).normalized();
                worldFlows[t][x][y][z]+= D.param[GR_DragStrength_____].val * ball.mass * (1.0 - std::abs(tmpVec.dot(ball.spin))) * tmpDir / BallToCell.normSquared();
              }
            }
          }
        }
      }
    }
  }

  // // Load PNG image for the background
  // static std::vector<std::vector<std::array<double, 4>>> loadedImage;
  // if (loadedImage.empty()) {
  //   try {
  //     SrtFileInput::LoadImagePNGFile("HubbleDeepField.png", loadedImage, true);
  //   } catch (...) {
  //   }
  // }
  // for (int t= 0; t < worldNbT; t++) {
  //   for (int y= 0; y < worldNbY; y++) {
  //     for (int z= 0; z < worldNbZ; z++) {
  //       int imgY= y * int(loadedImage.size()) / worldNbY;
  //       int imgZ= z * int(loadedImage[0].size()) / worldNbZ;
  //       worldColor[t][0][y][z].set(loadedImage[imgY][imgZ][0], loadedImage[imgY][imgZ][1], loadedImage[imgY][imgZ][2]);
  //     }
  //   }
  // }

  // Initialize  the photon field
  photonPos= Util::AllocField3D(screenNbH, screenNbV, screenNbS, Math::Vec3(-1.0, -1.0, -1.0));
  photonVel= Util::AllocField3D(screenNbH, screenNbV, screenNbS, Math::Vec3(0.0, 0.0, 0.0));
  photonTim= Util::AllocField3D(screenNbH, screenNbV, screenNbS, double(worldNbT - 1));
  for (int h= 0; h < screenNbH; h++) {
    for (int v= 0; v < screenNbV; v++) {
      photonPos[h][v][0]= Math::Vec3(1.0 - 0.5 / double(screenNbS), (0.5 + double(h)) / double(screenNbH), (0.5 + double(v)) / double(screenNbV));
      photonVel[h][v][0]= Math::Vec3(-2.0, 0.0, 0.0);
    }
  }

  // Compute the photon paths to render the scene on the screen
  screenColor= Util::AllocField2D(screenNbH, screenNbV, Math::Vec3(0.0, 0.0, 0.0));
  screenCount= Util::AllocField2D(screenNbH, screenNbV, 1);
#pragma omp parallel for
  for (int h= 0; h < screenNbH; h++) {
    for (int v= 0; v < screenNbV; v++) {
      for (int s= 0; s < screenNbS - 1; s++) {
        int idxX= int(std::floor(photonPos[h][v][s][0] * worldNbX));
        int idxY= int(std::floor(photonPos[h][v][s][1] * worldNbY));
        int idxZ= int(std::floor(photonPos[h][v][s][2] * worldNbZ));
        if (idxX < 0 || idxX >= worldNbX || idxY < 0 || idxY >= worldNbY || idxZ < 0 || idxZ >= worldNbZ) {
          double velDif= D.param[GR_DopplerShift_____].val * (photonVel[h][v][0].norm() - photonVel[h][v][s].norm());
          screenColor[h][v]= Math::Vec3(0.1, 0.1, 0.1) * (1.0 + velDif);
          break;
        }
        int idxT= std::min(std::max(int(std::floor(photonTim[h][v][s])), 0), worldNbT - 1);
        int begX= std::min(std::max(idxX, 0), worldNbX - 1);
        int begY= std::min(std::max(idxY, 0), worldNbY - 1);
        int begZ= std::min(std::max(idxZ, 0), worldNbZ - 1);
        photonPos[h][v][s + 1]= photonPos[h][v][s] + photonVel[h][v][s] / double(screenNbS);
        photonVel[h][v][s + 1]= photonVel[h][v][s] + worldFlows[idxT][begX][begY][begZ] / double(screenNbS);
        photonTim[h][v][s + 1]= photonTim[h][v][s] - D.param[GR_TimeDilation_____].val * (double(worldNbT) / double(screenNbS));
        screenCount[h][v]++;
        int endX= std::min(std::max(int(std::floor(photonPos[h][v][s + 1][0] * worldNbX)), 0), worldNbX - 1);
        int endY= std::min(std::max(int(std::floor(photonPos[h][v][s + 1][1] * worldNbY)), 0), worldNbY - 1);
        int endZ= std::min(std::max(int(std::floor(photonPos[h][v][s + 1][2] * worldNbZ)), 0), worldNbZ - 1);

        bool foundColision= false;
        std::vector<std::array<int, 3>> listVox= Bresenham3D(begX, begY, begZ, endX, endY, endZ);
        for (std::array<int, 3> voxIdx : listVox) {
          if (worldSolid[idxT][voxIdx[0]][voxIdx[1]][voxIdx[2]]) {
            double velDif= D.param[GR_DopplerShift_____].val * (photonVel[h][v][0].norm() - photonVel[h][v][s].norm());
            screenColor[h][v]= worldColor[idxT][voxIdx[0]][voxIdx[1]][voxIdx[2]] * (1.0 + velDif);
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
    int idxT= std::min(std::max(int(std::floor(D.param[GR_CursorPosT_______].val)), 0), worldNbT - 1);
    glPushMatrix();
    glScalef(1.0f / float(worldNbX), 1.0f / float(worldNbY), 1.0f / float(worldNbZ));
    glTranslatef(0.5f, 0.5f, 0.5f);
    for (int x= 0; x < worldNbX; x++) {
      for (int y= 0; y < worldNbY; y++) {
        for (int z= 0; z < worldNbZ; z++) {
          glPushMatrix();
          if (worldSolid[idxT][x][y][z]) {
            glTranslatef(float(x), float(y), float(z));
            myColor3f(worldColor[idxT][x][y][z]);
            glutSolidCube(1.0);
          }
          glPopMatrix();
        }
      }
    }
    glPopMatrix();
  }

  // Draw the space time flow field
  if (D.showFlow) {
    int idxT= std::min(std::max(int(std::floor(D.param[GR_CursorPosT_______].val)), 0), worldNbT - 1);

    glBegin(GL_LINES);
    int displaySkipsize= std::pow((worldNbX * worldNbY * worldNbZ) / 1000, 1.0 / 3.0);
    for (int x= displaySkipsize / 2; x < worldNbX; x+= displaySkipsize) {
      for (int y= displaySkipsize / 2; y < worldNbY; y+= displaySkipsize) {
        for (int z= displaySkipsize / 2; z < worldNbZ; z+= displaySkipsize) {
          if (!worldSolid[idxT][x][y][z]) {
            double r, g, b;
            SrtColormap::RatioToJetBrightSmooth(worldFlows[idxT][x][y][z].norm(), r, g, b);
            glColor3d(r, g, b);
            Math::Vec3 pos((double(x) + 0.5) / double(worldNbX), (double(y) + 0.5) / double(worldNbY), (double(z) + 0.5) / double(worldNbZ));
            myVertex3f(pos);
            myVertex3f(pos + 0.02 * worldFlows[idxT][x][y][z]);
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
          if (photonTim[h][v][s] > D.param[GR_CursorPosT_______].val) {
            myColor3f(screenColor[h][v]);
            myVertex3f(photonPos[h][v][s]);
            myColor3f(screenColor[h][v]);
            myVertex3f(photonPos[h][v][s + 1]);
          }
        }
      }
    }
    glEnd();
    glPointSize(3.0f);
    glBegin(GL_POINTS);
    for (int h= displaySkipsize / 2; h < screenNbH; h+= displaySkipsize) {
      for (int v= displaySkipsize / 2; v < screenNbV; v+= displaySkipsize) {
        for (int s= 0; s < screenCount[h][v]; s++) {
          if (photonTim[h][v][s] > D.param[GR_CursorPosT_______].val) {
            myColor3f(screenColor[h][v]);
            myVertex3f(photonPos[h][v][s]);
          }
        }
      }
    }
    glEnd();
    glPointSize(1.0f);
  }

  // Draw photon path for pixel selected by cursor
  if (D.showCursor) {
    int h= std::min(std::max(int(D.param[GR_CursorPosY_______].val), 0), screenNbH - 1);
    int v= std::min(std::max(int(D.param[GR_CursorPosZ_______].val), 0), screenNbV - 1);
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
