
#include "CompuFluidDyna.hpp"


// Standard lib
#include <cstdio>
#include <vector>

// GLUT lib
#include <GL/freeglut.h>

// Project lib
#include "../Data.hpp"
#include "../FileIO/FileInput.hpp"
#include "../Util/Colormap.hpp"
#include "../Util/Field.hpp"
#include "../Util/Timer.hpp"
#include "../Util/Vector.hpp"


extern Data D;

enum ParamType
{
  Scenario____________,
  ResolutionX_________,
  ResolutionY_________,
  ResolutionZ_________,
  TimeStepSize________,
  GaussSeiderIter_____,
  CoeffSmoke__________,
  CoeffVisco__________,
  CoeffForceX_________,
  CoeffForceY_________,
  CoeffForceZ_________,
  CoeffSource_________,
  ObstaclePosX________,
  ObstaclePosY________,
  ObstaclePosZ________,
  ObstacleSize________,
  ScaleFactor_________,
  ColorFactor_________,
  ColorThresh_________,
  SolvMode____________,
};


CompuFluidDyna::CompuFluidDyna() {
  D.param.clear();
  D.plotData.clear();
  isActiveProject= false;
  isInitialized= false;
  isRefreshed= false;
}


void CompuFluidDyna::SetActiveProject() {
  if (!isActiveProject) {
    D.param.push_back(ParamUI("Scenario____________", 0));
    D.param.push_back(ParamUI("ResolutionX_________", 1));
    D.param.push_back(ParamUI("ResolutionY_________", 200));
    D.param.push_back(ParamUI("ResolutionZ_________", 100));
    D.param.push_back(ParamUI("TimeStepSize________", 0.1));
    D.param.push_back(ParamUI("GaussSeiderIter_____", 20));
    D.param.push_back(ParamUI("CoeffSmoke__________", 0.0));
    D.param.push_back(ParamUI("CoeffVisco__________", 0.0));
    D.param.push_back(ParamUI("CoeffForceX_________", 0.0));
    D.param.push_back(ParamUI("CoeffForceY_________", 0.3));
    D.param.push_back(ParamUI("CoeffForceZ_________", 0.0));
    D.param.push_back(ParamUI("CoeffSource_________", 1.0));
    D.param.push_back(ParamUI("ObstaclePosX________", 0.5));
    D.param.push_back(ParamUI("ObstaclePosY________", 0.2));
    D.param.push_back(ParamUI("ObstaclePosZ________", 0.5));
    D.param.push_back(ParamUI("ObstacleSize________", 0.1));
    D.param.push_back(ParamUI("ScaleFactor_________", 1.0));
    D.param.push_back(ParamUI("ColorFactor_________", 1.0));
    D.param.push_back(ParamUI("ColorThresh_________", 0.0));
    D.param.push_back(ParamUI("SolvMode____________", 0.0));
  }

  isActiveProject= true;
  isInitialized= false;
  isRefreshed= false;
  Initialize();
}

void CompuFluidDyna::Initialize() {
  // Check if need to skip
  if (!isActiveProject) return;
  if (D.param[Scenario____________].hasChanged()) isInitialized= false;
  if (D.param[ResolutionX_________].hasChanged()) isInitialized= false;
  if (D.param[ResolutionY_________].hasChanged()) isInitialized= false;
  if (D.param[ResolutionZ_________].hasChanged()) isInitialized= false;
  if (isInitialized) return;
  isInitialized= true;

  // Get UI parameters
  nbX= std::max((int)std::round(D.param[ResolutionX_________].Get()), 1);
  nbY= std::max((int)std::round(D.param[ResolutionY_________].Get()), 1);
  nbZ= std::max((int)std::round(D.param[ResolutionZ_________].Get()), 1);

  // Allocate data
  Bound= Field::AllocField3D(nbX, nbY, nbZ, 0);
  Force= Field::AllocField3D(nbX, nbY, nbZ, 0);
  Sourc= Field::AllocField3D(nbX, nbY, nbZ, 0);
  SmokCur= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  VelXCur= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  VelYCur= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  VelZCur= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);

  // Force refresh
  isRefreshed= false;
  Refresh();
}


