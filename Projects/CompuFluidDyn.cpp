#include "CompuFluidDyn.hpp"


// Standard lib
#include <cstdio>
#include <vector>

// GLUT lib
#include <GL/freeglut.h>

// Project lib
#include "../Data.hpp"
#include "../math/Vectors.hpp"
#include "../util/Colormap.hpp"
#include "../util/Field.hpp"


extern Data D;

enum ParamType
{
  ResolutionX_________,
  ResolutionY_________,
  ResolutionZ_________,
  GaussSeidelIter_____,
  Overrelaxation______,
  TimeStep____________,
  InletVelocity_______,
  ObstacleSize________,
  ObstaclePosX________,
  ObstaclePosY________,
  ObstaclePosZ________,
  ScaleFactor_________,
  ColorFactor_________,
};

CompuFluidDyn::CompuFluidDyn() {
  isInitialized= false;
  isRefreshed= false;
}


void CompuFluidDyn::Init() {
  isInitialized= true;
  isRefreshed= false;

  D.param.clear();
  D.param.push_back(ParamUI("ResolutionX_________", 1));
  D.param.push_back(ParamUI("ResolutionY_________", 20));
  D.param.push_back(ParamUI("ResolutionZ_________", 20));
  D.param.push_back(ParamUI("GaussSeidelIter_____", 1));
  D.param.push_back(ParamUI("Overrelaxation______", 1.9));
  D.param.push_back(ParamUI("TimeStep____________", 0.01));
  D.param.push_back(ParamUI("InletVelocity_______", 1.0));
  D.param.push_back(ParamUI("ObstacleSize________", 0.2));
  D.param.push_back(ParamUI("ObstaclePosX________", 0.5));
  D.param.push_back(ParamUI("ObstaclePosY________", 0.3));
  D.param.push_back(ParamUI("ObstaclePosZ________", 0.5));
  D.param.push_back(ParamUI("ScaleFactor_________", 0.2));
  D.param.push_back(ParamUI("ColorFactor_________", 0.2));
}


void CompuFluidDyn::Refresh() {
  if (!isInitialized) return;
  isRefreshed= true;

  nbX= std::max(int(std::round(D.param[ResolutionX_________].val)), 1);
  nbY= std::max(int(std::round(D.param[ResolutionY_________].val)), 1);
  nbZ= std::max(int(std::round(D.param[ResolutionZ_________].val)), 1);

  density= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  divergence= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  velX= Field::AllocField3D(nbX + 1, nbY, nbZ, 0.0f);
  velY= Field::AllocField3D(nbX, nbY + 1, nbZ, 0.0f);
  velZ= Field::AllocField3D(nbX, nbY, nbZ + 1, 0.0f);
  lockX= Field::AllocField3D(nbX + 1, nbY, nbZ, false);
  lockY= Field::AllocField3D(nbX, nbY + 1, nbZ, false);
  lockZ= Field::AllocField3D(nbX, nbY, nbZ + 1, false);

  for (int x= 0; x < nbX; x++) {
    for (int y= 0; y < nbY; y++) {
      for (int z= 0; z < nbZ; z++) {
        // Lock velocities leaving the domain
        if (x == 0) {
          velX[x + 0][y][z]= 0.0f;
          lockX[x + 0][y][z]= true;
        }
        if (x == nbX - 1) {
          velX[x + 1][y][z]= 0.0f;
          lockX[x + 1][y][z]= true;
        }
        if (y == 0) {
          velY[x][y + 0][z]= 0.0f;
          lockY[x][y + 0][z]= true;
        }
        if (y == nbY - 1) {
          velY[x][y + 1][z]= 0.0f;
          lockY[x][y + 1][z]= true;
        }
        if (z == 0) {
          velZ[x][y][z + 0]= 0.0f;
          lockZ[x][y][z + 0]= true;
        }
        if (z == nbZ - 1) {
          velZ[x][y][z + 1]= 0.0f;
          lockZ[x][y][z + 1]= true;
        }
        // Add locked inlet on Y- side
        if (y == 0) {
          velY[x][y + 0][z]= D.param[InletVelocity_______].val;
          lockY[x][y + 0][z]= true;
        }
        // Add unlocked outlet on Y+ side
        if (y == nbY - 1) {
          velY[x][y + 1][z]= 0.0;
          lockY[x][y + 1][z]= false;
        }
        // Add locked obstacle
        Math::Vec3f posCell((float(x) + 0.5f) / float(nbX), (float(y) + 0.5f) / float(nbY), (float(z) + 0.5f) / float(nbZ));
        Math::Vec3f posObstacle(D.param[ObstaclePosX________].val, D.param[ObstaclePosY________].val, D.param[ObstaclePosZ________].val);
        if ((posCell - posObstacle).normSquared() <= std::pow(std::max(D.param[ObstacleSize________].val, 0.0), 2.0)) {
          density[x][y][z]= 1.0f;
          velX[x + 0][y][z]= 0.0f;
          velX[x + 1][y][z]= 0.0f;
          lockX[x + 0][y][z]= true;
          lockX[x + 1][y][z]= true;
          velY[x][y + 0][z]= 0.0f;
          velY[x][y + 1][z]= 0.0f;
          lockY[x][y + 0][z]= true;
          lockY[x][y + 1][z]= true;
          velZ[x][y][z + 0]= 0.0f;
          velZ[x][y][z + 1]= 0.0f;
          lockZ[x][y][z + 0]= true;
          lockZ[x][y][z + 1]= true;
        }
      }
    }
  }
}


