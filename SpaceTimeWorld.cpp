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


class Shape
{
  public:
  int type;
  Math::Vec3 posBeg;
  Math::Vec3 posEnd;
  Math::Vec3 col;
  double mass;
  double rad0;
  double rad1;

  Shape(
      int const iType,
      Math::Vec3 const iPosBeg,
      Math::Vec3 const iPosEnd,
      Math::Vec3 const iCol,
      double const iMass,
      double const iRad0,
      double const iRad1) {
    type= iType;
    posBeg= iPosBeg;
    posEnd= iPosEnd;
    col= iCol;
    mass= iMass;
    rad0= iRad0;
    rad1= iRad1;
  }

  double ImplicitEval(Math::Vec3 const iPosCell, double const iTimeRatio) {
    Math::Vec3 posObj= posBeg + (posEnd - posBeg) * iTimeRatio;

    double val= 1.0;
    if (type == 0)
      val= (posObj - iPosCell).norm() - rad0;
    if (type == 1)
      val= std::pow(std::sqrt(std::pow((posObj - iPosCell)[1], 2.0) + std::pow((posObj - iPosCell)[2], 2.0)) - rad0, 2.0) + std::pow((posObj - iPosCell)[0], 2.0) - std::pow(rad1, 2.0);

    return val;
  }
};


SpaceTimeWorld::SpaceTimeWorld() {
  isInitialized= false;
  isRefreshed= false;
}