void CompuFluidDyna::Refresh() {
  if (isRefreshed) return;
  isRefreshed= true;

  int scenarioType= (int)std::round(D.param[Scenario____________].Get());

  if (scenarioType == 0 && loadedImage.empty())
    FileInput::LoadImageBMPFile("Resources/CFD_TeslaValveTwinSharp.bmp", loadedImage, false);

  // Initialize problem
  for (int x= 0; x < nbX; x++) {
    for (int y= 0; y < nbY; y++) {
      for (int z= 0; z < nbZ; z++) {
        Bound[x][y][z]= 0;
        Force[x][y][z]= 0;
        Sourc[x][y][z]= 0;

        if (scenarioType == 0) {
          // Add data from loaded image
          int idxPixelW= std::min(std::max(((int)loadedImage.size() - 1) * y / nbY, 0), (int)loadedImage.size() - 1);
          int idxPixelH= std::min(std::max(((int)loadedImage[0].size() - 1) * z / nbZ, 0), (int)loadedImage[0].size() - 1);
          std::array<float, 3> pixel= loadedImage[idxPixelW][idxPixelH];
          if (pixel[0] > 0.9f && pixel[1] > 0.9f && pixel[2] > 0.9f) {
            Bound[x][y][z]= 0;
            Force[x][y][z]= 0;
            Sourc[x][y][z]= 0;
          }
          else if (pixel[0] < 0.1f && pixel[1] < 0.1f && pixel[2] < 0.1f) {
            Bound[x][y][z]= 1;
            Force[x][y][z]= 0;
            Sourc[x][y][z]= 0;
          }
          else if (pixel[0] < 0.4f || pixel[0] > 0.6f) {
            Bound[x][y][z]= 0;
            Force[x][y][z]= (pixel[0] < 0.5f) ? (-1) : (1);
            Sourc[x][y][z]= (pixel[0] < 0.5f) ? (-1) : (1);
          }
          else if (pixel[1] < 0.4f || pixel[1] > 0.6f) {
            Bound[x][y][z]= -1;
            Force[x][y][z]= 0;
            Sourc[x][y][z]= 0;
          }
        }

        if (scenarioType == 1) {
          // Add Pac Man positive inlet
          {
            Vector::Vec3f posCell(((float)x + 0.5f) / (float)nbX, ((float)y + 0.5f) / (float)nbY, ((float)z + 0.5f) / (float)nbZ);
            Vector::Vec3f posObstacle(D.param[ObstaclePosX________].Get(), D.param[ObstaclePosY________].Get(), D.param[ObstaclePosZ________].Get());
            float refRadius= std::max((float)D.param[ObstacleSize________].Get(), 0.0f);
            if ((posCell - posObstacle).norm() <= refRadius) {
              Vector::Vec3f vecFlow(D.param[CoeffForceX_________].Get(), D.param[CoeffForceY_________].Get(), D.param[CoeffForceZ_________].Get());
              vecFlow.normalize();
              Bound[x][y][z]= 1;
              if ((posCell - posObstacle - vecFlow * 0.5f * refRadius).norm() <= refRadius * 0.8f) {
                Bound[x][y][z]= 0;
                if ((posCell - posObstacle - vecFlow * 0.5f * refRadius).norm() <= refRadius * 0.4f) {
                  Force[x][y][z]= 1;
                  Sourc[x][y][z]= 1;
                }
              }
            }
          }

          // Add Pac Man negative inlet
          {
            Vector::Vec3f posCell(((float)x + 0.5f) / (float)nbX, ((float)y + 0.5f) / (float)nbY, ((float)z + 0.5f) / (float)nbZ);
            Vector::Vec3f posObstacle(1.0f - D.param[ObstaclePosX________].Get(), 1.0f - D.param[ObstaclePosY________].Get(), 1.0f - D.param[ObstaclePosZ________].Get());
            float refRadius= std::max((float)D.param[ObstacleSize________].Get(), 0.0f);
            if ((posCell - posObstacle).norm() <= refRadius) {
              Vector::Vec3f vecFlow(D.param[CoeffForceX_________].Get(), D.param[CoeffForceY_________].Get(), D.param[CoeffForceZ_________].Get());
              vecFlow.normalize();
              vecFlow= -1.0 * vecFlow;
              Bound[x][y][z]= 1;
              if ((posCell - posObstacle - vecFlow * 0.5f * refRadius).norm() <= refRadius * 0.8f) {
                Bound[x][y][z]= 0;
                if ((posCell - posObstacle - vecFlow * 0.5f * refRadius).norm() <= refRadius * 0.4f) {
                  Force[x][y][z]= -1;
                  Sourc[x][y][z]= -1;
                }
              }
            }
          }
        }

        if (scenarioType == 2) {
          if (z < 2) {
            Bound[x][y][z]= 0;
            Force[x][y][z]= 1;
            Sourc[x][y][z]= 1;
          }
          if (z >= nbZ - 2) {
            Bound[x][y][z]= 0;
            Force[x][y][z]= -1;
            Sourc[x][y][z]= -1;
          }
          if (y < 2 || y >= nbY - 2) {
            Bound[x][y][z]= -1;
            Force[x][y][z]= 0;
            Sourc[x][y][z]= 0;
          }
        }

        // todo rework vortex shedding BC setup
        if (scenarioType == 3) {
          Vector::Vec3f posCell(((float)x + 0.5f) / (float)nbX, ((float)y + 0.5f) / (float)nbY, ((float)z + 0.5f) / (float)nbZ);
          Vector::Vec3f posObstacle(D.param[ObstaclePosX________].Get(), D.param[ObstaclePosY________].Get(), D.param[ObstaclePosZ________].Get());
          float refRadius= std::max((float)D.param[ObstacleSize________].Get(), 0.0f);
          if ((posCell - posObstacle).norm() <= refRadius) {
            Bound[x][y][z]= 1;
            Force[x][y][z]= 0;
            Sourc[x][y][z]= 0;
          }
          if (y < 2 || y >= nbY - 2 || z < 2 || z >= nbZ - 2) {
            Bound[x][y][z]= -1;
            Force[x][y][z]= 0;
            Sourc[x][y][z]= 0;
          }
          if (y >= 2 && y <= 6 && z >= 2 && z < nbZ - 2) {
            Bound[x][y][z]= 0;
            Force[x][y][z]= 0;
            if (z < (nbZ - 1) / 2) Sourc[x][y][z]= 1;
            if ((nbZ - 1) - z < (nbZ - 1) / 2) Sourc[x][y][z]= -1;
          }
          if (y >= 7 && y <= 8 && z >= 2 && z < nbZ - 2) {
            Bound[x][y][z]= 0;
            Force[x][y][z]= 1;
            Sourc[x][y][z]= 0;
          }
        }
      }
    }
  }
}


