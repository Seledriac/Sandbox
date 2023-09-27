#include "CompuFluidDyna.hpp"


// Standard lib
#include <cstdio>
#include <vector>

// GLUT lib
#include "../freeglut/include/GL/freeglut.h"

// Project lib
#include "../Data.hpp"
#include "../FileIO/FileInput.hpp"
#include "../Util/Colormap.hpp"
#include "../Util/Field.hpp"
#include "../Util/Vector.hpp"


// Link to shared sandbox data
extern Data D;

// List of UI parameters for this project
enum ParamType
{
  Scenario____,
  InputFile___,
  ResolutionX_,
  ResolutionY_,
  ResolutionZ_,
  TimeStep____,
  SolvMaxIter_,
  SolvTolRhs__,
  SolvTolRel__,
  SolvSOR_____,
  CoeffDiffu__,
  CoeffVisco__,
  CoeffVorti__,
  CoeffVelX___,
  CoeffVelY___,
  CoeffVelZ___,
  CoeffPres___,
  CoeffSmok___,
  ObjectPosX__,
  ObjectPosY__,
  ObjectPosZ__,
  ObjectSize__,
  ColorFactor_,
  ColorThresh_,
  ColorMode___,
  SlicePlotX__,
  SlicePlotY__,
  SlicePlotZ__,
  SolvCGPress_,
  SolvCGVisco_,
  SolvCGSmoke_,
  Verbose_____,
};


// Constructor
CompuFluidDyna::CompuFluidDyna() {
  isActivProj= false;
  isAllocated= false;
  isRefreshed= false;
}


// Initialize Project UI parameters
void CompuFluidDyna::SetActiveProject() {
  if (!isActivProj) {
    D.param.clear();
    D.param.push_back(ParamUI("Scenario____", 2));
    D.param.push_back(ParamUI("InputFile___", 2));
    D.param.push_back(ParamUI("ResolutionX_", 1));
    D.param.push_back(ParamUI("ResolutionY_", 80));
    D.param.push_back(ParamUI("ResolutionZ_", 40));
    D.param.push_back(ParamUI("TimeStep____", 0.02));
    D.param.push_back(ParamUI("SolvMaxIter_", 50));
    D.param.push_back(ParamUI("SolvTolRhs__", 1.e-5));
    D.param.push_back(ParamUI("SolvTolRel__", 1.e-5));
    D.param.push_back(ParamUI("SolvSOR_____", 1.75));
    D.param.push_back(ParamUI("CoeffDiffu__", 0.0001));
    D.param.push_back(ParamUI("CoeffVisco__", 0.002));
    D.param.push_back(ParamUI("CoeffVorti__", 0.0));
    D.param.push_back(ParamUI("CoeffVelX___", 0.0));
    D.param.push_back(ParamUI("CoeffVelY___", 1.0));
    D.param.push_back(ParamUI("CoeffVelZ___", 0.0));
    D.param.push_back(ParamUI("CoeffPres___", 0.02));
    D.param.push_back(ParamUI("CoeffSmok___", 1.0));
    D.param.push_back(ParamUI("ObjectPosX__", 0.5));
    D.param.push_back(ParamUI("ObjectPosY__", 0.25));
    D.param.push_back(ParamUI("ObjectPosZ__", 0.5));
    D.param.push_back(ParamUI("ObjectSize__", 0.08));
    D.param.push_back(ParamUI("ColorFactor_", 1.0));
    D.param.push_back(ParamUI("ColorThresh_", 0.0));
    D.param.push_back(ParamUI("ColorMode___", 2));
    D.param.push_back(ParamUI("SlicePlotX__", 0.5));
    D.param.push_back(ParamUI("SlicePlotY__", 0.5));
    D.param.push_back(ParamUI("SlicePlotZ__", 0.5));
    D.param.push_back(ParamUI("SolvCGPress_", 0.5));
    D.param.push_back(ParamUI("SolvCGVisco_", 0.5));
    D.param.push_back(ParamUI("SolvCGSmoke_", 0.5));
    D.param.push_back(ParamUI("Verbose_____", -0.5));
  }

  D.boxMin= {0.0, 0.0, 0.0};
  D.boxMax= {1.0, 1.0, 1.0};

  isActivProj= true;
  isAllocated= false;
  isRefreshed= false;
  Allocate();
  Refresh();
}


// Check if parameter changes should trigger an allocation
void CompuFluidDyna::CheckAlloc() {
  if (D.param[Scenario____].hasChanged() ||
      D.param[InputFile___].hasChanged() ||
      D.param[ResolutionX_].hasChanged() ||
      D.param[ResolutionY_].hasChanged() ||
      D.param[ResolutionZ_].hasChanged()) isAllocated= false;
}


// Check if parameter changes should trigger a refresh
void CompuFluidDyna::CheckRefresh() {
  if (D.param[CoeffVelX___].hasChanged() ||
      D.param[CoeffVelY___].hasChanged() ||
      D.param[CoeffVelZ___].hasChanged() ||
      D.param[CoeffPres___].hasChanged() ||
      D.param[CoeffSmok___].hasChanged() ||
      D.param[ObjectPosX__].hasChanged() ||
      D.param[ObjectPosY__].hasChanged() ||
      D.param[ObjectPosZ__].hasChanged() ||
      D.param[ObjectSize__].hasChanged()) isRefreshed= false;
}


// Allocate the project data
void CompuFluidDyna::Allocate() {
  if (!isActivProj) return;
  CheckAlloc();
  if (isAllocated) return;
  isRefreshed= false;
  isAllocated= true;

  // Get UI parameters
  nbX= std::max((int)std::round(D.param[ResolutionX_].Get()), 1);
  nbY= std::max((int)std::round(D.param[ResolutionY_].Get()), 1);
  nbZ= std::max((int)std::round(D.param[ResolutionZ_].Get()), 1);
  voxSize= 1.0f / (float)std::max(std::max(nbX, nbY), nbZ);
  D.boxMin= {0.5f - 0.5f * (float)nbX * voxSize, 0.5f - 0.5f * (float)nbY * voxSize, 0.5f - 0.5f * (float)nbZ * voxSize};
  D.boxMax= {0.5f + 0.5f * (float)nbX * voxSize, 0.5f + 0.5f * (float)nbY * voxSize, 0.5f + 0.5f * (float)nbZ * voxSize};

  // Allocate data
  Solid= Field::AllocField3D(nbX, nbY, nbZ, false);
  VelBC= Field::AllocField3D(nbX, nbY, nbZ, false);
  PreBC= Field::AllocField3D(nbX, nbY, nbZ, false);
  SmoBC= Field::AllocField3D(nbX, nbY, nbZ, false);
  VelXForced= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  VelYForced= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  VelZForced= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  PresForced= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  SmokForced= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);

  Dum0= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  Dum1= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  Dum2= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  Dum3= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  Dum4= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  Vort= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  Pres= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  Dive= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  Smok= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  VelX= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  VelY= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  VelZ= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  CurX= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  CurY= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  CurZ= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
}