void CompuFluidDyn::Animate() {
  if (!isInitialized) return;
  if (!isRefreshed) return;

  // Solve for incompressibility via Gauss Seidel
  for (int k= 0; k < std::max(int(std::round(D.param[GaussSeidelIter_____].val)), 1); k++) {
    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          divergence[x][y][z]= 0.0;
          divergence[x][y][z]-= velX[x + 0][y][z];
          divergence[x][y][z]+= velX[x + 1][y][z];
          divergence[x][y][z]-= velY[x][y + 0][z];
          divergence[x][y][z]+= velY[x][y + 1][z];
          divergence[x][y][z]-= velZ[x][y][z + 0];
          divergence[x][y][z]+= velZ[x][y][z + 1];

          int count= 0;
          if (!lockX[x + 0][y][z]) count++;
          if (!lockX[x + 1][y][z]) count++;
          if (!lockY[x][y + 0][z]) count++;
          if (!lockY[x][y + 1][z]) count++;
          if (!lockZ[x][y][z + 0]) count++;
          if (!lockZ[x][y][z + 1]) count++;

          if (!lockX[x + 0][y][z]) velX[x + 0][y][z]+= D.param[Overrelaxation______].val * divergence[x][y][z] / float(count);
          if (!lockX[x + 1][y][z]) velX[x + 1][y][z]-= D.param[Overrelaxation______].val * divergence[x][y][z] / float(count);
          if (!lockY[x][y + 0][z]) velY[x][y + 0][z]+= D.param[Overrelaxation______].val * divergence[x][y][z] / float(count);
          if (!lockY[x][y + 1][z]) velY[x][y + 1][z]-= D.param[Overrelaxation______].val * divergence[x][y][z] / float(count);
          if (!lockZ[x][y][z + 0]) velZ[x][y][z + 0]+= D.param[Overrelaxation______].val * divergence[x][y][z] / float(count);
          if (!lockZ[x][y][z + 1]) velZ[x][y][z + 1]-= D.param[Overrelaxation______].val * divergence[x][y][z] / float(count);
        }
      }
    }
  }

  // Compute velocity advection
  std::vector<std::vector<std::vector<float>>> velXold= velX;
  std::vector<std::vector<std::vector<float>>> velYold= velY;
  std::vector<std::vector<std::vector<float>>> velZold= velZ;
  for (int x= 0; x < nbX; x++) {
    for (int y= 0; y < nbY; y++) {
      for (int z= 0; z < nbZ; z++) {
        Math::Vec3f posCell((float(x) + 0.5f) / float(nbX), (float(y) + 0.5f) / float(nbY), (float(z) + 0.5f) / float(nbZ));
        Math::Vec3f velCell(velXold[x][y][z] + velXold[x + 1][y][z], velYold[x][y][z] + velYold[x][y + 1][z], velZold[x][y][z] + velZold[x][y][z + 1]);
        Math::Vec3f posSource= posCell - velCell * std::max(D.param[TimeStep____________].val, 0.0);

        int xOff= int(std::floor(posSource[0] * float(nbX)));
        int yOff= int(std::floor(posSource[1] * float(nbY)));
        int zOff= int(std::floor(posSource[2] * float(nbZ)));
        if (xOff < 0 || xOff >= nbX || yOff < 0 || yOff >= nbY || zOff < 0 || zOff >= nbZ) continue;

        if (!lockX[xOff + 0][yOff][zOff]) velX[xOff + 0][yOff][zOff]+= D.param[TimeStep____________].val*velCell[0];
        if (!lockX[xOff + 1][yOff][zOff]) velX[xOff + 1][yOff][zOff]+= D.param[TimeStep____________].val*velCell[0];
        if (!lockY[xOff][yOff + 0][zOff]) velY[xOff][yOff + 0][zOff]+= D.param[TimeStep____________].val*velCell[1];
        if (!lockY[xOff][yOff + 1][zOff]) velY[xOff][yOff + 1][zOff]+= D.param[TimeStep____________].val*velCell[1];
        if (!lockZ[xOff][yOff][zOff + 0]) velZ[xOff][yOff][zOff + 0]+= D.param[TimeStep____________].val*velCell[2];
        if (!lockZ[xOff][yOff][zOff + 1]) velZ[xOff][yOff][zOff + 1]+= D.param[TimeStep____________].val*velCell[2];
      }
    }
  }
}


