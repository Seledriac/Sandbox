
#include "CompuFluidDyn.hpp"


// Standard lib
#include <cstdio>
#include <vector>

// GLUT lib
#include <GL/freeglut.h>

// Project lib
#include "../Data.hpp"
#include "../fileio/FileInput.hpp"
#include "../math/Vectors.hpp"
#include "../util/Colormap.hpp"
#include "../util/Field.hpp"


const int MaskSize= 6;
const int Mask[MaskSize][3]=
    {{+1, 0, 0},
     {-1, 0, 0},
     {0, +1, 0},
     {0, -1, 0},
     {0, 0, +1},
     {0, 0, -1}};


void CompuFluidDyn::AddSource(
    const std::vector<std::vector<std::vector<float>>>& iSource, const float iTimestep,
    std::vector<std::vector<std::vector<float>>>& ioField) {
#pragma omp parallel for
  for (int x= 0; x < nbX; x++)
    for (int y= 0; y < nbY; y++)
      for (int z= 0; z < nbZ; z++)
        ioField[x][y][z]+= iTimestep * iSource[x][y][z];
}


void CompuFluidDyn::ApplyBC(
    const std::vector<std::vector<std::vector<int>>>& iType, const bool iMirror,
    std::vector<std::vector<std::vector<float>>>& ioField) {
  // Sweep through the field
  std::vector<std::vector<std::vector<float>>> oldField= ioField;
#pragma omp parallel for
  for (int x= 0; x < nbX; x++) {
    for (int y= 0; y < nbY; y++) {
      for (int z= 0; z < nbZ; z++) {
        // Find BC voxels
        if (iType[x][y][z] == 0) continue;
        float sum= 0.0f;
        int count= 0;
        // Set BC voxel according to valid neighborhood values and flags
        for (int k= 0; k < MaskSize; k++) {
          if (x + Mask[k][0] < 0 || x + Mask[k][0] >= nbX) continue;
          if (y + Mask[k][1] < 0 || y + Mask[k][1] >= nbY) continue;
          if (z + Mask[k][2] < 0 || z + Mask[k][2] >= nbZ) continue;
          if (iType[x + Mask[k][0]][y + Mask[k][1]][z + Mask[k][2]] > 0) continue;
          float val= oldField[x + Mask[k][0]][y + Mask[k][1]][z + Mask[k][2]];
          if (iMirror && iType[x][y][z] > 0)
            val= -val;
          sum+= val;
          count++;
        }
        if (count > 0)
          ioField[x][y][z]= sum / float(count);
      }
    }
  }
}


