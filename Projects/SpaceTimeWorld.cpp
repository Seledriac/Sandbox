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
#include "../Data.hpp"
#include "../fileio/FileInput.hpp"
#include "../math/Vectors.hpp"
#include "../util/Colormap.hpp"
#include "../util/Field.hpp"


extern Data D;

enum ParamType
{
  GR_WorldNbT_________,
  GR_WorldNbX_________,
  GR_WorldNbY_________,
  GR_WorldNbZ_________,
  GR_ScreenNbH________,
  GR_ScreenNbV________,
  GR_ScreenNbS________,
  GR_CursorWorldT_____,
  GR_MassReach________,
  GR_TimePersist______,
  GR_FactorCurv_______,
  GR_FactorDoppler____,
};

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
  Math::Vec3f posBeg;
  Math::Vec3f posEnd;
  Math::Vec3f col;
  float mass;
  float rad0;
  float rad1;

  Shape(
      int const iType,
      Math::Vec3f const iPosBeg,
      Math::Vec3f const iPosEnd,
      Math::Vec3f const iCol,
      float const iMass,
      float const iRad0,
      float const iRad1) {
    type= iType;
    posBeg= iPosBeg;
    posEnd= iPosEnd;
    col= iCol;
    mass= iMass;
    rad0= iRad0;
    rad1= iRad1;
  }

  float ImplicitEval(Math::Vec3f const iPosCell, float const iTimeRatio) {
    Math::Vec3f posObj= posBeg + (posEnd - posBeg) * iTimeRatio;

    float val= 1.0f;
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

  D.param.clear();
  D.param.push_back(ParamUI("GR_WorldNbT_________", 16));
  D.param.push_back(ParamUI("GR_WorldNbX_________", 32));
  D.param.push_back(ParamUI("GR_WorldNbY_________", 32));
  D.param.push_back(ParamUI("GR_WorldNbZ_________", 32));
  D.param.push_back(ParamUI("GR_ScreenNbH________", 64));
  D.param.push_back(ParamUI("GR_ScreenNbV________", 64));
  D.param.push_back(ParamUI("GR_ScreenNbS________", 64));
  D.param.push_back(ParamUI("GR_CursorWorldT_____", 8));
  D.param.push_back(ParamUI("GR_MassReach________", 8));
  D.param.push_back(ParamUI("GR_TimePersist______", 0.8));
  D.param.push_back(ParamUI("GR_FactorCurv_______", 1.0));
  D.param.push_back(ParamUI("GR_FactorDoppler____", 1.0));
}