void CompuFluidDyna::Animate() {
  if (!isActiveProject) return;
  if (!isInitialized) return;
  if (!isRefreshed) return;

  isRefreshed= false;
  Refresh();

  const int maxIter= std::max((int)std::round(D.param[GaussSeiderIter_____].Get()), 0);
  const float timestep= D.param[TimeStepSize________].Get();
  const float coeffSmoke= D.param[CoeffSmoke__________].Get();
  const float coeffVisco= D.param[CoeffVisco__________].Get();

  // Simulate velocity step
  for (int x= 0; x < nbX; x++) {
    for (int y= 0; y < nbY; y++) {
      for (int z= 0; z < nbZ; z++) {
        if (Force[x][y][z] != 0) {
          VelXCur[x][y][z]= (float)Force[x][y][z] * D.param[CoeffForceX_________].Get();
          VelYCur[x][y][z]= (float)Force[x][y][z] * D.param[CoeffForceY_________].Get();
          VelZCur[x][y][z]= (float)Force[x][y][z] * D.param[CoeffForceZ_________].Get();
        }
      }
    }
  }
  GaussSeidelSolve(1, maxIter, timestep, true, coeffVisco, VelXCur);
  GaussSeidelSolve(2, maxIter, timestep, true, coeffVisco, VelYCur);
  GaussSeidelSolve(3, maxIter, timestep, true, coeffVisco, VelZCur);
  ProjectField(maxIter, timestep, VelXCur, VelYCur, VelZCur);
  std::vector<std::vector<std::vector<float>>> tmpVelX= VelXCur;
  std::vector<std::vector<std::vector<float>>> tmpVelY= VelYCur;
  std::vector<std::vector<std::vector<float>>> tmpVelZ= VelZCur;
  AdvectField(1, timestep, tmpVelX, tmpVelY, tmpVelZ, VelXCur);
  AdvectField(2, timestep, tmpVelX, tmpVelY, tmpVelZ, VelYCur);
  AdvectField(3, timestep, tmpVelX, tmpVelY, tmpVelZ, VelZCur);
  ProjectField(maxIter, timestep, VelXCur, VelYCur, VelZCur);

  // Simulate smoke step
  for (int x= 0; x < nbX; x++)
    for (int y= 0; y < nbY; y++)
      for (int z= 0; z < nbZ; z++)
        if (Sourc[x][y][z] != 0)
          SmokCur[x][y][z]= (float)Sourc[x][y][z] * D.param[CoeffSource_________].Get();
  GaussSeidelSolve(0, maxIter, timestep, true, coeffSmoke, SmokCur);
  AdvectField(0, timestep, VelXCur, VelYCur, VelZCur, SmokCur);

  // Plot field info
  float totVelX= 0.0f;
  float totVelY= 0.0f;
  float totVelZ= 0.0f;
  float totSmok= 0.0f;
  for (int x= 0; x < nbX; x++) {
    for (int y= 0; y < nbY; y++) {
      for (int z= 0; z < nbZ; z++) {
        totVelX+= VelXCur[x][y][z];
        totVelY+= VelYCur[x][y][z];
        totVelZ+= VelZCur[x][y][z];
        totSmok+= SmokCur[x][y][z];
      }
    }
  }
  D.plotData.resize(4);
  D.plotData[0].first= "TotVelX";
  D.plotData[1].first= "TotVelY";
  D.plotData[2].first= "TotVelZ";
  D.plotData[3].first= "TotSmok";
  D.plotData[0].second.push_back((double)totVelX);
  D.plotData[1].second.push_back((double)totVelY);
  D.plotData[2].second.push_back((double)totVelZ);
  D.plotData[3].second.push_back((double)totSmok);
}


