
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
#include "../Util/Random.hpp"
#include "../Util/Timer.hpp"
#include "../Util/Vector.hpp"


extern Data D;

enum ParamType
{
  Scenario____,
  InputFile___,
  ResolutionX_,
  ResolutionY_,
  ResolutionZ_,
  TimeStep____,
  SolvGSIter__,
  SolvGSCoeff_,
  CoeffDiffu__,
  CoeffVisco__,
  CoeffForceX_,
  CoeffForceY_,
  CoeffForceZ_,
  CoeffSmoke__,
  ObjectPosX__,
  ObjectPosY__,
  ObjectPosZ__,
  ObjectSize__,
  ScaleFactor_,
  ColorFactor_,
  ColorThresh_,
  ColorMode___,
  SlicePlotX__,
  SlicePlotY__,
  SlicePlotZ__,
};


CompuFluidDyna::CompuFluidDyna() {
  D.param.clear();
  D.plotData.clear();
  D.scatData.clear();
  isActiveProject= false;
  isInitialized= false;
  isRefreshed= false;
}


void CompuFluidDyna::SetActiveProject() {
  if (!isActiveProject) {
    D.param.push_back(ParamUI("Scenario____", 0));
    D.param.push_back(ParamUI("InputFile___", 2));
    D.param.push_back(ParamUI("ResolutionX_", 1));
    D.param.push_back(ParamUI("ResolutionY_", 100));
    D.param.push_back(ParamUI("ResolutionZ_", 100));
    D.param.push_back(ParamUI("TimeStep____", 0.02));
    D.param.push_back(ParamUI("SolvGSIter__", 20));
    D.param.push_back(ParamUI("SolvGSCoeff_", 1.9));
    D.param.push_back(ParamUI("CoeffDiffu__", 0.0));
    D.param.push_back(ParamUI("CoeffVisco__", 0.0));
    D.param.push_back(ParamUI("CoeffForceX_", 0.0));
    D.param.push_back(ParamUI("CoeffForceY_", 0.6));
    D.param.push_back(ParamUI("CoeffForceZ_", 0.0));
    D.param.push_back(ParamUI("CoeffSmoke__", 1.0));
    D.param.push_back(ParamUI("ObjectPosX__", 0.5));
    D.param.push_back(ParamUI("ObjectPosY__", 0.25));
    D.param.push_back(ParamUI("ObjectPosZ__", 0.5));
    D.param.push_back(ParamUI("ObjectSize__", 0.08));
    D.param.push_back(ParamUI("ScaleFactor_", 5.0));
    D.param.push_back(ParamUI("ColorFactor_", 1.0));
    D.param.push_back(ParamUI("ColorThresh_", 0.0));
    D.param.push_back(ParamUI("ColorMode___", 1));
    D.param.push_back(ParamUI("SlicePlotX__", 0.5));
    D.param.push_back(ParamUI("SlicePlotY__", 0.5));
    D.param.push_back(ParamUI("SlicePlotZ__", 0.5));
  }

  isActiveProject= true;
  isInitialized= false;
  isRefreshed= false;
  Initialize();
}


void CompuFluidDyna::CheckInit() {
  if (D.param[Scenario____].hasChanged()) isInitialized= false;
  if (D.param[InputFile___].hasChanged()) isInitialized= false;
  if (D.param[ResolutionX_].hasChanged()) isInitialized= false;
  if (D.param[ResolutionY_].hasChanged()) isInitialized= false;
  if (D.param[ResolutionZ_].hasChanged()) isInitialized= false;
}


void CompuFluidDyna::CheckRefresh() {
  if (D.param[CoeffForceX_].hasChanged()) isRefreshed= false;
  if (D.param[CoeffForceY_].hasChanged()) isRefreshed= false;
  if (D.param[CoeffForceZ_].hasChanged()) isRefreshed= false;
  if (D.param[CoeffSmoke__].hasChanged()) isRefreshed= false;
  if (D.param[ObjectPosX__].hasChanged()) isRefreshed= false;
  if (D.param[ObjectPosY__].hasChanged()) isRefreshed= false;
  if (D.param[ObjectPosZ__].hasChanged()) isRefreshed= false;
  if (D.param[ObjectSize__].hasChanged()) isRefreshed= false;
}