void CompuFluidDyn::GaussSeidelSolve(
    const std::vector<std::vector<std::vector<int>>>& iType, const bool iMirror,
    const int iIter, const bool iAdvancedMode, const float iMultip,
    std::vector<std::vector<std::vector<float>>>& ioField) {
  if (iAdvancedMode && iMultip == 0.0f) return;
  // Sweep through the field
  std::vector<std::vector<std::vector<float>>> oldField= ioField;
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
              if (iType[x][y][z] > 0) continue;
              int count= 0;
              float sum= 0.0f;
              // Get count and sum of valid neighbors in current field for Gauss Seidel propagation
              for (int k= 0; k < MaskSize; k++) {
                if (x + Mask[k][0] < 0 || x + Mask[k][0] >= nbX) continue;
                if (y + Mask[k][1] < 0 || y + Mask[k][1] >= nbY) continue;
                if (z + Mask[k][2] < 0 || z + Mask[k][2] >= nbZ) continue;
                if (iType[x + Mask[k][0]][y + Mask[k][1]][z + Mask[k][2]] > 0) continue;
                sum+= FieldA[x + Mask[k][0]][y + Mask[k][1]][z + Mask[k][2]];
                count++;
              }
              // Set new value according to coefficients and flags
              if (iAdvancedMode)
                FieldA[x][y][z]= (oldField[x][y][z] + iMultip * sum) / (1.0f + iMultip * float(count));
              else {
                if (count > 0)
                  FieldA[x][y][z]= (oldField[x][y][z] + sum) / float(count);
              }
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
              if (iType[x][y][z] > 0) continue;
              int count= 0;
              float sum= 0.0f;
              // Get count and sum of valid neighbors in current field for Gauss Seidel propagation
              for (int k= 0; k < MaskSize; k++) {
                if (x + Mask[k][0] < 0 || x + Mask[k][0] >= nbX) continue;
                if (y + Mask[k][1] < 0 || y + Mask[k][1] >= nbY) continue;
                if (z + Mask[k][2] < 0 || z + Mask[k][2] >= nbZ) continue;
                if (iType[x + Mask[k][0]][y + Mask[k][1]][z + Mask[k][2]] > 0) continue;
                sum+= FieldB[x + Mask[k][0]][y + Mask[k][1]][z + Mask[k][2]];
                count++;
              }
              // Set new value according to coefficients and flags
              if (iAdvancedMode)
                FieldB[x][y][z]= (oldField[x][y][z] + iMultip * sum) / (1.0f + iMultip * float(count));
              else {
                if (count > 0)
                  FieldB[x][y][z]= (oldField[x][y][z] + sum) / float(count);
              }
            }
          }
        }
      }

      // Recombine forward and backward passes
      for (int x= 0; x < nbX; x++)
        for (int y= 0; y < nbY; y++)
          for (int z= 0; z < nbZ; z++)
            ioField[x][y][z]= 0.5 * (FieldA[x][y][z] + FieldB[x][y][z]);

      // Reapply BC to maintain consistency
      ApplyBC(iType, iMirror, ioField);
    }
  }
}


void CompuFluidDyn::DiffuseField(
    const std::vector<std::vector<std::vector<int>>>& iType, const bool iMirror,
    const int iIter, const float iTimeStep, const float iDiffusionCoeff,
    std::vector<std::vector<std::vector<float>>>& ioField) {
  float multip= iTimeStep * float(nbX * nbY * nbZ) * iDiffusionCoeff;
  GaussSeidelSolve(iType, iMirror, iIter, true, multip, ioField);
}


float CompuFluidDyn::TrilinearInterpolation(
    const float iPosX, const float iPosY, const float iPosZ,
    const std::vector<std::vector<std::vector<float>>>& iFieldRef) {
  int x0= std::min(std::max(int(std::floor(iPosX)), 0), nbX - 1);
  int y0= std::min(std::max(int(std::floor(iPosY)), 0), nbY - 1);
  int z0= std::min(std::max(int(std::floor(iPosZ)), 0), nbZ - 1);
  int x1= std::min(std::max(int(std::floor(iPosX)) + 1, 0), nbX - 1);
  int y1= std::min(std::max(int(std::floor(iPosY)) + 1, 0), nbY - 1);
  int z1= std::min(std::max(int(std::floor(iPosZ)) + 1, 0), nbZ - 1);

  float xWeight1= iPosX - float(x0);
  float yWeight1= iPosY - float(y0);
  float zWeight1= iPosZ - float(z0);
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


void CompuFluidDyn::AdvectField(
    const std::vector<std::vector<std::vector<int>>>& iType,
    const bool iMirror, const float iTimeStep,
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
        if (iType[x][y][z] != 0) continue;
        float posX= float(x) - iTimeStep * float(std::max(std::max(nbX, nbY), nbZ)) * iVelX[x][y][z];
        float posY= float(y) - iTimeStep * float(std::max(std::max(nbX, nbY), nbZ)) * iVelY[x][y][z];
        float posZ= float(z) - iTimeStep * float(std::max(std::max(nbX, nbY), nbZ)) * iVelZ[x][y][z];
        // Trilinear interpolation
        ioField[x][y][z]= TrilinearInterpolation(posX, posY, posZ, oldField);
      }
    }
  }
  // Reapply BC to maintain consistency
  ApplyBC(iType, iMirror, ioField);
}


