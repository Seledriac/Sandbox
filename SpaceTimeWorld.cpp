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
  double rad;
  double mass;

  Ball(
      Math::Vec3 const iPosBeg,
      Math::Vec3 const iPosEnd,
      Math::Vec3 const iCol,
      double const iRad,
      double const iMass) {
    posBeg= iPosBeg;
    posEnd= iPosEnd;
    col= iCol;
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
  screenNbT= int(std::round(D.param[GR_ScreenNbT________].val));
  screenNbH= int(std::round(D.param[GR_ScreenNbH________].val));
  screenNbV= int(std::round(D.param[GR_ScreenNbV________].val));
  screenNbS= int(std::round(D.param[GR_ScreenNbS________].val));

  // Load the PNG image for the background
  static std::vector<std::vector<std::array<double, 4>>> loadedImage;
  if (loadedImage.empty())
    SrtFileInput::LoadImagePNGFile("HubbleDeepField.png", loadedImage, true);

  // Create list of objects with macro properties
  std::vector<Ball> balls;
  balls.push_back(Ball(Math::Vec3(0.6, -0.20, -0.20), Math::Vec3(0.6, 1.20, 1.20), Math::Vec3(0.2, 0.6, 0.2), 0.1, 100.0));
  // balls.push_back(Ball(Math::Vec3(0.25, 1.20, -0.20), Math::Vec3(0.25, -0.20, 1.20), Math::Vec3(0.6, 0.2, 0.2), 0.1, -100.0));

  // balls.push_back(Ball(Math::Vec3(0.5, 0.2, 0.2), Math::Vec3(0.5, 0.8, 0.8), Math::Vec3(0.6, 0.2, 0.2), 0.1, 10.0));

  // Set the world fields
  worldSolid= Util::AllocField4D(worldNbT, worldNbX, worldNbY, worldNbZ, false);
  worldIsFix= Util::AllocField4D(worldNbT, worldNbX, worldNbY, worldNbZ, false);
  worldCurva= Util::AllocField4D(worldNbT, worldNbX, worldNbY, worldNbZ, 0.0);
  worldColor= Util::AllocField4D(worldNbT, worldNbX, worldNbY, worldNbZ, Math::Vec3(0.0, 0.0, 0.0));
  for (int t= 0; t < worldNbT; t++) {
    for (int x= 0; x < worldNbX; x++) {
      for (int y= 0; y < worldNbY; y++) {
        for (int z= 0; z < worldNbZ; z++) {
          // Add boundary conditions to spatial domain border
          if (x % (worldNbX - 1) == 0 || y % (worldNbY - 1) == 0 || z % (worldNbZ - 1) == 0) {
            worldIsFix[t][x][y][z]= true;
            worldCurva[t][x][y][z]= 0.0;
          }

          // Add grid background layer
          if (x == 0) {
            worldSolid[t][x][y][z]= true;
            worldColor[t][x][y][z].set(0.6, 0.6, 0.6);
            if ((y + 1) % 10 <= 1 || (z + 1) % 10 <= 1)
              worldColor[t][x][y][z].set(0.4, 0.4, 0.4);
          }
          if (y == 0 || y == worldNbY - 1) {
            worldSolid[t][x][y][z]= true;
            worldColor[t][x][y][z].set(0.6, 0.6, 0.6);
            if ((x + 1) % 10 <= 1 || (z + 1) % 10 <= 1)
              worldColor[t][x][y][z].set(0.4, 0.4, 0.4);
          }
          if (z == 0 || z == worldNbZ - 1) {
            worldSolid[t][x][y][z]= true;
            worldColor[t][x][y][z].set(0.6, 0.6, 0.6);
            if ((x + 1) % 10 <= 1 || (y + 1) % 10 <= 1)
              worldColor[t][x][y][z].set(0.4, 0.4, 0.4);
          }

          // // Add PNG background layer
          // if (x == 0) {
          //   int imgY= y * int(loadedImage.size()) / worldNbY;
          //   int imgZ= z * int(loadedImage[0].size()) / worldNbZ;
          //   worldSolid[t][x][y][z]= true;
          //   worldColor[t][x][y][z].set(loadedImage[imgY][imgZ][0], loadedImage[imgY][imgZ][1], loadedImage[imgY][imgZ][2]);
          // }

          // Add balls
          for (Ball ball : balls) {
            Math::Vec3 posCell((double(x) + 0.5) / double(worldNbX), (double(y) + 0.5) / double(worldNbY), (double(z) + 0.5) / double(worldNbZ));
            // Get the ball position
            Math::Vec3 posBall;
            if (worldNbT > 1)
              posBall= ball.posBeg + (ball.posEnd - ball.posBeg) * (double(t) / double(worldNbT - 1));
            else
              posBall= 0.5 * (ball.posBeg + ball.posEnd);

            // Set the voxel values
            if ((posCell - posBall).normSquared() < ball.rad * ball.rad) {
              worldSolid[t][x][y][z]= true;
              worldIsFix[t][x][y][z]= true;
              worldCurva[t][x][y][z]= ball.mass;
              worldColor[t][x][y][z]= ball.col;
              // worldColor[t][x][y][z]= (1.0 - 0.6 * (posCell - posBall)[1] / ball.rad) * ball.col;
              // worldColor[t][x][y][z]= ((x + y + z) % 2 == 0) ? ball.col : 0.8 * ball.col;
            }
          }
        }
      }
    }
  }

  // Jacobi style smooth
  std::vector<std::vector<std::vector<std::vector<bool>>>> worldIsSet= worldIsFix;
  for (int k= int(std::floor(D.param[GR_CurvaSmoothness__].val)); k >= 1; k--) {
    std::vector<std::vector<std::vector<std::vector<double>>>> worldCurvaOld= worldCurva;
    // #pragma omp parallel for
    for (int t= 0; t < worldNbT; t++) {
      for (int x= 0; x < worldNbX; x++) {
        for (int y= 0; y < worldNbY; y++) {
          for (int z= 0; z < worldNbZ; z++) {
            if (worldIsFix[t][x][y][z]) continue;
            double sum= 0.0, sumWeight= 0;
            // for (int tOff= std::max(t - 2 * k, 0); tOff <= std::min(t + 2 * k, worldNbT - 1); tOff+= k) {
            //   for (int xOff= std::max(x - 2 * k, 0); xOff <= std::min(x + 2 * k, worldNbX - 1); xOff+= k) {
            //     for (int yOff= std::max(y - 2 * k, 0); yOff <= std::min(y + 2 * k, worldNbY - 1); yOff+= k) {
            //       for (int zOff= std::max(z - 2 * k, 0); zOff <= std::min(z + 2 * k, worldNbZ - 1); zOff+= k) {
            // for (int tOff= t - 2 * k; tOff <= t + 2 * k; tOff+= k) {
            //   if (tOff < 0 || tOff >= worldNbT) continue;
            //   for (int xOff= x - 2 * k; xOff <= x + 2 * k; xOff+= k) {
            //     if (xOff < 0 || xOff >= worldNbX) continue;
            //     for (int yOff= y - 2 * k; yOff <= y + 2 * k; yOff+= k) {
            //       if (yOff < 0 || yOff >= worldNbY) continue;
            //       for (int zOff= z - 2 * k; zOff <= z + 2 * k; zOff+= k) {
            //         if (zOff < 0 || zOff >= worldNbZ) continue;
            for (int tOff= t - k; tOff <= t + k; tOff+= k) {
              if (tOff < 0 || tOff >= worldNbT) continue;
              for (int xOff= x - k; xOff <= x + k; xOff+= k) {
                if (xOff < 0 || xOff >= worldNbX) continue;
                for (int yOff= y - k; yOff <= y + k; yOff+= k) {
                  if (yOff < 0 || yOff >= worldNbY) continue;
                  for (int zOff= z - k; zOff <= z + k; zOff+= k) {
                    if (zOff < 0 || zOff >= worldNbZ) continue;
                    if (worldIsSet[tOff][xOff][yOff][zOff]) {
                      // double weight= D.param[GR_CurvTime_________].val * (1.0-double(std::abs(t - tOff))/(3.0*double(k)));
                      // weight+= std::sqrt((3 * k) * (3 * k) - (x - xOff) * (x - xOff) + (y - yOff) * (y - yOff) + (z - zOff) * (z - zOff)) * D.param[GR_CurvSpace________].val;
                      double weight= 1.0;
                      sum+= weight * worldCurvaOld[tOff][xOff][yOff][zOff];
                      sumWeight+= weight;
                    }
                  }
                }
              }
            }
            worldIsSet[t][x][y][z]= true;
            if (sumWeight != 0.0) worldCurva[t][x][y][z]= sum / double(sumWeight);
          }
        }
      }
    }
  }

  // Compute the world flow
  worldFlows= Util::AllocField4D(worldNbT, worldNbX, worldNbY, worldNbZ, Math::Vec4(0.0, 0.0, 0.0, 0.0));
  for (int t= 0; t < worldNbT; t++) {
    for (int x= 0; x < worldNbX; x++) {
      for (int y= 0; y < worldNbY; y++) {
        for (int z= 0; z < worldNbZ; z++) {
          if (t > 0 && t < worldNbT - 1) worldFlows[t][x][y][z][0]= worldCurva[t + 1][x][y][z] - worldCurva[t - 1][x][y][z];
          if (x > 0 && x < worldNbX - 1) worldFlows[t][x][y][z][1]= worldCurva[t][x + 1][y][z] - worldCurva[t][x - 1][y][z];
          if (y > 0 && y < worldNbY - 1) worldFlows[t][x][y][z][2]= worldCurva[t][x][y + 1][z] - worldCurva[t][x][y - 1][z];
          if (z > 0 && z < worldNbZ - 1) worldFlows[t][x][y][z][3]= worldCurva[t][x][y][z + 1] - worldCurva[t][x][y][z - 1];
        }
      }
    }
  }

  // Allocate the screen fields and photon fields
  screenColor= Util::AllocField3D(screenNbT, screenNbH, screenNbV, Math::Vec3(0.0, 0.0, 0.0));
  screenCount= Util::AllocField3D(screenNbT, screenNbH, screenNbV, 1);
  photonPos= Util::AllocField4D(screenNbT, screenNbH, screenNbV, screenNbS, Math::Vec4(0.0, 0.0, 0.0, 0.0));
  photonVel= Util::AllocField4D(screenNbT, screenNbH, screenNbV, screenNbS, Math::Vec4(0.0, 0.0, 0.0, 0.0));

  // Initialize the photon fields
  for (int t= 0; t < screenNbT; t++) {
    for (int h= 0; h < screenNbH; h++) {
      for (int v= 0; v < screenNbV; v++) {
        double posT= 0.5 / double(worldNbT);
        if (worldNbT > 1) posT+= double(t) / double(screenNbT - 1) * (double(worldNbT - 1) / double(worldNbT));
        photonPos[t][h][v][0]= Math::Vec4(posT, 1.0 - 1.0 / double(worldNbX), (0.5 + double(h)) / double(screenNbH), (0.5 + double(v)) / double(screenNbV));
        photonVel[t][h][v][0]= Math::Vec4(0.0, -2.0, 0.0, 0.0);
      }
    }
  }

// Loop through each screen frame
#pragma omp parallel for
  for (int t= 0; t < screenNbT; t++) {
    // Compute the photon paths to render the scene on the screen
    for (int h= 0; h < screenNbH; h++) {
      for (int v= 0; v < screenNbV; v++) {
        for (int s= 0; s < screenNbS - 1; s++) {
          // TODO try periodic photon path unit cube space loop closed thing ?

          // TODO round instead of floor ?
          int idxT= int(std::floor(photonPos[t][h][v][s][0] * worldNbT));
          int idxX= int(std::floor(photonPos[t][h][v][s][1] * worldNbX));
          int idxY= int(std::floor(photonPos[t][h][v][s][2] * worldNbY));
          int idxZ= int(std::floor(photonPos[t][h][v][s][3] * worldNbZ));
          if (idxT < 0 || idxT >= worldNbT || idxX < 0 || idxX >= worldNbX || idxY < 0 || idxY >= worldNbY || idxZ < 0 || idxZ >= worldNbZ) {
            // double velDif= D.param[GR_DopplerShift_____].val * (photonVel[t][h][v][0].norm() - photonVel[t][h][v][s].norm());
            // screenColor[t][h][v]= Math::Vec3(0.1, 0.1, 0.1) * (1.0 + velDif);
            break;
          }
          idxT= std::min(std::max(idxT, 0), worldNbT - 1);
          idxX= std::min(std::max(idxX, 0), worldNbX - 1);
          idxY= std::min(std::max(idxY, 0), worldNbY - 1);
          idxZ= std::min(std::max(idxZ, 0), worldNbZ - 1);

          photonPos[t][h][v][s + 1]= photonPos[t][h][v][s] + photonVel[t][h][v][s] / double(screenNbS);
          photonVel[t][h][v][s + 1]= photonVel[t][h][v][s] + worldFlows[idxT][idxX][idxY][idxZ] / double(screenNbS);
          screenCount[t][h][v]++;
          int endX= std::min(std::max(int(std::floor(photonPos[t][h][v][s + 1][1] * worldNbX)), 0), worldNbX - 1);
          int endY= std::min(std::max(int(std::floor(photonPos[t][h][v][s + 1][2] * worldNbY)), 0), worldNbY - 1);
          int endZ= std::min(std::max(int(std::floor(photonPos[t][h][v][s + 1][3] * worldNbZ)), 0), worldNbZ - 1);

          bool foundCollision= false;
          std::vector<std::array<int, 3>> listVox= Bresenham3D(idxX, idxY, idxZ, endX, endY, endZ);
          for (std::array<int, 3> voxIdx : listVox) {
            if (worldSolid[idxT][voxIdx[0]][voxIdx[1]][voxIdx[2]]) {
              double velDif= D.param[GR_DopplerShift_____].val * (photonVel[t][h][v][0].norm() - photonVel[t][h][v][s].norm());
              screenColor[t][h][v]= worldColor[idxT][voxIdx[0]][voxIdx[1]][voxIdx[2]] * (1.0 + velDif);
              foundCollision= true;
              break;
            }
          }
          if (foundCollision) break;
        }
      }
    }
  }
}