void CompuFluidDyna::Initialize() {
  // Check if need to skip
  if (!isActiveProject) return;
  CheckInit();
  if (isInitialized) return;
  isInitialized= true;

  // Get UI parameters
  nbX= std::max((int)std::round(D.param[ResolutionX_].Get()), 1);
  nbY= std::max((int)std::round(D.param[ResolutionY_].Get()), 1);
  nbZ= std::max((int)std::round(D.param[ResolutionZ_].Get()), 1);
  maxDim= std::max(std::max(nbX, nbY), nbZ);
  voxSize= 1.0f / (float)maxDim;
  boxMin= {0.5f - 0.5f * (float)nbX * voxSize, 0.5f - 0.5f * (float)nbY * voxSize, 0.5f - 0.5f * (float)nbZ * voxSize};
  boxMax= {0.5f + 0.5f * (float)nbX * voxSize, 0.5f + 0.5f * (float)nbY * voxSize, 0.5f + 0.5f * (float)nbZ * voxSize};

  // Allocate data
  Solid= Field::AllocField3D(nbX, nbY, nbZ, false);
  Passi= Field::AllocField3D(nbX, nbY, nbZ, false);
  VelBC= Field::AllocField3D(nbX, nbY, nbZ, false);
  SmoBC= Field::AllocField3D(nbX, nbY, nbZ, false);
  VelXForced= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  VelYForced= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  VelZForced= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  SmoForced= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);

  Press= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  Smoke= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  VelX= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  VelY= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  VelZ= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);

  // Force refresh
  isRefreshed= false;
  Refresh();
}