void CompuFluidDyna::Draw() {
  if (!isActiveProject) return;
  if (!isInitialized) return;
  if (!isRefreshed) return;

  const int maxDim= std::max(std::max(nbX, nbY), nbZ);
  const float voxSize= 1.0f / (float)maxDim;

  // Draw the voxels
  if (D.displayMode1) {
    glEnable(GL_LIGHTING);
    glPushMatrix();
    glTranslatef(0.5f + 0.5f * voxSize - 0.5f * (float)nbX / (float)maxDim,
                 0.5f + 0.5f * voxSize - 0.5f * (float)nbY / (float)maxDim,
                 0.5f + 0.5f * voxSize - 0.5f * (float)nbZ / (float)maxDim);
    glScalef(voxSize, voxSize, voxSize);
    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          bool drawCube= false;
          if (Bound[x][y][z] != 0) {
            drawCube= true;
            if (Bound[x][y][z] > 0) glColor3f(0.5f, 0.5f, 0.5f);
            if (Bound[x][y][z] < 0) glColor3f(0.2f, 0.5f, 0.2f);
          }
          if (Force[x][y][z] != 0) {
            drawCube= true;
            if (Force[x][y][z] > 0) glColor3f(0.8f, 0.8f, 0.8f);
            if (Force[x][y][z] < 0) glColor3f(0.2f, 0.2f, 0.2f);
          }
          if (Sourc[x][y][z] != 0) {
            drawCube= true;
            if (Sourc[x][y][z] > 0) glColor3f(0.7f, 0.0f, 0.0f);
            if (Sourc[x][y][z] < 0) glColor3f(0.0f, 0.0f, 0.7f);
          }
          if (drawCube) {
            glPushMatrix();
            glTranslatef((float)x, (float)y, (float)z);
            glutSolidCube(1.0);
            glPopMatrix();
          }
        }
      }
    }
    glPopMatrix();
    glDisable(GL_LIGHTING);
  }

  // Draw the velocity field
  if (D.displayMode2) {
    glLineWidth(2.0f);
    glPushMatrix();
    glTranslatef(0.5f + 0.5f * voxSize - 0.5f * (float)nbX / (float)maxDim,
                 0.5f + 0.5f * voxSize - 0.5f * (float)nbY / (float)maxDim,
                 0.5f + 0.5f * voxSize - 0.5f * (float)nbZ / (float)maxDim);
    glScalef(voxSize, voxSize, voxSize);
    glBegin(GL_LINES);
    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          Vector::Vec3f vec(VelXCur[x][y][z], VelYCur[x][y][z], VelZCur[x][y][z]);
          if (vec.normSquared() > 0.0f) {
            float r= 0.0f, g= 0.0f, b= 0.0f;
            Colormap::RatioToJetBrightSmooth(vec.norm() * D.param[ColorFactor_________].Get(), r, g, b);
            glColor3f(r, g, b);
            Vector::Vec3f pos((float)x, (float)y, (float)z);
            glVertex3fv(pos.array());
            glVertex3fv(pos + vec.normalized() * D.param[ScaleFactor_________].Get());
            // glVertex3fv(pos + std::log(vec.norm() + 1.0f) * vec * D.param[ScaleFactor_________].Get());
          }
        }
      }
    }
    glEnd();
    glPopMatrix();
    glLineWidth(1.0f);
  }

  // Draw the scalar field
  if (D.displayMode3) {
    glPointSize(3.0f);
    glPushMatrix();
    glTranslatef(0.5f + 0.5f * voxSize - 0.5f * (float)nbX / (float)maxDim,
                 0.5f + 0.5f * voxSize - 0.5f * (float)nbY / (float)maxDim,
                 0.5f + 0.5f * voxSize - 0.5f * (float)nbZ / (float)maxDim);
    glScalef(voxSize, voxSize, voxSize);
    glBegin(GL_POINTS);
    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          if (std::abs(SmokCur[x][y][z]) < D.param[ColorThresh_________].Get()) continue;
          float r= 0.0f, g= 0.0f, b= 0.0f;
          Colormap::RatioToJetBrightSmooth(0.5f + 0.5f * SmokCur[x][y][z] * D.param[ColorFactor_________].Get(), r, g, b);
          glColor3f(r, g, b);
          glVertex3f((float)x, (float)y, (float)z);
        }
      }
    }
    glEnd();
    glPopMatrix();
    glPointSize(1.0f);
  }
}