// Refresh the project
void CompuFluidDyna::Refresh() {
  if (!isActivProj) return;
  CheckAlloc();
  if (!isAllocated) Allocate();
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

  // Initialize scenario values
  for (int x= 0; x < nbX; x++) {
    for (int y= 0; y < nbY; y++) {
      for (int z= 0; z < nbZ; z++) {
        Solid[x][y][z]= false;
        VelBC[x][y][z]= false;
        PreBC[x][y][z]= false;
        SmoBC[x][y][z]= false;
        VelXForced[x][y][z]= 0.0f;
        VelYForced[x][y][z]= 0.0f;
        VelZForced[x][y][z]= 0.0f;
        PresForced[x][y][z]= 0.0f;
        SmokForced[x][y][z]= 0.0f;
      }
    }
  }

  // Set scenario values
  for (int x= 0; x < nbX; x++) {
    for (int y= 0; y < nbY; y++) {
      for (int z= 0; z < nbZ; z++) {
        // Scenario from loaded BMP file
        if (scenarioType == 0) {
          // Get pixel colors
          const float posW= (float)(imageRGBA.size() - 1) * ((float)y + 0.5f) / (float)nbY;
          const float posH= (float)(imageRGBA[0].size() - 1) * ((float)z + 0.5f) / (float)nbZ;
          const int idxPixelW= std::min(std::max((int)std::round(posW), 0), (int)imageRGBA.size() - 1);
          const int idxPixelH= std::min(std::max((int)std::round(posH), 0), (int)imageRGBA[0].size() - 1);
          const std::array<float, 4> colRGBA= imageRGBA[idxPixelW][idxPixelH];
          // Set flags from pixel colors
          if (colRGBA[3] < 0.1f) {
            Solid[x][y][z]= true;
          }
          else {
            if (std::abs(colRGBA[0] - 0.5f) > 0.1f) PreBC[x][y][z]= true;
            if (std::abs(colRGBA[1] - 0.5f) > 0.1f) VelBC[x][y][z]= true;
            if (std::abs(colRGBA[2] - 0.5f) > 0.1f) SmoBC[x][y][z]= true;
          }
          // Set forced values
          if (PreBC[x][y][z]) {
            PresForced[x][y][z]= (colRGBA[0] > 0.5f) ? (D.param[CoeffPres___].Get()) : (-D.param[CoeffPres___].Get());
          }
          if (VelBC[x][y][z]) {
            VelXForced[x][y][z]= (colRGBA[1] > 0.5f) ? (D.param[CoeffVelX___].Get()) : (-D.param[CoeffVelX___].Get());
            VelYForced[x][y][z]= (colRGBA[1] > 0.5f) ? (D.param[CoeffVelY___].Get()) : (-D.param[CoeffVelY___].Get());
            VelZForced[x][y][z]= (colRGBA[1] > 0.5f) ? (D.param[CoeffVelZ___].Get()) : (-D.param[CoeffVelZ___].Get());
          }
          if (SmoBC[x][y][z]) {
            SmokForced[x][y][z]= (colRGBA[2] > 0.5f) ? (D.param[CoeffSmok___].Get()) : (-D.param[CoeffSmok___].Get());
          }
        }

        // Double facing inlets
        if (scenarioType == 1) {
          if ((nbX > 1 && (x == 0 || x == nbX - 1)) ||
              (nbY > 1 && (y == 0 || y == nbY - 1)) ||
              (nbZ > 1 && (z == 0 || z == nbZ - 1))) {
            PreBC[x][y][z]= true;
            PresForced[x][y][z]= 0.0f;
          }
          for (int k= 0; k < 2; k++) {
            Math::Vec3f posCell(((float)x + 0.5f) / (float)nbX, ((float)y + 0.5f) / (float)nbY, ((float)z + 0.5f) / (float)nbZ);
            Math::Vec3f posObstacle(D.param[ObjectPosX__].Get(), D.param[ObjectPosY__].Get(), D.param[ObjectPosZ__].Get());
            if (k == 1) posObstacle= Math::Vec3f(1.0f, 1.0f, 1.0f) - posObstacle;
            if ((posCell - posObstacle).norm() <= std::max((float)D.param[ObjectSize__].Get(), 0.0f)) {
              VelBC[x][y][z]= true;
              SmoBC[x][y][z]= true;
              VelXForced[x][y][z]= (k == 1) ? (-D.param[CoeffVelX___].Get()) : (D.param[CoeffVelX___].Get());
              VelYForced[x][y][z]= (k == 1) ? (-D.param[CoeffVelY___].Get()) : (D.param[CoeffVelY___].Get());
              VelZForced[x][y][z]= (k == 1) ? (-D.param[CoeffVelZ___].Get()) : (D.param[CoeffVelZ___].Get());
              SmokForced[x][y][z]= (k == 1) ? (-D.param[CoeffSmok___].Get()) : (D.param[CoeffSmok___].Get());
            }
          }
        }

        // Circular obstacle in corridor showing vortex shedding
        if (scenarioType == 2) {
          if ((nbX > 1 && (x == 0 || x == nbX - 1)) ||
              (nbZ > 1 && (z == 0 || z == nbZ - 1))) {
            Solid[x][y][z]= true;
          }
          else if (y == nbY - 1) {
            PreBC[x][y][z]= true;
            PresForced[x][y][z]= 0.0f;
          }
          else if (y == 0) {
            VelBC[x][y][z]= true;
            VelXForced[x][y][z]= D.param[CoeffVelX___].Get();
            VelYForced[x][y][z]= D.param[CoeffVelY___].Get();
            VelZForced[x][y][z]= D.param[CoeffVelZ___].Get();
            SmoBC[x][y][z]= true;
            SmokForced[x][y][z]= (std::max(z, nbZ - 1 - z) % 16 < 8) ? (D.param[CoeffSmok___].Get()) : (-D.param[CoeffSmok___].Get());
          }
          else {
            Math::Vec3f posCell(((float)x + 0.5f) / (float)nbX, ((float)y + 0.5f) / (float)nbY, ((float)z + 0.5f) / (float)nbZ);
            Math::Vec3f posObstacle(D.param[ObjectPosX__].Get(), D.param[ObjectPosY__].Get(), D.param[ObjectPosZ__].Get());
            Math::Vec3f dist= (posCell - posObstacle);
            dist[0]*= (float)(nbX - 1) * voxSize;
            dist[1]*= (float)(nbY - 1) * voxSize;
            dist[2]*= (float)(nbZ - 1) * voxSize;
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
            VelXForced[x][y][z]= D.param[CoeffVelX___].Get();
            VelYForced[x][y][z]= D.param[CoeffVelY___].Get();
            VelZForced[x][y][z]= D.param[CoeffVelZ___].Get();
          }
          // Add smoke source for visualization
          else if (y == nbY / 2 && z > nbZ / 2) {
            SmoBC[x][y][z]= true;
            SmokForced[x][y][z]= (z % 16 < 8) ? (D.param[CoeffSmok___].Get()) : (-D.param[CoeffSmok___].Get());
          }
        }

        // Vortex ring with inlet in Y-, outlet in Y+ and wall with hole in the corridor
        if (scenarioType == 4) {
          int wallPos= (float)D.param[ObjectPosY__].Get() * (float)nbY;
          int wallThick= (float)D.param[ObjectSize__].Get() * (float)nbY;
          if ((nbX > 1 && (x == 0 || x == nbX - 1)) ||
              (nbZ > 1 && (z == 0 || z == nbZ - 1))) {
            Solid[x][y][z]= true;
          }
          else if (y == 0) {
            VelBC[x][y][z]= true;
            VelXForced[x][y][z]= D.param[CoeffVelX___].Get();
            VelYForced[x][y][z]= D.param[CoeffVelY___].Get();
            VelZForced[x][y][z]= D.param[CoeffVelZ___].Get();
          }
          else if (std::abs(y - wallPos) <= wallThick) {
            Math::Vec3f posCell(((float)x + 0.5f) / (float)nbX, 0.0f, ((float)z + 0.5f) / (float)nbZ);
            Math::Vec3f posObstacle(D.param[ObjectPosX__].Get(), 0.0f, D.param[ObjectPosZ__].Get());
            Math::Vec3f dist= (posCell - posObstacle);
            dist[0]*= (float)(nbX - 1) * voxSize;
            dist[1]*= (float)(nbY - 1) * voxSize;
            dist[2]*= (float)(nbZ - 1) * voxSize;
            if (dist.norm() >= std::max((float)D.param[ObjectSize__].Get(), 0.0f))
              Solid[x][y][z]= true;
          }
          else if (y < wallPos) {
            SmoBC[x][y][z]= true;
            SmokForced[x][y][z]= D.param[CoeffVelY___].Get() * D.param[CoeffSmok___].Get();
          }
          else if (y == nbY - 1) {
            PreBC[x][y][z]= true;
            PresForced[x][y][z]= 0.0f;
          }
        }

        // Central bloc with initial velocity
        if (scenarioType == 5) {
          if ((nbX > 1 && (x == 0 || x == nbX - 1)) ||
              (nbY > 1 && (y == 0 || y == nbY - 1)) ||
              (nbZ > 1 && (z == 0 || z == nbZ - 1))) {
            Solid[x][y][z]= true;
          }
          else if ((nbX > 1 && (x == 1 || x == nbX - 2)) ||
                   (nbY > 1 && (y == 1 || y == nbY - 2)) ||
                   (nbZ > 1 && (z == 1 || z == nbZ - 2))) {
            PreBC[x][y][z]= true;
            PresForced[x][y][z]= 0.0f;
          }
          else if (((nbX == 1) != (std::min(x, nbX - 1 - x) > nbX / 3)) &&
                   ((nbY == 1) != (std::min(y, nbY - 1 - y) > nbY / 3)) &&
                   ((nbZ == 1) != (std::min(z, nbZ - 1 - z) > nbZ / 3))) {
            VelBC[x][y][z]= true;
            VelXForced[x][y][z]= D.param[CoeffVelX___].Get();
            VelYForced[x][y][z]= D.param[CoeffVelY___].Get();
            VelZForced[x][y][z]= D.param[CoeffVelZ___].Get();
            SmoBC[x][y][z]= true;
            SmokForced[x][y][z]= (std::min(z, nbZ - 1 - z) < 4 * (nbZ - 1) / 9) ? -D.param[CoeffSmok___].Get() : D.param[CoeffSmok___].Get();
          }
        }

        // Poiseuille/Couette flow in tube with pressure gradient
        if (scenarioType == 6) {
          if ((nbX > 1 && (x == 0 || x == nbX - 1)) ||
              (nbZ > 1 && (z == 0 || z == nbZ - 1))) {
            Solid[x][y][z]= true;
          }
          else if (nbZ > 1 && (z == 1 || z == nbZ - 2)) {
            VelBC[x][y][z]= true;
            VelXForced[x][y][z]= (z < nbZ / 2) ? -D.param[CoeffVelX___].Get() : D.param[CoeffVelX___].Get();
            VelYForced[x][y][z]= (z < nbZ / 2) ? -D.param[CoeffVelY___].Get() : D.param[CoeffVelY___].Get();
            VelZForced[x][y][z]= (z < nbZ / 2) ? -D.param[CoeffVelZ___].Get() : D.param[CoeffVelZ___].Get();
          }
          else if (nbY > 1 && (y == 0 || y == nbY - 1)) {
            PreBC[x][y][z]= true;
            PresForced[x][y][z]= (y > nbY / 2) ? -D.param[CoeffPres___].Get() : D.param[CoeffPres___].Get();
          }
          else if (std::max(y, nbY - 1 - y) == nbY / 2) {
            SmoBC[x][y][z]= true;
            SmokForced[x][y][z]= (std::max(z, nbZ - 1 - z) % 16 < 8) ? (D.param[CoeffSmok___].Get()) : (-D.param[CoeffSmok___].Get());
          }
        }
      }
    }
  }

  // Apply BC on fields
  ApplyBC(FieldID::IDSmok, Smok);
  ApplyBC(FieldID::IDVelX, VelX);
  ApplyBC(FieldID::IDVelY, VelY);
  ApplyBC(FieldID::IDVelZ, VelZ);
  ApplyBC(FieldID::IDPres, Dive);
  ApplyBC(FieldID::IDPres, Pres);
}