void CompuFluidDyna::Refresh() {
  CheckRefresh();
  if (isRefreshed) return;
  isRefreshed= true;

  // Get scenario ID and optionnally load bitmap file
  const int scenarioType= (int)std::round(D.param[Scenario____].Get());
  const int inputFile= (int)std::round(D.param[InputFile___].Get());
  std::vector<std::vector<std::array<float, 4>>> imageRGBA;
  if (scenarioType == 0) {
    if (inputFile == 0)
      FileInput::LoadImageBMPFile("Resources/CFD_TeslaValveTwinSharp.bmp", imageRGBA, false);
    else if (inputFile == 1)
      FileInput::LoadImageBMPFile("Resources/CFD_Venturi.bmp", imageRGBA, false);
    else
      FileInput::LoadImageBMPFile("Resources/CFD_Wing.bmp", imageRGBA, false);
  }

  // Set scenario values
  for (int x= 0; x < nbX; x++) {
    for (int y= 0; y < nbY; y++) {
      for (int z= 0; z < nbZ; z++) {
        // Initialize scenario values
        Solid[x][y][z]= false;
        Passi[x][y][z]= false;
        VelBC[x][y][z]= false;
        SmoBC[x][y][z]= false;
        VelXForced[x][y][z]= 0.0f;
        VelYForced[x][y][z]= 0.0f;
        VelZForced[x][y][z]= 0.0f;
        SmoForced[x][y][z]= 0.0f;

        // Scenario from loaded BMP file
        if (scenarioType == 0) {
          // Get pixel colors
          const float posW= (float)(imageRGBA.size() - 1) * ((float)y + 0.5f) / (float)nbY;
          const float posH= (float)(imageRGBA[0].size() - 1) * ((float)z + 0.5f) / (float)nbZ;
          const int idxPixelW= std::min(std::max((int)std::round(posW), 0), (int)imageRGBA.size() - 1);
          const int idxPixelH= std::min(std::max((int)std::round(posH), 0), (int)imageRGBA[0].size() - 1);
          const std::array<float, 3> color= {imageRGBA[idxPixelW][idxPixelH][0], imageRGBA[idxPixelW][idxPixelH][1], imageRGBA[idxPixelW][idxPixelH][2]};
          // Set flags from pixel colors
          if (color[0] < 0.1f) Solid[x][y][z]= true;
          if (color[0] > 0.9f) Passi[x][y][z]= true;
          if (std::abs(color[1] - 0.5f) > 0.1f) VelBC[x][y][z]= true;
          if (std::abs(color[2] - 0.5f) > 0.1f) SmoBC[x][y][z]= true;
          // Set forced values for velocity and smoke
          if (VelBC[x][y][z]) {
            VelXForced[x][y][z]= (color[1] > 0.5f) ? (D.param[CoeffForceX_].Get()) : (-D.param[CoeffForceX_].Get());
            VelYForced[x][y][z]= (color[1] > 0.5f) ? (D.param[CoeffForceY_].Get()) : (-D.param[CoeffForceY_].Get());
            VelZForced[x][y][z]= (color[1] > 0.5f) ? (D.param[CoeffForceZ_].Get()) : (-D.param[CoeffForceZ_].Get());
          }
          if (SmoBC[x][y][z]) {
            SmoForced[x][y][z]= (color[2] > 0.5f) ? (D.param[CoeffSmoke__].Get()) : (-D.param[CoeffSmoke__].Get());
          }
        }

        // Double facing inlets
        if (scenarioType == 1) {
          if (y <= 1 || y >= nbY - 2 || z <= 1 || z >= nbZ - 2) {
            Passi[x][y][z]= true;
          }
          for (int k= 0; k < 2; k++) {
            Math::Vec3f posCell(((float)x + 0.5f) / (float)nbX, ((float)y + 0.5f) / (float)nbY, ((float)z + 0.5f) / (float)nbZ);
            Math::Vec3f posObstacle(D.param[ObjectPosX__].Get(), D.param[ObjectPosY__].Get(), D.param[ObjectPosZ__].Get());
            if (k == 1) posObstacle= Math::Vec3f(1.0f, 1.0f, 1.0f) - posObstacle;
            if ((posCell - posObstacle).norm() <= std::max((float)D.param[ObjectSize__].Get(), 0.0f)) {
              VelBC[x][y][z]= true;
              SmoBC[x][y][z]= true;
              VelXForced[x][y][z]= (k == 1) ? (-D.param[CoeffForceX_].Get()) : (D.param[CoeffForceX_].Get());
              VelYForced[x][y][z]= (k == 1) ? (-D.param[CoeffForceY_].Get()) : (D.param[CoeffForceY_].Get());
              VelZForced[x][y][z]= (k == 1) ? (-D.param[CoeffForceZ_].Get()) : (D.param[CoeffForceZ_].Get());
              SmoForced[x][y][z]= (k == 1) ? (-D.param[CoeffSmoke__].Get()) : (D.param[CoeffSmoke__].Get());
            }
          }
        }

        // Circular obstacle in corridor showing vortex shedding
        if (scenarioType == 2) {
          if (z == 0 || z == nbZ - 1) {
            Solid[x][y][z]= true;
          }
          else if (y <= 1) {
            VelBC[x][y][z]= true;
            SmoBC[x][y][z]= true;
            VelXForced[x][y][z]= D.param[CoeffForceX_].Get();
            VelYForced[x][y][z]= D.param[CoeffForceY_].Get();
            VelZForced[x][y][z]= D.param[CoeffForceZ_].Get();
            SmoForced[x][y][z]= (std::max(z, nbZ - 1 - z) % 16 < 8) ? (D.param[CoeffSmoke__].Get()) : (-D.param[CoeffSmoke__].Get());
          }
          else if (y >= nbY - 2) {
            Passi[x][y][z]= true;
          }
          else {
            Math::Vec3f posCell(((float)x + 0.5f) / (float)nbX, ((float)y + 0.5f) / (float)nbY, ((float)z + 0.5f) / (float)nbZ);
            Math::Vec3f posObstacle(D.param[ObjectPosX__].Get(), D.param[ObjectPosY__].Get(), D.param[ObjectPosZ__].Get());
            Math::Vec3f dist= (posCell - posObstacle);
            dist[0]*= (float)nbX / (float)maxDim;
            dist[1]*= (float)nbY / (float)maxDim;
            dist[2]*= (float)nbZ / (float)maxDim;
            if (dist.norm() <= std::max((float)D.param[ObjectSize__].Get(), 0.0f))
              Solid[x][y][z]= true;
          }
        }

        // Cavity lid shear benchmark
        if (scenarioType == 3) {
          // Force zero velocity for no-slip condition on cavity wall
          if (y == 0 || y == nbY - 1 || z == 0) {
            VelBC[x][y][z]= true;
            VelXForced[x][y][z]= 0;
            VelYForced[x][y][z]= 0;
            VelZForced[x][y][z]= 0;
          }
          // Force tangential velocity on cavity lid
          else if (z == nbZ - 1) {
            VelBC[x][y][z]= true;
            VelXForced[x][y][z]= D.param[CoeffForceX_].Get();
            VelYForced[x][y][z]= D.param[CoeffForceY_].Get();
            VelZForced[x][y][z]= D.param[CoeffForceZ_].Get();
          }
          // Add smoke source for visualization
          else if (y == nbY / 2 && z > nbZ / 2) {
            SmoBC[x][y][z]= true;
            SmoForced[x][y][z]= (z % 16 < 8) ? (D.param[CoeffSmoke__].Get()) : (-D.param[CoeffSmoke__].Get());
          }
        }
      }
    }
  }
}