void SpaceTimeWorld::Refresh() {
  if (!isInitialized) return;
  isRefreshed= true;

  // Ensure parameter validity
  D.param[GR_WorldNbT_________].val= std::max(1.0, D.param[GR_WorldNbT_________].val);
  D.param[GR_WorldNbX_________].val= std::max(1.0, D.param[GR_WorldNbX_________].val);
  D.param[GR_WorldNbY_________].val= std::max(1.0, D.param[GR_WorldNbY_________].val);
  D.param[GR_WorldNbZ_________].val= std::max(1.0, D.param[GR_WorldNbZ_________].val);

  // Get dimensions
  worldNbT= int(std::round(D.param[GR_WorldNbT_________].val));
  worldNbX= int(std::round(D.param[GR_WorldNbX_________].val));
  worldNbY= int(std::round(D.param[GR_WorldNbY_________].val));
  worldNbZ= int(std::round(D.param[GR_WorldNbZ_________].val));

  // Load the BMP image for the background
  static std::vector<std::vector<std::array<float, 3>>> loadedImage;
  if (loadedImage.empty())
    FileInput::LoadImageBMPFile("Resources/Background_AlbertArt.bmp", loadedImage, true);

  // Initialize the world fields
  worldSolid= Field::AllocField4D(worldNbT, worldNbX, worldNbY, worldNbZ, false);
  worldIsFix= Field::AllocField4D(worldNbT, worldNbX, worldNbY, worldNbZ, false);
  worldMasss= Field::AllocField4D(worldNbT, worldNbX, worldNbY, worldNbZ, 0.0f);
  worldColor= Field::AllocField4D(worldNbT, worldNbX, worldNbY, worldNbZ, Math::Vec3f(0.0f, 0.0f, 0.0f));

  // Add the background
  for (int t= 0; t < worldNbT; t++) {
    for (int x= 0; x < worldNbX; x++) {
      for (int y= 0; y < worldNbY; y++) {
        for (int z= 0; z < worldNbZ; z++) {
          // Add boundary conditions to spatial domain border
          if (x % worldNbX == 0 || y % worldNbY == 0 || z % worldNbZ == 0) {
            worldIsFix[t][x][y][z]= true;
            worldMasss[t][x][y][z]= 0.0f;
          }

          // Add grid background layer
          if (x == 0) {
            worldSolid[t][x][y][z]= true;
            worldColor[t][x][y][z].set(0.6f, 0.6f, 0.6f);
            if ((y + 1) % 8 <= 1 || (z + 1) % 8 <= 1)
              worldColor[t][x][y][z].set(0.4f, 0.4f, 0.4f);
          }
          // if (y == 0 || y == worldNbY - 1) {
          //   worldSolid[t][x][y][z]= true;
          //   worldColor[t][x][y][z].set(0.6f, 0.6f, 0.6f);
          //   if ((x + 1) % 10 <= 1 || (z + 1) % 10 <= 1)
          //     worldColor[t][x][y][z].set(0.4f, 0.4f, 0.4f);
          // }
          // if (z == 0 || z == worldNbZ - 1) {
          //   worldSolid[t][x][y][z]= true;
          //   worldColor[t][x][y][z].set(0.6f, 0.6f, 0.6f);
          //   if ((x + 1) % 10 <= 1 || (y + 1) % 10 <= 1)
          //     worldColor[t][x][y][z].set(0.4f, 0.4f, 0.4f);
          // }

          // Add image background layer
          if (x == 0) {
            int imgY= y * int(loadedImage.size()) / worldNbY;
            int imgZ= z * int(loadedImage[0].size()) / worldNbZ;
            worldSolid[t][x][y][z]= true;
            worldColor[t][x][y][z].set(loadedImage[imgY][imgZ][0], loadedImage[imgY][imgZ][1], loadedImage[imgY][imgZ][2]);
          }
        }
      }
    }
  }

  // Create list of shapes to add
  std::vector<Shape> shapes;
  shapes.push_back(Shape(0, Math::Vec3f(0.6f, -0.2f, -0.2f), Math::Vec3f(0.6f, +1.2f, 1.2f), Math::Vec3f(0.2f, 0.6f, 0.2f), +10.0f, 0.05f, 0.00f));  // 2 crossing balls
  // shapes.push_back(Shape(0, Math::Vec3f(0.3f, +1.2f, -0.2f), Math::Vec3f(0.3f, -0.2f, 1.2f), Math::Vec3f(0.6f, 0.2f, 0.2f), -10.0f, 0.05f, 0.00f));  // 2 crossing balls
  // shapes.push_back(Shape(0, Math::Vec3f(0.2f, +0.5f, +0.5f), Math::Vec3f(0.8f, +0.5f, 0.5f), Math::Vec3f(0.6f, 0.2f, 0.2f), +20.0f, 0.03f, 0.00f));  // 1 small approaching ball
  // shapes.push_back(Shape(1, Math::Vec3f(0.5f, -0.5f, +0.5f), Math::Vec3f(0.5f, +1.5f, 0.5f), Math::Vec3f(0.3f, 0.3f, 0.7f), +10.0f, 0.15f, 0.03f));  // 1 moving donut

  // Add the shapes
  for (int t= 0; t < worldNbT; t++) {
    float posT= 0.5f;
    if (worldNbT > 1) posT= float(t) / float(worldNbT - 1);
    for (int x= 0; x < worldNbX; x++) {
      for (int y= 0; y < worldNbY; y++) {
        for (int z= 0; z < worldNbZ; z++) {
          Math::Vec3f posCell((float(x) + 0.5f) / float(worldNbX), (float(y) + 0.5f) / float(worldNbY), (float(z) + 0.5f) / float(worldNbZ));
          for (Shape shape : shapes) {
            if (shape.ImplicitEval(posCell, posT) < 0.0f) {
              worldSolid[t][x][y][z]= true;
              worldIsFix[t][x][y][z]= true;
              worldMasss[t][x][y][z]= shape.mass;
              worldColor[t][x][y][z]= shape.col;
              // worldColor[t][x][y][z]= (1.0f - 0.6f * (posT-posCell)[1] / shape.rad0) * shape.col;
              // worldColor[t][x][y][z]= ((x + y + z) % 2 == 0) ? shape.col : 0.8f * shape.col;
            }
          }
        }
      }
    }
  }

  // Precompute a mask for the world flow
  int maskSize= int(std::floor(D.param[GR_MassReach________].val));
  std::vector<std::vector<std::vector<std::vector<Math::Vec4f>>>> maskVec= Field::AllocField4D(2 * maskSize + 1, 2 * maskSize + 1, 2 * maskSize + 1, 2 * maskSize + 1, Math::Vec4f(0.0f, 0.0f, 0.0f, 0.0f));
  for (int t= 0; t < maskSize * 2 + 1; t++) {
    for (int x= 0; x < maskSize * 2 + 1; x++) {
      for (int y= 0; y < maskSize * 2 + 1; y++) {
        for (int z= 0; z < maskSize * 2 + 1; z++) {
          if (t == maskSize && x == maskSize && y == maskSize && z == maskSize) continue;
          Math::Vec4f vec(float(maskSize - t), float(maskSize - x), float(maskSize - y), float(maskSize - z));
          maskVec[t][x][y][z]= vec.normalized() / vec.normSquared();
        }
      }
    }
  }

  // Compute the world flow
  worldFlows= Field::AllocField4D(worldNbT, worldNbX, worldNbY, worldNbZ, Math::Vec4f(0.0f, 0.0f, 0.0f, 0.0f));
#pragma omp parallel for
  for (int t= 0; t < worldNbT; t++) {
    for (int x= 0; x < worldNbX; x++) {
      for (int y= 0; y < worldNbY; y++) {
        for (int z= 0; z < worldNbZ; z++) {
          if (worldMasss[t][x][y][z] == 0.0) continue;
          // for (int tOff= t; tOff <= t; tOff++)
          for (int tOff= t; tOff <= std::min(t + maskSize, worldNbT - 1); tOff++)
            // for (int tOff= std::max(t - maskSize, 0); tOff <= std::min(t + maskSize, worldNbT - 1); tOff++)
            for (int xOff= std::max(x - maskSize, 0); xOff <= std::min(x + maskSize, worldNbX - 1); xOff++)
              for (int yOff= std::max(y - maskSize, 0); yOff <= std::min(y + maskSize, worldNbY - 1); yOff++)
                for (int zOff= std::max(z - maskSize, 0); zOff <= std::min(z + maskSize, worldNbZ - 1); zOff++)
                  worldFlows[tOff][xOff][yOff][zOff]+= worldMasss[t][x][y][z] * maskVec[maskSize + tOff - t][maskSize + xOff - x][maskSize + yOff - y][maskSize + zOff - z];
        }
      }
    }
  }

  // // Add persistance between timesteps
  // for (int t= 1; t < worldNbT; t++)
  //   for (int x= 0; x < worldNbX; x++)
  //     for (int y= 0; y < worldNbY; y++)
  //       for (int z= 0; z < worldNbZ; z++)
  //         worldFlows[t][x][y][z]= worldFlows[t][x][y][z] + D.param[GR_TimePersist______].val * worldFlows[t - 1][x][y][z];

  // Ensure parameter validity
  D.param[GR_ScreenNbH________].val= std::max(1.0, D.param[GR_ScreenNbH________].val);
  D.param[GR_ScreenNbV________].val= std::max(1.0, D.param[GR_ScreenNbV________].val);
  D.param[GR_ScreenNbS________].val= std::max(1.0, D.param[GR_ScreenNbS________].val);
  D.param[GR_CursorWorldT_____].val= std::min(std::max(std::floor(D.param[GR_CursorWorldT_____].val), 0.0), double(worldNbT - 1));

  // Get dimensions
  screenNbH= int(std::round(D.param[GR_ScreenNbH________].val));
  screenNbV= int(std::round(D.param[GR_ScreenNbV________].val));
  screenNbS= int(std::round(D.param[GR_ScreenNbS________].val));

  // Allocate the screen fields and photon fields
  if (!Field::CheckFieldDimensions(screenColor, screenNbH, screenNbV)) screenColor= Field::AllocField2D(screenNbH, screenNbV, Math::Vec3f(0.0f, 0.0f, 0.0f));
  if (!Field::CheckFieldDimensions(screenCount, screenNbH, screenNbV)) screenCount= Field::AllocField2D(screenNbH, screenNbV, 1);
  if (!Field::CheckFieldDimensions(photonPos, screenNbH, screenNbV, screenNbS)) photonPos= Field::AllocField3D(screenNbH, screenNbV, screenNbS, Math::Vec4f(0.0f, 0.0f, 0.0f, 0.0f));
  if (!Field::CheckFieldDimensions(photonVel, screenNbH, screenNbV, screenNbS)) photonVel= Field::AllocField3D(screenNbH, screenNbV, screenNbS, Math::Vec4f(0.0f, 0.0f, 0.0f, 0.0f));

  // Initialize the photon fields
  for (int h= 0; h < screenNbH; h++) {
    for (int v= 0; v < screenNbV; v++) {
      screenColor[h][v]= Math::Vec3f(0.0f, 0.0f, 0.0f);
      screenCount[h][v]= 1;
      photonPos[h][v][0]= Math::Vec4f((D.param[GR_CursorWorldT_____].val + 0.5f) / float(worldNbT), 1.0f - 1.0f / float(worldNbX), (0.5f + float(h)) / float(screenNbH), (0.5f + float(v)) / float(screenNbV));
      photonVel[h][v][0]= Math::Vec4f(0.0f, -2.0f, 0.0f, 0.0f);
    }
  }

  // Compute the photon paths to render the scene on the screen
#pragma omp parallel for
  for (int h= 0; h < screenNbH; h++) {
    for (int v= 0; v < screenNbV; v++) {
      for (int s= 0; s < screenNbS - 1; s++) {
        int idxT= int(std::floor(photonPos[h][v][s][0] * float(worldNbT)));
        int idxX= int(std::floor(photonPos[h][v][s][1] * float(worldNbX)));
        int idxY= int(std::floor(photonPos[h][v][s][2] * float(worldNbY)));
        int idxZ= int(std::floor(photonPos[h][v][s][3] * float(worldNbZ)));
        if (idxT < 0 || idxT >= worldNbT || idxX < 0 || idxX >= worldNbX || idxY < 0 || idxY >= worldNbY || idxZ < 0 || idxZ >= worldNbZ) break;

        // if (idxT < 0 || idxT >= worldNbT || idxX < 0 || idxX >= worldNbX || idxY < 0 || idxY >= worldNbY || idxZ < 0 || idxZ >= worldNbZ) {
        //   float velDif= D.param[GR_FactorDoppler____].val * (photonVel[h][v][0].norm() - photonVel[h][v][s].norm());
        //   screenColor[h][v]= Math::Vec3f(0.1, 0.1, 0.1) * (1.0 + velDif);
        //   break;
        // }

        photonPos[h][v][s + 1]= photonPos[h][v][s] + photonVel[h][v][s] / float(screenNbS);
        Math::Vec4f flow(
            worldFlows[idxT][idxX][idxY][idxZ][0] * D.param[GR_TimePersist______].val,
            worldFlows[idxT][idxX][idxY][idxZ][1] * D.param[GR_FactorCurv_______].val,
            worldFlows[idxT][idxX][idxY][idxZ][2] * D.param[GR_FactorCurv_______].val,
            worldFlows[idxT][idxX][idxY][idxZ][3] * D.param[GR_FactorCurv_______].val);
        photonVel[h][v][s + 1]= photonVel[h][v][s] + D.param[GR_FactorCurv_______].val * flow / float(screenNbS);
        screenCount[h][v]++;

        int endX= std::min(std::max(int(std::floor(photonPos[h][v][s + 1][1] * worldNbX)), 0), worldNbX - 1);
        int endY= std::min(std::max(int(std::floor(photonPos[h][v][s + 1][2] * worldNbY)), 0), worldNbY - 1);
        int endZ= std::min(std::max(int(std::floor(photonPos[h][v][s + 1][3] * worldNbZ)), 0), worldNbZ - 1);
        bool foundCollision= false;
        std::vector<std::array<int, 3>> listVox= Bresenham3D(idxX, idxY, idxZ, endX, endY, endZ);
        for (std::array<int, 3> voxIdx : listVox) {
          if (worldSolid[idxT][voxIdx[0]][voxIdx[1]][voxIdx[2]]) {
            // float velDif= D.param[GR_FactorDoppler____].val * (photonVel[h][v][0].norm() - photonVel[h][v][s].norm());
            // screenColor[h][v]= worldColor[idxT][voxIdx[0]][voxIdx[1]][voxIdx[2]] * (1.0 + velDif);
            float velDif= D.param[GR_FactorDoppler____].val * (photonPos[h][v][0][0] - photonPos[h][v][s][0]);
            screenColor[h][v]= worldColor[idxT][voxIdx[0]][voxIdx[1]][voxIdx[2]] * (1.0f + velDif);
            foundCollision= true;
            break;
          }
        }
        // if (worldSolid[idxT][endX][endY][endZ]) {
        //   // float velDif= D.param[GR_FactorDoppler____].val * (photonVel[h][v][0].norm() - photonVel[h][v][s].norm());
        //   // screenColor[h][v]= worldColor[idxT][voxIdx[0]][voxIdx[1]][voxIdx[2]] * (1.0f + velDif);
        //   float velDif= D.param[GR_FactorDoppler____].val * (photonPos[h][v][0][0] - photonPos[h][v][s][0]);
        //   screenColor[h][v]= worldColor[idxT][endX][endY][endZ] * (1.0f + velDif);
        //   foundCollision= true;
        //   break;
        // }
        if (foundCollision) break;
      }
    }
  }
}