void CompuFluidDyn::ProjectField(
    const std::vector<std::vector<std::vector<int>>>& iType, const int iIter,
    std::vector<std::vector<std::vector<float>>>& ioVelX,
    std::vector<std::vector<std::vector<float>>>& ioVelY,
    std::vector<std::vector<std::vector<float>>>& ioVelZ) {
  float maxDim= float(std::max(std::max(nbX, nbY), nbZ));
  // Compute divergence
  std::vector<std::vector<std::vector<float>>> Diver= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
#pragma omp parallel for
  for (int x= 0; x < nbX; x++) {
    for (int y= 0; y < nbY; y++) {
      for (int z= 0; z < nbZ; z++) {
        if (iType[x][y][z] != 0) continue;
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
  ApplyBC(iType, false, Diver);

  // Solve for pressure
  GaussSeidelSolve(iType, false, iIter, false, 1, Diver);

  // Update velocities based on pressure
#pragma omp parallel for
  for (int x= 0; x < nbX; x++) {
    for (int y= 0; y < nbY; y++) {
      for (int z= 0; z < nbZ; z++) {
        if (iType[x][y][z] != 0) continue;
        // todo check if need to handle asymmetric neighbors
        if (x - 1 >= 0 && x + 1 < nbX) ioVelX[x][y][z]-= 0.5f * maxDim * (Diver[x + 1][y][z] - Diver[x - 1][y][z]);
        if (y - 1 >= 0 && y + 1 < nbY) ioVelY[x][y][z]-= 0.5f * maxDim * (Diver[x][y + 1][z] - Diver[x][y - 1][z]);
        if (z - 1 >= 0 && z + 1 < nbZ) ioVelZ[x][y][z]-= 0.5f * maxDim * (Diver[x][y][z + 1] - Diver[x][y][z - 1]);
      }
    }
  }
  // Reapply BC to maintain consistency
  ApplyBC(iType, true, ioVelX);
  ApplyBC(iType, true, ioVelY);
  ApplyBC(iType, true, ioVelZ);
}


void CompuFluidDyn::DensityStep(
    const std::vector<std::vector<std::vector<int>>>& iType,
    const int iIter, const float iTimeStep, const float iDiffusionCoeff,
    const std::vector<std::vector<std::vector<float>>>& iVelX,
    const std::vector<std::vector<std::vector<float>>>& iVelY,
    const std::vector<std::vector<std::vector<float>>>& iVelZ,
    const std::vector<std::vector<std::vector<float>>>& ioDensAdd,
    std::vector<std::vector<std::vector<float>>>& ioDensCur) {
  std::vector<std::vector<std::vector<float>>> tmp;
  AddSource(ioDensAdd, iTimeStep, ioDensCur);
  DiffuseField(iType, false, iIter, iTimeStep, iDiffusionCoeff, ioDensCur);
  AdvectField(iType, false, iTimeStep, iVelX, iVelY, iVelZ, ioDensCur);
}


void CompuFluidDyn::VelocityStep(
    const std::vector<std::vector<std::vector<int>>>& iType,
    const int iIter, const float iTimeStep, const float iDiffusionCoeff,
    const std::vector<std::vector<std::vector<float>>>& iVelXAdd,
    const std::vector<std::vector<std::vector<float>>>& iVelYAdd,
    const std::vector<std::vector<std::vector<float>>>& iVelZAdd,
    std::vector<std::vector<std::vector<float>>>& ioVelXCur,
    std::vector<std::vector<std::vector<float>>>& ioVelYCur,
    std::vector<std::vector<std::vector<float>>>& ioVelZCur) {
  std::vector<std::vector<std::vector<float>>> tmp;
  AddSource(iVelXAdd, iTimeStep, ioVelXCur);
  AddSource(iVelYAdd, iTimeStep, ioVelYCur);
  AddSource(iVelZAdd, iTimeStep, ioVelZCur);
  DiffuseField(iType, true, iIter, iTimeStep, iDiffusionCoeff, ioVelXCur);
  DiffuseField(iType, true, iIter, iTimeStep, iDiffusionCoeff, ioVelYCur);
  DiffuseField(iType, true, iIter, iTimeStep, iDiffusionCoeff, ioVelZCur);
  ProjectField(iType, iIter, ioVelXCur, ioVelYCur, ioVelZCur);
  std::vector<std::vector<std::vector<float>>> tmpVelX= ioVelXCur;
  std::vector<std::vector<std::vector<float>>> tmpVelY= ioVelYCur;
  std::vector<std::vector<std::vector<float>>> tmpVelZ= ioVelZCur;
  AdvectField(iType, true, iTimeStep, tmpVelX, tmpVelY, tmpVelZ, ioVelXCur);
  AdvectField(iType, true, iTimeStep, tmpVelX, tmpVelY, tmpVelZ, ioVelYCur);
  AdvectField(iType, true, iTimeStep, tmpVelX, tmpVelY, tmpVelZ, ioVelZCur);
  ProjectField(iType, iIter, ioVelXCur, ioVelYCur, ioVelZCur);
}


extern Data D;

enum ParamType
{
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
  DisplayUpsampling___,
  Scenario____________,
};


CompuFluidDyn::CompuFluidDyn() {
  isInitialized= false;
  isRefreshed= false;
}


CompuFluidDyn::~CompuFluidDyn() {
  isInitialized= false;
  isRefreshed= false;
}


void CompuFluidDyn::Init() {
  isRefreshed= false;
  if (isInitialized) return;

  if (D.param.empty()) {
    D.param.push_back(ParamUI("ResolutionX_________", 1));
    D.param.push_back(ParamUI("ResolutionY_________", 100));
    D.param.push_back(ParamUI("ResolutionZ_________", 100));
    D.param.push_back(ParamUI("TimeStepSize________", 0.1));
    D.param.push_back(ParamUI("GaussSeiderIter_____", 10));
    D.param.push_back(ParamUI("CoeffDiffusion______", 0.0));
    D.param.push_back(ParamUI("CoeffViscosity______", 0.0));
    D.param.push_back(ParamUI("CoeffForceX_________", 0.0));
    D.param.push_back(ParamUI("CoeffForceY_________", 1.0));
    D.param.push_back(ParamUI("CoeffForceZ_________", 0.0));
    D.param.push_back(ParamUI("CoeffSource_________", 1.0));
    D.param.push_back(ParamUI("ObstaclePosX________", 0.5));
    D.param.push_back(ParamUI("ObstaclePosY________", 0.2));
    D.param.push_back(ParamUI("ObstaclePosZ________", 0.5));
    D.param.push_back(ParamUI("ObstacleSize________", 0.1));
    D.param.push_back(ParamUI("ScaleFactor_________", 10.0));
    D.param.push_back(ParamUI("ColorFactor_________", 1.0));
    D.param.push_back(ParamUI("ColorThresh_________", 0.2));
    D.param.push_back(ParamUI("DisplayUpsampling___", 1.0));
    D.param.push_back(ParamUI("Scenario____________", 0.0));
  }

  isInitialized= true;
}


void CompuFluidDyn::CheckNeedRefresh() {
  if (nbX != std::max(int(std::round(D.param[ResolutionX_________].val)), 1)) isRefreshed= false;
  if (nbY != std::max(int(std::round(D.param[ResolutionY_________].val)), 1)) isRefreshed= false;
  if (nbZ != std::max(int(std::round(D.param[ResolutionZ_________].val)), 1)) isRefreshed= false;

  nbX= std::max(int(std::round(D.param[ResolutionX_________].val)), 1);
  nbY= std::max(int(std::round(D.param[ResolutionY_________].val)), 1);
  nbZ= std::max(int(std::round(D.param[ResolutionZ_________].val)), 1);
}


void CompuFluidDyn::Refresh() {
  if (!isInitialized) return;
  CheckNeedRefresh();
  if (isRefreshed) return;

  Solid= Field::AllocField3D(nbX, nbY, nbZ, 0);
  Force= Field::AllocField3D(nbX, nbY, nbZ, 0);
  Sourc= Field::AllocField3D(nbX, nbY, nbZ, 0);
  DensAdd= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  DensCur= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  VelXAdd= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  VelYAdd= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  VelZAdd= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  VelXCur= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  VelYCur= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  VelZCur= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);

  isRefreshed= true;

  CompuFluidDyn::Animate();
}


void CompuFluidDyn::Animate() {
  if (!isInitialized) return;
  if (!isRefreshed) return;

  if (int(std::round(D.param[Scenario____________].val)) == 0) {
    if (loadedImage.empty())
      FileInput::LoadImageBMPFile("Resources/CFD_TeslaValveTwinSharp.bmp", loadedImage, false);
  }

  // Initialize problem
  for (int x= 0; x < nbX; x++) {
    for (int y= 0; y < nbY; y++) {
      for (int z= 0; z < nbZ; z++) {
        Solid[x][y][z]= 0;
        Force[x][y][z]= 0;
        Sourc[x][y][z]= 0;

        if (int(std::round(D.param[Scenario____________].val)) == 0) {
          // Add data from loaded image
          int idxPixelW= std::min(std::max((int(loadedImage.size()) - 1) * y / nbY, 0), int(loadedImage.size()) - 1);
          int idxPixelH= std::min(std::max((int(loadedImage[0].size()) - 1) * z / nbZ, 0), int(loadedImage[0].size()) - 1);
          std::array<float, 4> pixel= loadedImage[idxPixelW][idxPixelH];
          if (pixel[0] > 0.9f && pixel[1] > 0.9f && pixel[2] > 0.9f) {
            Solid[x][y][z]= 0;
            Force[x][y][z]= 0;
            Sourc[x][y][z]= 0;
          }
          else if (pixel[0] < 0.1f && pixel[1] < 0.1f && pixel[2] < 0.1f) {
            Solid[x][y][z]= 1;
            Force[x][y][z]= 0;
            Sourc[x][y][z]= 0;
          }
          else if (pixel[0] < 0.4f || pixel[0] > 0.6f) {
            Solid[x][y][z]= 0;
            Force[x][y][z]= (pixel[0] < 0.5f) ? (-1) : (1);
            Sourc[x][y][z]= (pixel[0] < 0.5f) ? (-1) : (1);
          }
          else if (pixel[1] < 0.4f || pixel[1] > 0.6f) {
            Solid[x][y][z]= -1;
            Force[x][y][z]= 0;
            Sourc[x][y][z]= 0;
          }
        }

        if (int(std::round(D.param[Scenario____________].val)) == 1) {
          // Add Pac Man positive inlet
          {
            Math::Vec3f posCell((float(x) + 0.5f) / float(nbX), (float(y) + 0.5f) / float(nbY), (float(z) + 0.5f) / float(nbZ));
            Math::Vec3f posObstacle(D.param[ObstaclePosX________].val, D.param[ObstaclePosY________].val, D.param[ObstaclePosZ________].val);
            double refRadius= std::max(D.param[ObstacleSize________].val, 0.0);
            if ((posCell - posObstacle).norm() <= refRadius) {
              Math::Vec3f vecFlow(D.param[CoeffForceX_________].val, D.param[CoeffForceY_________].val, D.param[CoeffForceZ_________].val);
              vecFlow.normalize();
              Solid[x][y][z]= 1;
              if ((posCell - posObstacle - vecFlow * 0.5 * refRadius).norm() <= refRadius * 0.8) {
                Solid[x][y][z]= 0;
                if ((posCell - posObstacle - vecFlow * 0.5 * refRadius).norm() <= refRadius * 0.4) {
                  Force[x][y][z]= 1;
                  Sourc[x][y][z]= 1;
                }
              }
            }
          }

          // Add Pac Man negative inlet
          {
            Math::Vec3f posCell((float(x) + 0.5f) / float(nbX), (float(y) + 0.5f) / float(nbY), (float(z) + 0.5f) / float(nbZ));
            Math::Vec3f posObstacle(1.0 - D.param[ObstaclePosX________].val, 1.0 - D.param[ObstaclePosY________].val, 1.0 - D.param[ObstaclePosZ________].val);
            double refRadius= std::max(D.param[ObstacleSize________].val, 0.0);
            if ((posCell - posObstacle).norm() <= refRadius) {
              Math::Vec3f vecFlow(D.param[CoeffForceX_________].val, D.param[CoeffForceY_________].val, D.param[CoeffForceZ_________].val);
              vecFlow.normalize();
              vecFlow= -1.0 * vecFlow;
              Solid[x][y][z]= 1;
              if ((posCell - posObstacle - vecFlow * 0.5 * refRadius).norm() <= refRadius * 0.8) {
                Solid[x][y][z]= 0;
                if ((posCell - posObstacle - vecFlow * 0.5 * refRadius).norm() <= refRadius * 0.4) {
                  Force[x][y][z]= -1;
                  Sourc[x][y][z]= -1;
                }
              }
            }
          }
        }

        if (int(std::round(D.param[Scenario____________].val)) == 2) {
          if (z < 2) {
            Solid[x][y][z]= 0;
            Force[x][y][z]= 1;
            Sourc[x][y][z]= 1;
          }
          if (z >= nbZ - 2) {
            Solid[x][y][z]= 0;
            Force[x][y][z]= -1;
            Sourc[x][y][z]= -1;
          }
          if (y < 2 || y >= nbY - 2) {
            Solid[x][y][z]= -1;
            Force[x][y][z]= 0;
            Sourc[x][y][z]= 0;
          }
        }

        if (int(std::round(D.param[Scenario____________].val)) == 3) {
          Math::Vec3f posCell((float(x) + 0.5f) / float(nbX), (float(y) + 0.5f) / float(nbY), (float(z) + 0.5f) / float(nbZ));
          Math::Vec3f posObstacle(D.param[ObstaclePosX________].val, D.param[ObstaclePosY________].val, D.param[ObstaclePosZ________].val);
          double refRadius= std::max(D.param[ObstacleSize________].val, 0.0);
          if ((posCell - posObstacle).norm() <= refRadius) {
            Solid[x][y][z]= 1;
            Force[x][y][z]= 0;
            Sourc[x][y][z]= 0;
          }
          if (y < 2 || y >= nbY - 2 || z < 2 || z >= nbZ - 2) {
            Solid[x][y][z]= -1;
            Force[x][y][z]= 0;
            Sourc[x][y][z]= 0;
          }
          else if (y >= 2 && y <= 6) {
            Solid[x][y][z]= 0;
            Force[x][y][z]= 0;
            if (z < (nbZ - 1) / 2) Sourc[x][y][z]= 1;
            if ((nbZ - 1) - z < (nbZ - 1) / 2) Sourc[x][y][z]= -1;
          }
          else if (y >= 7 && y <= 8) {
            Solid[x][y][z]= 0;
            Force[x][y][z]= 1;
            Sourc[x][y][z]= 0;
          }
        }
      }
    }
  }

  for (int x= 0; x < nbX; x++) {
    for (int y= 0; y < nbY; y++) {
      for (int z= 0; z < nbZ; z++) {
        VelXAdd[x][y][z]= 0.0f;
        VelYAdd[x][y][z]= 0.0f;
        VelZAdd[x][y][z]= 0.0f;
        DensAdd[x][y][z]= 0.0f;
        if (Force[x][y][z] != 0) {
          VelXAdd[x][y][z]= float(Force[x][y][z]) * D.param[CoeffForceX_________].val;
          VelYAdd[x][y][z]= float(Force[x][y][z]) * D.param[CoeffForceY_________].val;
          VelZAdd[x][y][z]= float(Force[x][y][z]) * D.param[CoeffForceZ_________].val;
        }
        if (Sourc[x][y][z] != 0) {
          DensAdd[x][y][z]= float(Sourc[x][y][z]) * D.param[CoeffSource_________].val;
        }
      }
    }
  }

  VelocityStep(Solid, std::max(int(std::round(D.param[GaussSeiderIter_____].val)), 0), D.param[TimeStepSize________].val,
               D.param[CoeffViscosity______].val, VelXAdd, VelYAdd, VelZAdd, VelXCur, VelYCur, VelZCur);
  DensityStep(Solid, std::max(int(std::round(D.param[GaussSeiderIter_____].val)), 0), D.param[TimeStepSize________].val,
              D.param[CoeffDiffusion______].val, VelXCur, VelYCur, VelZCur, DensAdd, DensCur);
}


void CompuFluidDyn::Draw() {
  if (!isInitialized) return;
  if (!isRefreshed) return;

  int maxDim= std::max(std::max(nbX, nbY), nbZ);
  float voxSize= 1.0 / float(maxDim);

  // Draw the solid voxels
  if (D.displayMode1) {
    glPushMatrix();
    glTranslatef(0.5f - 0.5f * float(nbX) / float(maxDim), 0.5f - 0.5f * float(nbY) / float(maxDim), 0.5f - 0.5f * float(nbZ) / float(maxDim));
    glScalef(voxSize, voxSize, voxSize);
    glTranslatef(0.5f, 0.5f, 0.5f);
    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          if (Solid[x][y][z] != 0) {
            glPushMatrix();
            glTranslatef(float(x), float(y), float(z));
            if (Solid[x][y][z] > 0) glColor3f(0.5f, 0.5f, 0.5f);
            if (Solid[x][y][z] < 0) glColor3f(0.2f, 0.5f, 0.2f);
            glutSolidCube(1.0);
            glPopMatrix();
          }
          if (Force[x][y][z] != 0) {
            glPushMatrix();
            glTranslatef(float(x), float(y), float(z));
            if (Force[x][y][z] > 0) glColor3f(0.8f, 0.8f, 0.8f);
            if (Force[x][y][z] < 0) glColor3f(0.2f, 0.2f, 0.2f);
            glutSolidCube(1.0);
            glPopMatrix();
          }
          if (Sourc[x][y][z] != 0) {
            glPushMatrix();
            glTranslatef(float(x), float(y), float(z));
            if (Sourc[x][y][z] > 0) glColor3f(1.0f, 0.0f, 0.0f);
            if (Sourc[x][y][z] < 0) glColor3f(0.0f, 0.0f, 1.0f);
            glutSolidCube(1.0);
            glPopMatrix();
          }
        }
      }
    }
    glPopMatrix();
  }

  // Draw the cell velocity field
  if (D.displayMode2) {
    glLineWidth(2.0f);
    glPushMatrix();
    glTranslatef(0.5f - 0.5f * float(nbX) / float(maxDim), 0.5f - 0.5f * float(nbY) / float(maxDim), 0.5f - 0.5f * float(nbZ) / float(maxDim));
    glScalef(voxSize, voxSize, voxSize);
    glTranslatef(0.5f, 0.5f, 0.5f);
    glBegin(GL_LINES);
    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          Math::Vec3f vec(VelXCur[x][y][z], VelYCur[x][y][z], VelZCur[x][y][z]);
          if (vec.normSquared() > 0.0) {
            float r= 0.0f, g= 0.0f, b= 0.0f;
            Colormap::RatioToJetBrightSmooth(vec.norm() * D.param[ColorFactor_________].val, r, g, b);
            glColor3f(r, g, b);
            Math::Vec3f pos= Math::Vec3f(float(x), float(y), float(z));
            glVertex3fv(pos.array());
            glVertex3fv(pos + std::log(vec.norm() + 1.0f) * vec * D.param[ScaleFactor_________].val);
          }
        }
      }
    }
    glEnd();
    glPopMatrix();
    glLineWidth(1.0f);
  }

  // Draw the pressure field
  if (D.displayMode3) {
    int nbXUp= int(std::round(float(nbX) * std::max(D.param[DisplayUpsampling___].val, 1.0)));
    int nbYUp= int(std::round(float(nbY) * std::max(D.param[DisplayUpsampling___].val, 1.0)));
    int nbZUp= int(std::round(float(nbZ) * std::max(D.param[DisplayUpsampling___].val, 1.0)));
    if (nbX == 1) nbXUp= 1;
    if (nbY == 1) nbYUp= 1;
    if (nbZ == 1) nbZUp= 1;
    int maxDimUp= std::max(std::max(nbXUp, nbYUp), nbZUp);
    float voxSizeUp= 1.0 / float(maxDimUp);

    glPointSize(3.0f);
    glPushMatrix();
    glTranslatef(0.5f - 0.5f * float(nbXUp) / float(maxDimUp), 0.5f - 0.5f * float(nbYUp) / float(maxDimUp), 0.5f - 0.5f * float(nbZUp) / float(maxDimUp));
    glScalef(voxSizeUp, voxSizeUp, voxSizeUp);
    glTranslatef(0.5f, 0.5f, 0.5f);
    glBegin(GL_POINTS);
    for (int x= 0; x < nbXUp; x++) {
      for (int y= 0; y < nbYUp; y++) {
        for (int z= 0; z < nbZUp; z++) {
          float val= TrilinearInterpolation(float(x * nbX) / float(nbXUp), float(y * nbY) / float(nbYUp), float(z * nbZ) / float(nbZUp), DensCur);
          if (std::abs(val) < D.param[ColorThresh_________].val) continue;
          float r= 0.0f, g= 0.0f, b= 0.0f;
          Colormap::RatioToJetBrightSmooth(0.5f + 0.5f * val * D.param[ColorFactor_________].val, r, g, b);
          glColor3f(r, g, b);
          glVertex3f(float(x), float(y), float(z));
        }
      }
    }
    glEnd();
    glPopMatrix();
    glPointSize(1.0f);
  }

  // Draw the pressure field
  if (D.displayMode4) {
    glPointSize(3.0f);
    glPushMatrix();
    glTranslatef(0.5f - 0.5f * float(nbX) / float(maxDim), 0.5f - 0.5f * float(nbY) / float(maxDim), 0.5f - 0.5f * float(nbZ) / float(maxDim));
    glScalef(voxSize, voxSize, voxSize);
    glTranslatef(0.5f, 0.5f, 0.5f);
    glBegin(GL_POINTS);
    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          float val= TrilinearInterpolation(float(x * nbX) / float(nbX), float(y * nbY) / float(nbY), float(z * nbZ) / float(nbZ), DensCur);
          if (std::abs(val) < D.param[ColorThresh_________].val) continue;
          float r= 0.0f, g= 0.0f, b= 0.0f;
          Colormap::RatioToJetBrightSmooth(0.5f + 0.5f * val * D.param[ColorFactor_________].val, r, g, b);
          glColor3f(r, g, b);
          glVertex3f(float(x), float(y), float(z));
        }
      }
    }
    glEnd();
    glPopMatrix();
    glPointSize(1.0f);
  }
}