// Animate the project
void CompuFluidDyna::Animate() {
  if (!isActivProj) return;
  CheckAlloc();
  if (!isAllocated) Allocate();
  CheckRefresh();
  if (!isRefreshed) Refresh();

  if (D.param[Verbose_____].Get() > 0.0) printf("\n");

  // Get simulation parameters
  const int maxIter= std::max((int)std::round(D.param[SolvMaxIter_].Get()), 0);
  const float timestep= D.param[TimeStep____].Get();
  const float coeffDiffu= std::max(D.param[CoeffDiffu__].Get(), 0.0);
  const float coeffVisco= std::max(D.param[CoeffVisco__].Get(), 0.0);
  const float coeffVorti= D.param[CoeffVorti__].Get();

  // Advection steps
  AdvectField(FieldID::IDSmok, timestep, VelX, VelY, VelZ, Smok);
  std::vector<std::vector<std::vector<float>>> oldVelX= VelX;
  std::vector<std::vector<std::vector<float>>> oldVelY= VelY;
  std::vector<std::vector<std::vector<float>>> oldVelZ= VelZ;
  AdvectField(FieldID::IDVelX, timestep, oldVelX, oldVelY, oldVelZ, VelX);
  AdvectField(FieldID::IDVelY, timestep, oldVelX, oldVelY, oldVelZ, VelY);
  AdvectField(FieldID::IDVelZ, timestep, oldVelX, oldVelY, oldVelZ, VelZ);

  // Diffusion steps
  std::vector<std::vector<std::vector<float>>> oldSmoke= Smok;
  if (D.param[SolvCGSmoke_].Get() < 0.0) {
    GaussSeidelSolve(FieldID::IDSmok, maxIter, timestep, true, coeffDiffu, oldSmoke, Smok);
  }
  else {
    ConjugateGradientSolve(FieldID::IDSmok, maxIter, timestep, true, coeffDiffu, oldSmoke, Smok);
  }
  oldVelX= VelX;
  oldVelY= VelY;
  oldVelZ= VelZ;
  if (D.param[SolvCGVisco_].Get() < 0.0) {
    GaussSeidelSolve(FieldID::IDVelX, maxIter, timestep, true, coeffVisco, oldVelX, VelX);
    GaussSeidelSolve(FieldID::IDVelY, maxIter, timestep, true, coeffVisco, oldVelY, VelY);
    GaussSeidelSolve(FieldID::IDVelZ, maxIter, timestep, true, coeffVisco, oldVelZ, VelZ);
  }
  else {
    ConjugateGradientSolve(FieldID::IDVelX, maxIter, timestep, true, coeffVisco, oldVelX, VelX);
    ConjugateGradientSolve(FieldID::IDVelY, maxIter, timestep, true, coeffVisco, oldVelY, VelY);
    ConjugateGradientSolve(FieldID::IDVelZ, maxIter, timestep, true, coeffVisco, oldVelZ, VelZ);
  }

  // Vorticity step
  VorticityConfinement(timestep, coeffVorti, VelX, VelY, VelZ);

  // Projection step
  ProjectField(maxIter, timestep, VelX, VelY, VelZ);

  // TODO test heuristic optimization of solid regions

  // // Draw the plot data
  // D.plotData.resize(7);
  // if (D.plotData[0].size() < 1000) {
  //   D.plotData[0]= "TotVelX";
  //   D.plotData[1]= "TotVelY";
  //   D.plotData[2]= "TotVelZ";
  //   D.plotData[3]= "TotVelMag";
  //   D.plotData[4]= "TotSmoke";
  //   D.plotData[5]= "TotPress";
  //   D.plotData[6]= "TotVorti";
  //   D.plotData[0].push_back(0.0f);
  //   D.plotData[1].push_back(0.0f);
  //   D.plotData[2].push_back(0.0f);
  //   D.plotData[3].push_back(0.0f);
  //   D.plotData[4].push_back(0.0f);
  //   D.plotData[5].push_back(0.0f);
  //   D.plotData[6].push_back(0.0f);
  //   for (int x= 0; x < nbX; x++) {
  //     for (int y= 0; y < nbY; y++) {
  //       for (int z= 0; z < nbZ; z++) {
  //         if (!Solid[x][y][z]) {
  //           D.plotData[0][D.plotData[0].size() - 1]+= VelX[x][y][z];
  //           D.plotData[1][D.plotData[1].size() - 1]+= VelY[x][y][z];
  //           D.plotData[2][D.plotData[2].size() - 1]+= VelZ[x][y][z];
  //           D.plotData[3][D.plotData[3].size() - 1]+= std::sqrt(VelX[x][y][z] * VelX[x][y][z] + VelY[x][y][z] * VelY[x][y][z] + VelZ[x][y][z] * VelZ[x][y][z]);
  //           D.plotData[4][D.plotData[4].size() - 1]+= Smok[x][y][z];
  //           D.plotData[5][D.plotData[5].size() - 1]+= Pres[x][y][z];
  //           D.plotData[6][D.plotData[6].size() - 1]+= Vort[x][y][z];
  //         }
  //       }
  //     }
  //   }
  // }

  // Draw the scatter data
  D.scatLegend.resize(4);
  D.scatLegend[0]= "Horiz VZ";
  D.scatLegend[1]= "Verti VY";
  D.scatLegend[2]= "Horiz P";
  D.scatLegend[3]= "Verti P";
  D.scatData.resize(4);
  for (int k= 0; k < (int)D.scatData.size(); k++)
    D.scatData[k].clear();
  if (nbZ > 1) {
    const int z= std::min(std::max((int)std::round((float)(nbZ - 1) * (float)D.param[SlicePlotZ__].Get()), 0), nbZ - 1);
    for (int y= 0; y < nbY; y++) {
      D.scatData[0].push_back(std::array<double, 2>({(double)y / (double)(nbY - 1), VelZ[nbX / 2][y][z] + (double)z / (double)(nbZ - 1)}));
      D.scatData[2].push_back(std::array<double, 2>({(double)y / (double)(nbY - 1), Pres[nbX / 2][y][z] + (double)z / (double)(nbZ - 1)}));
    }
  }
  if (nbY > 1) {
    const int y= std::min(std::max((int)std::round((float)(nbY - 1) * (float)D.param[SlicePlotY__].Get()), 0), nbY - 1);
    for (int z= 0; z < nbZ; z++) {
      D.scatData[1].push_back(std::array<double, 2>({VelY[nbX / 2][y][z] + (double)y / (double)(nbY - 1), (double)z / (double)(nbZ - 1)}));
      D.scatData[3].push_back(std::array<double, 2>({Pres[nbX / 2][y][z] + (double)y / (double)(nbY - 1), (double)z / (double)(nbZ - 1)}));
    }
  }

  // Add hard coded lid driven cavity flow benchmark for visual comparison
  if ((int)std::round(D.param[Scenario____].Get()) == 3) {
    D.scatLegend.resize(4);
    D.scatLegend[2]= "Ghia 100";
    D.scatLegend[3]= "Ghia 100";
    D.scatData.resize(4);
    D.scatData[2].clear();
    D.scatData[3].clear();
    const std::vector<double> GhiaData0X({+0.00000, +0.06250, +0.07030, +0.07810, +0.09380, +0.15630, +0.22660, +0.23440, +0.50000, +0.80470, +0.85940, +0.90630, +0.94530, +0.95310, +0.96090, +0.96880, +1.00000});  // coord along horiz slice
    // Data from Ghia 1982 http://www.msaidi.ir/upload/Ghia1982.pdf
    const std::vector<double> GhiaData0Y({+0.00000, +0.09233, +0.10091, +0.10890, +0.12317, +0.16077, +0.17507, +0.17527, +0.05454, -0.24533, -0.22445, -0.16914, -0.10313, -0.08864, -0.07391, -0.05906, +0.00000});  // Re 100   verti vel along horiz slice
    // const std::vector<double> GhiaData0Y({+0.00000, +0.18360, +0.19713, +0.20920, +0.22965, +0.28124, +0.30203, +0.30174, +0.05186, -0.38598, -0.44993, -0.23827, -0.22847, -0.19254, -0.15663, -0.12146, +0.00000});  // Re 400   verti vel along horiz slice
    // const std::vector<double> GhiaData0Y({+0.00000, +0.27485, +0.29012, +0.30353, +0.32627, +0.37095, +0.33075, +0.32235, +0.02526, -0.31966, -0.42665, -0.51500, -0.39188, -0.33714, -0.27669, -0.21388, +0.00000});  // Re 1000  verti vel along horiz slice
    // const std::vector<double> GhiaData0Y({+0.00000, +0.39560, +0.40917, +0.41906, +0.42768, +0.37119, +0.29030, +0.28188, +0.00999, -0.31184, -0.37401, -0.44307, -0.54053, -0.52357, -0.47425, -0.39017, +0.00000});  // Re 3200  verti vel along horiz slice
    // const std::vector<double> GhiaData0Y({+0.00000, +0.42447, +0.43329, +0.43648, +0.42951, +0.35368, +0.28066, +0.27280, +0.00945, -0.30018, -0.36214, -0.41442, -0.52876, -0.55408, -0.55069, -0.49774, +0.00000});  // Re 5000  verti vel along horiz slice
    // const std::vector<double> GhiaData0Y({+0.00000, +0.43979, +0.44030, +0.43564, +0.41824, +0.35060, +0.28117, +0.27348, +0.00824, -0.30448, -0.36213, -0.41050, -0.48590, -0.52347, -0.55216, -0.53858, +0.00000});  // Re 7500  verti vel along horiz slice
    // const std::vector<double> GhiaData0Y({+0.00000, +0.43983, +0.43733, +0.43124, +0.41487, +0.35070, +0.28003, +0.27224, +0.00831, -0.30719, -0.36737, -0.41496, -0.45863, -0.49099, -0.52987, -0.54302, +0.00000});  // Re 10000 verti vel along horiz slice
    const std::vector<double> GhiaData1X({+0.00000, -0.03717, -0.04192, -0.04775, -0.06434, -0.10150, -0.15662, -0.21090, -0.20581, -0.13641, +0.00332, +0.23151, +0.68717, +0.73722, +0.78871, +0.84123, +1.00000});  // Re 100   horiz vel on verti slice
    // const std::vector<double> GhiaData1X({+0.00000, -0.08186, -0.09266, -0.10338, -0.14612, -0.24299, -0.32726, -0.17119, -0.11477, +0.02135, +0.16256, +0.29093, +0.55892, +0.61756, +0.68439, +0.75837, +1.00000});  // Re 400   horiz vel on verti slice
    // const std::vector<double> GhiaData1X({+0.00000, -0.18109, -0.20196, -0.22220, -0.29730, -0.38289, -0.27805, -0.10648, -0.06080, +0.05702, +0.18719, +0.33304, +0.46604, +0.51117, +0.57492, +0.65928, +1.00000});  // Re 1000  horiz vel on verti slice
    // const std::vector<double> GhiaData1X({+0.00000, -0.32407, -0.35344, -0.37827, -0.41933, -0.34323, -0.24427, -0.86636, -0.04272, +0.07156, +0.19791, +0.34682, +0.46101, +0.46547, +0.48296, +0.53236, +1.00000});  // Re 3200  horiz vel on verti slice
    // const std::vector<double> GhiaData1X({+0.00000, -0.41165, -0.42901, -0.43643, -0.40435, -0.33050, -0.22855, -0.07404, -0.03039, +0.08183, +0.20087, +0.33556, +0.46036, +0.45992, +0.46120, +0.48223, +1.00000});  // Re 5000  horiz vel on verti slice
    // const std::vector<double> GhiaData1X({+0.00000, -0.43154, -0.43590, -0.43025, -0.38324, -0.32393, -0.23176, -0.07503, -0.03800, +0.08342, +0.20591, +0.34228, +0.47167, +0.47323, +0.47048, +0.47244, +1.00000});  // Re 7500  horiz vel on verti slice
    // const std::vector<double> GhiaData1X({+0.00000, -0.42735, -0.42537, -0.41657, -0.38000, -0.32709, -0.23186, -0.07540, +0.03111, +0.08344, +0.20673, +0.34635, +0.47804, +0.48070, +0.47783, +0.47221, +1.00000});  // Re 10000 horiz vel on verti slice
    const std::vector<double> GhiaData1Y({+0.00000, +0.05470, +0.06250, +0.07030, +0.10160, +0.17190, +0.28130, +0.45310, +0.50000, +0.61720, +0.73440, +0.85160, +0.95310, +0.96090, +0.96880, +0.97660, +1.00000});  // coord along verti slice
    for (int k= 0; k < (int)GhiaData0X.size(); k++) {
      D.scatData[2].push_back(std::array<double, 2>({GhiaData0X[k], GhiaData0Y[k] + 0.5f}));
      D.scatData[3].push_back(std::array<double, 2>({GhiaData1X[k] + 0.5f, GhiaData1Y[k]}));
    }
    D.scatLegend.resize(6);
    D.scatLegend[4]= "Ertu 5k";
    D.scatLegend[5]= "Ertu 5k";
    D.scatData.resize(6);
    D.scatData[4].clear();
    D.scatData[5].clear();
    const std::vector<double> ErtuData0X({+0.00000, +0.01500, +0.03000, +0.04500, +0.06000, +0.07500, +0.09000, +0.10500, +0.12000, +0.13500, +0.15000, +0.50000, +0.85000, +0.86500, +0.88000, +0.89500, +0.91000, +0.92500, +0.94000, +0.95500, +0.97000, +0.98500, +1.00000});  // coord along horiz slice
    // Data from Erturk 2005 https://arxiv.org/pdf/physics/0505121.pdf
    // const std::vector<double> ErtuData0Y({+0.00000, +0.10190, +0.17920, +0.23490, +0.27460, +0.30410, +0.32730, +0.34600, +0.36050, +0.37050, +0.37560, +0.02580, -0.40280, -0.44070, -0.48030, -0.51320, -0.52630, -0.50520, -0.44170, -0.34000, -0.21730, -0.09730, +0.00000});  // Re 1000  verti vel along horiz slice
    // const std::vector<double> ErtuData0Y({+0.00000, +0.16070, +0.26330, +0.32380, +0.36490, +0.39500, +0.41420, +0.42170, +0.41870, +0.40780, +0.39180, +0.01600, -0.36710, -0.38430, -0.40420, -0.43210, -0.47410, -0.52680, -0.56030, -0.51920, -0.37250, -0.16750, +0.00000});  // Re 2500  verti vel along horiz slice
    const std::vector<double> ErtuData0Y({+0.00000, +0.21600, +0.32630, +0.38680, +0.42580, +0.44260, +0.44030, +0.42600, +0.40700, +0.38780, +0.36990, +0.01170, -0.36240, -0.38060, -0.39820, -0.41470, -0.43180, -0.45950, -0.51390, -0.57000, -0.50190, -0.24410, +0.00000});  // Re 5000  verti vel along horiz slice
    // const std::vector<double> ErtuData0Y({+0.00000, +0.25090, +0.36080, +0.42100, +0.44940, +0.44950, +0.43370, +0.41370, +0.39500, +0.37790, +0.36160, +0.00990, -0.35740, -0.37550, -0.39380, -0.41180, -0.42830, -0.44430, -0.47480, -0.54340, -0.55500, -0.29910, +0.00000});  // Re 7500  verti vel along horiz slice
    // const std::vector<double> ErtuData0Y({+0.00000, +0.27560, +0.38440, +0.44090, +0.45660, +0.44490, +0.42470, +0.40560, +0.38850, +0.37220, +0.35620, +0.00880, -0.35380, -0.37150, -0.38950, -0.40780, -0.42560, -0.44110, -0.45920, -0.51240, -0.57120, -0.34190, +0.00000});  // Re 10000 verti vel along horiz slice
    // const std::vector<double> ErtuData0Y({+0.00000, +0.29400, +0.40180, +0.45220, +0.45630, +0.43830, +0.41800, +0.40040, +0.38400, +0.36780, +0.35190, +0.00800, -0.35080, -0.36820, -0.38590, -0.40400, -0.42210, -0.43880, -0.45340, -0.48990, -0.56940, -0.37620, +0.00000});  // Re 12500 verti vel along horiz slice
    // const std::vector<double> ErtuData0Y({+0.00000, +0.30830, +0.41520, +0.45800, +0.45290, +0.43230, +0.41320, +0.39640, +0.38010, +0.36410, +0.34830, +0.00740, -0.34810, -0.36540, -0.38280, -0.40050, -0.41860, -0.43610, -0.45050, -0.47540, -0.55930, -0.40410, +0.00000});  // Re 15000 verti vel along horiz slice
    // const std::vector<double> ErtuData0Y({+0.00000, +0.31970, +0.42540, +0.46020, +0.44840, +0.42730, +0.40930, +0.39290, +0.37670, +0.36080, +0.34520, +0.00690, -0.34570, -0.36270, -0.38000, -0.39750, -0.41530, -0.43310, -0.44820, -0.46640, -0.54600, -0.42690, +0.00000});  // Re 17500 verti vel along horiz slice
    // const std::vector<double> ErtuData0Y({+0.00000, +0.32900, +0.43320, +0.46010, +0.44380, +0.42320, +0.40600, +0.38970, +0.37360, +0.35790, +0.34230, +0.00650, -0.34340, -0.36030, -0.37740, -0.39460, -0.41220, -0.43000, -0.44590, -0.46050, -0.53210, -0.44570, +0.00000});  // Re 20000 verti vel along horiz slice
    // const std::vector<double> ErtuData0Y({+0.00000, +0.33230, +0.43570, +0.45960, +0.44200, +0.42180, +0.40480, +0.38850, +0.37250, +0.35670, +0.34130, +0.00630, -0.34250, -0.35930, -0.37640, -0.39360, -0.41100, -0.42870, -0.44490, -0.45880, -0.52660, -0.45220, +0.00000});  // Re 21000 verti vel along horiz slice
    // const std::vector<double> ErtuData1X({+0.00000, -0.07570, -0.13920, -0.19510, -0.24720, -0.29600, -0.33810, -0.36900, -0.38540, -0.38690, -0.37560, -0.06200, +0.38380, +0.39130, +0.39930, +0.41010, +0.42760, +0.45820, +0.51020, +0.59170, +0.70650, +0.84860, +1.00000});  // Re 1000  horiz vel on verti slice
    // const std::vector<double> ErtuData1X({+0.00000, -0.15170, -0.25470, -0.33720, -0.39790, -0.42500, -0.42000, -0.39650, -0.36880, -0.34390, -0.32280, -0.04030, +0.41410, +0.42560, +0.43530, +0.44240, +0.44700, +0.45060, +0.46070, +0.49710, +0.59240, +0.77040, +1.00000});  // Re 2500  horiz vel on verti slice
    const std::vector<double> ErtuData1X({+0.00000, -0.22230, -0.34800, -0.42720, -0.44190, -0.41680, -0.38760, -0.36520, -0.34670, -0.32850, -0.31000, -0.03190, +0.41550, +0.43070, +0.44520, +0.45820, +0.46830, +0.47380, +0.47390, +0.47490, +0.51590, +0.68660, +1.00000});  // Re 5000  horiz vel on verti slice
    // const std::vector<double> ErtuData1X({+0.00000, -0.26330, -0.39800, -0.44910, -0.42840, -0.39780, -0.37660, -0.35870, -0.34060, -0.32220, -0.30380, -0.02870, +0.41230, +0.42750, +0.44310, +0.45850, +0.47230, +0.48240, +0.48600, +0.48170, +0.49070, +0.63000, +1.00000});  // Re 7500  horiz vel on verti slice
    // const std::vector<double> ErtuData1X({+0.00000, -0.29070, -0.42590, -0.44690, -0.41420, -0.38990, -0.37210, -0.35430, -0.33610, -0.31790, -0.29980, -0.02680, +0.40950, +0.42430, +0.43980, +0.45560, +0.47110, +0.48430, +0.49170, +0.48910, +0.48370, +0.58910, +1.00000});  // Re 10000 horiz vel on verti slice
    // const std::vector<double> ErtuData1X({+0.00000, -0.31130, -0.44070, -0.43800, -0.40540, -0.38590, -0.36850, -0.35060, -0.33260, -0.31460, -0.29670, -0.02560, +0.40700, +0.42160, +0.43660, +0.45230, +0.46840, +0.48330, +0.49370, +0.49410, +0.48330, +0.55870, +1.00000});  // Re 12500 horiz vel on verti slice
    // const std::vector<double> ErtuData1X({+0.00000, -0.32780, -0.44740, -0.42860, -0.40010, -0.38270, -0.36520, -0.34740, -0.32970, -0.31190, -0.29420, -0.02470, +0.40470, +0.41900, +0.43380, +0.44920, +0.46530, +0.48110, +0.49370, +0.49690, +0.48500, +0.53580, +1.00000});  // Re 15000 horiz vel on verti slice
    // const std::vector<double> ErtuData1X({+0.00000, -0.34120, -0.44900, -0.42060, -0.39650, -0.37970, -0.36220, -0.34460, -0.32710, -0.30960, -0.29200, -0.02400, +0.40240, +0.41660, +0.43120, +0.44630, +0.46220, +0.47840, +0.49250, +0.49820, +0.48710, +0.51830, +1.00000});  // Re 17500 horiz vel on verti slice
    // const std::vector<double> ErtuData1X({+0.00000, -0.35230, -0.44750, -0.41430, -0.39360, -0.37690, -0.35950, -0.34220, -0.32480, -0.30740, -0.28990, -0.02340, +0.40010, +0.41420, +0.42870, +0.44360, +0.45920, +0.47540, +0.49060, +0.49850, +0.48890, +0.50480, +1.00000});  // Re 20000 horiz vel on verti slice
    // const std::vector<double> ErtuData1X({+0.00000, -0.35620, -0.44630, -0.41210, -0.39250, -0.37580, -0.35850, -0.34120, -0.32390, -0.30660, -0.28920, -0.02320, +0.39920, +0.41320, +0.42770, +0.44250, +0.45800, +0.47420, +0.48970, +0.49830, +0.48950, +0.50030, +1.00000});  // Re 21000 horiz vel on verti slice
    const std::vector<double> ErtuData1Y({+0.00000, +0.02000, +0.04000, +0.06000, +0.08000, +0.10000, +0.12000, +0.14000, +0.16000, +0.18000, +0.20000, +0.50000, +0.90000, +0.91000, +0.92000, +0.93000, +0.94000, +0.95000, +0.96000, +0.97000, +0.98000, +0.99000, +1.00000});  // coord along verti slice
    for (int k= 0; k < (int)ErtuData0X.size(); k++) {
      D.scatData[4].push_back(std::array<double, 2>({ErtuData0X[k], ErtuData0Y[k] + 0.5f}));
      D.scatData[5].push_back(std::array<double, 2>({ErtuData1X[k] + 0.5f, ErtuData1Y[k]}));
    }
  }
}