void SpaceTimeWorld::Init() {
  isInitialized= true;
  isRefreshed= false;

  // Get dimensions
  worldNbT= int(std::round(D.param[GR_WorldNbT_________].val));
  worldNbX= int(std::round(D.param[GR_WorldNbX_________].val));
  worldNbY= int(std::round(D.param[GR_WorldNbY_________].val));
  worldNbZ= int(std::round(D.param[GR_WorldNbZ_________].val));

  // Load the PNG image for the background
  static std::vector<std::vector<std::array<double, 4>>> loadedImage;
  if (loadedImage.empty())
    SrtFileInput::LoadImagePNGFile("Background_AlbertArt.png", loadedImage, true);

  // Initialize the world fields
  worldSolid= Util::AllocField4D(worldNbT, worldNbX, worldNbY, worldNbZ, false);
  worldIsFix= Util::AllocField4D(worldNbT, worldNbX, worldNbY, worldNbZ, false);
  worldCurva= Util::AllocField4D(worldNbT, worldNbX, worldNbY, worldNbZ, 0.0);
  worldColor= Util::AllocField4D(worldNbT, worldNbX, worldNbY, worldNbZ, Math::Vec3(0.0, 0.0, 0.0));

  // Add the background
  for (int t= 0; t < worldNbT; t++) {
    for (int x= 0; x < worldNbX; x++) {
      for (int y= 0; y < worldNbY; y++) {
        for (int z= 0; z < worldNbZ; z++) {
          // Add boundary conditions to spatial domain border
          if (x % worldNbX == 0 || y % worldNbY == 0 || z % worldNbZ == 0) {
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
          // if (y == 0 || y == worldNbY - 1) {
          //   worldSolid[t][x][y][z]= true;
          //   worldColor[t][x][y][z].set(0.6, 0.6, 0.6);
          //   if ((x + 1) % 10 <= 1 || (z + 1) % 10 <= 1)
          //     worldColor[t][x][y][z].set(0.4, 0.4, 0.4);
          // }
          // if (z == 0 || z == worldNbZ - 1) {
          //   worldSolid[t][x][y][z]= true;
          //   worldColor[t][x][y][z].set(0.6, 0.6, 0.6);
          //   if ((x + 1) % 10 <= 1 || (y + 1) % 10 <= 1)
          //     worldColor[t][x][y][z].set(0.4, 0.4, 0.4);
          // }

          // Add PNG background layer
          if (x == 0) {
            int imgY= y * int(loadedImage.size()) / worldNbY;
            int imgZ= (worldNbZ - 1 - z) * int(loadedImage[0].size()) / worldNbZ;
            worldSolid[t][x][y][z]= true;
            worldColor[t][x][y][z].set(loadedImage[imgY][imgZ][0], loadedImage[imgY][imgZ][1], loadedImage[imgY][imgZ][2]);
          }
        }
      }
    }
  }

  // Create list of shapes to add
  std::vector<Shape> shapes;
  // shapes.push_back(Shape(0, Math::Vec3(0.6, -0.2, -0.2), Math::Vec3(0.6, +1.2, 1.2), Math::Vec3(0.2, 0.6, 0.2), +1.0, 0.10, 0.00));  // 2 crossing balls
  // shapes.push_back(Shape(0, Math::Vec3(0.2, +1.2, -0.2), Math::Vec3(0.2, -0.2, 1.2), Math::Vec3(0.6, 0.2, 0.2), -1.0, 0.10, 0.00));  // 2 crossing balls
  // shapes.push_back(Shape(0, Math::Vec3(0.2, +0.5, +0.5), Math::Vec3(0.8, +0.5, 0.5), Math::Vec3(0.6, 0.2, 0.2), +2.0, 0.04, 0.00));  // 1 small approaching ball
  shapes.push_back(Shape(1, Math::Vec3(0.6, -0.2, -0.2), Math::Vec3(0.6, 1.2, 1.2), Math::Vec3(0.3, 0.3, 0.7), +1.0, 0.20, 0.05));  // 1 moving donut

  // Add the shapes
  for (int t= 0; t < worldNbT; t++) {
    double posT= 0.5;
    if (worldNbT > 1) posT= double(t) / double(worldNbT - 1);
    for (int x= 0; x < worldNbX; x++) {
      for (int y= 0; y < worldNbY; y++) {
        for (int z= 0; z < worldNbZ; z++) {
          Math::Vec3 posCell((double(x) + 0.5) / double(worldNbX), (double(y) + 0.5) / double(worldNbY), (double(z) + 0.5) / double(worldNbZ));
          for (Shape shape : shapes) {
            if (shape.ImplicitEval(posCell, posT) < 0.0) {
              worldSolid[t][x][y][z]= true;
              worldIsFix[t][x][y][z]= true;
              worldCurva[t][x][y][z]= shape.mass;
              worldColor[t][x][y][z]= shape.col;
              // worldColor[t][x][y][z]= (1.0 - 0.6 * (posCell - pos)[1] / object.rad) * object.col;
              // worldColor[t][x][y][z]= ((x + y + z) % 2 == 0) ? object.col : 0.8 * object.col;
            }
          }
        }
      }
    }
  }


  //   // Jacobi style smooth
  //   for (int t= 0; t < worldNbT; t++) {
  //     for (int k= int(std::floor(D.param[GR_CurvaSmoothIter__].val)); k >= 1; k--) {
  //       // for (int k= 0; k < int(std::floor(D.param[GR_CurvaSmoothIter__].val)); k++) {
  //       std::vector<std::vector<std::vector<double>>> spaceCurvaOld= worldCurva[t];
  // #pragma omp parallel for
  //       for (int x= 0; x < worldNbX; x++) {
  //         for (int y= 0; y < worldNbY; y++) {
  //           for (int z= 0; z < worldNbZ; z++) {
  //             double sum= 0.0, sumWeight= 0.0;
  //             // for (int xOff= std::max(x - k, 0); xOff <= std::min(x + k, worldNbX - 1); xOff+= k) {
  //             //   for (int yOff= std::max(y - k, 0); yOff <= std::min(y + k, worldNbY - 1); yOff+= k) {
  //             //     for (int zOff= std::max(z - k, 0); zOff <= std::min(z + k, worldNbZ - 1); zOff+= k) {
  //             for (int xOff= x - k; xOff <= x + k; xOff+= k) {
  //               if (xOff < 0 || xOff >= worldNbX) continue;
  //               for (int yOff= y - k; yOff <= y + k; yOff+= k) {
  //                 if (yOff < 0 || yOff >= worldNbY) continue;
  //                 for (int zOff= z - k; zOff <= z + k; zOff+= k) {
  //                   if (zOff < 0 || zOff >= worldNbZ) continue;
  //                   double weight= 1.0;
  //                   sum+= weight * spaceCurvaOld[xOff][yOff][zOff];
  //                   sumWeight+= weight;
  //                 }
  //               }
  //             }
  //             if (sumWeight != 0.0) worldCurva[t][x][y][z]= sum / double(sumWeight);
  //           }
  //         }
  //       }
  //     }
  //   }

  // Precompute a mask for the world flow
  int maskSize= int(std::floor(D.param[GR_MassReach________].val));
  std::vector<std::vector<std::vector<Math::Vec4>>> maskVec= Util::AllocField3D(2 * maskSize + 1, 2 * maskSize + 1, 2 * maskSize + 1, Math::Vec4(0.0, 0.0, 0.0, 0.0));
  for (int x= 0; x < maskSize * 2 + 1; x++) {
    for (int y= 0; y < maskSize * 2 + 1; y++) {
      for (int z= 0; z < maskSize * 2 + 1; z++) {
        if (x == maskSize && y == maskSize && z == maskSize) continue;
        Math::Vec4 vec(double(0.0), double(maskSize - x), double(maskSize - y), double(maskSize - z));
        maskVec[x][y][z]= vec.normalized() / vec.normSquared();
      }
    }
  }

  // Compute the world flow
  worldFlows= Util::AllocField4D(worldNbT, worldNbX, worldNbY, worldNbZ, Math::Vec4(0.0, 0.0, 0.0, 0.0));
#pragma omp parallel for
  for (int t= 0; t < worldNbT; t++) {
    for (int x= 0; x < worldNbX; x++) {
      for (int y= 0; y < worldNbY; y++) {
        for (int z= 0; z < worldNbZ; z++) {
          if (worldCurva[t][x][y][z] == 0.0) continue;
          for (int xOff= std::max(x - maskSize, 0); xOff <= std::min(x + maskSize, worldNbX - 1); xOff++) {
            for (int yOff= std::max(y - maskSize, 0); yOff <= std::min(y + maskSize, worldNbY - 1); yOff++) {
              for (int zOff= std::max(z - maskSize, 0); zOff <= std::min(z + maskSize, worldNbZ - 1); zOff++) {
                worldFlows[t][xOff][yOff][zOff]+= worldCurva[t][x][y][z] * maskVec[maskSize + xOff - x][maskSize + yOff - y][maskSize + zOff - z];
              }
            }
          }
        }
      }
    }
  }

  // Add persistance between timesteps
  for (int t= 1; t < worldNbT; t++)
    for (int x= 0; x < worldNbX; x++)
      for (int y= 0; y < worldNbY; y++)
        for (int z= 0; z < worldNbZ; z++)
          worldFlows[t][x][y][z]= worldFlows[t][x][y][z] + D.param[GR_MassTimePersist__].val * worldFlows[t - 1][x][y][z];


  //   // Compute the world flow
  //   worldFlows= Util::AllocField4D(worldNbT, worldNbX, worldNbY, worldNbZ, Math::Vec4(0.0, 0.0, 0.0, 0.0));
  //   for (int t= 0; t < worldNbT; t++) {
  //     int maskSize= int(std::floor(D.param[GR_MassReach________].val));
  // #pragma omp parallel for
  //     for (int x= 0; x < worldNbX; x++) {
  //       for (int y= 0; y < worldNbY; y++) {
  //         for (int z= 0; z < worldNbZ; z++) {
  //           for (int xOff= std::max(x - maskSize, 0); xOff <= std::min(x + maskSize, worldNbX - 1); xOff++) {
  //             for (int yOff= std::max(y - maskSize, 0); yOff <= std::min(y + maskSize, worldNbY - 1); yOff++) {
  //               for (int zOff= std::max(z - maskSize, 0); zOff <= std::min(z + maskSize, worldNbZ - 1); zOff++) {
  //                 if (xOff == x && yOff == y && zOff == z) continue;
  //                 Math::Vec4 vec(double(0.0), double(xOff - x), double(yOff - y), double(zOff - z));
  //                 worldFlows[t][x][y][z]+= worldCurva[t][xOff][yOff][zOff] * vec.normalized() / vec.normSquared();
  //               }
  //             }
  //           }
  //         }
  //       }
  //     }
  //   }

  // worldFlows= Util::AllocField4D(worldNbT, worldNbX, worldNbY, worldNbZ, Math::Vec4(0.0, 0.0, 0.0, 0.0));
  // for (int t= 0; t < worldNbT; t++) {
  //   int maskSize= int(std::floor(D.param[GR_MassReach________].val));
  //   for (int x= 0; x < worldNbX; x++) {
  //     for (int y= 0; y < worldNbY; y++) {
  //       for (int z= 0; z < worldNbZ; z++) {
  //         // if (t > 0) worldFlows[t][x][y][z][0]= (worldCurva[t][x][y][z] - worldCurva[t - 1][x][y][z]);
  //         if (x > 0) worldFlows[t][x][y][z][1]= 0.5 * (worldCurva[t][x][y][z] - worldCurva[t][x - 1][y][z]);
  //         if (y > 0) worldFlows[t][x][y][z][2]= 0.5 * (worldCurva[t][x][y][z] - worldCurva[t][x][y - 1][z]);
  //         if (z > 0) worldFlows[t][x][y][z][3]= 0.5 * (worldCurva[t][x][y][z] - worldCurva[t][x][y][z - 1]);
  //         if (x < worldNbX - 1) worldFlows[t][x][y][z][1]= 0.5 * (worldCurva[t][x + 1][y][z] - worldCurva[t][x][y][z]);
  //         if (y < worldNbY - 1) worldFlows[t][x][y][z][2]= 0.5 * (worldCurva[t][x][y + 1][z] - worldCurva[t][x][y][z]);
  //         if (z < worldNbZ - 1) worldFlows[t][x][y][z][3]= 0.5 * (worldCurva[t][x][y][z + 1] - worldCurva[t][x][y][z]);
  //       }
  //     }
  //   }
  // }
}


void SpaceTimeWorld::Refresh() {
  if (!isInitialized) return;
  isRefreshed= true;

  // Get dimensions
  screenNbH= int(std::round(D.param[GR_ScreenNbH________].val));
  screenNbV= int(std::round(D.param[GR_ScreenNbV________].val));
  screenNbS= int(std::round(D.param[GR_ScreenNbS________].val));

  // Allocate the screen fields and photon fields
  if (!Util::CheckFieldDimensions(screenColor, screenNbH, screenNbV)) screenColor= Util::AllocField2D(screenNbH, screenNbV, Math::Vec3(0.0, 0.0, 0.0));
  if (!Util::CheckFieldDimensions(screenCount, screenNbH, screenNbV)) screenCount= Util::AllocField2D(screenNbH, screenNbV, 1);
  if (!Util::CheckFieldDimensions(photonPos, screenNbH, screenNbV, screenNbS)) photonPos= Util::AllocField3D(screenNbH, screenNbV, screenNbS, Math::Vec4(0.0, 0.0, 0.0, 0.0));
  if (!Util::CheckFieldDimensions(photonVel, screenNbH, screenNbV, screenNbS)) photonVel= Util::AllocField3D(screenNbH, screenNbV, screenNbS, Math::Vec4(0.0, 0.0, 0.0, 0.0));

  // Initialize the photon fields
  for (int h= 0; h < screenNbH; h++) {
    for (int v= 0; v < screenNbV; v++) {
      screenColor[h][v]= Math::Vec3(0.0, 0.0, 0.0);
      screenCount[h][v]= 1;
      photonPos[h][v][0]= Math::Vec4((D.param[GR_CursorWorldT_____].val + 0.5) / double(worldNbT), 1.0 - 1.0 / double(worldNbX), (0.5 + double(h)) / double(screenNbH), (0.5 + double(v)) / double(screenNbV));
      photonVel[h][v][0]= Math::Vec4(0.0, -2.0, 0.0, 0.0);
    }
  }

  // Compute the photon paths to render the scene on the screen
#pragma omp parallel for
  for (int h= 0; h < screenNbH; h++) {
    for (int v= 0; v < screenNbV; v++) {
      for (int s= 0; s < screenNbS - 1; s++) {
        int idxT= int(std::floor(photonPos[h][v][s][0] * double(worldNbT)));
        int idxX= int(std::floor(photonPos[h][v][s][1] * double(worldNbX)));
        int idxY= int(std::floor(photonPos[h][v][s][2] * double(worldNbY)));
        int idxZ= int(std::floor(photonPos[h][v][s][3] * double(worldNbZ)));
        if (idxT < 0 || idxT >= worldNbT || idxX < 0 || idxX >= worldNbX || idxY < 0 || idxY >= worldNbY || idxZ < 0 || idxZ >= worldNbZ) {
          double velDif= D.param[GR_FactorDoppler____].val * (photonVel[h][v][0].norm() - photonVel[h][v][s].norm());
          screenColor[h][v]= Math::Vec3(0.1, 0.1, 0.1) * (1.0 + velDif);
          break;
        }

        photonPos[h][v][s + 1]= photonPos[h][v][s] + photonVel[h][v][s] / double(screenNbS);
        photonVel[h][v][s + 1]= photonVel[h][v][s] + D.param[GR_FactorCurv_______].val * worldFlows[idxT][idxX][idxY][idxZ] / double(screenNbS);
        screenCount[h][v]++;

        int endX= std::min(std::max(int(std::floor(photonPos[h][v][s + 1][1] * worldNbX)), 0), worldNbX - 1);
        int endY= std::min(std::max(int(std::floor(photonPos[h][v][s + 1][2] * worldNbY)), 0), worldNbY - 1);
        int endZ= std::min(std::max(int(std::floor(photonPos[h][v][s + 1][3] * worldNbZ)), 0), worldNbZ - 1);
        bool foundCollision= false;
        std::vector<std::array<int, 3>> listVox= Bresenham3D(idxX, idxY, idxZ, endX, endY, endZ);
        for (std::array<int, 3> voxIdx : listVox) {
          if (worldSolid[idxT][voxIdx[0]][voxIdx[1]][voxIdx[2]]) {
            double velDif= D.param[GR_FactorDoppler____].val * (photonVel[h][v][0].norm() - photonVel[h][v][s].norm());
            screenColor[h][v]= worldColor[idxT][voxIdx[0]][voxIdx[1]][voxIdx[2]] * (1.0 + velDif);
            foundCollision= true;
            break;
          }
        }
        if (foundCollision) break;
      }
    }
  }
}


void SpaceTimeWorld::Draw() {
  if (!isInitialized) return;
  if (!isRefreshed) return;

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
    // int displaySkipsize= std::pow((worldNbX * worldNbY * worldNbZ) / 10000, 1.0 / 3.0);
    // for (int x= displaySkipsize / 2; x < worldNbX; x+= displaySkipsize) {
    //   for (int y= displaySkipsize / 2; y < worldNbY; y+= displaySkipsize) {
    //     for (int z= displaySkipsize / 2; z < worldNbZ; z+= displaySkipsize) {
    for (int x= 0; x < worldNbX; x++) {
      for (int y= 0; y < worldNbY; y++) {
        for (int z= 0; z < worldNbZ; z++) {
          // if (worldSolid[idxT][x][y][z]) continue;
          Math::Vec3 flowVec(worldFlows[idxT][x][y][z][1], worldFlows[idxT][x][y][z][2], worldFlows[idxT][x][y][z][3]);
          double r, g, b;
          SrtColormap::RatioToJetBrightSmooth(0.5 + D.param[GR_FactorCurv_______].val * flowVec.norm() / double(screenNbS), r, g, b);
          glColor3d(r, g, b);
          Math::Vec3 pos((double(x) + 0.5) / double(worldNbX), (double(y) + 0.5) / double(worldNbY), (double(z) + 0.5) / double(worldNbZ));
          glVertex3dv(pos.array());
          glVertex3dv((pos + D.param[GR_FactorCurv_______].val * flowVec / double(screenNbS)).array());
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
        glColor3dv(screenColor[h][v].array());
        glRectf(float(h) / float(screenNbH), float(v) / float(screenNbV), float(h + 1) / float(screenNbH), float(v + 1) / float(screenNbV));
      }
    }
    glPopMatrix();
  }

  // Draw the photon paths
  if (D.showPhotonPath) {
    glBegin(GL_LINES);
    for (int h= 0; h < screenNbH; h++) {
      for (int v= 0; v < screenNbV; v++) {
        // int displaySkipsize= std::sqrt((screenNbH * screenNbV) / 400);
        // for (int h= displaySkipsize / 2; h < screenNbH; h+= displaySkipsize) {
        //   for (int v= displaySkipsize / 2; v < screenNbV; v+= displaySkipsize) {
        for (int s= 0; s < screenCount[h][v] - 1; s++) {
          Math::Vec3 photonBeg(photonPos[h][v][s][1], photonPos[h][v][s][2], photonPos[h][v][s][3]);
          Math::Vec3 photonEnd(photonPos[h][v][s + 1][1], photonPos[h][v][s + 1][2], photonPos[h][v][s + 1][3]);
          glColor3dv(screenColor[h][v].array());
          glVertex3dv(photonBeg.array());
          glColor3dv(screenColor[h][v].array());
          glVertex3dv(photonEnd.array());
        }
      }
    }
    glEnd();
    glPointSize(2.0f);
    glBegin(GL_POINTS);
    for (int h= 0; h < screenNbH; h++) {
      for (int v= 0; v < screenNbV; v++) {
        // for (int h= displaySkipsize / 2; h < screenNbH; h+= displaySkipsize) {
        //   for (int v= displaySkipsize / 2; v < screenNbV; v+= displaySkipsize) {
        for (int s= 0; s < screenCount[h][v]; s++) {
          Math::Vec3 photonBeg(photonPos[h][v][s][1], photonPos[h][v][s][2], photonPos[h][v][s][3]);
          glColor3dv(screenColor[h][v].array());
          glVertex3dv(photonBeg.array());
        }
      }
    }
    glEnd();
    glPointSize(1.0f);
  }
}