void CompuFluidDyna::Animate() {
  if (!isActiveProject) return;
  CheckInit();
  if (!isInitialized) Initialize();
  CheckRefresh();
  if (!isRefreshed) Refresh();

  // Get simulation parameters
  const int maxIter= std::max((int)std::round(D.param[SolvGSIter__].Get()), 0);
  const float timestep= D.param[TimeStep____].Get();
  const float coeffDiffu= std::max(D.param[CoeffDiffu__].Get(), 0.0);
  const float coeffVisco= std::max(D.param[CoeffVisco__].Get(), 0.0);

  // Simulate velocity step
  ApplyBC(1, VelX);
  ApplyBC(2, VelY);
  ApplyBC(3, VelZ);
  GaussSeidelSolve(1, maxIter, timestep, true, coeffVisco, VelX);
  GaussSeidelSolve(2, maxIter, timestep, true, coeffVisco, VelY);
  GaussSeidelSolve(3, maxIter, timestep, true, coeffVisco, VelZ);
  ProjectField(maxIter, timestep, VelX, VelY, VelZ);
  std::vector<std::vector<std::vector<float>>> tmpVelX= VelX;
  std::vector<std::vector<std::vector<float>>> tmpVelY= VelY;
  std::vector<std::vector<std::vector<float>>> tmpVelZ= VelZ;
  AdvectField(1, timestep, tmpVelX, tmpVelY, tmpVelZ, VelX);
  AdvectField(2, timestep, tmpVelX, tmpVelY, tmpVelZ, VelY);
  AdvectField(3, timestep, tmpVelX, tmpVelY, tmpVelZ, VelZ);
  ProjectField(maxIter, timestep, VelX, VelY, VelZ);

  // Simulate smoke step
  ApplyBC(0, Smoke);
  GaussSeidelSolve(0, maxIter, timestep, true, coeffDiffu, Smoke);
  AdvectField(0, timestep, VelX, VelY, VelZ, Smoke);

  // Plot field info
  D.plotData.resize(6);
  if (D.plotData[0].second.size() < 1000) {
    D.plotData[0].first= "TotVelX";
    D.plotData[1].first= "TotVelY";
    D.plotData[2].first= "TotVelZ";
    D.plotData[3].first= "TotVelMag";
    D.plotData[4].first= "TotSmoke";
    D.plotData[5].first= "TotPress";
    D.plotData[0].second.push_back(0.0f);
    D.plotData[1].second.push_back(0.0f);
    D.plotData[2].second.push_back(0.0f);
    D.plotData[3].second.push_back(0.0f);
    D.plotData[4].second.push_back(0.0f);
    D.plotData[5].second.push_back(0.0f);
    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          if (!Solid[x][y][z]) {
            D.plotData[0].second[D.plotData[0].second.size() - 1]+= VelX[x][y][z];
            D.plotData[1].second[D.plotData[1].second.size() - 1]+= VelY[x][y][z];
            D.plotData[2].second[D.plotData[2].second.size() - 1]+= VelZ[x][y][z];
            D.plotData[3].second[D.plotData[3].second.size() - 1]+= std::sqrt(std::pow(VelX[x][y][z], 2.0f) + std::pow(VelY[x][y][z], 2.0f) + std::pow(VelZ[x][y][z], 2.0f));
            D.plotData[4].second[D.plotData[4].second.size() - 1]+= Smoke[x][y][z];
            D.plotData[5].second[D.plotData[4].second.size() - 1]+= Press[x][y][z];
          }
        }
      }
    }
  }

  D.scatData.resize(2);
  D.scatData[0].first= "Vert Yax";
  D.scatData[1].first= "Hori Zax";
  D.scatData[0].second.clear();
  D.scatData[1].second.clear();
  if (nbZ > 1) {
    const int z= std::min(std::max((int)std::round((float)(nbZ - 1) * (float)D.param[SlicePlotZ__].Get()), 0), nbZ - 1);
    for (int y= 0; y < nbY; y++)
      D.scatData[0].second.push_back(std::array<double, 2>({(double)y / (double)(nbY - 1), VelZ[nbX / 2][y][z] + (double)z / (double)(nbZ - 1)}));
  }
  if (nbY > 1) {
    const int y= std::min(std::max((int)std::round((float)(nbY - 1) * (float)D.param[SlicePlotY__].Get()), 0), nbY - 1);
    for (int z= 0; z < nbZ; z++)
      D.scatData[1].second.push_back(std::array<double, 2>({VelY[nbX / 2][y][z] + (double)y / (double)(nbY - 1), (double)z / (double)(nbZ - 1)}));
  }

  // Add hard coded lid driven cavity flow benchmark for visual comparison
  if ((int)std::round(D.param[Scenario____].Get()) == 3) {
    D.scatData.resize(4);
    D.scatData[2].first= "Vert Yax";
    D.scatData[3].first= "Hori Zax";
    D.scatData[2].second.clear();
    D.scatData[3].second.clear();
    const std::vector<double> rawData0X({0, 0.0625, 0.0703, 0.0781, 0.0983, 0.1563, 0.2266, 0.2344, 0.5, 0.8047, 0.8594, 0.9063, 0.9453, 0.9531, 0.9609, 0.9688, 1});                            // coord along horiz slice
    const std::vector<double> rawData0Y({0, 0.1836, 0.19713, 0.20920, 0.22965, 0.28124, 0.30203, 0.30174, 0.05186, -0.38598, -0.44993, -0.23827, -0.22847, -0.19254, -0.15663, -0.12146, 0});    // verti vel along horiz slice
    const std::vector<double> rawData1X({0, -0.08186, -0.09266, -0.10338, -0.14612, -0.24299, -0.32726, -0.17119, -0.11477, 0.02135, 0.16256, 0.29093, 0.55892, 0.61756, 0.68439, 0.75837, 1});  // horiz vel on verti slice
    const std::vector<double> rawData1Y({0, 0.0547, 0.0625, 0.0703, 0.1016, 0.1719, 0.2813, 0.4531, 0.5, 0.6172, 0.7344, 0.8516, 0.9531, 0.9609, 0.9688, 0.9766, 1});                            // coord along verti slice
    for (int k= 0; k < (int)rawData0X.size(); k++) {
      D.scatData[2].second.push_back(std::array<double, 2>({rawData0X[k], rawData0Y[k] + 0.5f}));
      D.scatData[3].second.push_back(std::array<double, 2>({rawData1X[k] + 0.5f, rawData1Y[k]}));
    }
  }
}