void SpaceTimeWorld::Animate() {
  if (!isInitialized) return;
  if (!isRefreshed) return;
}


void SpaceTimeWorld::Draw() {
  if (!isInitialized) return;
  if (!isRefreshed) return;

  // Draw the solid voxels
  if (D.displayMode1) {
    int idxT= int(std::floor(D.param[GR_CursorWorldT_____].val));
    glPushMatrix();
    glScalef(1.0f / float(worldNbX), 1.0f / float(worldNbY), 1.0f / float(worldNbZ));
    glTranslatef(0.5f, 0.5f, 0.5f);
    for (int x= 0; x < worldNbX; x++) {
      for (int y= 0; y < worldNbY; y++) {
        for (int z= 0; z < worldNbZ; z++) {
          if (worldSolid[idxT][x][y][z]) {
            glPushMatrix();
            glTranslatef(float(x), float(y), float(z));
            glColor3fv(worldColor[idxT][x][y][z].array());
            glutSolidCube(1.0);
            glPopMatrix();
          }
        }
      }
    }
    glPopMatrix();
  }

  // Draw the space time flow field
  if (D.displayMode2) {
    int idxT= int(std::floor(D.param[GR_CursorWorldT_____].val));
    glBegin(GL_LINES);
    // int displaySkipsize= std::pow((worldNbX * worldNbY * worldNbZ) / 10000, 1.0 / 3.0);
    // for (int x= displaySkipsize / 2; x < worldNbX; x+= displaySkipsize) {
    //   for (int y= displaySkipsize / 2; y < worldNbY; y+= displaySkipsize) {
    //     for (int z= displaySkipsize / 2; z < worldNbZ; z+= displaySkipsize) {
    for (int x= 0; x < worldNbX; x++) {
      for (int y= 0; y < worldNbY; y++) {
        for (int z= 0; z < worldNbZ; z++) {
          // if (worldSolid[idxT][x][y][z]) continue;
          Math::Vec3f flowVec(worldFlows[idxT][x][y][z][1], worldFlows[idxT][x][y][z][2], worldFlows[idxT][x][y][z][3]);
          float r, g, b;
          Colormap::RatioToJetBrightSmooth(0.5 + worldFlows[idxT][x][y][z][0], r, g, b);
          glColor3f(r, g, b);
          Math::Vec3f pos((float(x) + 0.5f) / float(worldNbX), (float(y) + 0.5f) / float(worldNbY), (float(z) + 0.5f) / float(worldNbZ));
          glVertex3fv(pos.array());
          glVertex3fv((pos + D.param[GR_FactorCurv_______].val * flowVec / float(screenNbS)).array());
        }
      }
    }
    glEnd();
  }

  // Draw the screen
  if (D.displayMode3) {
    glPushMatrix();
    glTranslatef(1.0f, 0.0f, 0.0f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    for (int h= 0; h < screenNbH; h++) {
      for (int v= 0; v < screenNbV; v++) {
        glColor3fv(screenColor[h][v].array());
        glRectf(float(h) / float(screenNbH), float(v) / float(screenNbV), float(h + 1) / float(screenNbH), float(v + 1) / float(screenNbV));
      }
    }
    glPopMatrix();
  }

  // Draw the photon paths
  if (D.displayMode4) {
    glBegin(GL_LINES);
    // for (int h= 0; h < screenNbH; h++) {
    //   for (int v= 0; v < screenNbV; v++) {
    int displaySkipsize= std::sqrt((screenNbH * screenNbV) / 400);
    for (int h= displaySkipsize / 2; h < screenNbH; h+= displaySkipsize) {
      for (int v= displaySkipsize / 2; v < screenNbV; v+= displaySkipsize) {
        for (int s= 0; s < screenCount[h][v] - 1; s++) {
          Math::Vec3f photonBeg(photonPos[h][v][s][1], photonPos[h][v][s][2], photonPos[h][v][s][3]);
          Math::Vec3f photonEnd(photonPos[h][v][s + 1][1], photonPos[h][v][s + 1][2], photonPos[h][v][s + 1][3]);
          glColor3fv(screenColor[h][v].array());
          glVertex3fv(photonBeg.array());
          glColor3fv(screenColor[h][v].array());
          glVertex3fv(photonEnd.array());
        }
      }
    }
    glEnd();
    glPointSize(2.0f);
    glBegin(GL_POINTS);
    // for (int h= 0; h < screenNbH; h++) {
    //   for (int v= 0; v < screenNbV; v++) {
    for (int h= displaySkipsize / 2; h < screenNbH; h+= displaySkipsize) {
      for (int v= displaySkipsize / 2; v < screenNbV; v+= displaySkipsize) {
        for (int s= 0; s < screenCount[h][v]; s++) {
          Math::Vec3f photonBeg(photonPos[h][v][s][1], photonPos[h][v][s][2], photonPos[h][v][s][3]);
          glColor3fv(screenColor[h][v].array());
          glVertex3fv(photonBeg.array());
        }
      }
    }
    glEnd();
    glPointSize(1.0f);
  }
}