// Draw the project
void CompuFluidDyna::Draw() {
  if (!isActivProj) return;
  if (!isAllocated) return;
  if (!isRefreshed) return;

  // Draw the voxels
  if (D.displayMode1) {
    glEnable(GL_LIGHTING);
    glLineWidth(2.0f);
    // Set the scene transformation
    glPushMatrix();
    glTranslatef(D.boxMin[0] + 0.5f * voxSize, D.boxMin[1] + 0.5f * voxSize, D.boxMin[2] + 0.5f * voxSize);
    glScalef(voxSize, voxSize, voxSize);
    // Sweep the field
    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          // Set the voxel color components
          float r= 0.4f, g= 0.4f, b= 0.4f;
          if (Solid[x][y][z]) r= 0.0f;
          if (PreBC[x][y][z]) r= 0.7f;
          if (VelBC[x][y][z]) g= 0.7f;
          if (SmoBC[x][y][z]) b= 0.7f;
          // Draw the cube
          if (Solid[x][y][z] || PreBC[x][y][z] || VelBC[x][y][z] || SmoBC[x][y][z]) {
            glColor3f(r, g, b);
            glPushMatrix();
            glTranslatef((float)x, (float)y, (float)z);
            glutWireCube(1.0);
            glPopMatrix();
          }
        }
      }
    }
    glPopMatrix();
    glLineWidth(1.0f);
    glDisable(GL_LIGHTING);
  }

  // Draw the scalar fields
  if (D.displayMode2) {
    // Set the scene transformation
    glPushMatrix();
    glTranslatef(D.boxMin[0] + 0.5f * voxSize, D.boxMin[1] + 0.5f * voxSize, D.boxMin[2] + 0.5f * voxSize);
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
          if ((int)std::round(D.param[ColorMode___].Get()) == 1) {
            if (std::abs(Pres[x][y][z]) < D.param[ColorThresh_].Get()) continue;
            Colormap::RatioToBlueToRed(0.5f + 0.5f * (Pres[x][y][z] / voxSize) * D.param[ColorFactor_].Get(), r, g, b);
          }
          // Color by smoke
          if ((int)std::round(D.param[ColorMode___].Get()) == 2) {
            if (std::abs(Smok[x][y][z]) < D.param[ColorThresh_].Get()) continue;
            Colormap::RatioToRainbow(0.5f + 0.5f * Smok[x][y][z] * D.param[ColorFactor_].Get(), r, g, b);
          }
          // Color by velocity magnitude
          if ((int)std::round(D.param[ColorMode___].Get()) == 3) {
            Math::Vec3f vec(VelX[x][y][z], VelY[x][y][z], VelZ[x][y][z]);
            if (vec.norm() < D.param[ColorThresh_].Get()) continue;
            Colormap::RatioToJetBrightSmooth(vec.norm() * D.param[ColorFactor_].Get(), r, g, b);
          }
          // Color by vorticity
          if ((int)std::round(D.param[ColorMode___].Get()) == 4) {
            if (std::abs(Vort[x][y][z]) < D.param[ColorThresh_].Get()) continue;
            Colormap::RatioToJetBrightSmooth(2.0f * Vort[x][y][z] * D.param[ColorFactor_].Get(), r, g, b);
          }
          // Color by curl in X
          if ((int)std::round(D.param[ColorMode___].Get()) == 5) {
            if (std::abs(CurX[x][y][z]) < D.param[ColorThresh_].Get()) continue;
            Colormap::RatioToJetBrightSmooth(0.5f + 2.0f * CurX[x][y][z] * D.param[ColorFactor_].Get(), r, g, b);
          }
          // Color by curl in Y
          if ((int)std::round(D.param[ColorMode___].Get()) == 6) {
            if (std::abs(CurY[x][y][z]) < D.param[ColorThresh_].Get()) continue;
            Colormap::RatioToJetBrightSmooth(0.5f + 2.0f * CurY[x][y][z] * D.param[ColorFactor_].Get(), r, g, b);
          }
          // Color by curl in Z
          if ((int)std::round(D.param[ColorMode___].Get()) == 7) {
            if (std::abs(CurZ[x][y][z]) < D.param[ColorThresh_].Get()) continue;
            Colormap::RatioToJetBrightSmooth(0.5f + 2.0f * CurZ[x][y][z] * D.param[ColorFactor_].Get(), r, g, b);
          }
          // Color by divergence
          if ((int)std::round(D.param[ColorMode___].Get()) == 8) {
            if (std::abs(Dive[x][y][z]) < D.param[ColorThresh_].Get()) continue;
            Colormap::RatioToBlueToRed(0.5f + 0.5f * (Dive[x][y][z] / voxSize) * D.param[ColorFactor_].Get(), r, g, b);
          }
          // Color by dummy values
          if ((int)std::round(D.param[ColorMode___].Get()) >= 10 && (int)std::round(D.param[ColorMode___].Get()) <= 14) {
            float val= 0.0f;
            if ((int)std::round(D.param[ColorMode___].Get()) == 10) val= Dum0[x][y][z];
            if ((int)std::round(D.param[ColorMode___].Get()) == 11) val= Dum1[x][y][z];
            if ((int)std::round(D.param[ColorMode___].Get()) == 12) val= Dum2[x][y][z];
            if ((int)std::round(D.param[ColorMode___].Get()) == 13) val= Dum3[x][y][z];
            if ((int)std::round(D.param[ColorMode___].Get()) == 14) val= Dum4[x][y][z];
            if (std::abs(val) < D.param[ColorThresh_].Get()) continue;
            Colormap::RatioToJetBrightSmooth(0.5f + 0.5f * val * D.param[ColorFactor_].Get(), r, g, b);
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
    constexpr int nbLineWidths= 3;
    // Set the scene transformation
    glPushMatrix();
    if (nbX == 1) glTranslatef(voxSize, 0.0f, 0.0f);
    if (nbY == 1) glTranslatef(0.0f, voxSize, 0.0f);
    if (nbZ == 1) glTranslatef(0.0f, 0.0f, voxSize);
    glTranslatef(D.boxMin[0] + 0.5f * voxSize, D.boxMin[1] + 0.5f * voxSize, D.boxMin[2] + 0.5f * voxSize);
    glScalef(voxSize, voxSize, voxSize);
    // Sweep the field
    for (int k= 0; k < nbLineWidths; k++) {
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
              glVertex3fv(pos + vec.normalized() * 5.0f * std::log(vec.norm() + 1.0f) * (1.0f - ((float)k / (float)nbLineWidths)));
            }
          }
        }
      }
      glEnd();
    }
    glLineWidth(1.0f);
    glPopMatrix();
  }

  // // Testing colormaps
  // if (D.displayMode5) {
  //   // Set the scene transformation
  //   glPushMatrix();
  //   glTranslatef(D.boxMin[0] + 0.5f * voxSize, D.boxMin[1] + 0.5f * voxSize, D.boxMin[2] + 0.5f * voxSize);
  //   glScalef(voxSize, voxSize, voxSize);
  //   if (nbX == 1) glScalef(0.1f, 1.0f, 1.0f);
  //   if (nbY == 1) glScalef(1.0f, 0.1f, 1.0f);
  //   if (nbZ == 1) glScalef(1.0f, 1.0f, 0.1f);
  //   // Sweep the field
  //   for (int x= 0; x < nbX; x++) {
  //     for (int y= 0; y < nbY; y++) {
  //       for (int z= 0; z < nbZ; z++) {
  //         float r= 0.0f, g= 0.0f, b= 0.0f;
  //         if (z / 5 == 3) Colormap::RatioToBlackBody((float)y / (float)(nbY - 1), r, g, b);
  //         if (z / 5 == 4) Colormap::RatioToBlueToRed((float)y / (float)(nbY - 1), r, g, b);
  //         if (z / 5 == 5) Colormap::RatioToGrayscale((float)y / (float)(nbY - 1), r, g, b);
  //         if (z / 5 == 6) Colormap::RatioToGreenToRed((float)y / (float)(nbY - 1), r, g, b);
  //         if (z / 5 == 7) Colormap::RatioToJet((float)y / (float)(nbY - 1), r, g, b);
  //         if (z / 5 == 8) Colormap::RatioToJetBright((float)y / (float)(nbY - 1), r, g, b);
  //         if (z / 5 == 9) Colormap::RatioToJetBrightSmooth((float)y / (float)(nbY - 1), r, g, b);
  //         if (z / 5 == 10) Colormap::RatioToJetSmooth((float)y / (float)(nbY - 1), r, g, b);
  //         if (z / 5 == 11) Colormap::RatioToPlasma((float)y / (float)(nbY - 1), r, g, b);
  //         if (z / 5 == 12) Colormap::RatioToRainbow((float)y / (float)(nbY - 1), r, g, b);
  //         if (z / 5 == 13) Colormap::RatioToRedGreenBlueRed((float)y / (float)(nbY - 1), r, g, b);
  //         if (z / 5 == 14) Colormap::RatioToTurbo((float)y / (float)(nbY - 1), r, g, b);
  //         if (z / 5 == 15) Colormap::RatioToViridis((float)y / (float)(nbY - 1), r, g, b);
  //         if (z / 5 == 16) Colormap::RatioBands5((float)y / (float)(nbY - 1), r, g, b);
  //         if (z / 5 == 17) Colormap::RatioBands10((float)y / (float)(nbY - 1), r, g, b);
  //         if (z / 5 == 18) Colormap::RatioBands20((float)y / (float)(nbY - 1), r, g, b);
  //         glColor3f(r, g, b);
  //         glPushMatrix();
  //         glTranslatef((float)x, (float)y, (float)z);
  //         if (nbX > 1 && nbY > 1 && nbZ > 1) glutWireCube(1.0);
  //         else glutSolidCube(1.0);
  //         glPopMatrix();
  //       }
  //     }
  //   }
  //   glPopMatrix();
  // }
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
  // Sweep through the field and set forced values