constexpr int MaskSize= 6;
constexpr int Mask[MaskSize][3]=
    {{+1, 0, 0},
     {-1, 0, 0},
     {0, +1, 0},
     {0, -1, 0},
     {0, 0, +1},
     {0, 0, -1}};


void CompuFluidDyna::ApplyBC(const int iFieldID, std::vector<std::vector<std::vector<float>>>& ioField) {
  // Sweep through the field
  std::vector<std::vector<std::vector<float>>> oldField= ioField;
#pragma omp parallel for
  for (int x= 0; x < nbX; x++) {
    for (int y= 0; y < nbY; y++) {
      for (int z= 0; z < nbZ; z++) {
        // Find BC voxels
        if (Bound[x][y][z] == 0) continue;
        float sum= 0.0f;
        int count= 0;
        // Set BC voxel according to valid neighborhood values and flags
        for (int k= 0; k < MaskSize; k++) {
          if (iFieldID == 1 && k != 0 && k != 1) continue;  // todo check if good idea to use these skips
          if (iFieldID == 2 && k != 2 && k != 3) continue;
          if (iFieldID == 3 && k != 4 && k != 5) continue;
          if (x + Mask[k][0] < 0 || x + Mask[k][0] >= nbX) continue;
          if (y + Mask[k][1] < 0 || y + Mask[k][1] >= nbY) continue;
          if (z + Mask[k][2] < 0 || z + Mask[k][2] >= nbZ) continue;
          if (Bound[x + Mask[k][0]][y + Mask[k][1]][z + Mask[k][2]] > 0) continue;
          if (iFieldID != 0 && Bound[x][y][z] > 0)
            sum-= oldField[x + Mask[k][0]][y + Mask[k][1]][z + Mask[k][2]];
          else
            sum+= oldField[x + Mask[k][0]][y + Mask[k][1]][z + Mask[k][2]];
          count++;
        }
        if (count > 0)
          ioField[x][y][z]= sum / (float)count;
        else
          ioField[x][y][z]= 0.0f;
      }
    }
  }
}


