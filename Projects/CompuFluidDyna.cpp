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
  VoxelSize___,
  TimeStep____,
  SolvMaxIter_,
  SolvTolRhs__,
  SolvTolRel__,
  CoeffAdvecS_,
  CoeffAdvecV_,
  CoeffDiffuS_,
  CoeffDiffuV_,
  CoeffVorti__,
  CoeffProj___,
  CoeffVelX___,
  CoeffVelY___,
  CoeffVelZ___,
  CoeffPres___,
  CoeffSmok___,
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
    D.UI.clear();
    D.UI.push_back(ParamUI("Scenario____", 6));        // Scenario ID, 0= load file, 1> hard coded scenarii
    D.UI.push_back(ParamUI("InputFile___", 2));        // BMP file to load
    D.UI.push_back(ParamUI("ResolutionX_", 1));        // Eulerian mesh resolution
    D.UI.push_back(ParamUI("ResolutionY_", 50));       // Eulerian mesh resolution
    D.UI.push_back(ParamUI("ResolutionZ_", 50));       // Eulerian mesh resolution
    D.UI.push_back(ParamUI("VoxelSize___", 0.01));     // Element size
    D.UI.push_back(ParamUI("TimeStep____", 0.02));     // Simulation time step
    D.UI.push_back(ParamUI("SolvMaxIter_", 50));       // Max number of solver iterations
    D.UI.push_back(ParamUI("SolvTolRhs__", 0.0));      // Solver tolerance relative to RHS norm
    D.UI.push_back(ParamUI("SolvTolRel__", 1.e-4));    // Solver tolerance relative to initial guess
    D.UI.push_back(ParamUI("CoeffAdvecS_", 2.0));      // 0= no advection, 1= linear advection, 2= MacCormack advection
    D.UI.push_back(ParamUI("CoeffAdvecV_", 2.0));      // 0= no advection, 1= linear advection, 2= MacCormack advection
    D.UI.push_back(ParamUI("CoeffDiffuS_", 0.00001));  // Diffusion of smoke field
    D.UI.push_back(ParamUI("CoeffDiffuV_", 0.001));    // Diffusion of velocity field, i.e viscosity
    D.UI.push_back(ParamUI("CoeffVorti__", 0.0));      // Vorticity confinement
    D.UI.push_back(ParamUI("CoeffProj___", 1.0));      // 0 = No correction, 1= incompressiblity correction, 2= time dependant correction
    D.UI.push_back(ParamUI("CoeffVelX___", 0.0));      // Velocity value for voxels with enforced velocity
    D.UI.push_back(ParamUI("CoeffVelY___", 1.0));      // Velocity value for voxels with enforced velocity
    D.UI.push_back(ParamUI("CoeffVelZ___", 0.0));      // Velocity value for voxels with enforced velocity
    D.UI.push_back(ParamUI("CoeffPres___", 0.02));     // Pressure value for voxels with enforced pressure
    D.UI.push_back(ParamUI("CoeffSmok___", 1.0));      // Smoke value for voxels with enforced smoke
    D.UI.push_back(ParamUI("ObjectPosX__", 0.5));      // Coordinates for objects in hard coded scenarios
    D.UI.push_back(ParamUI("ObjectPosY__", 0.25));     // Coordinates for objects in hard coded scenarios
    D.UI.push_back(ParamUI("ObjectPosZ__", 0.5));      // Coordinates for objects in hard coded scenarios
    D.UI.push_back(ParamUI("ObjectSize__", 0.08));     // Size for objects in hard coded scenarios
    D.UI.push_back(ParamUI("ScaleFactor_", 1.0));      // Scale factor for drawn geometry
    D.UI.push_back(ParamUI("ColorFactor_", 1.0));      // Color factor for drawn geometry
    D.UI.push_back(ParamUI("ColorThresh_", 0.0));      // Color cutoff drawn geometry
    D.UI.push_back(ParamUI("ColorMode___", 2));        // Selector for the scalar field to be drawn
    D.UI.push_back(ParamUI("SlicePlotX__", 0.5));      // Positions for the scatter plot slices
    D.UI.push_back(ParamUI("SlicePlotY__", 0.5));      // Positions for the scatter plot slices
    D.UI.push_back(ParamUI("SlicePlotZ__", 0.5));      // Positions for the scatter plot slices
    D.UI.push_back(ParamUI("Verbose_____", -0.5));     // Verbose mode
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
  if (D.UI[Scenario____].hasChanged() ||
      D.UI[InputFile___].hasChanged() ||
      D.UI[ResolutionX_].hasChanged() ||
      D.UI[ResolutionY_].hasChanged() ||
      D.UI[ResolutionZ_].hasChanged() ||
      D.UI[VoxelSize___].hasChanged()) isAllocated= false;
}


// Check if parameter changes should trigger a refresh
void CompuFluidDyna::CheckRefresh() {
  if (D.UI[CoeffVelX___].hasChanged() ||
      D.UI[CoeffVelY___].hasChanged() ||
      D.UI[CoeffVelZ___].hasChanged() ||
      D.UI[CoeffPres___].hasChanged() ||
      D.UI[CoeffSmok___].hasChanged() ||
      D.UI[ObjectPosX__].hasChanged() ||
      D.UI[ObjectPosY__].hasChanged() ||
      D.UI[ObjectPosZ__].hasChanged() ||
      D.UI[ObjectSize__].hasChanged()) isRefreshed= false;
}