#pragma omp parallel for
  for (int x= 0; x < nbX; x++) {
    for (int y= 0; y < nbY; y++) {
      for (int z= 0; z < nbZ; z++) {
        if (iFieldID == FieldID::IDSmok && SmoBC[x][y][z]) ioField[x][y][z]= SmokForced[x][y][z];
        if (iFieldID == FieldID::IDVelX && Solid[x][y][z]) ioField[x][y][z]= 0.0f;
        if (iFieldID == FieldID::IDVelY && Solid[x][y][z]) ioField[x][y][z]= 0.0f;
        if (iFieldID == FieldID::IDVelZ && Solid[x][y][z]) ioField[x][y][z]= 0.0f;
        if (iFieldID == FieldID::IDVelX && VelBC[x][y][z]) ioField[x][y][z]= VelXForced[x][y][z];
        if (iFieldID == FieldID::IDVelY && VelBC[x][y][z]) ioField[x][y][z]= VelYForced[x][y][z];
        if (iFieldID == FieldID::IDVelZ && VelBC[x][y][z]) ioField[x][y][z]= VelZForced[x][y][z];
        if (iFieldID == FieldID::IDPres && PreBC[x][y][z]) ioField[x][y][z]= PresForced[x][y][z];
      }
    }
  }

  // Sweep through the field and set continuity values
  std::vector<std::vector<std::vector<float>>> oldField= ioField;