void CompuFluidDyna::GaussSeidelSolve(const int iFieldID, const int iIter, const float iTimeStep,
                                      const bool iDiffuMode, const float iDiffuCoeff,
                                      std::vector<std::vector<std::vector<float>>>& ioField) {
  // Skip if non changing field
  if (iDiffuMode && iDiffuCoeff == 0.0f) return;
  const float diffuVal= iTimeStep * (float)(nbX * nbY * nbZ) * iDiffuCoeff;

  // Solve
  std::vector<std::vector<std::vector<float>>> oldField= ioField;
  for (int k= 0; k < iIter; k++) {
    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          // Work on active voxels
          if (Bound[x][y][z] > 0) continue;
          // Get count and sum of valid neighbors in current field for Gauss Seidel propagation
          int count= 0;
          float sum= 0.0f;
          for (int k= 0; k < MaskSize; k++) {
            if (x + Mask[k][0] < 0 || x + Mask[k][0] >= nbX) continue;
            if (y + Mask[k][1] < 0 || y + Mask[k][1] >= nbY) continue;
            if (z + Mask[k][2] < 0 || z + Mask[k][2] >= nbZ) continue;
            if (Bound[x + Mask[k][0]][y + Mask[k][1]][z + Mask[k][2]] > 0) continue;
            sum+= ioField[x + Mask[k][0]][y + Mask[k][1]][z + Mask[k][2]];
            count++;
          }
          // Set new value according to coefficients and flags
          if (iDiffuMode)
            ioField[x][y][z]= (oldField[x][y][z] + diffuVal * sum) / (1.0f + diffuVal * (float)count);
          else if (count > 0)
            ioField[x][y][z]= (oldField[x][y][z] + sum) / (float)count;
        }
      }
    }

    // Reapply BC to maintain consistency
    ApplyBC(iFieldID, ioField);
  }
}


float CompuFluidDyna::TrilinearInterpolation(
    const float iPosX, const float iPosY, const float iPosZ,
    const std::vector<std::vector<std::vector<float>>>& iFieldRef) {
  const int x0= std::min(std::max((int)std::floor(iPosX), 0), nbX - 1);
  const int y0= std::min(std::max((int)std::floor(iPosY), 0), nbY - 1);
  const int z0= std::min(std::max((int)std::floor(iPosZ), 0), nbZ - 1);
  const int x1= std::min(std::max((int)std::floor(iPosX) + 1, 0), nbX - 1);
  const int y1= std::min(std::max((int)std::floor(iPosY) + 1, 0), nbY - 1);
  const int z1= std::min(std::max((int)std::floor(iPosZ) + 1, 0), nbZ - 1);

  const float xWeight1= iPosX - (float)x0;
  const float yWeight1= iPosY - (float)y0;
  const float zWeight1= iPosZ - (float)z0;
  const float xWeight0= 1.0f - xWeight1;
  const float yWeight0= 1.0f - yWeight1;
  const float zWeight0= 1.0f - zWeight1;

  const float v000= iFieldRef[x0][y0][z0];
  const float v001= iFieldRef[x0][y0][z1];
  const float v010= iFieldRef[x0][y1][z0];
  const float v011= iFieldRef[x0][y1][z1];
  const float v100= iFieldRef[x1][y0][z0];
  const float v101= iFieldRef[x1][y0][z1];
  const float v110= iFieldRef[x1][y1][z0];
  const float v111= iFieldRef[x1][y1][z1];

  float val= 0.0f;
  val+= v000 * (xWeight0 * yWeight0 * zWeight0);
  val+= v001 * (xWeight0 * yWeight0 * zWeight1);
  val+= v010 * (xWeight0 * yWeight1 * zWeight0);
  val+= v011 * (xWeight0 * yWeight1 * zWeight1);
  val+= v100 * (xWeight1 * yWeight0 * zWeight0);
  val+= v101 * (xWeight1 * yWeight0 * zWeight1);
  val+= v110 * (xWeight1 * yWeight1 * zWeight0);
  val+= v111 * (xWeight1 * yWeight1 * zWeight1);

  return val;
}


void CompuFluidDyna::AdvectField(
    const int iFieldID, const float iTimeStep,
    const std::vector<std::vector<std::vector<float>>>& iVelX,
    const std::vector<std::vector<std::vector<float>>>& iVelY,
    const std::vector<std::vector<std::vector<float>>>& iVelZ,
    std::vector<std::vector<std::vector<float>>>& ioField) {
  // Sweep through the field
  std::vector<std::vector<std::vector<float>>> oldField= ioField;
#pragma omp parallel for
  for (int x= 0; x < nbX; x++) {
    for (int y= 0; y < nbY; y++) {
      for (int z= 0; z < nbZ; z++) {
        // Find source position for active voxel
        if (Bound[x][y][z] != 0) continue;
        float posX= (float)x - iTimeStep * (float)std::max(std::max(nbX, nbY), nbZ) * iVelX[x][y][z];
        float posY= (float)y - iTimeStep * (float)std::max(std::max(nbX, nbY), nbZ) * iVelY[x][y][z];
        float posZ= (float)z - iTimeStep * (float)std::max(std::max(nbX, nbY), nbZ) * iVelZ[x][y][z];
        // Trilinear interpolation
        // todo check validity of spatial localization
        ioField[x][y][z]= TrilinearInterpolation(posX, posY, posZ, oldField);
      }
    }
  }
  // Reapply BC to maintain consistency
  ApplyBC(iFieldID, ioField);
}