// Allocate the project data
void CompuFluidDyna::Allocate() {
  if (!isActivProj) return;
  CheckAlloc();
  if (isAllocated) return;
  isRefreshed= false;
  isAllocated= true;

  // Get UI parameters
  nbX= std::max(D.UI[ResolutionX_].GetI(), 1);
  nbY= std::max(D.UI[ResolutionY_].GetI(), 1);
  nbZ= std::max(D.UI[ResolutionZ_].GetI(), 1);
  voxSize= std::max(D.UI[VoxelSize___].GetF(), 1.e-6f);
  voxMeasure= std::pow(voxSize, (nbX > 1) + (nbY > 1) + (nbZ > 1));
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
  AdvX= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  AdvY= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  AdvZ= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
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
  const int scenarioType= D.UI[Scenario____].GetI();
  const int inputFile= D.UI[InputFile___].GetI();
  std::vector<std::vector<std::array<float, 4>>> imageRGBA;
  if (scenarioType == 0) {
    if (inputFile == 0)
      FileInput::LoadImageBMPFile("Resources/CFD_TeslaValveTwinSharp.bmp", imageRGBA, false);
    else if (inputFile == 1)
      FileInput::LoadImageBMPFile("Resources/CFD_Venturi.bmp", imageRGBA, false);
    else if (inputFile == 2)
      FileInput::LoadImageBMPFile("Resources/CFD_Wing.bmp", imageRGBA, false);
    else
      FileInput::LoadImageBMPFile("Resources/CFD_Nozzle.bmp", imageRGBA, false);
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
            PresForced[x][y][z]= (colRGBA[0] > 0.5f) ? (D.UI[CoeffPres___].GetF()) : (-D.UI[CoeffPres___].GetF());
          }
          if (VelBC[x][y][z]) {
            VelXForced[x][y][z]= (colRGBA[1] > 0.5f) ? (D.UI[CoeffVelX___].GetF()) : (-D.UI[CoeffVelX___].GetF());
            VelYForced[x][y][z]= (colRGBA[1] > 0.5f) ? (D.UI[CoeffVelY___].GetF()) : (-D.UI[CoeffVelY___].GetF());
            VelZForced[x][y][z]= (colRGBA[1] > 0.5f) ? (D.UI[CoeffVelZ___].GetF()) : (-D.UI[CoeffVelZ___].GetF());
          }
          if (SmoBC[x][y][z]) {
            SmokForced[x][y][z]= (colRGBA[2] > 0.5f) ? (D.UI[CoeffSmok___].GetF()) : (-D.UI[CoeffSmok___].GetF());
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
            Math::Vec3f posObstacle(D.UI[ObjectPosX__].GetF(), D.UI[ObjectPosY__].GetF(), D.UI[ObjectPosZ__].GetF());
            if (k == 1) posObstacle= Math::Vec3f(1.0f, 1.0f, 1.0f) - posObstacle;
            if ((posCell - posObstacle).norm() <= std::max(D.UI[ObjectSize__].GetF(), 0.0f)) {
              VelBC[x][y][z]= true;
              SmoBC[x][y][z]= true;
              VelXForced[x][y][z]= (k == 1) ? (-D.UI[CoeffVelX___].GetF()) : (D.UI[CoeffVelX___].GetF());
              VelYForced[x][y][z]= (k == 1) ? (-D.UI[CoeffVelY___].GetF()) : (D.UI[CoeffVelY___].GetF());
              VelZForced[x][y][z]= (k == 1) ? (-D.UI[CoeffVelZ___].GetF()) : (D.UI[CoeffVelZ___].GetF());
              SmokForced[x][y][z]= (k == 1) ? (-D.UI[CoeffSmok___].GetF()) : (D.UI[CoeffSmok___].GetF());
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
            VelXForced[x][y][z]= D.UI[CoeffVelX___].GetF();
            VelYForced[x][y][z]= D.UI[CoeffVelY___].GetF();
            VelZForced[x][y][z]= D.UI[CoeffVelZ___].GetF();
            SmoBC[x][y][z]= true;
            SmokForced[x][y][z]= (std::max(z, nbZ - 1 - z) % 16 < 8) ? (D.UI[CoeffSmok___].GetF()) : (-D.UI[CoeffSmok___].GetF());
          }
          else {
            Math::Vec3f posCell(((float)x + 0.5f) / (float)nbX, ((float)y + 0.5f) / (float)nbY, ((float)z + 0.5f) / (float)nbZ);
            Math::Vec3f posObstacle(D.UI[ObjectPosX__].GetF(), D.UI[ObjectPosY__].GetF(), D.UI[ObjectPosZ__].GetF());
            Math::Vec3f dist= (posCell - posObstacle);
            dist[0]*= (float)(nbX - 1) * voxSize;
            dist[1]*= (float)(nbY - 1) * voxSize;
            dist[2]*= (float)(nbZ - 1) * voxSize;
            if (dist.norm() <= std::max(D.UI[ObjectSize__].GetF(), 0.0f))
              Solid[x][y][z]= true;
          }
        }

        // Cavity lid shear benchmark
        if (scenarioType == 3) {
          // Force zero velocity for no-slip condition on cavity wall
          if (y == 0 || y == nbY - 1 || z == 0) {
            // VelBC[x][y][z]= true;
            // VelXForced[x][y][z]= 0;
            // VelYForced[x][y][z]= 0;
            // VelZForced[x][y][z]= 0;
            Solid[x][y][z]= true;
          }
          // Force tangential velocity on cavity lid
          else if (z == nbZ - 1) {
            VelBC[x][y][z]= true;
            VelXForced[x][y][z]= D.UI[CoeffVelX___].GetF();
            VelYForced[x][y][z]= D.UI[CoeffVelY___].GetF();
            VelZForced[x][y][z]= D.UI[CoeffVelZ___].GetF();
            PreBC[x][y][z]= true;
            PresForced[x][y][z]= 0.0f;
          }
          // Add smoke source for visualization
          else if (y == nbY / 2 && z > nbZ / 2) {
            SmoBC[x][y][z]= true;
            SmokForced[x][y][z]= (z % 16 < 8) ? (D.UI[CoeffSmok___].GetF()) : (-D.UI[CoeffSmok___].GetF());
          }
        }

        // Vortex ring with inlet in Y-, outlet in Y+ and wall with hole in the corridor
        if (scenarioType == 4) {
          int wallPos= std::round(D.UI[ObjectPosY__].GetF() * (float)nbY);
          int wallThick= std::round(D.UI[ObjectSize__].GetF() * (float)nbY);
          if ((nbX > 1 && (x == 0 || x == nbX - 1)) ||
              (nbZ > 1 && (z == 0 || z == nbZ - 1))) {
            Solid[x][y][z]= true;
          }
          else if (y == 0) {
            VelBC[x][y][z]= true;
            VelXForced[x][y][z]= D.UI[CoeffVelX___].GetF();
            VelYForced[x][y][z]= D.UI[CoeffVelY___].GetF();
            VelZForced[x][y][z]= D.UI[CoeffVelZ___].GetF();
          }
          else if (std::abs(y - wallPos) <= wallThick) {
            Math::Vec3f posCell(((float)x + 0.5f) / (float)nbX, 0.0f, ((float)z + 0.5f) / (float)nbZ);
            Math::Vec3f posObstacle(D.UI[ObjectPosX__].GetF(), 0.0f, D.UI[ObjectPosZ__].GetF());
            Math::Vec3f dist= (posCell - posObstacle);
            dist[0]*= (float)(nbX - 1) * voxSize;
            dist[1]*= (float)(nbY - 1) * voxSize;
            dist[2]*= (float)(nbZ - 1) * voxSize;
            if (dist.norm() >= std::max(D.UI[ObjectSize__].GetF(), 0.0f))
              Solid[x][y][z]= true;
          }
          else if (y < wallPos) {
            SmoBC[x][y][z]= true;
            SmokForced[x][y][z]= D.UI[CoeffVelY___].GetF() * D.UI[CoeffSmok___].GetF();
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
            VelXForced[x][y][z]= D.UI[CoeffVelX___].GetF();
            VelYForced[x][y][z]= D.UI[CoeffVelY___].GetF();
            VelZForced[x][y][z]= D.UI[CoeffVelZ___].GetF();
            SmoBC[x][y][z]= true;
            SmokForced[x][y][z]= (std::min(z, nbZ - 1 - z) < 4 * (nbZ - 1) / 9) ? -D.UI[CoeffSmok___].GetF() : D.UI[CoeffSmok___].GetF();
          }
        }

        // Poiseuille/Couette flow in tube with pressure gradient
        if (scenarioType == 6) {
          if ((nbX > 1 && (x == 0 || x == nbX - 1)) ||
              (nbZ > 1 && (z == 0 || z == nbZ - 1))) {
            VelBC[x][y][z]= true;
            VelXForced[x][y][z]= (z < nbZ / 2) ? -D.UI[CoeffVelX___].GetF() : D.UI[CoeffVelX___].GetF();
            VelYForced[x][y][z]= (z < nbZ / 2) ? -D.UI[CoeffVelY___].GetF() : D.UI[CoeffVelY___].GetF();
            VelZForced[x][y][z]= (z < nbZ / 2) ? -D.UI[CoeffVelZ___].GetF() : D.UI[CoeffVelZ___].GetF();
          }
          else if (nbY > 1 && (y == 0 || y == nbY - 1)) {
            PreBC[x][y][z]= true;
            PresForced[x][y][z]= (y > nbY / 2) ? -D.UI[CoeffPres___].GetF() : D.UI[CoeffPres___].GetF();
          }
          else if (std::max(y, nbY - 1 - y) == nbY / 2) {
            SmoBC[x][y][z]= true;
            SmokForced[x][y][z]= (std::max(z, nbZ - 1 - z) % 16 < 8) ? (D.UI[CoeffSmok___].GetF()) : (-D.UI[CoeffSmok___].GetF());
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

  if (D.UI[Verbose_____].GetB()) printf("\n");

  // Get simulation parameters
  const int maxIter= std::max(D.UI[SolvMaxIter_].GetI(), 0);
  const float timestep= D.UI[TimeStep____].GetF();
  const float coeffDiffu= std::max(D.UI[CoeffDiffuS_].GetF(), 0.0f);
  const float coeffVisco= std::max(D.UI[CoeffDiffuV_].GetF(), 0.0f);
  const float coeffVorti= D.UI[CoeffVorti__].GetF();

  // Advection steps
  if (D.UI[CoeffAdvecS_].GetB()) {
    AdvectField(FieldID::IDSmok, timestep, VelX, VelY, VelZ, Smok);
  }
  if (D.UI[CoeffAdvecV_].GetB()) {
    std::vector<std::vector<std::vector<float>>> oldVelX= VelX;
    std::vector<std::vector<std::vector<float>>> oldVelY= VelY;
    std::vector<std::vector<std::vector<float>>> oldVelZ= VelZ;
    if (nbX > 1) AdvectField(FieldID::IDVelX, timestep, oldVelX, oldVelY, oldVelZ, VelX);
    if (nbY > 1) AdvectField(FieldID::IDVelY, timestep, oldVelX, oldVelY, oldVelZ, VelY);
    if (nbZ > 1) AdvectField(FieldID::IDVelZ, timestep, oldVelX, oldVelY, oldVelZ, VelZ);
  }

  // Diffusion steps
  if (D.UI[CoeffDiffuS_].GetB()) {
    std::vector<std::vector<std::vector<float>>> oldSmoke= Smok;
    ConjugateGradientSolve(FieldID::IDSmok, maxIter, timestep, true, coeffDiffu, oldSmoke, Smok);
  }
  if (D.UI[CoeffDiffuV_].GetB()) {
    std::vector<std::vector<std::vector<float>>> oldVelX= VelX;
    std::vector<std::vector<std::vector<float>>> oldVelY= VelY;
    std::vector<std::vector<std::vector<float>>> oldVelZ= VelZ;
    if (nbX > 1) ConjugateGradientSolve(FieldID::IDVelX, maxIter, timestep, true, coeffVisco, oldVelX, VelX);
    if (nbY > 1) ConjugateGradientSolve(FieldID::IDVelY, maxIter, timestep, true, coeffVisco, oldVelY, VelY);
    if (nbZ > 1) ConjugateGradientSolve(FieldID::IDVelZ, maxIter, timestep, true, coeffVisco, oldVelZ, VelZ);
  }

  // Vorticity step
  if (D.UI[CoeffVorti__].GetB()) {
    VorticityConfinement(timestep, coeffVorti, VelX, VelY, VelZ);
  }

  // Projection step
  if (D.UI[CoeffProj___].GetB()) {
    ProjectField(maxIter, timestep, VelX, VelY, VelZ);
  }

  // TODO test heuristic optimization of solid regions

  // Draw the scatter data
  const int yCursor= std::min(std::max((int)std::round((float)(nbY - 1) * D.UI[SlicePlotY__].GetF()), 0), nbY - 1);
  const int zCursor= std::min(std::max((int)std::round((float)(nbZ - 1) * D.UI[SlicePlotZ__].GetF()), 0), nbZ - 1);
  D.scatLegend.resize(4);
  D.scatLegend[0]= "Horiz VZ";
  D.scatLegend[1]= "Verti VY";
  D.scatLegend[2]= "Horiz P";
  D.scatLegend[3]= "Verti P";
  D.scatData.resize(4);
  for (int k= 0; k < (int)D.scatData.size(); k++)
    D.scatData[k].clear();
  if (nbZ > 1) {
    for (int y= 0; y < nbY; y++) {
      D.scatData[0].push_back(std::array<double, 2>({(double)y / (double)(nbY - 1), VelZ[nbX / 2][y][zCursor] + (double)zCursor / (double)(nbZ - 1)}));
      D.scatData[2].push_back(std::array<double, 2>({(double)y / (double)(nbY - 1), Pres[nbX / 2][y][zCursor] + (double)zCursor / (double)(nbZ - 1)}));
    }
  }
  if (nbY > 1) {
    for (int z= 0; z < nbZ; z++) {
      D.scatData[1].push_back(std::array<double, 2>({VelY[nbX / 2][yCursor][z] + (double)yCursor / (double)(nbY - 1), (double)z / (double)(nbZ - 1)}));
      D.scatData[3].push_back(std::array<double, 2>({Pres[nbX / 2][yCursor][z] + (double)yCursor / (double)(nbY - 1), (double)z / (double)(nbZ - 1)}));
    }
  }

  // Add hard coded lid driven cavity flow benchmark for visual comparison
  if (D.UI[Scenario____].GetI() == 3) {
    D.scatLegend.resize(4);
    D.scatLegend[2]= "Ghia 100";
    D.scatLegend[3]= "Ghia 100";
    D.scatData.resize(4);
    D.scatData[2].clear();
    D.scatData[3].clear();
    // Data from Ghia 1982 http://www.msaidi.ir/upload/Ghia1982.pdf
    const std::vector<double> GhiaData0X({+0.00000, +0.06250, +0.07030, +0.07810, +0.09380, +0.15630, +0.22660, +0.23440, +0.50000, +0.80470, +0.85940, +0.90630, +0.94530, +0.95310, +0.96090, +0.96880, +1.00000});  // coord along horiz slice
    const std::vector<double> GhiaData0Y({+0.00000, +0.09233, +0.10091, +0.10890, +0.12317, +0.16077, +0.17507, +0.17527, +0.05454, -0.24533, -0.22445, -0.16914, -0.10313, -0.08864, -0.07391, -0.05906, +0.00000});  // Re 100   verti vel along horiz slice
    const std::vector<double> GhiaData1X({+0.00000, -0.03717, -0.04192, -0.04775, -0.06434, -0.10150, -0.15662, -0.21090, -0.20581, -0.13641, +0.00332, +0.23151, +0.68717, +0.73722, +0.78871, +0.84123, +1.00000});  // Re 100   horiz vel on verti slice
    const std::vector<double> GhiaData1Y({+0.00000, +0.05470, +0.06250, +0.07030, +0.10160, +0.17190, +0.28130, +0.45310, +0.50000, +0.61720, +0.73440, +0.85160, +0.95310, +0.96090, +0.96880, +0.97660, +1.00000});  // coord along verti slice
    for (int k= 0; k < (int)GhiaData0X.size(); k++) {
      D.scatData[2].push_back(std::array<double, 2>({GhiaData0X[k], GhiaData0Y[k] + 0.5f}));
      D.scatData[3].push_back(std::array<double, 2>({GhiaData1X[k] + 0.5f, GhiaData1Y[k]}));
    }
    D.scatLegend.resize(6);
    D.scatLegend[4]= "Ertu 10k";
    D.scatLegend[5]= "Ertu 10k";
    D.scatData.resize(6);
    D.scatData[4].clear();
    D.scatData[5].clear();
    // Data from Erturk 2005 https://arxiv.org/pdf/physics/0505121.pdf
    const std::vector<double> ErtuData0X({+0.0000, +0.0150, +0.0300, +0.0450, +0.0600, +0.0750, +0.0900, +0.1050, +0.1200, +0.1350, +0.1500, +0.5000, +0.8500, +0.8650, +0.8800, +0.8950, +0.9100, +0.9250, +0.9400, +0.9550, +0.9700, +0.9850, +1.0000});  // coord along horiz slice
    const std::vector<double> ErtuData0Y({+0.0000, +0.2756, +0.3844, +0.4409, +0.4566, +0.4449, +0.4247, +0.4056, +0.3885, +0.3722, +0.3562, +0.0088, -0.3538, -0.3715, -0.3895, -0.4078, -0.4256, -0.4411, -0.4592, -0.5124, -0.5712, -0.3419, +0.0000});  // Re 10000 verti vel along horiz slice
    const std::vector<double> ErtuData1X({+0.0000, -0.2907, -0.4259, -0.4469, -0.4142, -0.3899, -0.3721, -0.3543, -0.3361, -0.3179, -0.2998, -0.0268, +0.4095, +0.4243, +0.4398, +0.4556, +0.4711, +0.4843, +0.4917, +0.4891, +0.4837, +0.5891, +1.0000});  // Re 10000 horiz vel on verti slice
    const std::vector<double> ErtuData1Y({+0.0000, +0.0200, +0.0400, +0.0600, +0.0800, +0.1000, +0.1200, +0.1400, +0.1600, +0.1800, +0.2000, +0.5000, +0.9000, +0.9100, +0.9200, +0.9300, +0.9400, +0.9500, +0.9600, +0.9700, +0.9800, +0.9900, +1.0000});  // coord along verti slice
    for (int k= 0; k < (int)ErtuData0X.size(); k++) {
      D.scatData[4].push_back(std::array<double, 2>({ErtuData0X[k], ErtuData0Y[k] + 0.5f}));
      D.scatData[5].push_back(std::array<double, 2>({ErtuData1X[k] + 0.5f, ErtuData1Y[k]}));
    }
  }

  // Add hard coded analytical solution of Poiseuille flow
  if (D.UI[Scenario____].GetI() == 6) {
    D.scatData[0].clear();
    D.scatData[3].clear();
    D.scatLegend.resize(6);
    D.scatData.resize(6);
    D.scatLegend[4]= "Analy VY";
    D.scatLegend[5]= "Analy P";
    D.scatData[4].clear();
    D.scatData[5].clear();
    const float press0= D.UI[CoeffPres___].GetF();
    const float press1= -D.UI[CoeffPres___].GetF();
    const float kinVisco= D.UI[CoeffDiffuV_].GetF();
    if (nbY > 1) {
      for (int z= 0; z < nbZ; z++) {
        const float width= voxSize * (float)(nbY - 1);
        const float height= voxSize * (float)(nbZ - 1);
        const float posZ= (float)z * voxSize;
        const float pressDiff= (press1 - press0) / width;
        const float analyVelY= -pressDiff * (1.0f / (2.0f * kinVisco)) * posZ * (height - posZ);
        D.scatData[4].push_back(std::array<double, 2>({analyVelY + (double)yCursor / (double)(nbY - 1), (double)z / (double)(nbZ - 1)}));
      }
    }
    if (nbZ > 1) {
      for (int y= 0; y < nbY; y++) {
        const float analyP= press0 + (press1 - press0) * (float)y / (float)(nbY - 1);
        D.scatData[5].push_back(std::array<double, 2>({(double)y / (double)(nbY - 1), analyP + (double)zCursor / (double)(nbZ - 1)}));
      }
    }
  }
  // D.scatData[1].clear();  // Delete velocity scopes for testing
  // D.scatData[4].clear();  // Delete velocity scopes for testing
  // D.scatData[2].clear();  // Delete pressure scopes for testing
  // D.scatData[5].clear();  // Delete pressure scopes for testing
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
          if (D.UI[ColorMode___].GetI() == 1) {
            if (std::abs(Pres[x][y][z]) < D.UI[ColorThresh_].GetF()) continue;
            Colormap::RatioToBlueToRed(0.5f + 0.5f * Pres[x][y][z] * D.UI[ColorFactor_].GetF(), r, g, b);
          }
          // Color by smoke
          if (D.UI[ColorMode___].GetI() == 2) {
            if (std::abs(Smok[x][y][z]) < D.UI[ColorThresh_].GetF()) continue;
            Colormap::RatioToRainbow(0.5f + 0.5f * Smok[x][y][z] * D.UI[ColorFactor_].GetF(), r, g, b);
          }
          // Color by velocity magnitude
          if (D.UI[ColorMode___].GetI() == 3) {
            Math::Vec3f vec(VelX[x][y][z], VelY[x][y][z], VelZ[x][y][z]);
            if (vec.norm() < D.UI[ColorThresh_].GetF()) continue;
            Colormap::RatioToJetBrightSmooth(vec.norm() * D.UI[ColorFactor_].GetF(), r, g, b);
          }
          // Color by divergence
          if (D.UI[ColorMode___].GetI() == 4) {
            if (std::abs(Dive[x][y][z]) < D.UI[ColorThresh_].GetF()) continue;
            Colormap::RatioToGreenToRed(0.5f + 0.5f * Dive[x][y][z] * D.UI[ColorFactor_].GetF(), r, g, b);
          }
          // Color by vorticity
          if (D.UI[ColorMode___].GetI() == 5) {
            if (std::abs(Vort[x][y][z]) < D.UI[ColorThresh_].GetF()) continue;
            Colormap::RatioToJetBrightSmooth(Vort[x][y][z] * D.UI[ColorFactor_].GetF(), r, g, b);
          }
          // Color by curl in X
          if (D.UI[ColorMode___].GetI() == 6) {
            if (std::abs(CurX[x][y][z]) < D.UI[ColorThresh_].GetF()) continue;
            Colormap::RatioToJetBrightSmooth(0.5f + 0.5f * CurX[x][y][z] * D.UI[ColorFactor_].GetF(), r, g, b);
          }
          // Color by curl in Y
          if (D.UI[ColorMode___].GetI() == 7) {
            if (std::abs(CurY[x][y][z]) < D.UI[ColorThresh_].GetF()) continue;
            Colormap::RatioToJetBrightSmooth(0.5f + 0.5f * CurY[x][y][z] * D.UI[ColorFactor_].GetF(), r, g, b);
          }
          // Color by curl in Z
          if (D.UI[ColorMode___].GetI() == 8) {
            if (std::abs(CurZ[x][y][z]) < D.UI[ColorThresh_].GetF()) continue;
            Colormap::RatioToJetBrightSmooth(0.5f + 0.5f * CurZ[x][y][z] * D.UI[ColorFactor_].GetF(), r, g, b);
          }
          // Color by dummy values
          if (D.UI[ColorMode___].GetI() >= 10 && D.UI[ColorMode___].GetI() <= 14) {
            float val= 0.0f;
            if (D.UI[ColorMode___].GetI() == 10) val= Dum0[x][y][z];
            if (D.UI[ColorMode___].GetI() == 11) val= Dum1[x][y][z];
            if (D.UI[ColorMode___].GetI() == 12) val= Dum2[x][y][z];
            if (D.UI[ColorMode___].GetI() == 13) val= Dum3[x][y][z];
            if (D.UI[ColorMode___].GetI() == 14) val= Dum4[x][y][z];
            if (std::abs(val) < D.UI[ColorThresh_].GetF()) continue;
            Colormap::RatioToJetBrightSmooth(0.5f + 0.5f * val * D.UI[ColorFactor_].GetF(), r, g, b);
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

  // Draw the velocity field
  if (D.displayMode3) {
    // Set the scene transformation
    glPushMatrix();
    if (nbX == 1) glTranslatef(voxSize, 0.0f, 0.0f);
    if (nbY == 1) glTranslatef(0.0f, voxSize, 0.0f);
    if (nbZ == 1) glTranslatef(0.0f, 0.0f, voxSize);
    glTranslatef(D.boxMin[0] + 0.5f * voxSize, D.boxMin[1] + 0.5f * voxSize, D.boxMin[2] + 0.5f * voxSize);
    glScalef(voxSize, voxSize, voxSize);
    // Sweep the field
    constexpr int nbLineWidths= 3;
    for (int k= 0; k < nbLineWidths; k++) {
      const float segmentRelLength= 1.0f - (float)k / (float)nbLineWidths;
      glLineWidth((float)k + 1.0f);
      glBegin(GL_LINES);
      for (int x= 0; x < nbX; x++) {
        for (int y= 0; y < nbY; y++) {
          for (int z= 0; z < nbZ; z++) {
            // Draw the velocity field
            Math::Vec3f vec(VelX[x][y][z], VelY[x][y][z], VelZ[x][y][z]);
            if (vec.normSquared() > 0.0f) {
              float r= 0.0f, g= 0.0f, b= 0.0f;
              Colormap::RatioToJetBrightSmooth(vec.norm() * D.UI[ColorFactor_].GetF(), r, g, b);
              glColor3f(r, g, b);
              Math::Vec3f pos((float)x, (float)y, (float)z);
              glVertex3fv(pos.array());
              glVertex3fv(pos + vec * segmentRelLength * D.UI[ScaleFactor_].GetF());
              // glVertex3fv(pos + vec.normalized() * segmentRelLength * D.UI[ScaleFactor_].GetF());
              // glVertex3fv(pos + vec.normalized() * segmentRelLength * D.UI[ScaleFactor_].GetF() * std::log(vec.norm() + 1.0f));
            }
          }
        }
      }
      glEnd();
    }
    glLineWidth(1.0f);
    glPopMatrix();
  }

  // Draw the advection source field
  if (D.displayMode4) {
    // Set the scene transformation
    glPushMatrix();
    if (nbX == 1) glTranslatef(voxSize, 0.0f, 0.0f);
    if (nbY == 1) glTranslatef(0.0f, voxSize, 0.0f);
    if (nbZ == 1) glTranslatef(0.0f, 0.0f, voxSize);
    glTranslatef(D.boxMin[0] + 0.5f * voxSize, D.boxMin[1] + 0.5f * voxSize, D.boxMin[2] + 0.5f * voxSize);
    glScalef(voxSize, voxSize, voxSize);
    // Sweep the field
    glBegin(GL_LINES);
    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          // Draw the velocity field
          Math::Vec3f vec(AdvX[x][y][z], AdvY[x][y][z], AdvZ[x][y][z]);
          if (vec.normSquared() > 0.0f) {
            float r= 0.5f, g= 0.5f, b= 0.5f;
            glColor3f(r, g, b);
            Math::Vec3f pos((float)x, (float)y, (float)z);
            glVertex3fv(pos.array());
            glVertex3fv(pos + vec * D.UI[ScaleFactor_].GetF());
          }
        }
      }
    }
    glEnd();
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
  const float diffuVal= iDiffuCoeff * iTimeStep / voxMeasure;
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
            oField[x][y][z]= 1.0f / (1.0f + diffuVal * (float)count) * iField[x][y][z];             //           [   -D*dt/v]
          else                                                                                      // [-D*dt/v] [1+4*D*dt/v] [-D*dt/v]
            oField[x][y][z]= -diffuVal * sum + (1.0f + diffuVal * (float)count) * iField[x][y][z];  //           [   -D*dt/v]
        }
        else {
          if (iPrecondMode)
            oField[x][y][z]= 1.0f / ((float)count / voxMeasure) * iField[x][y][z];              //       [ 1/v]
          else                                                                                  // [1/v] [-4/v] [1/v]
            oField[x][y][z]= sum / voxMeasure - ((float)count / voxMeasure) * iField[x][y][z];  //       [ 1/v]
        }
      }
    }
  }
}