void SpaceTimeWorld::Draw() {
  if (!isInitialized) return;

  // Draw the solid voxels
  if (D.showWorld) {
    int idxT= (worldNbT - 1) / 2;
    if (D.param[GR_CursorWorldT_____].val > 0)
      idxT= std::min(std::max(int(std::floor(D.param[GR_CursorWorldT_____].val)), 0), worldNbT - 1);

    glPushMatrix();
    glScalef(1.0f / float(worldNbX), 1.0f / float(worldNbY), 1.0f / float(worldNbZ));
    glTranslatef(0.5f, 0.5f, 0.5f);
    for (int x= 0; x < worldNbX; x++) {
      for (int y= 0; y < worldNbY; y++) {
        for (int z= 0; z < worldNbZ; z++) {
          glPushMatrix();
          if (worldSolid[idxT][x][y][z]) {
            glTranslatef(float(x), float(y), float(z));
            glColor3dv(worldColor[idxT][x][y][z].array());
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
    int idxT= (worldNbT - 1) / 2;
    if (D.param[GR_CursorWorldT_____].val > 0)
      idxT= std::min(std::max(int(std::floor(D.param[GR_CursorWorldT_____].val)), 0), worldNbT - 1);

    glBegin(GL_LINES);
    // int displaySkipsize= std::pow((worldNbX * worldNbY * worldNbZ) / 1000, 1.0 / 3.0);
    // for (int x= displaySkipsize / 2; x < worldNbX; x+= displaySkipsize) {
    //   for (int y= displaySkipsize / 2; y < worldNbY; y+= displaySkipsize) {
    //     for (int z= displaySkipsize / 2; z < worldNbZ; z+= displaySkipsize) {
    for (int x= 0; x < worldNbX; x++) {
      for (int y= 0; y < worldNbY; y++) {
        for (int z= 0; z < worldNbZ; z++) {
          if (worldSolid[idxT][x][y][z]) continue;
          double flowTime= worldFlows[idxT][x][y][z][0] * D.param[testVar0____________].val;
          Math::Vec3 flowPos(worldFlows[idxT][x][y][z][1], worldFlows[idxT][x][y][z][2], worldFlows[idxT][x][y][z][3]);
          double r, g, b;
          SrtColormap::RatioToJetBrightSmooth(0.5 + flowTime, r, g, b);
          glColor3d(r, g, b);
          Math::Vec3 pos((double(x) + 0.5) / double(worldNbX), (double(y) + 0.5) / double(worldNbY), (double(z) + 0.5) / double(worldNbZ));
          glVertex3dv(pos.array());
          glVertex3dv((pos + 0.002 * flowPos).array());
        }
      }
    }
    glEnd();
  }

  // Draw the screen
  if (D.showScreen) {
    int idxT= (screenNbT - 1) / 2;
    if (D.param[GR_CursorScreenT____].val > 0)
      idxT= std::min(std::max(int(std::floor(D.param[GR_CursorScreenT____].val)), 0), screenNbT - 1);

    glPushMatrix();
    glTranslatef(1.0f, 0.0f, 0.0f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    for (int h= 0; h < screenNbH; h++) {
      for (int v= 0; v < screenNbV; v++) {
        glColor3dv(screenColor[idxT][h][v].array());
        glRectf(float(h) / float(screenNbH), float(v) / float(screenNbV), float(h + 1) / float(screenNbH), float(v + 1) / float(screenNbV));
      }
    }
    glPopMatrix();
  }

  // Draw the photon paths
  if (D.showPhotonPath) {
    int idxT= (screenNbT - 1) / 2;
    if (D.param[GR_CursorScreenT____].val > 0)
      idxT= std::min(std::max(int(std::floor(D.param[GR_CursorScreenT____].val)), 0), screenNbT - 1);

    int displaySkipsize= std::sqrt((screenNbH * screenNbV) / 400);
    glBegin(GL_LINES);
    for (int h= displaySkipsize / 2; h < screenNbH; h+= displaySkipsize) {
      for (int v= displaySkipsize / 2; v < screenNbV; v+= displaySkipsize) {
        for (int s= 0; s < screenCount[idxT][h][v] - 1; s++) {
          Math::Vec3 photonBeg(photonPos[idxT][h][v][s][1], photonPos[idxT][h][v][s][2], photonPos[idxT][h][v][s][3]);
          Math::Vec3 photonEnd(photonPos[idxT][h][v][s + 1][1], photonPos[idxT][h][v][s + 1][2], photonPos[idxT][h][v][s + 1][3]);
          glColor3dv(screenColor[idxT][h][v].array());
          glVertex3dv(photonBeg.array());
          glColor3dv(screenColor[idxT][h][v].array());
          glVertex3dv(photonEnd.array());
        }
      }
    }
    glEnd();
    glPointSize(3.0f);
    glBegin(GL_POINTS);
    for (int h= displaySkipsize / 2; h < screenNbH; h+= displaySkipsize) {
      for (int v= displaySkipsize / 2; v < screenNbV; v+= displaySkipsize) {
        for (int s= 0; s < screenCount[idxT][h][v]; s++) {
          Math::Vec3 photonBeg(photonPos[idxT][h][v][s][1], photonPos[idxT][h][v][s][2], photonPos[idxT][h][v][s][3]);
          glColor3dv(screenColor[idxT][h][v].array());
          glVertex3dv(photonBeg.array());
        }
      }
    }
    glEnd();
    glPointSize(1.0f);
  }
}