void CompuFluidDyna::ProjectField(const int iIter, const float iTimeStep,
                                  std::vector<std::vector<std::vector<float>>>& ioVelX,
                                  std::vector<std::vector<std::vector<float>>>& ioVelY,
                                  std::vector<std::vector<std::vector<float>>>& ioVelZ) {
  const float maxDim= (float)std::min(std::min(nbX, nbY), nbZ);
  // Compute divergence
  std::vector<std::vector<std::vector<float>>> Diver= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
#pragma omp parallel for
  for (int x= 0; x < nbX; x++) {
    for (int y= 0; y < nbY; y++) {
      for (int z= 0; z < nbZ; z++) {
        if (Bound[x][y][z] != 0) continue;
        float val= 0.0f;
        // todo check if need to handle asymmetric neighbors
        if (x - 1 >= 0 && x + 1 < nbX) val+= ioVelX[x + 1][y][z] - ioVelX[x - 1][y][z];
        if (y - 1 >= 0 && y + 1 < nbY) val+= ioVelY[x][y + 1][z] - ioVelY[x][y - 1][z];
        if (z - 1 >= 0 && z + 1 < nbZ) val+= ioVelZ[x][y][z + 1] - ioVelZ[x][y][z - 1];
        Diver[x][y][z]= -0.5f * val / maxDim;
      }
    }
  }

  // Reapply BC to maintain consistency
  ApplyBC(0, Diver);

  // Solve for pressure
  GaussSeidelSolve(0, iIter, iTimeStep, false, 0.0f, Diver);

  // Update velocities based on pressure
#pragma omp parallel for
  for (int x= 0; x < nbX; x++) {
    for (int y= 0; y < nbY; y++) {
      for (int z= 0; z < nbZ; z++) {
        if (Bound[x][y][z] != 0) continue;
        // todo check if need to handle asymmetric neighbors
        if (x - 1 >= 0 && x + 1 < nbX) ioVelX[x][y][z]-= 0.5f * maxDim * (Diver[x + 1][y][z] - Diver[x - 1][y][z]);
        if (y - 1 >= 0 && y + 1 < nbY) ioVelY[x][y][z]-= 0.5f * maxDim * (Diver[x][y + 1][z] - Diver[x][y - 1][z]);
        if (z - 1 >= 0 && z + 1 < nbZ) ioVelZ[x][y][z]-= 0.5f * maxDim * (Diver[x][y][z + 1] - Diver[x][y][z - 1]);
      }
    }
  }

  // Reapply BC to maintain consistency
  ApplyBC(1, ioVelX);
  ApplyBC(2, ioVelY);
  ApplyBC(3, ioVelZ);
}


// 3D CFD solver adapted from the following publication
// http://graphics.cs.cmu.edu/nsp/course/15-464/Fall09/papers/StamFluidforGames.pdf
// Copy of code for reference
// http://www.dgp.toronto.edu/people/stam/reality/Research/zip/CDROM_GDC03.zip
// https://www.dgp.toronto.edu/public_user/stam/reality/Research/pub.html

// #define IX(i,j) ((i)+(N+2)*(j))
// #define SWAP(x0,x) {float * tmp=x0;x0=x;x=tmp;}
// #define FOR_EACH_CELL for ( i=1 ; i<=N ; i++ ) { for ( j=1 ; j<=N ; j++ ) {
// #define END_FOR }}

// void add_source ( int N, float * x, float * s, float dt )
// {
// 	int i, size=(N+2)*(N+2);
// 	for ( i=0 ; i<size ; i++ ) x[i] += dt*s[i];
// }

