
#include "CompuFluidDyna.hpp"


// Standard lib
#include <cstdio>
#include <vector>

// GLUT lib
#include <GL/freeglut.h>

// Project lib
#include "../Data.hpp"
#include "../fileio/FileInput.hpp"
#include "../util/Colormap.hpp"
#include "../util/Field.hpp"
#include "../util/Vector.hpp"


extern Data D;

enum ParamType
{
  Scenario____________,
  ResolutionX_________,
  ResolutionY_________,
  ResolutionZ_________,
  TimeStepSize________,
  GaussSeiderIter_____,
  CoeffDiffusion______,
  CoeffViscosity______,
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
    D.param.push_back(ParamUI("ResolutionY_________", 100));
    D.param.push_back(ParamUI("ResolutionZ_________", 100));
    D.param.push_back(ParamUI("TimeStepSize________", 0.1));
    D.param.push_back(ParamUI("GaussSeiderIter_____", 20));
    D.param.push_back(ParamUI("CoeffDiffusion______", 0.0));
    D.param.push_back(ParamUI("CoeffViscosity______", 0.0));
    D.param.push_back(ParamUI("CoeffForceX_________", 0.0));
    D.param.push_back(ParamUI("CoeffForceY_________", 0.3));
    D.param.push_back(ParamUI("CoeffForceZ_________", 0.0));
    D.param.push_back(ParamUI("CoeffSource_________", 1.0));
    D.param.push_back(ParamUI("ObstaclePosX________", 0.5));
    D.param.push_back(ParamUI("ObstaclePosY________", 0.2));
    D.param.push_back(ParamUI("ObstaclePosZ________", 0.5));
    D.param.push_back(ParamUI("ObstacleSize________", 0.1));
    D.param.push_back(ParamUI("ScaleFactor_________", 100.0));
    D.param.push_back(ParamUI("ColorFactor_________", 1.0));
    D.param.push_back(ParamUI("ColorThresh_________", 0.0));
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
  DensCur= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
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

  int maxIter= std::max((int)std::round(D.param[GaussSeiderIter_____].Get()), 0);
  float timestep= D.param[TimeStepSize________].Get();
  float coeffDiffu= D.param[CoeffDiffusion______].Get();
  float coeffVisco= D.param[CoeffViscosity______].Get();

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
  DiffuseField(1, maxIter, timestep, coeffVisco, VelXCur);
  DiffuseField(2, maxIter, timestep, coeffVisco, VelYCur);
  DiffuseField(3, maxIter, timestep, coeffVisco, VelZCur);
  ProjectField(maxIter, VelXCur, VelYCur, VelZCur);
  std::vector<std::vector<std::vector<float>>> tmpVelX= VelXCur;
  std::vector<std::vector<std::vector<float>>> tmpVelY= VelYCur;
  std::vector<std::vector<std::vector<float>>> tmpVelZ= VelZCur;
  AdvectField(1, timestep, tmpVelX, tmpVelY, tmpVelZ, VelXCur);
  AdvectField(2, timestep, tmpVelX, tmpVelY, tmpVelZ, VelYCur);
  AdvectField(3, timestep, tmpVelX, tmpVelY, tmpVelZ, VelZCur);
  ProjectField(maxIter, VelXCur, VelYCur, VelZCur);

  // Simulate density step
  for (int x= 0; x < nbX; x++) {
    for (int y= 0; y < nbY; y++) {
      for (int z= 0; z < nbZ; z++) {
        if (Sourc[x][y][z] != 0) {
          DensCur[x][y][z]= (float)Sourc[x][y][z] * D.param[CoeffSource_________].Get();
        }
      }
    }
  }
  DiffuseField(0, maxIter, timestep, coeffDiffu, DensCur);
  AdvectField(0, timestep, VelXCur, VelYCur, VelZCur, DensCur);