#pragma omp parallel for
  for (int x= 0; x < nbX; x++) {
    for (int y= 0; y < nbY; y++) {
      for (int z= 0; z < nbZ; z++) {
        // Check if continuity should be applied
        bool applyContinuity= false;
        if (iFieldID == FieldID::IDSmok && Solid[x][y][z]) applyContinuity= true;
        if (iFieldID == FieldID::IDPres && Solid[x][y][z]) applyContinuity= true;
        // Apply continuity
        if (applyContinuity) {
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
          if (count > 0) ioField[x][y][z]/= (float)count;
        }
      }
    }
  }
}


void CompuFluidDyna::ImplicitFieldAdd(const std::vector<std::vector<std::vector<float>>>& iFieldA,
                                      const std::vector<std::vector<std::vector<float>>>& iFieldB,
                                      std::vector<std::vector<std::vector<float>>>& oField) {
  for (int x= 0; x < nbX; x++)
    for (int y= 0; y < nbY; y++)
      for (int z= 0; z < nbZ; z++)
        oField[x][y][z]= iFieldA[x][y][z] + iFieldB[x][y][z];
}


void CompuFluidDyna::ImplicitFieldSub(const std::vector<std::vector<std::vector<float>>>& iFieldA,
                                      const std::vector<std::vector<std::vector<float>>>& iFieldB,
                                      std::vector<std::vector<std::vector<float>>>& oField) {
  for (int x= 0; x < nbX; x++)
    for (int y= 0; y < nbY; y++)
      for (int z= 0; z < nbZ; z++)
        oField[x][y][z]= iFieldA[x][y][z] - iFieldB[x][y][z];
}


void CompuFluidDyna::ImplicitFieldScale(const float iVal,
                                        const std::vector<std::vector<std::vector<float>>>& iField,
                                        std::vector<std::vector<std::vector<float>>>& oField) {
  for (int x= 0; x < nbX; x++)
    for (int y= 0; y < nbY; y++)
      for (int z= 0; z < nbZ; z++)
        oField[x][y][z]= iField[x][y][z] * iVal;
}


float CompuFluidDyna::ImplicitFieldDotProd(const std::vector<std::vector<std::vector<float>>>& iFieldA,
                                           const std::vector<std::vector<std::vector<float>>>& iFieldB) {
  float val= 0.0f;
  for (int x= 0; x < nbX; x++)
    for (int y= 0; y < nbY; y++)
      for (int z= 0; z < nbZ; z++)
        val+= iFieldA[x][y][z] * iFieldB[x][y][z];
  return val;
}


void CompuFluidDyna::ImplicitFieldLaplacianMatMult(const int iFieldID, const float iTimeStep,
                                                   const bool iDiffuMode, const float iDiffuCoeff, const bool iPrecondMode,
                                                   const std::vector<std::vector<std::vector<float>>>& iField,
                                                   std::vector<std::vector<std::vector<float>>>& oField) {
  // Precompute value
  const float diffuVal= iDiffuCoeff * iTimeStep / (voxSize * voxSize);
  // Sweep through the field
#pragma omp parallel for
  for (int x= 0; x < nbX; x++) {
    for (int y= 0; y < nbY; y++) {
      for (int z= 0; z < nbZ; z++) {
        // Skip solid or fixed values
        if (Solid[x][y][z]) continue;
        if (SmoBC[x][y][z] && iFieldID == FieldID::IDSmok) continue;
        if (VelBC[x][y][z] && iFieldID == FieldID::IDVelX) continue;
        if (VelBC[x][y][z] && iFieldID == FieldID::IDVelY) continue;
        if (VelBC[x][y][z] && iFieldID == FieldID::IDVelZ) continue;
        if (PreBC[x][y][z] && iFieldID == FieldID::IDPres) continue;
        // Get count and sum of valid neighbors
        int count= 0;
        float sum= 0.0f;
        for (int k= 0; k < MaskSize; k++) {
          const int xOff= x + Mask[k][0];
          const int yOff= y + Mask[k][1];
          const int zOff= z + Mask[k][2];
          if (xOff < 0 || xOff >= nbX || yOff < 0 || yOff >= nbY || zOff < 0 || zOff >= nbZ) continue;
          sum+= iField[xOff][yOff][zOff];
          count++;
        }
        // Apply linear expression
        if (iDiffuMode) {
          if (iPrecondMode)
            oField[x][y][z]= 1.0f / (1.0f + diffuVal * (float)count) * iField[x][y][z];
          else
            oField[x][y][z]= -diffuVal * sum + (1.0f + diffuVal * (float)count) * iField[x][y][z];
        }
        else {
          if (iPrecondMode)
            oField[x][y][z]= 1.0f / (float)count * iField[x][y][z];
          else
            oField[x][y][z]= sum - (float)count * iField[x][y][z];
        }
      }
    }
  }
}


void CompuFluidDyna::ConjugateGradientSolve(const int iFieldID, const int iMaxIter, const float iTimeStep,
                                            const bool iDiffuMode, const float iDiffuCoeff,
                                            const std::vector<std::vector<std::vector<float>>>& iField,
                                            std::vector<std::vector<std::vector<float>>>& ioField) {
  // // Skip if non changing field
  // if (iDiffuMode && iDiffuCoeff == 0.0f) return;
  // if (iFieldID == FieldID::IDVelX && nbX == 1) return;
  // if (iFieldID == FieldID::IDVelY && nbY == 1) return;
  // if (iFieldID == FieldID::IDVelZ && nbZ == 1) return;

  // // Reset solution value
  // for (int x= 0; x < nbX; x++)
  //   for (int y= 0; y < nbY; y++)
  //     for (int z= 0; z < nbZ; z++)
  //       ioField[x][y][z]= 0.0f;
  // ApplyBC(iFieldID, ioField);

  // Prepare convergence plot
  D.plotLegend.resize(5);
  D.plotLegend[FieldID::IDSmok]= "Diffu S";
  D.plotLegend[FieldID::IDVelX]= "Diffu VX";
  D.plotLegend[FieldID::IDVelY]= "Diffu VY";
  D.plotLegend[FieldID::IDVelZ]= "Diffu VZ";
  D.plotLegend[FieldID::IDPres]= "Proj  P";
  D.plotData.resize(5);
  D.plotData[iFieldID].clear();
  const float normRHS= ImplicitFieldDotProd(iField, iField);

  // Allocate fields
  std::vector<std::vector<std::vector<float>>> rField= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  std::vector<std::vector<std::vector<float>>> qField= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  std::vector<std::vector<std::vector<float>>> dField= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  std::vector<std::vector<std::vector<float>>> sField= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  std::vector<std::vector<std::vector<float>>> t0Field= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  std::vector<std::vector<std::vector<float>>> t1Field= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);

  // r = b - A x
  ImplicitFieldLaplacianMatMult(iFieldID, iTimeStep, iDiffuMode, iDiffuCoeff, false, ioField, t0Field);
  ApplyBC(iFieldID, t0Field);
  ImplicitFieldSub(iField, t0Field, rField);

  // Error plot
  float errTmp= ImplicitFieldDotProd(rField, rField);
  if (D.param[Verbose_____].Get() > 0.0) printf("CG [%.3e] ", normRHS);
  if (D.param[Verbose_____].Get() > 0.0) printf("%.3e ", (normRHS != 0.0f) ? (errTmp / normRHS) : (0.0f));
  D.plotData[iFieldID].push_back((normRHS != 0.0f) ? errTmp / normRHS : 0.0f);

  // d = M^-1 r
  ImplicitFieldLaplacianMatMult(iFieldID, iTimeStep, iDiffuMode, iDiffuCoeff, true, rField, dField);

  // errNew = r^T d
  float errNew= ImplicitFieldDotProd(rField, dField);
  float errBeg= errNew;
  float errOld= 0.0f;

  // Iterate to solve
  for (int k= 0; k < iMaxIter; k++) {
    if (errNew / normRHS < (float)D.param[SolvTolRhs__].Get()) break;
    if (errNew / errBeg < (float)D.param[SolvTolRel__].Get()) break;

    // q = A d
    ImplicitFieldLaplacianMatMult(iFieldID, iTimeStep, iDiffuMode, iDiffuCoeff, false, dField, qField);

    // alpha = errNew / (d^T q)
    float denom= ImplicitFieldDotProd(dField, qField);
    if (denom == 0.0) {
      if (D.param[Verbose_____].Get() > 0.0) printf("div by zero denom");
      break;
    }
    float alpha= errNew / denom;

    // x = x + alpha d
    ImplicitFieldScale(alpha, dField, t0Field);
    ImplicitFieldAdd(ioField, t0Field, t1Field);
    ioField= t1Field;
    ApplyBC(iFieldID, ioField);

    // r = r - alpha q
    ImplicitFieldScale(alpha, qField, t0Field);
    ImplicitFieldSub(rField, t0Field, t1Field);
    rField= t1Field;

    // Error plot
    errTmp= ImplicitFieldDotProd(rField, rField);
    if (D.param[Verbose_____].Get() > 0.0) printf("%.3e ", (normRHS != 0.0f) ? (errTmp / normRHS) : (0.0f));
    D.plotData[iFieldID].push_back((normRHS != 0.0f) ? (errTmp / normRHS) : (0.0f));

    // s = M^-1 r
    ImplicitFieldLaplacianMatMult(iFieldID, iTimeStep, iDiffuMode, iDiffuCoeff, true, rField, sField);

    // errNew = r^T s
    errOld= errNew;
    errNew= ImplicitFieldDotProd(rField, sField);
    if (errOld == 0.0) {
      if (D.param[Verbose_____].Get() > 0.0) printf("div by zero errOld");
      break;
    }

    // beta = errNew / errOld
    float beta= errNew / errOld;

    // d = s + beta d
    ImplicitFieldScale(beta, dField, t0Field);
    ImplicitFieldAdd(sField, t0Field, dField);
  }
  if (iFieldID == FieldID::IDSmok) Dum0= rField;
  if (iFieldID == FieldID::IDVelX) Dum1= rField;
  if (iFieldID == FieldID::IDVelY) Dum2= rField;
  if (iFieldID == FieldID::IDVelZ) Dum3= rField;
  if (iFieldID == FieldID::IDPres) Dum4= rField;
  if (D.param[Verbose_____].Get() > 0.0) printf("\n");
}