void CompuFluidDyna::Draw() {
  if (!isActiveProject) return;
  CheckInit();
  if (!isInitialized) Initialize();
  CheckRefresh();
  if (!isRefreshed) Refresh();

  // Draw the voxels
  if (D.displayMode1) {
    glEnable(GL_LIGHTING);
    // Set the scene transformation
    glPushMatrix();
    glTranslatef(boxMin[0] + 0.5f * voxSize, boxMin[1] + 0.5f * voxSize, boxMin[2] + 0.5f * voxSize);
    glScalef(voxSize, voxSize, voxSize);
    // Sweep the field
    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          // Set the voxel color components
          std::array<float, 3> color= {0.2f, 0.2f, 0.2f};
          if (Solid[x][y][z]) color[0]= 0.0f;
          if (Passi[x][y][z]) color[0]= 0.7f;
          if (!Solid[x][y][z] && VelBC[x][y][z]) color[1]= 0.7f;
          if (!Solid[x][y][z] && SmoBC[x][y][z]) color[2]= 0.7f;
          // Draw the cube
          if (Solid[x][y][z] || Passi[x][y][z] || VelBC[x][y][z] || SmoBC[x][y][z]) {
            glColor3f(color[0], color[1], color[2]);
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

  // Draw the scalar fields
  if (D.displayMode2) {
    // Set the scene transformation
    glPushMatrix();
    glTranslatef(boxMin[0] + 0.5f * voxSize, boxMin[1] + 0.5f * voxSize, boxMin[2] + 0.5f * voxSize);
    glScalef(voxSize, voxSize, voxSize);
    if (nbX == 1) glScalef(0.1f, 1.0f, 1.0f);
    if (nbY == 1) glScalef(1.0f, 0.1f, 1.0f);
    if (nbZ == 1) glScalef(1.0f, 1.0f, 0.1f);
    // Sweep the field
    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          float r= 0.0f, g= 0.0f, b= 0.0f;
          // Color by pressure
          if (std::min(std::max((int)std::round(D.param[ColorMode___].Get()), 1), 3) == 1) {
            if (std::abs(2.0f * Press[x][y][z]) < D.param[ColorThresh_].Get()) continue;
            Colormap::RatioToBlueToRed(0.5f + Press[x][y][z] * D.param[ColorFactor_].Get(), r, g, b);
          }
          // Color by smoke
          if (std::min(std::max((int)std::round(D.param[ColorMode___].Get()), 1), 3) == 2) {
            if (std::abs(0.5f * Smoke[x][y][z]) < D.param[ColorThresh_].Get()) continue;
            Colormap::RatioToGreenToRed(0.5f + 0.5f * Smoke[x][y][z] * D.param[ColorFactor_].Get(), r, g, b);
          }
          // Color by velocity magnitude
          if (std::min(std::max((int)std::round(D.param[ColorMode___].Get()), 1), 3) == 3) {
            Math::Vec3f vec(VelX[x][y][z], VelY[x][y][z], VelZ[x][y][z]);
            if (vec.norm() < D.param[ColorThresh_].Get()) continue;
            Colormap::RatioToJetBrightSmooth(vec.norm() * D.param[ColorFactor_].Get(), r, g, b);
          }
          glColor3f(r, g, b);
          glPushMatrix();
          glTranslatef((float)x, (float)y, (float)z);
          if (nbX > 1 && nbY > 1 && nbZ > 1) glutWireCube(1.0);
          else glutSolidCube(1.0);
          glPopMatrix();
        }
      }
    }
    glPopMatrix();
  }

  // Draw the vector fields
  if (D.displayMode3) {
    constexpr int nbSizes= 3;
    // Set the scene transformation
    glPushMatrix();
    glTranslatef(boxMin[0] + 0.5f * voxSize, boxMin[1] + 0.5f * voxSize, boxMin[2] + 0.5f * voxSize);
    glScalef(voxSize, voxSize, voxSize);
    // Sweep the field
    for (int k= 0; k < nbSizes; k++) {
      glLineWidth((float)k + 1.0f);
      glBegin(GL_LINES);
      for (int x= 0; x < nbX; x++) {
        for (int y= 0; y < nbY; y++) {
          for (int z= 0; z < nbZ; z++) {
            float r= 0.0f, g= 0.0f, b= 0.0f;
            // Draw the velocity field
            Math::Vec3f vec(VelX[x][y][z], VelY[x][y][z], VelZ[x][y][z]);
            if (vec.normSquared() > 0.0f) {
              Colormap::RatioToJetBrightSmooth(vec.norm() * D.param[ColorFactor_].Get(), r, g, b);
              glColor3f(r, g, b);
              Math::Vec3f pos((float)x, (float)y, (float)z);
              glVertex3fv(pos.array());
              glVertex3fv(pos + vec.normalized() * D.param[ScaleFactor_].Get() * std::log(vec.norm() + 1.0f) * (1.0f - ((float)k / (float)nbSizes)));
            }
          }
        }
      }
      glEnd();
    }
    glLineWidth(1.0f);
    glPopMatrix();
  }
}