void CompuFluidDyn::Draw() {
  if (!isInitialized) return;
  if (!isRefreshed) return;

  float voxSize= 1.0 / float(std::max(std::max(nbX, nbY), nbZ));

  // Draw the solid voxels
  if (D.displayMode1) {
    glPushMatrix();
    glScalef(voxSize, voxSize, voxSize);
    glTranslatef(0.5f, 0.5f, 0.5f);
    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          if (density[x][y][z] > 0.5f) {
            glPushMatrix();
            glTranslatef(float(x), float(y), float(z));
            glColor3f(0.5f, 0.5f, 0.5f);
            glutSolidCube(1.0);
            glPopMatrix();
          }
        }
      }
    }
    glPopMatrix();
  }

  // Draw the velocity fields
  if (D.displayMode2) {
    glPushMatrix();
    glScalef(voxSize, voxSize, voxSize);
    glTranslatef(0.5f, 0.5f, 0.5f);
    glBegin(GL_LINES);
    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          Math::Vec3f vec(0.5 * (velX[x][y][z] + velX[x + 1][y][z]), 0.5 * (velY[x][y][z] + velY[x][y + 1][z]), 0.5 * (velZ[x][y][z] + velZ[x][y][z + 1]));
          float r= 0.0f, g= 0.0f, b= 0.0f;
          if (vec.normSquared() > 0.0)
            Colormap::RatioToJetBrightSmooth(vec.norm() * std::max(D.param[ColorFactor_________].val, 0.0), r, g, b);
          glColor3f(r, g, b);
          Math::Vec3f pos= Math::Vec3f(float(x), float(y), float(z));
          glVertex3fv(pos.array());
          glVertex3fv(pos + vec * std::max(D.param[ScaleFactor_________].val, 0.0));
        }
      }
    }
    glEnd();
    glPopMatrix();
  }

  // Draw the divergence field
  if (D.displayMode3) {
    glPushMatrix();
    glScalef(voxSize, voxSize, voxSize);
    glTranslatef(0.5f, 0.5f, 0.5f);
    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          glPushMatrix();
          glTranslatef(float(x), float(y), float(z));
          float r= 0.0f, g= 0.0f, b= 0.0f;
          Colormap::RatioToJetBrightSmooth(divergence[x][y][z] * std::max(D.param[ColorFactor_________].val, 0.0), r, g, b);
          glColor3f(r, g, b);
          glutSolidCube(1.0);
          glPopMatrix();
        }
      }
    }
    glPopMatrix();
  }
}