void CompuFluidDyna::GaussSeidelSolve(const int iFieldID, const int iMaxIter, const float iTimeStep,
                                      const bool iDiffuMode, const float iDiffuCoeff,
                                      const std::vector<std::vector<std::vector<float>>>& iField,
                                      std::vector<std::vector<std::vector<float>>>& ioField) {
  // // Skip if non changing field
  // if (iDiffuMode && iDiffuCoeff == 0.0f) return;
  // if (iFieldID == FieldID::IDVelX && nbX == 1) return;
  // if (iFieldID == FieldID::IDVelY && nbY == 1) return;
  // if (iFieldID == FieldID::IDVelZ && nbZ == 1) return;

  // // Reset solution value
  // for (int x= 0; x < nbX; x++)
  //   for (int y= 0; y < nbY; y++)
  //     for (int z= 0; z < nbZ; z++)
  //       ioField[x][y][z]= 0.0f;
  // ApplyBC(iFieldID, ioField);

  // Prepare convergence plot
  D.plotLegend.resize(5);
  D.plotLegend[FieldID::IDSmok]= "Diffu S";
  D.plotLegend[FieldID::IDVelX]= "Diffu VX";
  D.plotLegend[FieldID::IDVelY]= "Diffu VY";
  D.plotLegend[FieldID::IDVelZ]= "Diffu VZ";
  D.plotLegend[FieldID::IDPres]= "Proj  P";
  D.plotData.resize(5);
  D.plotData[iFieldID].clear();
  const float normRHS= ImplicitFieldDotProd(iField, iField);

  // Get parameters
  const float diffuVal= iTimeStep * (float)(nbX * nbY * nbZ) * iDiffuCoeff;
  const float coeffOverrelax= std::min(std::max((float)D.param[SolvSOR_____].Get(), 0.0f), 1.99f);

  std::vector<std::vector<std::vector<float>>> rField= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  std::vector<std::vector<std::vector<float>>> t0Field= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  ImplicitFieldLaplacianMatMult(iFieldID, iTimeStep, iDiffuMode, iDiffuCoeff, false, ioField, t0Field);
  ApplyBC(iFieldID, t0Field);
  ImplicitFieldSub(iField, t0Field, rField);
  float errNew= ImplicitFieldDotProd(rField, rField);
  float errBeg= errNew;
  if (D.param[Verbose_____].Get() > 0.0) printf("GS [%.3e] ", normRHS);
  if (D.param[Verbose_____].Get() > 0.0) printf("%.3e ", (normRHS != 0.0f) ? (errNew / normRHS) : (0.0f));
  D.plotData[iFieldID].push_back((normRHS != 0.0f) ? (errNew / normRHS) : (0.0f));

  // Solve with PArallel BIdirectionnal GAuss-Seidel Successive Over-Relaxation (PABIGASSOR)
  for (int k= 0; k < iMaxIter; k++) {
    if (errNew / normRHS < (float)D.param[SolvTolRhs__].Get()) break;
    if (errNew / errBeg < (float)D.param[SolvTolRel__].Get()) break;
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
              // Skip solid or fixed values
              if (Solid[x][y][z]) continue;
              if (SmoBC[x][y][z] && iFieldID == FieldID::IDSmok) continue;
              if (VelBC[x][y][z] && iFieldID == FieldID::IDVelX) continue;
              if (VelBC[x][y][z] && iFieldID == FieldID::IDVelY) continue;
              if (VelBC[x][y][z] && iFieldID == FieldID::IDVelZ) continue;
              if (PreBC[x][y][z] && iFieldID == FieldID::IDPres) continue;
              // Get count and sum of valid neighbors
              int count= 0;
              float sum= 0.0f;
              for (int k= 0; k < MaskSize; k++) {
                const int xOff= x + Mask[k][0];
                const int yOff= y + Mask[k][1];
                const int zOff= z + Mask[k][2];
                if (xOff < 0 || xOff >= nbX || yOff < 0 || yOff >= nbY || zOff < 0 || zOff >= nbZ) continue;
                if (Solid[xOff][yOff][zOff]) continue;
                sum+= FieldA[xOff][yOff][zOff];
                count++;
              }
              // Set new value according to coefficients and flags
              const float prevVal= FieldA[x][y][z];
              if (iDiffuMode)
                FieldA[x][y][z]= (iField[x][y][z] + diffuVal * sum) / (1.0f + diffuVal * (float)count);
              else if (count > 0)
                FieldA[x][y][z]= (sum - iField[x][y][z]) / (float)count;
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
              // Skip solid or fixed values
              if (Solid[x][y][z]) continue;
              if (SmoBC[x][y][z] && iFieldID == FieldID::IDSmok) continue;
              if (VelBC[x][y][z] && iFieldID == FieldID::IDVelX) continue;
              if (VelBC[x][y][z] && iFieldID == FieldID::IDVelY) continue;
              if (VelBC[x][y][z] && iFieldID == FieldID::IDVelZ) continue;
              if (PreBC[x][y][z] && iFieldID == FieldID::IDPres) continue;
              // Get count and sum of valid neighbors
              int count= 0;
              float sum= 0.0f;
              for (int k= 0; k < MaskSize; k++) {
                const int xOff= x + Mask[k][0];
                const int yOff= y + Mask[k][1];
                const int zOff= z + Mask[k][2];
                if (xOff < 0 || xOff >= nbX || yOff < 0 || yOff >= nbY || zOff < 0 || zOff >= nbZ) continue;
                if (Solid[xOff][yOff][zOff]) continue;
                sum+= FieldB[xOff][yOff][zOff];
                count++;
              }
              // Set new value according to coefficients and flags
              const float prevVal= FieldB[x][y][z];
              if (iDiffuMode)
                FieldB[x][y][z]= (iField[x][y][z] + diffuVal * sum) / (1.0f + diffuVal * (float)count);
              else if (count > 0)
                FieldB[x][y][z]= (sum - iField[x][y][z]) / (float)count;
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
    }
    // Reapply BC to maintain consistency
    ApplyBC(iFieldID, ioField);
    ImplicitFieldLaplacianMatMult(iFieldID, iTimeStep, iDiffuMode, iDiffuCoeff, false, ioField, t0Field);
    ApplyBC(iFieldID, t0Field);
    ImplicitFieldSub(iField, t0Field, rField);
    errNew= ImplicitFieldDotProd(rField, rField);
    if (D.param[Verbose_____].Get() > 0.0) printf("%.3e ", (normRHS != 0.0f) ? (errNew / normRHS) : (0.0f));
    D.plotData[iFieldID].push_back((normRHS != 0.0f) ? (errNew / normRHS) : (0.0f));
  }
  if (iFieldID == FieldID::IDSmok) Dum0= rField;
  if (iFieldID == FieldID::IDVelX) Dum1= rField;
  if (iFieldID == FieldID::IDVelY) Dum2= rField;
  if (iFieldID == FieldID::IDVelZ) Dum3= rField;
  if (iFieldID == FieldID::IDPres) Dum4= rField;
  if (D.param[Verbose_____].Get() > 0.0) printf("\n");
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
        Dive[x][y][z]= 0.0f;
        if (Solid[x][y][z] || PreBC[x][y][z]) continue;
        if (x - 1 >= 0 && x + 1 < nbX) Dive[x][y][z]+= 0.5f * (ioVelX[x + 1][y][z] - ioVelX[x - 1][y][z]) * voxSize;
        if (y - 1 >= 0 && y + 1 < nbY) Dive[x][y][z]+= 0.5f * (ioVelY[x][y + 1][z] - ioVelY[x][y - 1][z]) * voxSize;
        if (z - 1 >= 0 && z + 1 < nbZ) Dive[x][y][z]+= 0.5f * (ioVelZ[x][y][z + 1] - ioVelZ[x][y][z - 1]) * voxSize;
      }
    }
  }
  ApplyBC(FieldID::IDPres, Dive);

  // Solve for pressure
  if (D.param[SolvCGPress_].Get() < 0.0) {
    GaussSeidelSolve(FieldID::IDPres, iIter, iTimeStep, false, 0.0f, Dive, Pres);
  }
  else {
    ConjugateGradientSolve(FieldID::IDPres, iIter, iTimeStep, false, 0.0f, Dive, Pres);
  }

  // Update velocities based on neighboring pressures