  // Plot field info
  float totVelX= 0.0f;
  float totVelY= 0.0f;
  float totVelZ= 0.0f;
  float totDen= 0.0f;
  for (int x= 0; x < nbX; x++) {
    for (int y= 0; y < nbY; y++) {
      for (int z= 0; z < nbZ; z++) {
        totVelX+= VelXCur[x][y][z];
        totVelY+= VelYCur[x][y][z];
        totVelZ+= VelZCur[x][y][z];
        totDen+= DensCur[x][y][z];
      }
    }
  }
  D.plotData.resize(4);
  D.plotData[0].first= "TotVelX";
  D.plotData[1].first= "TotVelY";
  D.plotData[2].first= "TotVelZ";
  D.plotData[3].first= "TotDen";
  D.plotData[0].second.push_back((double)totVelX);
  D.plotData[1].second.push_back((double)totVelY);
  D.plotData[2].second.push_back((double)totVelZ);
  D.plotData[3].second.push_back((double)totDen);
}


void CompuFluidDyna::Draw() {
  if (!isActiveProject) return;
  if (!isInitialized) return;
  if (!isRefreshed) return;

  int maxDim= std::max(std::max(nbX, nbY), nbZ);
  float voxSize= 1.0 / (float)maxDim;

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
          if (vec.normSquared() > 0.0) {
            float r= 0.0f, g= 0.0f, b= 0.0f;
            Colormap::RatioToJetBrightSmooth(vec.norm() * D.param[ColorFactor_________].Get(), r, g, b);
            glColor3f(r, g, b);
            Vector::Vec3f pos((float)x, (float)y, (float)z);
            glVertex3fv(pos.array());
            glVertex3fv(pos + std::log(vec.norm() + 1.0f) * vec * D.param[ScaleFactor_________].Get());
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
          if (std::abs(DensCur[x][y][z]) < D.param[ColorThresh_________].Get()) continue;
          float r= 0.0f, g= 0.0f, b= 0.0f;
          Colormap::RatioToJetBrightSmooth(0.5f + 0.5f * DensCur[x][y][z] * D.param[ColorFactor_________].Get(), r, g, b);
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


const int MaskSize= 6;
const int Mask[MaskSize][3]=
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
          float val= oldField[x + Mask[k][0]][y + Mask[k][1]][z + Mask[k][2]];
          if (iFieldID != 0 && Bound[x][y][z] > 0)
            val= -val;
          sum+= val;
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


void CompuFluidDyna::GaussSeidelSolve(const int iFieldID,
                                      const int iIter, const bool iAdvancedMode, const float iMultip,
                                      std::vector<std::vector<std::vector<float>>>& ioField) {
  // Skip if non changing field
  if (iAdvancedMode && iMultip == 0.0f) return;

  // Solve
  std::vector<std::vector<std::vector<float>>> refField= ioField;
  for (int k= 0; k < iIter; k++) {
    std::vector<std::vector<std::vector<float>>> FieldA= ioField;
    std::vector<std::vector<std::vector<float>>> FieldB= ioField;
#pragma omp parallel sections
    {
#pragma omp section
      {
        // Forward pass
        for (int x= 0; x < nbX; x++) {
          for (int y= 0; y < nbY; y++) {
            for (int z= 0; z < nbZ; z++) {
              // Work on active voxels
              if (Bound[x][y][z] > 0) continue;
              int count= 0;
              float sum= 0.0f;
              // Get count and sum of valid neighbors in current field for Gauss Seidel propagation
              for (int k= 0; k < MaskSize; k++) {
                if (iFieldID == 1 && k != 0 && k != 1) continue;  // todo check if good idea to use these skips
                if (iFieldID == 2 && k != 2 && k != 3) continue;
                if (iFieldID == 3 && k != 4 && k != 5) continue;
                if (x + Mask[k][0] < 0 || x + Mask[k][0] >= nbX) continue;
                if (y + Mask[k][1] < 0 || y + Mask[k][1] >= nbY) continue;
                if (z + Mask[k][2] < 0 || z + Mask[k][2] >= nbZ) continue;
                if (Bound[x + Mask[k][0]][y + Mask[k][1]][z + Mask[k][2]] > 0) continue;
                sum+= FieldA[x + Mask[k][0]][y + Mask[k][1]][z + Mask[k][2]];
                count++;
              }
              // Set new value according to coefficients and flags
              if (iAdvancedMode)
                FieldA[x][y][z]= (refField[x][y][z] + iMultip * sum) / (1.0f + iMultip * (float)count);
              else if (count > 0)
                FieldA[x][y][z]= (refField[x][y][z] + sum) / (float)count;
            }
          }
        }
      }
#pragma omp section
      {
        // Backward pass
        for (int x= nbX - 1; x >= 0; x--) {
          for (int y= nbY - 1; y >= 0; y--) {
            for (int z= nbZ - 1; z >= 0; z--) {
              // Work on active voxels
              if (Bound[x][y][z] > 0) continue;
              int count= 0;
              float sum= 0.0f;
              // Get count and sum of valid neighbors in current field for Gauss Seidel propagation
              for (int k= 0; k < MaskSize; k++) {
                if (iFieldID == 1 && k != 0 && k != 1) continue;  // todo check if good idea to use these skips
                if (iFieldID == 2 && k != 2 && k != 3) continue;
                if (iFieldID == 3 && k != 4 && k != 5) continue;
                if (x + Mask[k][0] < 0 || x + Mask[k][0] >= nbX) continue;
                if (y + Mask[k][1] < 0 || y + Mask[k][1] >= nbY) continue;
                if (z + Mask[k][2] < 0 || z + Mask[k][2] >= nbZ) continue;
                if (Bound[x + Mask[k][0]][y + Mask[k][1]][z + Mask[k][2]] > 0) continue;
                sum+= FieldB[x + Mask[k][0]][y + Mask[k][1]][z + Mask[k][2]];
                count++;
              }
              // Set new value according to coefficients and flags
              if (iAdvancedMode)
                FieldB[x][y][z]= (refField[x][y][z] + iMultip * sum) / (1.0f + iMultip * (float)count);
              else if (count > 0)
                FieldB[x][y][z]= (refField[x][y][z] + sum) / (float)count;
            }
          }
        }
      }
    }

    // Recombine forward and backward passes
    for (int x= 0; x < nbX; x++)
      for (int y= 0; y < nbY; y++)
        for (int z= 0; z < nbZ; z++)
          ioField[x][y][z]= 0.5f * (FieldA[x][y][z] + FieldB[x][y][z]);

    // Reapply BC to maintain consistency
    ApplyBC(iFieldID, ioField);
  }
}


void CompuFluidDyna::DiffuseField(const int iFieldID, const int iIter, const float iTimeStep, const float iDiffusionCoeff,
                                  std::vector<std::vector<std::vector<float>>>& ioField) {
  float multip= iTimeStep * (float)(nbX * nbY * nbZ) * iDiffusionCoeff;
  GaussSeidelSolve(iFieldID, iIter, true, multip, ioField);
}


float CompuFluidDyna::TrilinearInterpolation(
    const float iPosX, const float iPosY, const float iPosZ,
    const std::vector<std::vector<std::vector<float>>>& iFieldRef) {
  int x0= std::min(std::max((int)std::floor(iPosX), 0), nbX - 1);
  int y0= std::min(std::max((int)std::floor(iPosY), 0), nbY - 1);
  int z0= std::min(std::max((int)std::floor(iPosZ), 0), nbZ - 1);
  int x1= std::min(std::max((int)std::floor(iPosX) + 1, 0), nbX - 1);
  int y1= std::min(std::max((int)std::floor(iPosY) + 1, 0), nbY - 1);
  int z1= std::min(std::max((int)std::floor(iPosZ) + 1, 0), nbZ - 1);

  float xWeight1= iPosX - (float)x0;
  float yWeight1= iPosY - (float)y0;
  float zWeight1= iPosZ - (float)z0;
  float xWeight0= 1.0f - xWeight1;
  float yWeight0= 1.0f - yWeight1;
  float zWeight0= 1.0f - zWeight1;

  float v000= iFieldRef[x0][y0][z0];
  float v001= iFieldRef[x0][y0][z1];
  float v010= iFieldRef[x0][y1][z0];
  float v011= iFieldRef[x0][y1][z1];
  float v100= iFieldRef[x1][y0][z0];
  float v101= iFieldRef[x1][y0][z1];
  float v110= iFieldRef[x1][y1][z0];
  float v111= iFieldRef[x1][y1][z1];

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


void CompuFluidDyna::ProjectField(const int iIter,
                                  std::vector<std::vector<std::vector<float>>>& ioVelX,
                                  std::vector<std::vector<std::vector<float>>>& ioVelY,
                                  std::vector<std::vector<std::vector<float>>>& ioVelZ) {
  float maxDim= (float)std::min(std::min(nbX, nbY), nbZ);
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
  GaussSeidelSolve(0, iIter, false, 1, Diver);

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