void CompuFluidDyna::ConjugateGradientSolve(const int iFieldID, const int iMaxIter, const float iTimeStep,
                                            const bool iDiffuMode, const float iDiffuCoeff,
                                            const std::vector<std::vector<std::vector<float>>>& iField,
                                            std::vector<std::vector<std::vector<float>>>& ioField) {
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
  if (D.UI[Verbose_____].GetB()) printf("CG [%.2e] ", normRHS);
  if (D.UI[Verbose_____].GetB()) printf("%.2e ", (normRHS != 0.0f) ? (errTmp / normRHS) : (0.0f));
  D.plotData[iFieldID].push_back((normRHS != 0.0f) ? errTmp / normRHS : 0.0f);

  // d = M^-1 r
  ImplicitFieldLaplacianMatMult(iFieldID, iTimeStep, iDiffuMode, iDiffuCoeff, true, rField, dField);

  // errNew = r^T d
  float errNew= ImplicitFieldDotProd(rField, dField);
  float errBeg= errNew;
  float errOld= 0.0f;

  // Iterate to solve
  for (int k= 0; k < iMaxIter; k++) {
    if (errNew / normRHS < D.UI[SolvTolRhs__].GetF()) break;
    if (errNew / errBeg < D.UI[SolvTolRel__].GetF()) break;

    // q = A d
    ImplicitFieldLaplacianMatMult(iFieldID, iTimeStep, iDiffuMode, iDiffuCoeff, false, dField, qField);

    // alpha = errNew / (d^T q)
    float denom= ImplicitFieldDotProd(dField, qField);
    if (denom == 0.0) {
      if (D.UI[Verbose_____].GetB()) printf("div by zero denom");
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
    if (D.UI[Verbose_____].GetB()) printf("%.2e ", (normRHS != 0.0f) ? (errTmp / normRHS) : (0.0f));
    D.plotData[iFieldID].push_back((normRHS != 0.0f) ? (errTmp / normRHS) : (0.0f));

    // s = M^-1 r
    ImplicitFieldLaplacianMatMult(iFieldID, iTimeStep, iDiffuMode, iDiffuCoeff, true, rField, sField);

    // errNew = r^T s
    errOld= errNew;
    errNew= ImplicitFieldDotProd(rField, sField);
    if (errOld == 0.0) {
      if (D.UI[Verbose_____].GetB()) printf("div by zero errOld");
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
  if (D.UI[Verbose_____].GetB()) printf("\n");
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
        // if (x - 1 >= 0 && x + 1 < nbX) Dive[x][y][z]+= (ioVelX[x + 1][y][z] - ioVelX[x - 1][y][z]) / (2.0f * voxSize);
        // if (y - 1 >= 0 && y + 1 < nbY) Dive[x][y][z]+= (ioVelY[x][y + 1][z] - ioVelY[x][y - 1][z]) / (2.0f * voxSize);
        // if (z - 1 >= 0 && z + 1 < nbZ) Dive[x][y][z]+= (ioVelZ[x][y][z + 1] - ioVelZ[x][y][z - 1]) / (2.0f * voxSize);
        const int countX= (x - 1 >= 0) + (x + 1 < nbX);
        const int countY= (y - 1 >= 0) + (y + 1 < nbY);
        const int countZ= (z - 1 >= 0) + (z + 1 < nbZ);
        if (x - 1 >= 0) Dive[x][y][z]+= (ioVelX[x][y][z] - ioVelX[x - 1][y][z]) / ((float)countX * voxSize);
        if (y - 1 >= 0) Dive[x][y][z]+= (ioVelY[x][y][z] - ioVelY[x][y - 1][z]) / ((float)countY * voxSize);
        if (z - 1 >= 0) Dive[x][y][z]+= (ioVelZ[x][y][z] - ioVelZ[x][y][z - 1]) / ((float)countZ * voxSize);
        if (x + 1 < nbX) Dive[x][y][z]+= (ioVelX[x + 1][y][z] - ioVelX[x][y][z]) / ((float)countX * voxSize);
        if (y + 1 < nbY) Dive[x][y][z]+= (ioVelY[x][y + 1][z] - ioVelY[x][y][z]) / ((float)countY * voxSize);
        if (z + 1 < nbZ) Dive[x][y][z]+= (ioVelZ[x][y][z + 1] - ioVelZ[x][y][z]) / ((float)countZ * voxSize);
      }
    }
  }
  ApplyBC(FieldID::IDPres, Dive);

  // Solve for pressure
  ConjugateGradientSolve(FieldID::IDPres, iIter, iTimeStep, false, 0.0f, Dive, Pres);

  // Update velocities based on neighboring pressures
#pragma omp parallel for
  for (int x= 0; x < nbX; x++) {
    for (int y= 0; y < nbY; y++) {
      for (int z= 0; z < nbZ; z++) {
        if (Solid[x][y][z] || VelBC[x][y][z]) continue;
        // if (x - 1 >= 0 && x + 1 < nbX) ioVelX[x][y][z]-= iTimeStep * D.UI[CoeffProj___].GetF() * (Pres[x + 1][y][z] - Pres[x - 1][y][z]) / (2.0f * voxSize);
        // if (y - 1 >= 0 && y + 1 < nbY) ioVelY[x][y][z]-= iTimeStep * D.UI[CoeffProj___].GetF() * (Pres[x][y + 1][z] - Pres[x][y - 1][z]) / (2.0f * voxSize);
        // if (z - 1 >= 0 && z + 1 < nbZ) ioVelZ[x][y][z]-= iTimeStep * D.UI[CoeffProj___].GetF() * (Pres[x][y][z + 1] - Pres[x][y][z - 1]) / (2.0f * voxSize);
        const int countX= (x - 1 >= 0) + (x + 1 < nbX);
        const int countY= (y - 1 >= 0) + (y + 1 < nbY);
        const int countZ= (z - 1 >= 0) + (z + 1 < nbZ);
        float timeDependance= (D.UI[CoeffProj___].GetI() == 2) ? iTimeStep : 1.0f;
        if (x - 1 >= 0) ioVelX[x][y][z]-= timeDependance * (Pres[x][y][z] - Pres[x - 1][y][z]) / ((float)countX * voxSize);
        if (y - 1 >= 0) ioVelY[x][y][z]-= timeDependance * (Pres[x][y][z] - Pres[x][y - 1][z]) / ((float)countY * voxSize);
        if (z - 1 >= 0) ioVelZ[x][y][z]-= timeDependance * (Pres[x][y][z] - Pres[x][y][z - 1]) / ((float)countZ * voxSize);
        if (x + 1 < nbX) ioVelX[x][y][z]-= timeDependance * (Pres[x + 1][y][z] - Pres[x][y][z]) / ((float)countX * voxSize);
        if (y + 1 < nbY) ioVelY[x][y][z]-= timeDependance * (Pres[x][y + 1][z] - Pres[x][y][z]) / ((float)countY * voxSize);
        if (z + 1 < nbZ) ioVelZ[x][y][z]-= timeDependance * (Pres[x][y][z + 1] - Pres[x][y][z]) / ((float)countZ * voxSize);
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

        // Find source position for active voxel
        Math::Vec3f posSource(0.0f, 0.0f, 0.0f);
        Math::Vec3f posEnd((float)x, (float)y, (float)z);
        Math::Vec3f velEnd(iVelX[x][y][z], iVelY[x][y][z], iVelZ[x][y][z]);
        if ((iFieldID == FieldID::IDSmok && D.UI[CoeffAdvecS_].GetI() == 2) ||
            (iFieldID == FieldID::IDVelX && D.UI[CoeffAdvecV_].GetI() == 2) ||
            (iFieldID == FieldID::IDVelY && D.UI[CoeffAdvecV_].GetI() == 2) ||
            (iFieldID == FieldID::IDVelZ && D.UI[CoeffAdvecV_].GetI() == 2)) {
          // 2nd order MacCormack backtracking
          Math::Vec3f posBeg= posEnd - iTimeStep * velEnd / voxSize;
          Math::Vec3f velBeg;
          velBeg[0]= TrilinearInterpolation(posBeg[0], posBeg[1], posBeg[2], iVelX);
          velBeg[1]= TrilinearInterpolation(posBeg[0], posBeg[1], posBeg[2], iVelY);
          velBeg[2]= TrilinearInterpolation(posBeg[0], posBeg[1], posBeg[2], iVelZ);
          Math::Vec3f vecErr= posEnd - (posBeg + velBeg);
          posSource= posBeg + vecErr / 2.0f;
        }
        else {
          // Naive linear backtracking
          posSource= posEnd - iTimeStep * velEnd / voxSize;
        }

        // Save source vector for display
        AdvX[x][y][z]= posSource[0] - posEnd[0];
        AdvY[x][y][z]= posSource[1] - posEnd[1];
        AdvZ[x][y][z]= posSource[2] - posEnd[2];

        // Trilinear interpolation at source position
        ioField[x][y][z]= TrilinearInterpolation(posSource[0], posSource[1], posSource[2], oldField);
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