#pragma omp parallel for
  for (int x= 0; x < nbX; x++) {
    for (int y= 0; y < nbY; y++) {
      for (int z= 0; z < nbZ; z++) {
        if (Solid[x][y][z] || VelBC[x][y][z]) continue;
        if (x - 1 >= 0 && x + 1 < nbX) ioVelX[x][y][z]-= 0.5f * (Pres[x + 1][y][z] - Pres[x - 1][y][z]) / voxSize;
        if (y - 1 >= 0 && y + 1 < nbY) ioVelY[x][y][z]-= 0.5f * (Pres[x][y + 1][z] - Pres[x][y - 1][z]) / voxSize;
        if (z - 1 >= 0 && z + 1 < nbZ) ioVelZ[x][y][z]-= 0.5f * (Pres[x][y][z + 1] - Pres[x][y][z - 1]) / voxSize;
      }
    }
  }

  // Reapply BC to maintain consistency
  ApplyBC(FieldID::IDVelX, ioVelX);
  ApplyBC(FieldID::IDVelY, ioVelY);
  ApplyBC(FieldID::IDVelZ, ioVelZ);
}


float CompuFluidDyna::TrilinearInterpolation(const float iPosX, const float iPosY, const float iPosZ,
                                             const std::vector<std::vector<std::vector<float>>>& iFieldRef) {
  // Get floor and ceil voxel indices
  const int x0= std::min(std::max((int)std::floor(iPosX), 0), nbX - 1);
  const int y0= std::min(std::max((int)std::floor(iPosY), 0), nbY - 1);
  const int z0= std::min(std::max((int)std::floor(iPosZ), 0), nbZ - 1);
  const int x1= std::min(std::max((int)std::ceil(iPosX), 0), nbX - 1);
  const int y1= std::min(std::max((int)std::ceil(iPosY), 0), nbY - 1);
  const int z1= std::min(std::max((int)std::ceil(iPosZ), 0), nbZ - 1);

  // Get floor and ceil voxel weights
  const float xWeight1= iPosX - (float)x0;
  const float yWeight1= iPosY - (float)y0;
  const float zWeight1= iPosZ - (float)z0;
  const float xWeight0= 1.0f - xWeight1;
  const float yWeight0= 1.0f - yWeight1;
  const float zWeight0= 1.0f - zWeight1;

  // Compute the weighted sum
  return iFieldRef[x0][y0][z0] * (xWeight0 * yWeight0 * zWeight0) +
         iFieldRef[x0][y0][z1] * (xWeight0 * yWeight0 * zWeight1) +
         iFieldRef[x0][y1][z0] * (xWeight0 * yWeight1 * zWeight0) +
         iFieldRef[x0][y1][z1] * (xWeight0 * yWeight1 * zWeight1) +
         iFieldRef[x1][y0][z0] * (xWeight1 * yWeight0 * zWeight0) +
         iFieldRef[x1][y0][z1] * (xWeight1 * yWeight0 * zWeight1) +
         iFieldRef[x1][y1][z0] * (xWeight1 * yWeight1 * zWeight0) +
         iFieldRef[x1][y1][z1] * (xWeight1 * yWeight1 * zWeight1);
}


void CompuFluidDyna::AdvectField(const int iFieldID, const float iTimeStep,
                                 const std::vector<std::vector<std::vector<float>>>& iVelX,
                                 const std::vector<std::vector<std::vector<float>>>& iVelY,
                                 const std::vector<std::vector<std::vector<float>>>& iVelZ,
                                 std::vector<std::vector<std::vector<float>>>& ioField) {
  // Sweep through field and apply semi Lagrangian advection
  std::vector<std::vector<std::vector<float>>> oldField= ioField;
#pragma omp parallel for
  for (int x= 0; x < nbX; x++) {
    for (int y= 0; y < nbY; y++) {
      for (int z= 0; z < nbZ; z++) {
        // Skip solid or fixed values
        if (Solid[x][y][z]) continue;
        if (SmoBC[x][y][z] && iFieldID == FieldID::IDSmok) continue;
        if (VelBC[x][y][z] && iFieldID == FieldID::IDVelX) continue;
        if (VelBC[x][y][z] && iFieldID == FieldID::IDVelY) continue;
        if (VelBC[x][y][z] && iFieldID == FieldID::IDVelZ) continue;

        // TODO try MacCormack to get 2nd order accuracy
        // https://commons.wikimedia.org/wiki/File:Backtracking_maccormack.png
        // https://physbam.stanford.edu/~fedkiw/papers/stanford2006-09.pdf
        // https://github.com/NiallHornFX/StableFluids3D-GL/blob/master/src/fluidsolver3d.cpp

        // Find source position of current voxel via backtracing
        float posX= (float)x - iTimeStep * iVelX[x][y][z] / voxSize;
        float posY= (float)y - iTimeStep * iVelY[x][y][z] / voxSize;
        float posZ= (float)z - iTimeStep * iVelZ[x][y][z] / voxSize;

        // Trilinear interpolation at source position
        ioField[x][y][z]= TrilinearInterpolation(posX, posY, posZ, oldField);
      }
    }
  }
  // Reapply BC to maintain consistency
  ApplyBC(iFieldID, ioField);
}


void CompuFluidDyna::VorticityConfinement(const float iTimeStep, const float iVortiCoeff,
                                          std::vector<std::vector<std::vector<float>>>& ioVelX,
                                          std::vector<std::vector<std::vector<float>>>& ioVelY,
                                          std::vector<std::vector<std::vector<float>>>& ioVelZ) {
  // Reset fields
  CurX= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  CurY= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  CurZ= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  Vort= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);

  // Compute curl and vorticity
#pragma omp parallel for
  for (int x= 0; x < nbX; x++) {
    for (int y= 0; y < nbY; y++) {
      for (int z= 0; z < nbZ; z++) {
        if (Solid[x][y][z]) continue;
        float dy_dx= 0.0f, dz_dx= 0.0f, dx_dy= 0.0f, dz_dy= 0.0f, dx_dz= 0.0f, dy_dz= 0.0f;
        if (x - 1 >= 0 && x + 1 < nbX) dy_dx= 0.5f * (ioVelY[x + 1][y][z] - ioVelY[x - 1][y][z]);
        if (x - 1 >= 0 && x + 1 < nbX) dz_dx= 0.5f * (ioVelZ[x + 1][y][z] - ioVelZ[x - 1][y][z]);
        if (y - 1 >= 0 && y + 1 < nbY) dx_dy= 0.5f * (ioVelX[x][y + 1][z] - ioVelX[x][y - 1][z]);
        if (y - 1 >= 0 && y + 1 < nbY) dz_dy= 0.5f * (ioVelZ[x][y + 1][z] - ioVelZ[x][y - 1][z]);
        if (z - 1 >= 0 && z + 1 < nbZ) dx_dz= 0.5f * (ioVelX[x][y][z + 1] - ioVelX[x][y][z - 1]);
        if (z - 1 >= 0 && z + 1 < nbZ) dy_dz= 0.5f * (ioVelY[x][y][z + 1] - ioVelY[x][y][z - 1]);
        CurX[x][y][z]= dz_dy - dy_dz;
        CurY[x][y][z]= dx_dz - dz_dx;
        CurZ[x][y][z]= dy_dx - dx_dy;
        Vort[x][y][z]= std::sqrt(CurX[x][y][z] * CurX[x][y][z] + CurY[x][y][z] * CurY[x][y][z] + CurZ[x][y][z] * CurZ[x][y][z]);
      }
    }
  }

  // Amplify non-zero vorticity
  if (iVortiCoeff > 0.0f) {
#pragma omp parallel for
    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          if (Solid[x][y][z] || VelBC[x][y][z]) continue;
          float dVort_dx= 0.0f;
          float dVort_dy= 0.0f;
          float dVort_dz= 0.0f;
          if (x - 1 >= 0 && x + 1 < nbX) dVort_dx= 0.5f * (Vort[x + 1][y][z] - Vort[x - 1][y][z]);
          if (y - 1 >= 0 && y + 1 < nbY) dVort_dy= 0.5f * (Vort[x][y + 1][z] - Vort[x][y - 1][z]);
          if (z - 1 >= 0 && z + 1 < nbZ) dVort_dz= 0.5f * (Vort[x][y][z + 1] - Vort[x][y][z - 1]);
          const float dVortNorm= std::sqrt(dVort_dx * dVort_dx + dVort_dy * dVort_dy + dVort_dz * dVort_dz);
          if (dVortNorm > 0.0f) {
            dVort_dx= iVortiCoeff * dVort_dx / dVortNorm;
            dVort_dy= iVortiCoeff * dVort_dy / dVortNorm;
            dVort_dz= iVortiCoeff * dVort_dz / dVortNorm;
            ioVelX[x][y][z]+= iTimeStep * (dVort_dy * CurZ[x][y][z] - dVort_dz * CurY[x][y][z]);
            ioVelY[x][y][z]+= iTimeStep * (dVort_dz * CurX[x][y][z] - dVort_dx * CurZ[x][y][z]);
            ioVelZ[x][y][z]+= iTimeStep * (dVort_dx * CurY[x][y][z] - dVort_dy * CurX[x][y][z]);
          }
        }
      }
    }

    // Reapply BC to maintain consistency
    ApplyBC(FieldID::IDVelX, ioVelX);
    ApplyBC(FieldID::IDVelY, ioVelY);
    ApplyBC(FieldID::IDVelZ, ioVelZ);
  }
}