// void set_bnd ( int N, int b, float * x )
// {
// 	int i;
// 	for ( i=1 ; i<=N ; i++ ) {
// 		x[IX(0  ,i)] = b==1 ? -x[IX(1,i)] : x[IX(1,i)];
// 		x[IX(N+1,i)] = b==1 ? -x[IX(N,i)] : x[IX(N,i)];
// 		x[IX(i,0  )] = b==2 ? -x[IX(i,1)] : x[IX(i,1)];
// 		x[IX(i,N+1)] = b==2 ? -x[IX(i,N)] : x[IX(i,N)];
// 	}
// 	x[IX(0  ,0  )] = 0.5f*(x[IX(1,0  )]+x[IX(0  ,1)]);
// 	x[IX(0  ,N+1)] = 0.5f*(x[IX(1,N+1)]+x[IX(0  ,N)]);
// 	x[IX(N+1,0  )] = 0.5f*(x[IX(N,0  )]+x[IX(N+1,1)]);
// 	x[IX(N+1,N+1)] = 0.5f*(x[IX(N,N+1)]+x[IX(N+1,N)]);
// }

// void lin_solve ( int N, int b, float * x, float * x0, float a, float c )
// {
// 	int i, j, k;
// 	for ( k=0 ; k<20 ; k++ ) {
// 		FOR_EACH_CELL
// 			x[IX(i,j)] = (x0[IX(i,j)] + a*(x[IX(i-1,j)]+x[IX(i+1,j)]+x[IX(i,j-1)]+x[IX(i,j+1)]))/c;
// 		END_FOR
// 		set_bnd ( N, b, x );
// 	}
// }

// void diffuse ( int N, int b, float * x, float * x0, float diff, float dt )
// {
// 	float a=dt*diff*N*N;
// 	lin_solve ( N, b, x, x0, a, 1+4*a );
// }

// void advect ( int N, int b, float * d, float * d0, float * u, float * v, float dt )
// {
// 	int i, j, i0, j0, i1, j1;
// 	float x, y, s0, t0, s1, t1, dt0;
// 	dt0 = dt*N;
// 	FOR_EACH_CELL
// 		x = i-dt0*u[IX(i,j)]; y = j-dt0*v[IX(i,j)];
// 		if (x<0.5f) x=0.5f; if (x>N+0.5f) x=N+0.5f; i0=(int)x; i1=i0+1;
// 		if (y<0.5f) y=0.5f; if (y>N+0.5f) y=N+0.5f; j0=(int)y; j1=j0+1;
// 		s1 = x-i0; s0 = 1-s1; t1 = y-j0; t0 = 1-t1;
// 		d[IX(i,j)] = s0*(t0*d0[IX(i0,j0)]+t1*d0[IX(i0,j1)])+
// 					 s1*(t0*d0[IX(i1,j0)]+t1*d0[IX(i1,j1)]);
// 	END_FOR
// 	set_bnd ( N, b, d );
// }

// void project ( int N, float * u, float * v, float * p, float * div )
// {
// 	int i, j;
// 	FOR_EACH_CELL
// 		div[IX(i,j)] = -0.5f*(u[IX(i+1,j)]-u[IX(i-1,j)]+v[IX(i,j+1)]-v[IX(i,j-1)])/N;
// 		p[IX(i,j)] = 0;
// 	END_FOR
// 	set_bnd ( N, 0, div ); set_bnd ( N, 0, p );
// 	lin_solve ( N, 0, p, div, 1, 4 );
// 	FOR_EACH_CELL
// 		u[IX(i,j)] -= 0.5f*N*(p[IX(i+1,j)]-p[IX(i-1,j)]);
// 		v[IX(i,j)] -= 0.5f*N*(p[IX(i,j+1)]-p[IX(i,j-1)]);
// 	END_FOR
// 	set_bnd ( N, 1, u ); set_bnd ( N, 2, v );
// }

// void vel_step ( int N, float * u, float * v, float * u0, float * v0, float visc, float dt )
// {
// 	add_source ( N, u, u0, dt ); add_source ( N, v, v0, dt );
// 	SWAP ( u0, u ); diffuse ( N, 1, u, u0, visc, dt );
// 	SWAP ( v0, v ); diffuse ( N, 2, v, v0, visc, dt );
// 	project ( N, u, v, u0, v0 );
// 	SWAP ( u0, u ); SWAP ( v0, v );
// 	advect ( N, 1, u, u0, u0, v0, dt ); advect ( N, 2, v, v0, u0, v0, dt );
// 	project ( N, u, v, u0, v0 );
// }

// void dens_step ( int N, float * x, float * x0, float * u, float * v, float diff, float dt )
// {
// 	add_source ( N, x, x0, dt );
// 	SWAP ( x0, x ); diffuse ( N, 0, x, x0, diff, dt );
// 	SWAP ( x0, x ); advect ( N, 0, x, x0, u, v, dt );
// }