constexpr int MaskSize= 6;
constexpr int Mask[MaskSize][3]=
    {{+1, +0, +0},
     {-1, +0, +0},
     {+0, +1, +0},
     {+0, -1, +0},
     {+0, +0, +1},
     {+0, +0, -1}};

void CompuFluidDyna::ApplyBC(const int iFieldID, std::vector<std::vector<std::vector<float>>>& ioField) {
  // Copy previous field values for reference
  std::vector<std::vector<std::vector<float>>> oldField= ioField;
  // Sweep through the field
#pragma omp parallel for
  for (int x= 0; x < nbX; x++) {
    for (int y= 0; y < nbY; y++) {
      for (int z= 0; z < nbZ; z++) {
        // Work on smoke field
        if (iFieldID == 0) {
          // Forced value on forced smoke voxel
          if (SmoBC[x][y][z]) {
            ioField[x][y][z]= SmoForced[x][y][z];
          }
          // Continuity/average on solid voxel neighboring at least one non-solid voxel
          else if (Solid[x][y][z]) {
            int count= 0;
            ioField[x][y][z]= 0.0f;
            for (int k= 0; k < MaskSize; k++) {
              if (x + Mask[k][0] < 0 || x + Mask[k][0] >= nbX) continue;
              if (y + Mask[k][1] < 0 || y + Mask[k][1] >= nbY) continue;
              if (z + Mask[k][2] < 0 || z + Mask[k][2] >= nbZ) continue;
              if (Solid[x + Mask[k][0]][y + Mask[k][1]][z + Mask[k][2]]) continue;
              ioField[x][y][z]+= oldField[x + Mask[k][0]][y + Mask[k][1]][z + Mask[k][2]];
              count++;
            }
            if (count > 0)
              ioField[x][y][z]/= (float)count;
          }
        }
        // Work on velocity fields
        else if (iFieldID == 1 || iFieldID == 2 || iFieldID == 3) {
          // Zero value on solid voxel
          if (Solid[x][y][z]) {
            ioField[x][y][z]= 0.0f;
          }
          // Continuity/average on passive voxel neighboring at least one non-solid voxel
          if (Passi[x][y][z]) {
            int count= 0;
            ioField[x][y][z]= 0.0f;
            for (int k= 0; k < MaskSize; k++) {
              if (x + Mask[k][0] < 0 || x + Mask[k][0] >= nbX) continue;
              if (y + Mask[k][1] < 0 || y + Mask[k][1] >= nbY) continue;
              if (z + Mask[k][2] < 0 || z + Mask[k][2] >= nbZ) continue;
              if (Solid[x + Mask[k][0]][y + Mask[k][1]][z + Mask[k][2]]) continue;
              ioField[x][y][z]+= oldField[x + Mask[k][0]][y + Mask[k][1]][z + Mask[k][2]];
              count++;
            }
            if (count > 0)
              ioField[x][y][z]/= (float)count;
          }
          // Forced value on forced velocity voxel
          if (VelBC[x][y][z]) {
            if (iFieldID == 1) ioField[x][y][z]= VelXForced[x][y][z];
            if (iFieldID == 2) ioField[x][y][z]= VelYForced[x][y][z];
            if (iFieldID == 3) ioField[x][y][z]= VelZForced[x][y][z];
          }
        }
        // Work on pressure field
        else if (iFieldID == 4) {
          // Continuity/average on solid voxel neighboring at least one non-solid voxel
          if (Solid[x][y][z]) {
            int count= 0;
            ioField[x][y][z]= 0.0f;
            for (int k= 0; k < MaskSize; k++) {
              if (x + Mask[k][0] < 0 || x + Mask[k][0] >= nbX) continue;
              if (y + Mask[k][1] < 0 || y + Mask[k][1] >= nbY) continue;
              if (z + Mask[k][2] < 0 || z + Mask[k][2] >= nbZ) continue;
              if (Solid[x + Mask[k][0]][y + Mask[k][1]][z + Mask[k][2]]) continue;
              ioField[x][y][z]+= oldField[x + Mask[k][0]][y + Mask[k][1]][z + Mask[k][2]];
              count++;
            }
            if (count > 0)
              ioField[x][y][z]/= (float)count;
          }
          // Zero value on passive voxel
          if (Passi[x][y][z]) {
            ioField[x][y][z]= 0.0f;
          }
        }
      }
    }
  }
}


void CompuFluidDyna::GaussSeidelSolve(
    const int iFieldID, const int iMaxIter, const float iTimeStep,
    const bool iDiffuMode, const float iDiffuCoeff,
    std::vector<std::vector<std::vector<float>>>& ioField) {
  // Skip if non changing field
  if (iDiffuMode && iDiffuCoeff == 0.0f) return;
  // Get parameters
  const float diffuVal= iTimeStep * (float)(nbX * nbY * nbZ) * iDiffuCoeff;
  const float coeffOverrelax= std::min(std::max((float)D.param[SolvGSCoeff_].Get(), 0.0f), 1.99f);
  // Copy previous field values for reference
  std::vector<std::vector<std::vector<float>>> oldField= ioField;
  // Solve with PArallel BIdirectionnal GAuss-Seidel Successive Over-Relaxation (PABIGASSOR)
  for (int k= 0; k < iMaxIter; k++) {
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
              // Ignore solid or fixed values
              if (Solid[x][y][z]) continue;
              if (iFieldID == 0 && SmoBC[x][y][z]) continue;
              if ((iFieldID == 1 || iFieldID == 2 || iFieldID == 3) && VelBC[x][y][z]) continue;
              // Get count and sum of valid neighbors
              int count= 0;
              float sum= 0.0f;
              for (int k= 0; k < MaskSize; k++) {
                if (x + Mask[k][0] < 0 || x + Mask[k][0] >= nbX) continue;
                if (y + Mask[k][1] < 0 || y + Mask[k][1] >= nbY) continue;
                if (z + Mask[k][2] < 0 || z + Mask[k][2] >= nbZ) continue;
                if (Solid[x + Mask[k][0]][y + Mask[k][1]][z + Mask[k][2]]) continue;
                sum+= FieldA[x + Mask[k][0]][y + Mask[k][1]][z + Mask[k][2]];
                count++;
              }
              // Set new value according to coefficients and flags
              const float prevVal= FieldA[x][y][z];
              if (iDiffuMode)
                FieldA[x][y][z]= (oldField[x][y][z] + diffuVal * sum) / (1.0f + diffuVal * (float)count);
              else if (count > 0)
                FieldA[x][y][z]= (oldField[x][y][z] + sum) / (float)count;
              // Apply overrelaxation trick
              FieldA[x][y][z]= prevVal + coeffOverrelax * (FieldA[x][y][z] - prevVal);
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
              // Ignore solid or fixed values
              if (Solid[x][y][z]) continue;
              if (iFieldID == 0 && SmoBC[x][y][z]) continue;
              if ((iFieldID == 1 || iFieldID == 2 || iFieldID == 3) && VelBC[x][y][z]) continue;
              // Get count and sum of valid neighbors
              int count= 0;
              float sum= 0.0f;
              for (int k= 0; k < MaskSize; k++) {
                if (x + Mask[k][0] < 0 || x + Mask[k][0] >= nbX) continue;
                if (y + Mask[k][1] < 0 || y + Mask[k][1] >= nbY) continue;
                if (z + Mask[k][2] < 0 || z + Mask[k][2] >= nbZ) continue;
                if (Solid[x + Mask[k][0]][y + Mask[k][1]][z + Mask[k][2]]) continue;
                sum+= FieldB[x + Mask[k][0]][y + Mask[k][1]][z + Mask[k][2]];
                count++;
              }
              // Set new value according to coefficients and flags
              const float prevVal= FieldB[x][y][z];
              if (iDiffuMode)
                FieldB[x][y][z]= (oldField[x][y][z] + diffuVal * sum) / (1.0f + diffuVal * (float)count);
              else if (count > 0)
                FieldB[x][y][z]= (oldField[x][y][z] + sum) / (float)count;
              // Apply overrelaxation trick
              FieldB[x][y][z]= prevVal + coeffOverrelax * (FieldB[x][y][z] - prevVal);
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

  float val=
      v000 * (xWeight0 * yWeight0 * zWeight0) +
      v001 * (xWeight0 * yWeight0 * zWeight1) +
      v010 * (xWeight0 * yWeight1 * zWeight0) +
      v011 * (xWeight0 * yWeight1 * zWeight1) +
      v100 * (xWeight1 * yWeight0 * zWeight0) +
      v101 * (xWeight1 * yWeight0 * zWeight1) +
      v110 * (xWeight1 * yWeight1 * zWeight0) +
      v111 * (xWeight1 * yWeight1 * zWeight1);

  return val;
}


void CompuFluidDyna::AdvectField(
    const int iFieldID, const float iTimeStep,
    const std::vector<std::vector<std::vector<float>>>& iVelX,
    const std::vector<std::vector<std::vector<float>>>& iVelY,
    const std::vector<std::vector<std::vector<float>>>& iVelZ,
    std::vector<std::vector<std::vector<float>>>& ioField) {
  // Copy previous field values for reference
  std::vector<std::vector<std::vector<float>>> oldField= ioField;
  // Sweep through field and apply semi Lagrangian advection
#pragma omp parallel for
  for (int x= 0; x < nbX; x++) {
    for (int y= 0; y < nbY; y++) {
      for (int z= 0; z < nbZ; z++) {
        // Ignore solid or fixed values
        if (Solid[x][y][z]) continue;
        if (iFieldID == 0 && SmoBC[x][y][z]) continue;
        if ((iFieldID == 1 || iFieldID == 2 || iFieldID == 3) && VelBC[x][y][z]) continue;
        // Find valid source position for active voxel
        float posX, posY, posZ;
        float distRatio= 1.0f;
        for (int idxSubstep= 0; idxSubstep < 10; idxSubstep++) {
          posX= (float)x - iTimeStep * (float)maxDim * distRatio * iVelX[x][y][z];
          posY= (float)y - iTimeStep * (float)maxDim * distRatio * iVelY[x][y][z];
          posZ= (float)z - iTimeStep * (float)maxDim * distRatio * iVelZ[x][y][z];
          const int idxX= std::min(std::max((int)std::round(posX), 0), nbX - 1);
          const int idxY= std::min(std::max((int)std::round(posY), 0), nbY - 1);
          const int idxZ= std::min(std::max((int)std::round(posZ), 0), nbZ - 1);
          if (Solid[idxX][idxY][idxZ]) distRatio*= 3.0f / 4.0f;
          else break;
        }
        // Trilinear interpolation
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
  // Compute divergence
#pragma omp parallel for
  for (int x= 0; x < nbX; x++) {
    for (int y= 0; y < nbY; y++) {
      for (int z= 0; z < nbZ; z++) {
        Press[x][y][z]= 0.0f;
        if (!Solid[x][y][z] && !Passi[x][y][z]) {
          if (x - 1 >= 0 && x + 1 < nbX) Press[x][y][z]+= ioVelX[x + 1][y][z] - ioVelX[x - 1][y][z];
          if (y - 1 >= 0 && y + 1 < nbY) Press[x][y][z]+= ioVelY[x][y + 1][z] - ioVelY[x][y - 1][z];
          if (z - 1 >= 0 && z + 1 < nbZ) Press[x][y][z]+= ioVelZ[x][y][z + 1] - ioVelZ[x][y][z - 1];
          // TODO check if need to handle asymmetric neighbors. Copy value for voxel outside ?
          Press[x][y][z]= -0.5f * Press[x][y][z];
        }
      }
    }
  }

  // Reapply BC to maintain consistency
  ApplyBC(4, Press);

  // Solve for pressure
  GaussSeidelSolve(4, iIter, iTimeStep, false, 0.0f, Press);

  // Update velocities based on pressure
#pragma omp parallel for
  for (int x= 0; x < nbX; x++) {
    for (int y= 0; y < nbY; y++) {
      for (int z= 0; z < nbZ; z++) {
        if (!Solid[x][y][z] && !Passi[x][y][z]) {
          // TODO check if need to handle asymmetric neighbors. Copy value for voxel outside ?
          if (x - 1 >= 0 && x + 1 < nbX) ioVelX[x][y][z]-= 0.5f * (Press[x + 1][y][z] - Press[x - 1][y][z]);
          if (y - 1 >= 0 && y + 1 < nbY) ioVelY[x][y][z]-= 0.5f * (Press[x][y + 1][z] - Press[x][y - 1][z]);
          if (z - 1 >= 0 && z + 1 < nbZ) ioVelZ[x][y][z]-= 0.5f * (Press[x][y][z + 1] - Press[x][y][z - 1]);
        }
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
