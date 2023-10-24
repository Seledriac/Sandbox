#include "CompuFluidDyna.hpp"


// Standard lib
#include <cstdio>
#include <cstring>
#include <vector>

// GLUT lib
#include "../freeglut/include/GL/freeglut.h"

// Project lib
#include "../Data.hpp"
#include "../FileIO/FileInput.hpp"
#include "../Util/Colormap.hpp"
#include "../Util/Field.hpp"
#include "../Util/Random.hpp"
#include "../Util/Timer.hpp"
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
  SolvPCG_____,
  SolvSOR_____,
  SolvTolRhs__,
  SolvTolRel__,
  CoeffGravi__,
  CoeffAdvec__,
  CoeffDiffuS_,
  CoeffDiffuV_,
  CoeffVorti__,
  CoeffProj___,
  BCVelX______,
  BCVelY______,
  BCVelZ______,
  BCPres______,
  BCSmok______,
  BCSmokTime__,
  BCAdvecWall_,
  ObjectPosX__,
  ObjectPosY__,
  ObjectPosZ__,
  ObjectSize__,
  ScaleFactor_,
  ColorFactor_,
  ColorThresh_,
  ColorMode___,
  SlicePlotY__,
  SlicePlotZ__,
  Verbose_____,
  VerboseSolv_,
  VerboseTime_,
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
    D.UI.push_back(ParamUI("Scenario____", 0));      // Scenario ID, 0= load file, 1> hard coded scenarios
    D.UI.push_back(ParamUI("InputFile___", 3));      // BMP file to load
    D.UI.push_back(ParamUI("ResolutionX_", 1));      // Eulerian mesh resolution
    D.UI.push_back(ParamUI("ResolutionY_", 60));     // Eulerian mesh resolution
    D.UI.push_back(ParamUI("ResolutionZ_", 60));     // Eulerian mesh resolution
    D.UI.push_back(ParamUI("VoxelSize___", 0.01));   // Element size
    D.UI.push_back(ParamUI("TimeStep____", 0.02));   // Simulation time step
    D.UI.push_back(ParamUI("SolvMaxIter_", 40));     // Max number of solver iterations
    D.UI.push_back(ParamUI("SolvPCG_____", 1));      // Flag to use Gauss Seidel (=0) or Preconditioned Conjugate Gradient (>0)
    D.UI.push_back(ParamUI("SolvSOR_____", 1.8));    // Overrelaxation coefficient in Gauss Seidel solver
    D.UI.push_back(ParamUI("SolvTolRhs__", 1.e-9));  // Solver tolerance relative to RHS norm
    D.UI.push_back(ParamUI("SolvTolRel__", 1.e-3));  // Solver tolerance relative to initial guess
    D.UI.push_back(ParamUI("CoeffGravi__", 0.0));    // Magnitude of gravity in Z- direction
    D.UI.push_back(ParamUI("CoeffAdvec__", 5.0));    // 0= no advection, 1= linear advection, >1 MacCormack correction iterations
    D.UI.push_back(ParamUI("CoeffDiffuS_", 1.e-4));  // Diffusion of smoke field, i.e. smoke spread/smear
    D.UI.push_back(ParamUI("CoeffDiffuV_", 1.e-3));  // Diffusion of velocity field, i.e. viscosity
    D.UI.push_back(ParamUI("CoeffVorti__", 0.0));    // Vorticity confinement to avoid dissipation of energy in small scale vortices
    D.UI.push_back(ParamUI("CoeffProj___", 1.0));    // Enable incompressibility projection
    D.UI.push_back(ParamUI("BCVelX______", 0.0));    // Velocity value for voxels with enforced velocity
    D.UI.push_back(ParamUI("BCVelY______", 1.0));    // Velocity value for voxels with enforced velocity
    D.UI.push_back(ParamUI("BCVelZ______", 0.0));    // Velocity value for voxels with enforced velocity
    D.UI.push_back(ParamUI("BCPres______", 0.0));    // Pressure value for voxels with enforced pressure
    D.UI.push_back(ParamUI("BCSmok______", 1.0));    // Smoke value for voxels with enforced smoke
    D.UI.push_back(ParamUI("BCSmokTime__", 1.0));    // Period duration for input smoke oscillation
    D.UI.push_back(ParamUI("BCAdvecWall_", 1.0));    // Enable advection of non-zero smoke from the walls
    D.UI.push_back(ParamUI("ObjectPosX__", 0.5));    // Coordinates for objects in hard coded scenarios
    D.UI.push_back(ParamUI("ObjectPosY__", 0.25));   // Coordinates for objects in hard coded scenarios
    D.UI.push_back(ParamUI("ObjectPosZ__", 0.5));    // Coordinates for objects in hard coded scenarios
    D.UI.push_back(ParamUI("ObjectSize__", 0.08));   // Size for objects in hard coded scenarios
    D.UI.push_back(ParamUI("ScaleFactor_", 1.0));    // Scale factor for drawn geometry
    D.UI.push_back(ParamUI("ColorFactor_", 1.0));    // Color factor for drawn geometry
    D.UI.push_back(ParamUI("ColorThresh_", 0.0));    // Color cutoff drawn geometry
    D.UI.push_back(ParamUI("ColorMode___", 1));      // Selector for the scalar field to be drawn
    D.UI.push_back(ParamUI("SlicePlotY__", 0.5));    // Positions for the scatter plot slices
    D.UI.push_back(ParamUI("SlicePlotZ__", 0.5));    // Positions for the scatter plot slices
    D.UI.push_back(ParamUI("Verbose_____", -0.5));   // Verbose mode
    D.UI.push_back(ParamUI("VerboseSolv_", -0.5));   // Verbose mode for linear solvers
    D.UI.push_back(ParamUI("VerboseTime_", -0.5));   // Verbose mode for linear solvers
  }

  D.boxMin= {0.0, 0.0, 0.0};
  D.boxMax= {1.0, 1.0, 1.0};

  isActivProj= true;
  isAllocated= false;
  isRefreshed= false;
}


// Check if parameter changes should trigger an allocation
void CompuFluidDyna::CheckAlloc() {
  if (D.UI[Scenario____].hasChanged()) isAllocated= false;
  if (D.UI[InputFile___].hasChanged()) isAllocated= false;
  if (D.UI[ResolutionX_].hasChanged()) isAllocated= false;
  if (D.UI[ResolutionY_].hasChanged()) isAllocated= false;
  if (D.UI[ResolutionZ_].hasChanged()) isAllocated= false;
  if (D.UI[VoxelSize___].hasChanged()) isAllocated= false;
}


// Check if parameter changes should trigger a refresh
void CompuFluidDyna::CheckRefresh() {
  if (D.UI[BCVelX______].hasChanged()) isRefreshed= false;
  if (D.UI[BCVelY______].hasChanged()) isRefreshed= false;
  if (D.UI[BCVelZ______].hasChanged()) isRefreshed= false;
  if (D.UI[BCPres______].hasChanged()) isRefreshed= false;
  if (D.UI[BCSmok______].hasChanged()) isRefreshed= false;
  if (D.UI[ObjectPosX__].hasChanged()) isRefreshed= false;
  if (D.UI[ObjectPosY__].hasChanged()) isRefreshed= false;
  if (D.UI[ObjectPosZ__].hasChanged()) isRefreshed= false;
  if (D.UI[ObjectSize__].hasChanged()) isRefreshed= false;
}


// Allocate the project data
void CompuFluidDyna::Allocate() {
  if (!isActivProj) return;
  CheckAlloc();
  if (isAllocated) return;
  isRefreshed= false;
  isAllocated= true;
  if (D.UI[Verbose_____].GetB()) printf("CompuFluidDyna::Allocate()\n");

  // Get UI parameters
  nX= std::max(D.UI[ResolutionX_].GetI(), 1);
  nY= std::max(D.UI[ResolutionY_].GetI(), 1);
  nZ= std::max(D.UI[ResolutionZ_].GetI(), 1);
  voxSize= std::max(D.UI[VoxelSize___].GetF(), 1.e-6f);
  D.boxMin= {0.5f - 0.5f * (float)nX * voxSize, 0.5f - 0.5f * (float)nY * voxSize, 0.5f - 0.5f * (float)nZ * voxSize};
  D.boxMax= {0.5f + 0.5f * (float)nX * voxSize, 0.5f + 0.5f * (float)nY * voxSize, 0.5f + 0.5f * (float)nZ * voxSize};

  fluidDensity= 1.0f;

  // Allocate data
  Solid= Field::AllocField3D(nX, nY, nZ, false);
  VelBC= Field::AllocField3D(nX, nY, nZ, false);
  PreBC= Field::AllocField3D(nX, nY, nZ, false);
  SmoBC= Field::AllocField3D(nX, nY, nZ, false);
  VelXForced= Field::AllocField3D(nX, nY, nZ, 0.0f);
  VelYForced= Field::AllocField3D(nX, nY, nZ, 0.0f);
  VelZForced= Field::AllocField3D(nX, nY, nZ, 0.0f);
  PresForced= Field::AllocField3D(nX, nY, nZ, 0.0f);
  SmokForced= Field::AllocField3D(nX, nY, nZ, 0.0f);

  Dum0= Field::AllocField3D(nX, nY, nZ, 0.0f);
  Dum1= Field::AllocField3D(nX, nY, nZ, 0.0f);
  Dum2= Field::AllocField3D(nX, nY, nZ, 0.0f);
  Dum3= Field::AllocField3D(nX, nY, nZ, 0.0f);
  Dum4= Field::AllocField3D(nX, nY, nZ, 0.0f);
  Vort= Field::AllocField3D(nX, nY, nZ, 0.0f);
  Pres= Field::AllocField3D(nX, nY, nZ, 0.0f);
  Dive= Field::AllocField3D(nX, nY, nZ, 0.0f);
  Smok= Field::AllocField3D(nX, nY, nZ, 0.0f);
  VelX= Field::AllocField3D(nX, nY, nZ, 0.0f);
  VelY= Field::AllocField3D(nX, nY, nZ, 0.0f);
  VelZ= Field::AllocField3D(nX, nY, nZ, 0.0f);
  CurX= Field::AllocField3D(nX, nY, nZ, 0.0f);
  CurY= Field::AllocField3D(nX, nY, nZ, 0.0f);
  CurZ= Field::AllocField3D(nX, nY, nZ, 0.0f);
  AdvX= Field::AllocField3D(nX, nY, nZ, 0.0f);
  AdvY= Field::AllocField3D(nX, nY, nZ, 0.0f);
  AdvZ= Field::AllocField3D(nX, nY, nZ, 0.0f);
}


// Refresh the project
void CompuFluidDyna::Refresh() {
  if (!isActivProj) return;
  CheckAlloc();
  if (!isAllocated) Allocate();
  CheckRefresh();
  if (isRefreshed) return;
  isRefreshed= true;
  if (D.UI[Verbose_____].GetB()) printf("CompuFluidDyna::Refresh()\n");

  // Initialize scenario values
  simTime= 0;
  for (int x= 0; x < nX; x++) {
    for (int y= 0; y < nY; y++) {
      for (int z= 0; z < nZ; z++) {
        Solid[x][y][z]= VelBC[x][y][z]= PreBC[x][y][z]= SmoBC[x][y][z]= false;
        VelXForced[x][y][z]= VelYForced[x][y][z]= VelZForced[x][y][z]= PresForced[x][y][z]= SmokForced[x][y][z]= 0.0f;
      }
    }
  }

  CompuFluidDyna::InitializeScenario();

  // Enforce scenario validity
  for (int x= 0; x < nX; x++) {
    for (int y= 0; y < nY; y++) {
      for (int z= 0; z < nZ; z++) {
        if (Solid[x][y][z]) {
          VelBC[x][y][z]= PreBC[x][y][z]= SmoBC[x][y][z]= false;
          VelXForced[x][y][z]= VelYForced[x][y][z]= VelZForced[x][y][z]= PresForced[x][y][z]= SmokForced[x][y][z]= 0.0f;
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
  if (D.UI[Verbose_____].GetB()) printf("CompuFluidDyna::Animate()\n");

  // Get simulation parameters
  const int maxIter= std::max(D.UI[SolvMaxIter_].GetI(), 0);
  const float timestep= D.UI[TimeStep____].GetF();
  const float coeffDiffu= std::max(D.UI[CoeffDiffuS_].GetF(), 0.0f);
  const float coeffVisco= std::max(D.UI[CoeffDiffuV_].GetF(), 0.0f);
  const float coeffVorti= D.UI[CoeffVorti__].GetF();
  simTime+= D.UI[TimeStep____].GetF();

  // Update periodic smoke in inlet
  if (D.UI[VerboseTime_].GetB()) Timer::PushTimer();
  ApplyBC(FieldID::IDSmok, Smok);
  if (D.UI[VerboseTime_].GetB()) printf("%f T ApplyBC\n", Timer::PopTimer());

  // Incompressible Navier Stokes
  // ∂vel/∂t + (vel · ∇) vel = − 1/ρ ∇press + visco ∇²vel + f
  // ∇ · vel = 0

  // Advection steps
  if (D.UI[VerboseTime_].GetB()) Timer::PushTimer();
  if (D.UI[CoeffAdvec__].GetB()) {
    AdvectField(FieldID::IDSmok, timestep, VelX, VelY, VelZ, Smok);
  }
  if (D.UI[CoeffAdvec__].GetB()) {
    std::vector<std::vector<std::vector<float>>> oldVelX= VelX;
    std::vector<std::vector<std::vector<float>>> oldVelY= VelY;
    std::vector<std::vector<std::vector<float>>> oldVelZ= VelZ;
    if (nX > 1) AdvectField(FieldID::IDVelX, timestep, oldVelX, oldVelY, oldVelZ, VelX);
    if (nY > 1) AdvectField(FieldID::IDVelY, timestep, oldVelX, oldVelY, oldVelZ, VelY);
    if (nZ > 1) AdvectField(FieldID::IDVelZ, timestep, oldVelX, oldVelY, oldVelZ, VelZ);
  }
  if (D.UI[VerboseTime_].GetB()) printf("%f T AdvectField\n", Timer::PopTimer());

  // Diffusion steps
  if (D.UI[VerboseTime_].GetB()) Timer::PushTimer();
  if (D.UI[CoeffDiffuS_].GetB()) {
    // (Id - diffu Δt ∇²) smo = smo
    std::vector<std::vector<std::vector<float>>> oldSmoke= Smok;
    if (D.UI[SolvPCG_____].GetB())
      ConjugateGradientSolve(FieldID::IDSmok, maxIter, timestep, true, coeffDiffu, oldSmoke, Smok);
    else
      GaussSeidelSolve(FieldID::IDSmok, maxIter, timestep, true, coeffDiffu, oldSmoke, Smok);
  }
  if (D.UI[CoeffDiffuV_].GetB()) {
    // (Id - visco Δt ∇²) vel = vel
    std::vector<std::vector<std::vector<float>>> oldVelX= VelX;
    std::vector<std::vector<std::vector<float>>> oldVelY= VelY;
    std::vector<std::vector<std::vector<float>>> oldVelZ= VelZ;
    if (D.UI[SolvPCG_____].GetB()) {
      if (nX > 1) ConjugateGradientSolve(FieldID::IDVelX, maxIter, timestep, true, coeffVisco, oldVelX, VelX);
      if (nY > 1) ConjugateGradientSolve(FieldID::IDVelY, maxIter, timestep, true, coeffVisco, oldVelY, VelY);
      if (nZ > 1) ConjugateGradientSolve(FieldID::IDVelZ, maxIter, timestep, true, coeffVisco, oldVelZ, VelZ);
    }
    else {
      if (nX > 1) GaussSeidelSolve(FieldID::IDVelX, maxIter, timestep, true, coeffVisco, oldVelX, VelX);
      if (nY > 1) GaussSeidelSolve(FieldID::IDVelY, maxIter, timestep, true, coeffVisco, oldVelY, VelY);
      if (nZ > 1) GaussSeidelSolve(FieldID::IDVelZ, maxIter, timestep, true, coeffVisco, oldVelZ, VelZ);
    }
  }
  if (D.UI[VerboseTime_].GetB()) printf("%f T Diffusion\n", Timer::PopTimer());

  // Vorticity step
  if (D.UI[VerboseTime_].GetB()) Timer::PushTimer();
  if (D.UI[CoeffVorti__].GetB()) {
    VorticityConfinement(timestep, coeffVorti, VelX, VelY, VelZ);
  }
  if (D.UI[VerboseTime_].GetB()) printf("%f T VorticityConfinement\n", Timer::PopTimer());

  // External forces
  if (D.UI[VerboseTime_].GetB()) Timer::PushTimer();
  if (D.UI[CoeffGravi__].GetB()) {
    ExternalForces();
  }
  if (D.UI[VerboseTime_].GetB()) printf("%f T ExternalForces\n", Timer::PopTimer());

  // Projection step
  if (D.UI[VerboseTime_].GetB()) Timer::PushTimer();
  if (D.UI[CoeffProj___].GetB()) {
    ProjectField(maxIter, timestep, VelX, VelY, VelZ);
  }
  if (D.UI[VerboseTime_].GetB()) printf("%f T ProjectField\n", Timer::PopTimer());

  // Compute field data for display
  if (D.UI[VerboseTime_].GetB()) Timer::PushTimer();
  ComputeVelocityDivergence();
  if (D.UI[VerboseTime_].GetB()) printf("%f T Divergence\n", Timer::PopTimer());
  if (D.UI[VerboseTime_].GetB()) Timer::PushTimer();
  ComputeVelocityCurlVorticity();
  if (D.UI[VerboseTime_].GetB()) printf("%f T CurlVorticity\n", Timer::PopTimer());

  // TODO Compute fluid density to check if constant as it should be in incompressible case

  // TODO Test heuristic optimization of solid regions

  // TODO Introduce solid interface normals calculations to better handle BC on sloped geometry ?
  // TODO Test with flow separation scenarios ?

  if (D.UI[VerboseTime_].GetB()) Timer::PushTimer();
  CompuFluidDyna::SetUpUIData();
  if (D.UI[VerboseTime_].GetB()) printf("%f T SetUpUIData\n", Timer::PopTimer());

  if (D.UI[VerboseTime_].GetB()) printf("\n");
}


// Draw the project
void CompuFluidDyna::Draw() {
  if (!isActivProj) return;
  if (!isAllocated) return;
  if (!isRefreshed) return;
  if (D.UI[Verbose_____].GetB()) printf("CompuFluidDyna::Draw()\n");

  // Draw the voxels
  if (D.displayMode1) {
    glEnable(GL_LIGHTING);
    glLineWidth(2.0f);
    // Set the scene transformation
    glPushMatrix();
    glTranslatef(D.boxMin[0] + 0.5f * voxSize, D.boxMin[1] + 0.5f * voxSize, D.boxMin[2] + 0.5f * voxSize);
    glScalef(voxSize, voxSize, voxSize);
    // Sweep the field
    for (int x= 0; x < nX; x++) {
      for (int y= 0; y < nY; y++) {
        for (int z= 0; z < nZ; z++) {
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
    if (nX == 1) glScalef(0.1f, 1.0f, 1.0f);
    if (nY == 1) glScalef(1.0f, 0.1f, 1.0f);
    if (nZ == 1) glScalef(1.0f, 1.0f, 0.1f);
    // Sweep the field
    for (int x= 0; x < nX; x++) {
      for (int y= 0; y < nY; y++) {
        for (int z= 0; z < nZ; z++) {
          float r= 0.0f, g= 0.0f, b= 0.0f;
          // Color by smoke
          if (D.UI[ColorMode___].GetI() == 1) {
            if (std::abs(Smok[x][y][z]) < D.UI[ColorThresh_].GetF()) continue;
            Colormap::RatioToPlasma(0.5f + 0.5f * Smok[x][y][z] * D.UI[ColorFactor_].GetF(), r, g, b);
          }
          // Color by velocity magnitude
          if (D.UI[ColorMode___].GetI() == 2) {
            Math::Vec3f vec(VelX[x][y][z], VelY[x][y][z], VelZ[x][y][z]);
            if (vec.norm() < D.UI[ColorThresh_].GetF()) continue;
            Colormap::RatioToJetBrightSmooth(vec.norm() * D.UI[ColorFactor_].GetF(), r, g, b);
          }
          // Color by pressure
          if (D.UI[ColorMode___].GetI() == 3) {
            if (std::abs(Pres[x][y][z]) < D.UI[ColorThresh_].GetF()) continue;
            Colormap::RatioToBlueToRed(0.5f + 0.5f * Pres[x][y][z] * D.UI[ColorFactor_].GetF(), r, g, b);
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
          if (nX > 1 && nY > 1 && nZ > 1) glutWireCube(1.0);
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
    if (nX == 1) glTranslatef(voxSize, 0.0f, 0.0f);
    if (nY == 1) glTranslatef(0.0f, voxSize, 0.0f);
    if (nZ == 1) glTranslatef(0.0f, 0.0f, voxSize);
    glTranslatef(D.boxMin[0] + 0.5f * voxSize, D.boxMin[1] + 0.5f * voxSize, D.boxMin[2] + 0.5f * voxSize);
    glScalef(voxSize, voxSize, voxSize);
    // Sweep the field
    constexpr int nbLineWidths= 3;
    for (int k= 0; k < nbLineWidths; k++) {
      const float segmentRelLength= 1.0f - (float)k / (float)nbLineWidths;
      glLineWidth((float)k + 1.0f);
      glBegin(GL_LINES);
      for (int x= 0; x < nX; x++) {
        for (int y= 0; y < nY; y++) {
          for (int z= 0; z < nZ; z++) {
            // Draw the velocity field
            Math::Vec3f vec(VelX[x][y][z], VelY[x][y][z], VelZ[x][y][z]);
            if (vec.normSquared() > 0.0f) {
              float r= 0.0f, g= 0.0f, b= 0.0f;
              Colormap::RatioToJetBrightSmooth(vec.norm() * D.UI[ColorFactor_].GetF(), r, g, b);
              glColor3f(r, g, b);
              Math::Vec3f pos((float)x, (float)y, (float)z);
              glVertex3fv(pos.array());
              // glVertex3fv(pos + vec * segmentRelLength * D.UI[ScaleFactor_].GetF());
              // glVertex3fv(pos + vec.normalized() * segmentRelLength * D.UI[ScaleFactor_].GetF());
              // glVertex3fv(pos + vec.normalized() * segmentRelLength * D.UI[ScaleFactor_].GetF() * std::log(vec.norm() + 1.0f));
              glVertex3fv(pos + vec.normalized() * segmentRelLength * D.UI[ScaleFactor_].GetF() * std::sqrt(vec.norm()));
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
    if (nX == 1) glTranslatef(voxSize, 0.0f, 0.0f);
    if (nY == 1) glTranslatef(0.0f, voxSize, 0.0f);
    if (nZ == 1) glTranslatef(0.0f, 0.0f, voxSize);
    glTranslatef(D.boxMin[0] + 0.5f * voxSize, D.boxMin[1] + 0.5f * voxSize, D.boxMin[2] + 0.5f * voxSize);
    glScalef(voxSize, voxSize, voxSize);
    // Sweep the field
    glBegin(GL_LINES);
    for (int x= 0; x < nX; x++) {
      for (int y= 0; y < nY; y++) {
        for (int z= 0; z < nZ; z++) {
          // Draw the velocity field
          Math::Vec3f vec(AdvX[x][y][z], AdvY[x][y][z], AdvZ[x][y][z]);
          if (vec.normSquared() > 0.0f) {
            const float r= 0.5f - vec[0];
            const float g= 0.5f - vec[1];
            const float b= 0.5f - vec[2];
            glColor3f(r, g, b);
            Math::Vec3f pos((float)x, (float)y, (float)z);
            glVertex3fv(pos.array());
            glVertex3fv(pos + vec);
          }
        }
      }
    }
    glEnd();
    glPopMatrix();
  }
}


void CompuFluidDyna::SetUpUIData() {
  // Draw the scatter data
  const int yCursor= std::min(std::max((int)std::round((float)(nY - 1) * D.UI[SlicePlotY__].GetF()), 0), nY - 1);
  const int zCursor= std::min(std::max((int)std::round((float)(nZ - 1) * D.UI[SlicePlotZ__].GetF()), 0), nZ - 1);
  D.scatLegend.resize(4);
  D.scatLegend[0]= "Horiz VZ";
  D.scatLegend[1]= "Verti VY";
  D.scatLegend[2]= "Horiz P";
  D.scatLegend[3]= "Verti P";
  D.scatData.resize(4);
  for (int k= 0; k < (int)D.scatData.size(); k++)
    D.scatData[k].clear();
  if (nZ > 1) {
    for (int y= 0; y < nY; y++) {
      D.scatData[0].push_back(std::array<double, 2>({(double)y / (double)(nY - 1), VelZ[nX / 2][y][zCursor]}));
      D.scatData[2].push_back(std::array<double, 2>({(double)y / (double)(nY - 1), Pres[nX / 2][y][zCursor]}));
    }
  }
  if (nY > 1) {
    for (int z= 0; z < nZ; z++) {
      D.scatData[1].push_back(std::array<double, 2>({VelY[nX / 2][yCursor][z], (double)z / (double)(nZ - 1)}));
      D.scatData[3].push_back(std::array<double, 2>({Pres[nX / 2][yCursor][z], (double)z / (double)(nZ - 1)}));
    }
  }

  // Add hard coded experimental values for lid driven cavity flow benchmark
  if (D.UI[Scenario____].GetI() == 3) {
    D.scatLegend.resize(4);
    D.scatLegend[2]= "Ghia Re1k";
    D.scatLegend[3]= "Ghia Re1k";
    D.scatData.resize(4);
    D.scatData[2].clear();
    D.scatData[3].clear();
    // Data from Ghia 1982 http://www.msaidi.ir/upload/Ghia1982.pdf
    const std::vector<double> GhiaData0X({0.0000, +0.0625, +0.0703, +0.0781, +0.0938, +0.1563, +0.2266, +0.2344, +0.5000, +0.8047, +0.8594, +0.9063, +0.9453, +0.9531, +0.9609, +0.9688, +1.0000});  // coord on horiz slice
    const std::vector<double> GhiaData1Y({0.0000, +0.0547, +0.0625, +0.0703, +0.1016, +0.1719, +0.2813, +0.4531, +0.5000, +0.6172, +0.7344, +0.8516, +0.9531, +0.9609, +0.9688, +0.9766, +1.0000});  // coord on verti slice
    // const std::vector<double> GhiaData0Y({0.0000, +0.0923, +0.1009, +0.1089, +0.1232, +0.1608, +0.1751, +0.1753, +0.0545, -0.2453, -0.2245, -0.1691, -0.1031, -0.0886, -0.0739, -0.0591, +0.0000});  // Re 100   verti vel on horiz slice
    // const std::vector<double> GhiaData1X({0.0000, -0.0372, -0.0419, -0.0478, -0.0643, -0.1015, -0.1566, -0.2109, -0.2058, -0.1364, +0.0033, +0.2315, +0.6872, +0.7372, +0.7887, +0.8412, +1.0000});  // Re 100   horiz vel on verti slice
    // const std::vector<double> GhiaData0Y({0.0000, +0.1836, +0.1971, +0.2092, +0.2297, +0.2812, +0.3020, +0.3017, +0.0519, -0.3860, -0.4499, -0.2383, -0.2285, -0.1925, -0.1566, -0.1215, +0.0000});  // Re 400   verti vel on horiz slice
    // const std::vector<double> GhiaData1X({0.0000, -0.0819, -0.0927, -0.1034, -0.1461, -0.2430, -0.3273, -0.1712, -0.1148, +0.0214, +0.1626, +0.2909, +0.5589, +0.6176, +0.6844, +0.7584, +1.0000});  // Re 400   horiz vel on verti slice
    const std::vector<double> GhiaData0Y({0.0000, +0.2749, +0.2901, +0.3035, +0.3263, +0.3710, +0.3308, +0.3224, +0.0253, -0.3197, -0.4267, -0.5150, -0.3919, -0.3371, -0.2767, -0.2139, +0.0000});  // Re 1000  verti vel on horiz slice
    const std::vector<double> GhiaData1X({0.0000, -0.1811, -0.2020, -0.2222, -0.2973, -0.3829, -0.2781, -0.1065, -0.0608, +0.0570, +0.1872, +0.3330, +0.4660, +0.5112, +0.5749, +0.6593, +1.0000});  // Re 1000  horiz vel on verti slice
    // const std::vector<double> GhiaData0Y({0.0000, +0.3956, +0.4092, +0.4191, +0.4277, +0.3712, +0.2903, +0.2819, +0.0100, -0.3118, -0.3740, -0.4431, -0.5405, -0.5236, -0.4743, -0.3902, +0.0000});  // Re 3200  verti vel on horiz slice
    // const std::vector<double> GhiaData1X({0.0000, -0.3241, -0.3534, -0.3783, -0.4193, -0.3432, -0.2443, -0.8664, -0.0427, +0.0716, +0.1979, +0.3468, +0.4610, +0.4655, +0.4830, +0.5324, +1.0000});  // Re 3200  horiz vel on verti slice
    // const std::vector<double> GhiaData0Y({0.0000, +0.4245, +0.4333, +0.4365, +0.4295, +0.3537, +0.2807, +0.2728, +0.0095, -0.3002, -0.3621, -0.4144, -0.5288, -0.5541, -0.5507, -0.4977, +0.0000});  // Re 5000  verti vel on horiz slice
    // const std::vector<double> GhiaData1X({0.0000, -0.4117, -0.4290, -0.4364, -0.4044, -0.3305, -0.2286, -0.0740, -0.0304, +0.0818, +0.2009, +0.3356, +0.4604, +0.4599, +0.4612, +0.4822, +1.0000});  // Re 5000  horiz vel on verti slice
    // const std::vector<double> GhiaData0Y({0.0000, +0.4398, +0.4403, +0.4356, +0.4182, +0.3506, +0.2812, +0.2735, +0.0082, -0.3045, -0.3621, -0.4105, -0.4859, -0.5235, -0.5522, -0.5386, +0.0000});  // Re 7500  verti vel on horiz slice
    // const std::vector<double> GhiaData1X({0.0000, -0.4315, -0.4359, -0.4303, -0.3832, -0.3239, -0.2318, -0.0750, -0.0380, +0.0834, +0.2059, +0.3423, +0.4717, +0.4732, +0.4705, +0.4724, +1.0000});  // Re 7500  horiz vel on verti slice
    // const std::vector<double> GhiaData0Y({0.0000, +0.4398, +0.4373, +0.4312, +0.4149, +0.3507, +0.2800, +0.2722, +0.0083, -0.3072, -0.3674, -0.4150, -0.4586, -0.4910, -0.5299, -0.5430, +0.0000});  // Re 10000 verti vel on horiz slice
    // const std::vector<double> GhiaData1X({0.0000, -0.4274, -0.4254, -0.4166, -0.3800, -0.3271, -0.2319, -0.0754, +0.0311, +0.0834, +0.2067, +0.3464, +0.4780, +0.4807, +0.4778, +0.4722, +1.0000});  // Re 10000 horiz vel on verti slice
    for (int k= 0; k < (int)GhiaData0X.size(); k++) {
      D.scatData[2].push_back(std::array<double, 2>({GhiaData0X[k], GhiaData0Y[k]}));
      D.scatData[3].push_back(std::array<double, 2>({GhiaData1X[k], GhiaData1Y[k]}));
    }
    D.scatLegend.resize(6);
    D.scatLegend[4]= "Ertu Re1k";
    D.scatLegend[5]= "Ertu Re1k";
    D.scatData.resize(6);
    D.scatData[4].clear();
    D.scatData[5].clear();
    // Data from Erturk 2005 https://arxiv.org/pdf/physics/0505121.pdf
    const std::vector<double> ErtuData0X({0.0000, +0.0150, +0.0300, +0.0450, +0.0600, +0.0750, +0.0900, +0.1050, +0.1200, +0.1350, +0.1500, +0.5000, +0.8500, +0.8650, +0.8800, +0.8950, +0.9100, +0.9250, +0.9400, +0.9550, +0.9700, +0.9850, +1.0000});  // coord on horiz slice
    const std::vector<double> ErtuData1Y({0.0000, +0.0200, +0.0400, +0.0600, +0.0800, +0.1000, +0.1200, +0.1400, +0.1600, +0.1800, +0.2000, +0.5000, +0.9000, +0.9100, +0.9200, +0.9300, +0.9400, +0.9500, +0.9600, +0.9700, +0.9800, +0.9900, +1.0000});  // coord on verti slice
    const std::vector<double> ErtuData0Y({0.0000, +0.1019, +0.1792, +0.2349, +0.2746, +0.3041, +0.3273, +0.3460, +0.3605, +0.3705, +0.3756, +0.0258, -0.4028, -0.4407, -0.4803, -0.5132, -0.5263, -0.5052, -0.4417, -0.3400, -0.2173, -0.0973, +0.0000});  // Re 1000  verti vel on horiz slice
    const std::vector<double> ErtuData1X({0.0000, -0.0757, -0.1392, -0.1951, -0.2472, -0.2960, -0.3381, -0.3690, -0.3854, -0.3869, -0.3756, -0.0620, +0.3838, +0.3913, +0.3993, +0.4101, +0.4276, +0.4582, +0.5102, +0.5917, +0.7065, +0.8486, +1.0000});  // Re 1000  horiz vel on verti slice
    // const std::vector<double> ErtuData0Y({0.0000, +0.1607, +0.2633, +0.3238, +0.3649, +0.3950, +0.4142, +0.4217, +0.4187, +0.4078, +0.3918, +0.0160, -0.3671, -0.3843, -0.4042, -0.4321, -0.4741, -0.5268, -0.5603, -0.5192, -0.3725, -0.1675, +0.0000});  // Re 2500  verti vel on horiz slice
    // const std::vector<double> ErtuData1X({0.0000, -0.1517, -0.2547, -0.3372, -0.3979, -0.4250, -0.4200, -0.3965, -0.3688, -0.3439, -0.3228, -0.0403, +0.4141, +0.4256, +0.4353, +0.4424, +0.4470, +0.4506, +0.4607, +0.4971, +0.5924, +0.7704, +1.0000});  // Re 2500  horiz vel on verti slice
    // const std::vector<double> ErtuData0Y({0.0000, +0.2160, +0.3263, +0.3868, +0.4258, +0.4426, +0.4403, +0.4260, +0.4070, +0.3878, +0.3699, +0.0117, -0.3624, -0.3806, -0.3982, -0.4147, -0.4318, -0.4595, -0.5139, -0.5700, -0.5019, -0.2441, +0.0000});  // Re 5000  verti vel on horiz slice
    // const std::vector<double> ErtuData1X({0.0000, -0.2223, -0.3480, -0.4272, -0.4419, -0.4168, -0.3876, -0.3652, -0.3467, -0.3285, -0.3100, -0.0319, +0.4155, +0.4307, +0.4452, +0.4582, +0.4683, +0.4738, +0.4739, +0.4749, +0.5159, +0.6866, +1.0000});  // Re 5000  horiz vel on verti slice
    // const std::vector<double> ErtuData0Y({0.0000, +0.2509, +0.3608, +0.4210, +0.4494, +0.4495, +0.4337, +0.4137, +0.3950, +0.3779, +0.3616, +0.0099, -0.3574, -0.3755, -0.3938, -0.4118, -0.4283, -0.4443, -0.4748, -0.5434, -0.5550, -0.2991, +0.0000});  // Re 7500  verti vel on horiz slice
    // const std::vector<double> ErtuData1X({0.0000, -0.2633, -0.3980, -0.4491, -0.4284, -0.3978, -0.3766, -0.3587, -0.3406, -0.3222, -0.3038, -0.0287, +0.4123, +0.4275, +0.4431, +0.4585, +0.4723, +0.4824, +0.4860, +0.4817, +0.4907, +0.6300, +1.0000});  // Re 7500  horiz vel on verti slice
    // const std::vector<double> ErtuData0Y({0.0000, +0.2756, +0.3844, +0.4409, +0.4566, +0.4449, +0.4247, +0.4056, +0.3885, +0.3722, +0.3562, +0.0088, -0.3538, -0.3715, -0.3895, -0.4078, -0.4256, -0.4411, -0.4592, -0.5124, -0.5712, -0.3419, +0.0000});  // Re 10000 verti vel on horiz slice
    // const std::vector<double> ErtuData1X({0.0000, -0.2907, -0.4259, -0.4469, -0.4142, -0.3899, -0.3721, -0.3543, -0.3361, -0.3179, -0.2998, -0.0268, +0.4095, +0.4243, +0.4398, +0.4556, +0.4711, +0.4843, +0.4917, +0.4891, +0.4837, +0.5891, +1.0000});  // Re 10000 horiz vel on verti slice
    // const std::vector<double> ErtuData0Y({0.0000, +0.2940, +0.4018, +0.4522, +0.4563, +0.4383, +0.4180, +0.4004, +0.3840, +0.3678, +0.3519, +0.0080, -0.3508, -0.3682, -0.3859, -0.4040, -0.4221, -0.4388, -0.4534, -0.4899, -0.5694, -0.3762, +0.0000});  // Re 12500 verti vel on horiz slice
    // const std::vector<double> ErtuData1X({0.0000, -0.3113, -0.4407, -0.4380, -0.4054, -0.3859, -0.3685, -0.3506, -0.3326, -0.3146, -0.2967, -0.0256, +0.4070, +0.4216, +0.4366, +0.4523, +0.4684, +0.4833, +0.4937, +0.4941, +0.4833, +0.5587, +1.0000});  // Re 12500 horiz vel on verti slice
    // const std::vector<double> ErtuData0Y({0.0000, +0.3083, +0.4152, +0.4580, +0.4529, +0.4323, +0.4132, +0.3964, +0.3801, +0.3641, +0.3483, +0.0074, -0.3481, -0.3654, -0.3828, -0.4005, -0.4186, -0.4361, -0.4505, -0.4754, -0.5593, -0.4041, +0.0000});  // Re 15000 verti vel on horiz slice
    // const std::vector<double> ErtuData1X({0.0000, -0.3278, -0.4474, -0.4286, -0.4001, -0.3827, -0.3652, -0.3474, -0.3297, -0.3119, -0.2942, -0.0247, +0.4047, +0.4190, +0.4338, +0.4492, +0.4653, +0.4811, +0.4937, +0.4969, +0.4850, +0.5358, +1.0000});  // Re 15000 horiz vel on verti slice
    // const std::vector<double> ErtuData0Y({0.0000, +0.3197, +0.4254, +0.4602, +0.4484, +0.4273, +0.4093, +0.3929, +0.3767, +0.3608, +0.3452, +0.0069, -0.3457, -0.3627, -0.3800, -0.3975, -0.4153, -0.4331, -0.4482, -0.4664, -0.5460, -0.4269, +0.0000});  // Re 17500 verti vel on horiz slice
    // const std::vector<double> ErtuData1X({0.0000, -0.3412, -0.4490, -0.4206, -0.3965, -0.3797, -0.3622, -0.3446, -0.3271, -0.3096, -0.2920, -0.0240, +0.4024, +0.4166, +0.4312, +0.4463, +0.4622, +0.4784, +0.4925, +0.4982, +0.4871, +0.5183, +1.0000});  // Re 17500 horiz vel on verti slice
    // const std::vector<double> ErtuData0Y({0.0000, +0.3290, +0.4332, +0.4601, +0.4438, +0.4232, +0.4060, +0.3897, +0.3736, +0.3579, +0.3423, +0.0065, -0.3434, -0.3603, -0.3774, -0.3946, -0.4122, -0.4300, -0.4459, -0.4605, -0.5321, -0.4457, +0.0000});  // Re 20000 verti vel on horiz slice
    // const std::vector<double> ErtuData1X({0.0000, -0.3523, -0.4475, -0.4143, -0.3936, -0.3769, -0.3595, -0.3422, -0.3248, -0.3074, -0.2899, -0.0234, +0.4001, +0.4142, +0.4287, +0.4436, +0.4592, +0.4754, +0.4906, +0.4985, +0.4889, +0.5048, +1.0000});  // Re 20000 horiz vel on verti slice
    // const std::vector<double> ErtuData0Y({0.0000, +0.3323, +0.4357, +0.4596, +0.4420, +0.4218, +0.4048, +0.3885, +0.3725, +0.3567, +0.3413, +0.0063, -0.3425, -0.3593, -0.3764, -0.3936, -0.4110, -0.4287, -0.4449, -0.4588, -0.5266, -0.4522, +0.0000});  // Re 21000 verti vel on horiz slice
    // const std::vector<double> ErtuData1X({0.0000, -0.3562, -0.4463, -0.4121, -0.3925, -0.3758, -0.3585, -0.3412, -0.3239, -0.3066, -0.2892, -0.0232, +0.3992, +0.4132, +0.4277, +0.4425, +0.4580, +0.4742, +0.4897, +0.4983, +0.4895, +0.5003, +1.0000});  // Re 21000 horiz vel on verti slice

    for (int k= 0; k < (int)ErtuData0X.size(); k++) {
      D.scatData[4].push_back(std::array<double, 2>({ErtuData0X[k], ErtuData0Y[k]}));
      D.scatData[5].push_back(std::array<double, 2>({ErtuData1X[k], ErtuData1Y[k]}));
    }
  }

  // Add hard coded analytical values for Poiseuille flow benchmark
  if (D.UI[Scenario____].GetI() == 6) {
    D.scatData[0].clear();
    D.scatData[3].clear();
    D.scatLegend.resize(6);
    D.scatData.resize(6);
    D.scatLegend[4]= "Analy VY";
    D.scatLegend[5]= "Analy P";
    D.scatData[4].clear();
    D.scatData[5].clear();
    const float press0= D.UI[BCPres______].GetF();
    const float press1= -D.UI[BCPres______].GetF();
    const float kinVisco= D.UI[CoeffDiffuV_].GetF();
    if (nY > 1) {
      for (int z= 0; z < nZ; z++) {
        const float width= voxSize * (float)(nY - 1);
        const float height= voxSize * (float)(nZ - 1);
        const float posZ= (float)z * voxSize;
        const float pressDiff= (press1 - press0) / width;
        const float analyVelY= -pressDiff * (1.0f / (2.0f * kinVisco)) * posZ * (height - posZ);
        D.scatData[4].push_back(std::array<double, 2>({analyVelY, (double)z / (double)(nZ - 1)}));
      }
    }
    if (nZ > 1) {
      for (int y= 0; y < nY; y++) {
        const float analyP= press0 + (press1 - press0) * (float)y / (float)(nY - 1);
        D.scatData[5].push_back(std::array<double, 2>({(double)y / (double)(nY - 1), analyP}));
      }
    }
  }

  if (!D.UI[VerboseSolv_].GetB()) {
    // Draw the plot data
    D.plotData.resize(5);
    D.plotLegend.resize(5);
    D.plotLegend[0]= "VelMag";
    D.plotLegend[1]= "Smok";
    D.plotLegend[2]= "Pres";
    D.plotLegend[3]= "DiveAbs";
    D.plotLegend[4]= "Vorti";
    if (D.plotData[0].size() < 1000) {
      for (int k= 0; k < (int)D.plotLegend.size(); k++)
        D.plotData[k].push_back(0.0f);
      for (int x= 0; x < nX; x++) {
        for (int y= 0; y < nY; y++) {
          for (int z= 0; z < nZ; z++) {
            D.plotData[0][D.plotData[0].size() - 1]+= std::sqrt(VelX[x][y][z] * VelX[x][y][z] + VelY[x][y][z] * VelY[x][y][z] + VelZ[x][y][z] * VelZ[x][y][z]);
            D.plotData[1][D.plotData[1].size() - 1]+= Smok[x][y][z];
            D.plotData[2][D.plotData[2].size() - 1]+= Pres[x][y][z];
            D.plotData[3][D.plotData[3].size() - 1]+= std::abs(Dive[x][y][z]);
            D.plotData[4][D.plotData[4].size() - 1]+= Vort[x][y][z];
          }
        }
      }
    }
  }
}


void CompuFluidDyna::InitializeScenario() {
  // Get scenario ID and optionnally load bitmap file
  const int scenarioType= D.UI[Scenario____].GetI();
  const int inputFile= D.UI[InputFile___].GetI();
  std::vector<std::vector<std::array<float, 4>>> imageRGBA;
  if (scenarioType == 0) {
    if (inputFile == 0) FileInput::LoadImageBMPFile("Resources/CFD_TeslaValveTwinSharp.bmp", imageRGBA, false);
    if (inputFile == 1) FileInput::LoadImageBMPFile("Resources/CFD_Venturi.bmp", imageRGBA, false);
    if (inputFile == 2) FileInput::LoadImageBMPFile("Resources/CFD_Wing.bmp", imageRGBA, false);
    if (inputFile == 3) FileInput::LoadImageBMPFile("Resources/CFD_Nozzle.bmp", imageRGBA, false);
    if (inputFile == 4) FileInput::LoadImageBMPFile("Resources/CFD_Wall.bmp", imageRGBA, false);
    if (inputFile == 5) FileInput::LoadImageBMPFile("Resources/CFD_Pipe.bmp", imageRGBA, false);
    if (inputFile == 6) FileInput::LoadImageBMPFile("Resources/CFD_CriCri.bmp", imageRGBA, false);
    if (inputFile == 7) FileInput::LoadImageBMPFile("Resources/CFD_TestScenario.bmp", imageRGBA, false);
  }

  // Set scenario values
  for (int x= 0; x < nX; x++) {
    for (int y= 0; y < nY; y++) {
      for (int z= 0; z < nZ; z++) {
        // Scenario from loaded BMP file
        if (scenarioType == 0 && !imageRGBA.empty()) {
          const float posW= (float)(imageRGBA.size() - 1) * ((float)y + 0.5f) / (float)nY;
          const float posH= (float)(imageRGBA[0].size() - 1) * ((float)z + 0.5f) / (float)nZ;
          const int idxPixelW= std::min(std::max((int)std::round(posW), 0), (int)imageRGBA.size() - 1);
          const int idxPixelH= std::min(std::max((int)std::round(posH), 0), (int)imageRGBA[0].size() - 1);
          const std::array<float, 4> colRGBA= imageRGBA[idxPixelW][idxPixelH];
          if (colRGBA[3] < 0.1f) {
            Solid[x][y][z]= true;
          }
          else {
            if (std::abs(colRGBA[0] - 0.5f) > 0.1f) PreBC[x][y][z]= true;
            if (std::abs(colRGBA[1] - 0.5f) > 0.1f) VelBC[x][y][z]= true;
            if (std::abs(colRGBA[2] - 0.5f) > 0.1f) SmoBC[x][y][z]= true;
          }
          if (PreBC[x][y][z]) {
            PresForced[x][y][z]= (colRGBA[0] > 0.5f) ? (D.UI[BCPres______].GetF()) : (-D.UI[BCPres______].GetF());
          }
          if (VelBC[x][y][z]) {
            VelXForced[x][y][z]= (colRGBA[1] > 0.5f) ? (D.UI[BCVelX______].GetF()) : (-D.UI[BCVelX______].GetF());
            VelYForced[x][y][z]= (colRGBA[1] > 0.5f) ? (D.UI[BCVelY______].GetF()) : (-D.UI[BCVelY______].GetF());
            VelZForced[x][y][z]= (colRGBA[1] > 0.5f) ? (D.UI[BCVelZ______].GetF()) : (-D.UI[BCVelZ______].GetF());
          }
          if (SmoBC[x][y][z]) {
            SmokForced[x][y][z]= (colRGBA[2] > 0.5f) ? (D.UI[BCSmok______].GetF()) : (-D.UI[BCSmok______].GetF());
          }
        }

        // Double facing inlets
        if (scenarioType == 1) {
          if ((nX > 1 && (x == 0 || x == nX - 1)) ||
              (nY > 1 && (y == 0 || y == nY - 1))) {
            PreBC[x][y][z]= true;
            PresForced[x][y][z]= 0.0f;
          }
          else if (nZ > 1 && (z == 0 || z == nZ - 1)) {
            Solid[x][y][z]= true;
          }
          else {
            for (int k= 0; k < 2; k++) {
              Math::Vec3f posCell(((float)x + 0.5f) / (float)nX, ((float)y + 0.5f) / (float)nY, ((float)z + 0.5f) / (float)nZ);
              Math::Vec3f posObstacle(D.UI[ObjectPosX__].GetF(), D.UI[ObjectPosY__].GetF(), D.UI[ObjectPosZ__].GetF());
              if (k == 1) posObstacle= Math::Vec3f(1.0f, 1.0f, 1.0f) - posObstacle;
              if ((posCell - posObstacle).norm() <= std::max(D.UI[ObjectSize__].GetF(), 0.0f)) {
                VelBC[x][y][z]= true;
                SmoBC[x][y][z]= true;
                VelXForced[x][y][z]= (k == 1) ? (-D.UI[BCVelX______].GetF()) : (D.UI[BCVelX______].GetF());
                VelYForced[x][y][z]= (k == 1) ? (-D.UI[BCVelY______].GetF()) : (D.UI[BCVelY______].GetF());
                VelZForced[x][y][z]= (k == 1) ? (-D.UI[BCVelZ______].GetF()) : (D.UI[BCVelZ______].GetF());
                SmokForced[x][y][z]= (k == 1) ? (-D.UI[BCSmok______].GetF()) : (D.UI[BCSmok______].GetF());
              }
            }
          }
        }

        // Circular obstacle in corridor showing vortex shedding
        if (scenarioType == 2) {
          if ((nX > 1 && (x == 0 || x == nX - 1)) ||
              (nZ > 1 && (z == 0 || z == nZ - 1))) {
            Solid[x][y][z]= true;
          }
          else if (y == nY - 1) {
            PreBC[x][y][z]= true;
            PresForced[x][y][z]= 0.0f;
          }
          else if (y == 0) {
            VelBC[x][y][z]= true;
            VelXForced[x][y][z]= D.UI[BCVelX______].GetF();
            VelYForced[x][y][z]= D.UI[BCVelY______].GetF();
            VelZForced[x][y][z]= D.UI[BCVelZ______].GetF();
            SmoBC[x][y][z]= true;
            SmokForced[x][y][z]= D.UI[BCSmok______].GetF();
          }
          else {
            Math::Vec3f posCell(((float)x + 0.5f) / (float)nX, ((float)y + 0.5f) / (float)nY, ((float)z + 0.5f) / (float)nZ);
            Math::Vec3f posObstacle(D.UI[ObjectPosX__].GetF(), D.UI[ObjectPosY__].GetF(), D.UI[ObjectPosZ__].GetF());
            Math::Vec3f dist= (posCell - posObstacle);
            dist[0]*= (float)(nX - 1) * voxSize;
            dist[1]*= (float)(nY - 1) * voxSize;
            dist[2]*= (float)(nZ - 1) * voxSize;
            if (dist.norm() <= std::max(D.UI[ObjectSize__].GetF(), 0.0f))
              Solid[x][y][z]= true;
          }
        }

        // Cavity lid shear benchmark
        if (scenarioType == 3) {
          if (y == 0 || y == nY - 1 || z == 0) {
            Solid[x][y][z]= true;
          }
          else if (z == nZ - 1) {
            VelBC[x][y][z]= true;
            VelXForced[x][y][z]= D.UI[BCVelX______].GetF();
            VelYForced[x][y][z]= D.UI[BCVelY______].GetF();
            VelZForced[x][y][z]= D.UI[BCVelZ______].GetF();
          }
          else if (y == nY / 2 && z > nZ / 2) {
            SmoBC[x][y][z]= true;
            SmokForced[x][y][z]= D.UI[BCSmok______].GetF();
          }
        }

        // Vortex ring with inlet in Y-, outlet in Y+ and wall with hole in the corridor
        if (scenarioType == 4) {
          const int wallPos= std::round(D.UI[ObjectPosY__].GetF() * (float)nY);
          const Math::Vec3f posCell(((float)x + 0.5f) / (float)nX, 0.0f, ((float)z + 0.5f) / (float)nZ);
          const Math::Vec3f posObstacle(D.UI[ObjectPosX__].GetF(), 0.0f, D.UI[ObjectPosZ__].GetF());
          Math::Vec3f dist= (posCell - posObstacle);
          dist[0]*= (float)(nX - 1) * voxSize;
          dist[1]*= (float)(nY - 1) * voxSize;
          dist[2]*= (float)(nZ - 1) * voxSize;
          if ((nX > 1 && (x == 0 || x == nX - 1)) ||
              (nZ > 1 && (z == 0 || z == nZ - 1))) {
            Solid[x][y][z]= true;
          }
          else if (std::abs(y - wallPos) <= nY / 6 && dist.norm() >= std::max(D.UI[ObjectSize__].GetF(), 0.0f)) {
            Solid[x][y][z]= true;
          }
          else if (y == 0) {
            VelBC[x][y][z]= true;
            VelXForced[x][y][z]= D.UI[BCVelX______].GetF();
            VelYForced[x][y][z]= D.UI[BCVelY______].GetF();
            VelZForced[x][y][z]= D.UI[BCVelZ______].GetF();
            SmoBC[x][y][z]= true;
            SmokForced[x][y][z]= D.UI[BCSmok______].GetF();
          }
          else if (y == nY - 1) {
            PreBC[x][y][z]= true;
            PresForced[x][y][z]= 0.0f;
          }
        }

        // Central bloc with initial velocity
        if (scenarioType == 5) {
          if (((nX == 1) != (std::min(x, nX - 1 - x) > nX / 3)) &&
              ((nY == 1) != (std::min(y, nY - 1 - y) > nY / 3)) &&
              ((nZ == 1) != (std::min(z, nZ - 1 - z) > nZ / 3))) {
            VelX[x][y][z]= D.UI[BCVelX______].GetF();
            VelY[x][y][z]= D.UI[BCVelY______].GetF();
            VelZ[x][y][z]= D.UI[BCVelZ______].GetF();
            Smok[x][y][z]= (std::min(z, nZ - 1 - z) < 4 * (nZ - 1) / 9) ? -D.UI[BCSmok______].GetF() : D.UI[BCSmok______].GetF();
          }
        }

        // Poiseuille/Couette flow in tube with pressure gradient
        if (scenarioType == 6) {
          if ((nX > 1 && (x == 0 || x == nX - 1)) ||
              (nZ > 1 && (z == 0 || z == nZ - 1))) {
            VelBC[x][y][z]= true;
            VelXForced[x][y][z]= (z < nZ / 2) ? -D.UI[BCVelX______].GetF() : D.UI[BCVelX______].GetF();
            VelYForced[x][y][z]= (z < nZ / 2) ? -D.UI[BCVelY______].GetF() : D.UI[BCVelY______].GetF();
            VelZForced[x][y][z]= (z < nZ / 2) ? -D.UI[BCVelZ______].GetF() : D.UI[BCVelZ______].GetF();
          }
          else if (nY > 1 && (y == 0 || y == nY - 1)) {
            PreBC[x][y][z]= true;
            PresForced[x][y][z]= (y > nY / 2) ? -D.UI[BCPres______].GetF() : D.UI[BCPres______].GetF();
          }
          else if (std::max(y, nY - 1 - y) == nY / 2) {
            SmoBC[x][y][z]= true;
            SmokForced[x][y][z]= D.UI[BCSmok______].GetF();
          }
        }

        // Thermal convection cell
        if (scenarioType == 7) {
          if ((nX > 1 && (x == 0 || x == nX - 1)) ||
              (nY > 1 && (y == 0 || y == nY - 1)) ||
              (nZ > 1 && (z == 0 || z == nZ - 1))) {
            Solid[x][y][z]= true;
          }
          else if (nZ > 1 && std::min(z, nZ - 1 - z) < nZ / 3) {
            Smok[x][y][z]= (z < nZ / 2) ? D.UI[BCSmok______].GetF() : -D.UI[BCSmok______].GetF();
            Smok[x][y][z]+= Random::Val(-0.01f, 0.01f);
          }
        }

        // Compression barrel test
        if (scenarioType == 8) {
          if ((nX > 1 && (x == 0 || x == nX - 1)) ||
              (nY > 1 && (y == 0 || y == nY - 1)) ||
              (nZ > 1 && (z == 0 || z == nZ - 1))) {
            Solid[x][y][z]= true;
          }
          else if (std::min(y, nY - 1 - y) == (nY - 1) / 2) {
            Smok[x][y][z]= D.UI[BCSmok______].GetF();
            VelY[x][y][z]= D.UI[BCVelY______].GetF();
          }
        }
      }
    }
  }
}


// Apply boundary conditions enforcing fixed values to fields
void CompuFluidDyna::ApplyBC(const int iFieldID, std::vector<std::vector<std::vector<float>>>& ioField) {
  // Sweep through the field
  for (int x= 0; x < nX; x++) {
    for (int y= 0; y < nY; y++) {
      for (int z= 0; z < nZ; z++) {
        // Set forced value
        if (Solid[x][y][z] && iFieldID == FieldID::IDSmok) ioField[x][y][z]= 0.0f;
        if (Solid[x][y][z] && iFieldID == FieldID::IDVelX) ioField[x][y][z]= 0.0f;
        if (Solid[x][y][z] && iFieldID == FieldID::IDVelY) ioField[x][y][z]= 0.0f;
        if (Solid[x][y][z] && iFieldID == FieldID::IDVelZ) ioField[x][y][z]= 0.0f;
        if (Solid[x][y][z] && iFieldID == FieldID::IDPres) ioField[x][y][z]= 0.0f;
        if (SmoBC[x][y][z] && iFieldID == FieldID::IDSmok) ioField[x][y][z]= SmokForced[x][y][z] * std::cos(simTime * 2.0f * M_PI / D.UI[BCSmokTime__].GetF());
        if (VelBC[x][y][z] && iFieldID == FieldID::IDVelX) ioField[x][y][z]= VelXForced[x][y][z];
        if (VelBC[x][y][z] && iFieldID == FieldID::IDVelY) ioField[x][y][z]= VelYForced[x][y][z];
        if (VelBC[x][y][z] && iFieldID == FieldID::IDVelZ) ioField[x][y][z]= VelZForced[x][y][z];
        if (PreBC[x][y][z] && iFieldID == FieldID::IDPres) ioField[x][y][z]= PresForced[x][y][z];
      }
    }
  }
}


// Addition of one field to an other
void CompuFluidDyna::ImplicitFieldAdd(const std::vector<std::vector<std::vector<float>>>& iFieldA,
                                      const std::vector<std::vector<std::vector<float>>>& iFieldB,
                                      std::vector<std::vector<std::vector<float>>>& oField) {
  for (int x= 0; x < nX; x++)
    for (int y= 0; y < nY; y++)
      for (int z= 0; z < nZ; z++)
        oField[x][y][z]= iFieldA[x][y][z] + iFieldB[x][y][z];
}


// Subtraction of one field to an other
void CompuFluidDyna::ImplicitFieldSub(const std::vector<std::vector<std::vector<float>>>& iFieldA,
                                      const std::vector<std::vector<std::vector<float>>>& iFieldB,
                                      std::vector<std::vector<std::vector<float>>>& oField) {
  for (int x= 0; x < nX; x++)
    for (int y= 0; y < nY; y++)
      for (int z= 0; z < nZ; z++)
        oField[x][y][z]= iFieldA[x][y][z] - iFieldB[x][y][z];
}


// Multiplication of field by scalar
void CompuFluidDyna::ImplicitFieldScale(const float iVal,
                                        const std::vector<std::vector<std::vector<float>>>& iField,
                                        std::vector<std::vector<std::vector<float>>>& oField) {
  for (int x= 0; x < nX; x++)
    for (int y= 0; y < nY; y++)
      for (int z= 0; z < nZ; z++)
        oField[x][y][z]= iField[x][y][z] * iVal;
}


// Dot product between two fields
float CompuFluidDyna::ImplicitFieldDotProd(const std::vector<std::vector<std::vector<float>>>& iFieldA,
                                           const std::vector<std::vector<std::vector<float>>>& iFieldB) {
  float val= 0.0f;
  for (int x= 0; x < nX; x++)
    for (int y= 0; y < nY; y++)
      for (int z= 0; z < nZ; z++)
        val+= iFieldA[x][y][z] * iFieldB[x][y][z];
  return val;
}


// TODO check BC properly handled
// Perform a matrix-vector multiplication without explicitly assembling the Laplacian matrix
void CompuFluidDyna::ImplicitFieldLaplacianMatMult(const int iFieldID, const float iTimeStep,
                                                   const bool iDiffuMode, const float iDiffuCoeff, const bool iPrecondMode,
                                                   const std::vector<std::vector<std::vector<float>>>& iField,
                                                   std::vector<std::vector<std::vector<float>>>& oField) {
  // Precompute value
  const float diffuVal= iDiffuCoeff * iTimeStep / (voxSize * voxSize);
  // Sweep through the field
  for (int x= 0; x < nX; x++) {
    for (int y= 0; y < nY; y++) {
      for (int z= 0; z < nZ; z++) {
        // Skip solid or fixed values
        if (Solid[x][y][z]) continue;
        if (SmoBC[x][y][z] && iFieldID == FieldID::IDSmok) continue;
        if (VelBC[x][y][z] && iFieldID == FieldID::IDVelX) continue;
        if (VelBC[x][y][z] && iFieldID == FieldID::IDVelY) continue;
        if (VelBC[x][y][z] && iFieldID == FieldID::IDVelZ) continue;
        if (PreBC[x][y][z] && iFieldID == FieldID::IDPres) continue;
        // Get count and sum of valid neighbors
        const int count= (x > 0) + (y > 0) + (z > 0) + (x < nX - 1) + (y < nY - 1) + (z < nZ - 1);
        float sum= 0.0f;
        if (!iPrecondMode) {
          const float xBCVal= (iFieldID == FieldID::IDVelX) ? (-iField[x][y][z]) : ((iFieldID == FieldID::IDSmok || iFieldID == FieldID::IDPres) ? (iField[x][y][z]) : (0.0f));
          const float yBCVal= (iFieldID == FieldID::IDVelY) ? (-iField[x][y][z]) : ((iFieldID == FieldID::IDSmok || iFieldID == FieldID::IDPres) ? (iField[x][y][z]) : (0.0f));
          const float zBCVal= (iFieldID == FieldID::IDVelZ) ? (-iField[x][y][z]) : ((iFieldID == FieldID::IDSmok || iFieldID == FieldID::IDPres) ? (iField[x][y][z]) : (0.0f));
          if (x - 1 >= 0) sum+= Solid[x - 1][y][z] ? xBCVal : iField[x - 1][y][z];
          if (x + 1 < nX) sum+= Solid[x + 1][y][z] ? xBCVal : iField[x + 1][y][z];
          if (y - 1 >= 0) sum+= Solid[x][y - 1][z] ? yBCVal : iField[x][y - 1][z];
          if (y + 1 < nY) sum+= Solid[x][y + 1][z] ? yBCVal : iField[x][y + 1][z];
          if (z - 1 >= 0) sum+= Solid[x][y][z - 1] ? zBCVal : iField[x][y][z - 1];
          if (z + 1 < nZ) sum+= Solid[x][y][z + 1] ? zBCVal : iField[x][y][z + 1];
        }
        // Apply linear expression
        if (iDiffuMode) {
          if (iPrecondMode)
            oField[x][y][z]= 1.0f / (1.0f + diffuVal * (float)count) * iField[x][y][z];            //               [   -D*dt/(h*h)]
          else                                                                                     // [-D*dt/(h*h)] [1+4*D*dt/(h*h)] [-D*dt/(h*h)]
            oField[x][y][z]= (1.0f + diffuVal * (float)count) * iField[x][y][z] - diffuVal * sum;  //               [   -D*dt/(h*h)]
        }
        else {
          if (iPrecondMode)
            oField[x][y][z]= ((voxSize * voxSize) / (float)count) * iField[x][y][z];        //            [-1/(h*h)]
          else                                                                              // [-1/(h*h)] [ 4/(h*h)] [-1/(h*h)]
            oField[x][y][z]= ((float)count * iField[x][y][z] - sum) / (voxSize * voxSize);  //            [-1/(h*h)]
        }
      }
    }
  }
}


// TODO check if can remove ApplyBC
// TODO check BC properly handled
// Solve linear system with diagonal/Jacobi preconditioned conjugate gradient
// References for linear solvers and particularily PCG
// https://www.cs.cmu.edu/~quake-papers/painless-conjugate-gradient.pdf
// https://services.math.duke.edu/~holee/math361-2020/lectures/Conjugate_gradients.pdf
// https://www3.nd.edu/~zxu2/acms60212-40212-S12/final_project/Linear_solvers_GPU.pdf
// https://github.com/awesson/stable-fluids/tree/master
// https://en.wikipedia.org/wiki/Conjugate_gradient_method
void CompuFluidDyna::ConjugateGradientSolve(const int iFieldID, const int iMaxIter, const float iTimeStep,
                                            const bool iDiffuMode, const float iDiffuCoeff,
                                            const std::vector<std::vector<std::vector<float>>>& iField,
                                            std::vector<std::vector<std::vector<float>>>& ioField) {
  // Prepare convergence plot
  const float normRHS= ImplicitFieldDotProd(iField, iField);
  if (D.UI[VerboseSolv_].GetB()) {
    D.plotLegend.resize(5);
    D.plotLegend[FieldID::IDSmok]= "Diffu S";
    D.plotLegend[FieldID::IDVelX]= "Diffu VX";
    D.plotLegend[FieldID::IDVelY]= "Diffu VY";
    D.plotLegend[FieldID::IDVelZ]= "Diffu VZ";
    D.plotLegend[FieldID::IDPres]= "Proj  P";
    D.plotData.resize(5);
    D.plotData[iFieldID].clear();
  }
  // Allocate fields
  std::vector<std::vector<std::vector<float>>> rField= Field::AllocField3D(nX, nY, nZ, 0.0f);
  std::vector<std::vector<std::vector<float>>> qField= Field::AllocField3D(nX, nY, nZ, 0.0f);
  std::vector<std::vector<std::vector<float>>> dField= Field::AllocField3D(nX, nY, nZ, 0.0f);
  std::vector<std::vector<std::vector<float>>> sField= Field::AllocField3D(nX, nY, nZ, 0.0f);
  std::vector<std::vector<std::vector<float>>> t0Field= Field::AllocField3D(nX, nY, nZ, 0.0f);
  std::vector<std::vector<std::vector<float>>> t1Field= Field::AllocField3D(nX, nY, nZ, 0.0f);
  // r = b - A x
  ImplicitFieldLaplacianMatMult(iFieldID, iTimeStep, iDiffuMode, iDiffuCoeff, false, ioField, t0Field);
  ApplyBC(iFieldID, t0Field);
  ImplicitFieldSub(iField, t0Field, rField);
  // Error plot
  if (D.UI[VerboseSolv_].GetB()) {
    const float errTmp= ImplicitFieldDotProd(rField, rField);
    D.plotData[iFieldID].push_back((normRHS != 0.0f) ? errTmp / normRHS : 0.0f);
    if (iFieldID == FieldID::IDSmok) printf("CG Diffu S  [%.2e] ", normRHS);
    if (iFieldID == FieldID::IDVelX) printf("CG Diffu VX [%.2e] ", normRHS);
    if (iFieldID == FieldID::IDVelY) printf("CG Diffu VY [%.2e] ", normRHS);
    if (iFieldID == FieldID::IDVelZ) printf("CG Diffu VZ [%.2e] ", normRHS);
    if (iFieldID == FieldID::IDPres) printf("CG Proj  P  [%.2e] ", normRHS);
    printf("%.2e ", (normRHS != 0.0f) ? (errTmp / normRHS) : (0.0f));
  }
  // d = M^-1 r
  ImplicitFieldLaplacianMatMult(iFieldID, iTimeStep, iDiffuMode, iDiffuCoeff, true, rField, dField);
  // errNew = r^T d
  float errNew= ImplicitFieldDotProd(rField, dField);
  const float errBeg= errNew;
  // Iterate to solve
  for (int k= 0; k < iMaxIter; k++) {
    // TODO tweak handling of exit conditions and flow of iterations
    if (errNew / normRHS <= std::max(D.UI[SolvTolRhs__].GetF(), 0.0f)) break;
    if (errNew / errBeg <= std::max(D.UI[SolvTolRel__].GetF(), 0.0f)) break;
    // q = A d
    ImplicitFieldLaplacianMatMult(iFieldID, iTimeStep, iDiffuMode, iDiffuCoeff, false, dField, qField);
    // alpha = errNew / (d^T q)
    const float denom= ImplicitFieldDotProd(dField, qField);
    if (denom == 0.0) break;
    const float alpha= errNew / denom;
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
    if (D.UI[VerboseSolv_].GetB()) {
      const float errTmp= ImplicitFieldDotProd(rField, rField);
      D.plotData[iFieldID].push_back((normRHS != 0.0f) ? (errTmp / normRHS) : (0.0f));
      printf("%.2e ", (normRHS != 0.0f) ? (errTmp / normRHS) : (0.0f));
    }
    // s = M^-1 r
    ImplicitFieldLaplacianMatMult(iFieldID, iTimeStep, iDiffuMode, iDiffuCoeff, true, rField, sField);
    // errNew = r^T s
    const float errOld= errNew;
    errNew= ImplicitFieldDotProd(rField, sField);
    // beta = errNew / errOld
    const float beta= errNew / errOld;
    // d = s + beta d
    ImplicitFieldScale(beta, dField, t0Field);
    ImplicitFieldAdd(sField, t0Field, dField);
  }
  // Error plot
  if (D.UI[VerboseSolv_].GetB()) {
    if (iFieldID == FieldID::IDSmok) Dum0= rField;
    if (iFieldID == FieldID::IDVelX) Dum1= rField;
    if (iFieldID == FieldID::IDVelY) Dum2= rField;
    if (iFieldID == FieldID::IDVelZ) Dum3= rField;
    if (iFieldID == FieldID::IDPres) Dum4= rField;
    printf("\n");
  }
}


// TODO check BC properly handled
// Solve linear system with an iterative Gauss Seidel scheme
// Solving each equation sequentially by cascading latest solution to next equation
// Run a forward and backward pass in parallel to avoid element ordering bias
// Apply successive overrelaxation coefficient to accelerate convergence
void CompuFluidDyna::GaussSeidelSolve(const int iFieldID, const int iMaxIter, const float iTimeStep,
                                      const bool iDiffuMode, const float iDiffuCoeff,
                                      const std::vector<std::vector<std::vector<float>>>& iField,
                                      std::vector<std::vector<std::vector<float>>>& ioField) {
  // Prepare convergence plot
  const float normRHS= ImplicitFieldDotProd(iField, iField);
  if (D.UI[VerboseSolv_].GetB()) {
    D.plotLegend.resize(5);
    D.plotLegend[FieldID::IDSmok]= "Diffu S";
    D.plotLegend[FieldID::IDVelX]= "Diffu VX";
    D.plotLegend[FieldID::IDVelY]= "Diffu VY";
    D.plotLegend[FieldID::IDVelZ]= "Diffu VZ";
    D.plotLegend[FieldID::IDPres]= "Proj  P";
    D.plotData.resize(5);
    D.plotData[iFieldID].clear();
  }
  // Allocate fields
  std::vector<std::vector<std::vector<float>>> rField= Field::AllocField3D(nX, nY, nZ, 0.0f);
  std::vector<std::vector<std::vector<float>>> t0Field= Field::AllocField3D(nX, nY, nZ, 0.0f);
  // r = b - A x
  ImplicitFieldLaplacianMatMult(iFieldID, iTimeStep, iDiffuMode, iDiffuCoeff, false, ioField, t0Field);
  ApplyBC(iFieldID, t0Field);
  ImplicitFieldSub(iField, t0Field, rField);
  // Error plot
  if (D.UI[VerboseSolv_].GetB()) {
    const float errTmp= ImplicitFieldDotProd(rField, rField);
    D.plotData[iFieldID].push_back((normRHS != 0.0f) ? errTmp / normRHS : 0.0f);
    if (iFieldID == FieldID::IDSmok) printf("CG Diffu S  [%.2e] ", normRHS);
    if (iFieldID == FieldID::IDVelX) printf("CG Diffu VX [%.2e] ", normRHS);
    if (iFieldID == FieldID::IDVelY) printf("CG Diffu VY [%.2e] ", normRHS);
    if (iFieldID == FieldID::IDVelZ) printf("CG Diffu VZ [%.2e] ", normRHS);
    if (iFieldID == FieldID::IDPres) printf("CG Proj  P  [%.2e] ", normRHS);
    printf("%.2e ", (normRHS != 0.0f) ? (errTmp / normRHS) : (0.0f));
  }
  // errNew = r^T d
  float errNew= ImplicitFieldDotProd(rField, rField);
  const float errBeg= errNew;
  // Solve with PArallel BIdirectionnal GAuss-Seidel Successive Over-Relaxation (PABIGASSOR)
  const float diffuVal= iDiffuCoeff * iTimeStep / (voxSize * voxSize);
  const float coeffOverrelax= std::max(D.UI[SolvSOR_____].GetF(), 0.0f);
  for (int k= 0; k < iMaxIter; k++) {
    // TODO tweak handling of exit conditions and flow of iterations
    if (errNew / normRHS <= std::max(D.UI[SolvTolRhs__].GetF(), 0.0f)) break;
    if (errNew / errBeg <= std::max(D.UI[SolvTolRel__].GetF(), 0.0f)) break;
    // Initialize fields for forward and backward passes
    std::vector<std::vector<std::vector<std::vector<float>>>> FieldT;
    FieldT.resize(2);
    FieldT[0]= ioField;
    FieldT[1]= ioField;
#pragma omp parallel for
    for (int k= 0; k < 2; k++) {
      // Set the loop settings for each pass
      const int xBeg= (k == 0) ? 0 : nX - 1;
      const int yBeg= (k == 0) ? 0 : nY - 1;
      const int zBeg= (k == 0) ? 0 : nZ - 1;
      const int xEnd= (k == 0) ? nX : -1;
      const int yEnd= (k == 0) ? nY : -1;
      const int zEnd= (k == 0) ? nZ : -1;
      const int xInc= (k == 0) ? 1 : -1;
      const int yInc= (k == 0) ? 1 : -1;
      const int zInc= (k == 0) ? 1 : -1;
      // Apply the current pass
      for (int x= xBeg; x != xEnd; x+= xInc) {
        for (int y= yBeg; y != yEnd; y+= yInc) {
          for (int z= zBeg; z != zEnd; z+= zInc) {
            // Skip solid or fixed values
            if (Solid[x][y][z]) continue;
            if (SmoBC[x][y][z] && iFieldID == FieldID::IDSmok) continue;
            if (VelBC[x][y][z] && iFieldID == FieldID::IDVelX) continue;
            if (VelBC[x][y][z] && iFieldID == FieldID::IDVelY) continue;
            if (VelBC[x][y][z] && iFieldID == FieldID::IDVelZ) continue;
            if (PreBC[x][y][z] && iFieldID == FieldID::IDPres) continue;
            // Get count and sum of valid neighbors
            const int count= (x > 0) + (y > 0) + (z > 0) + (x < nX - 1) + (y < nY - 1) + (z < nZ - 1);
            const float xBCVal= (iFieldID == FieldID::IDVelX) ? (-FieldT[k][x][y][z]) : ((iFieldID == FieldID::IDSmok || iFieldID == FieldID::IDPres) ? (FieldT[k][x][y][z]) : (0.0f));
            const float yBCVal= (iFieldID == FieldID::IDVelY) ? (-FieldT[k][x][y][z]) : ((iFieldID == FieldID::IDSmok || iFieldID == FieldID::IDPres) ? (FieldT[k][x][y][z]) : (0.0f));
            const float zBCVal= (iFieldID == FieldID::IDVelZ) ? (-FieldT[k][x][y][z]) : ((iFieldID == FieldID::IDSmok || iFieldID == FieldID::IDPres) ? (FieldT[k][x][y][z]) : (0.0f));
            float sum= 0.0f;
            if (x - 1 >= 0) sum+= Solid[x - 1][y][z] ? xBCVal : FieldT[k][x - 1][y][z];
            if (x + 1 < nX) sum+= Solid[x + 1][y][z] ? xBCVal : FieldT[k][x + 1][y][z];
            if (y - 1 >= 0) sum+= Solid[x][y - 1][z] ? yBCVal : FieldT[k][x][y - 1][z];
            if (y + 1 < nY) sum+= Solid[x][y + 1][z] ? yBCVal : FieldT[k][x][y + 1][z];
            if (z - 1 >= 0) sum+= Solid[x][y][z - 1] ? zBCVal : FieldT[k][x][y][z - 1];
            if (z + 1 < nZ) sum+= Solid[x][y][z + 1] ? zBCVal : FieldT[k][x][y][z + 1];
            // Set new value according to coefficients and flags
            if (count > 0) {
            const float prevVal= FieldT[k][x][y][z];
            if (iDiffuMode) FieldT[k][x][y][z]= (iField[x][y][z] + diffuVal * sum) / (1.0f + diffuVal * (float)count);
              else FieldT[k][x][y][z]= ((voxSize * voxSize) * iField[x][y][z] + sum) / (float)count;
            FieldT[k][x][y][z]= prevVal + coeffOverrelax * (FieldT[k][x][y][z] - prevVal);
            }
          }
        }
      }
    }
    // Recombine forward and backward passes
    for (int x= 0; x < nX; x++)
      for (int y= 0; y < nY; y++)
        for (int z= 0; z < nZ; z++)
          ioField[x][y][z]= (FieldT[0][x][y][z] + FieldT[1][x][y][z]) / 2.0f;
    // r = b - A x
    ImplicitFieldLaplacianMatMult(iFieldID, iTimeStep, iDiffuMode, iDiffuCoeff, false, ioField, t0Field);
    ApplyBC(iFieldID, t0Field);
    ImplicitFieldSub(iField, t0Field, rField);
    errNew= ImplicitFieldDotProd(rField, rField);
    // Error plot
    if (D.UI[VerboseSolv_].GetB()) {
      D.plotData[iFieldID].push_back((normRHS != 0.0f) ? (errNew / normRHS) : (0.0f));
      printf("%.2e ", (normRHS != 0.0f) ? (errNew / normRHS) : (0.0f));
    }
  }
  // Error plot
  if (D.UI[VerboseSolv_].GetB()) {
    if (iFieldID == FieldID::IDSmok) Dum0= rField;
    if (iFieldID == FieldID::IDVelX) Dum1= rField;
    if (iFieldID == FieldID::IDVelY) Dum2= rField;
    if (iFieldID == FieldID::IDVelZ) Dum3= rField;
    if (iFieldID == FieldID::IDPres) Dum4= rField;
    printf("\n");
  }
}


// Add external forces to velocity field
// vel ⇐ vel + Δt * F / ρ
void CompuFluidDyna::ExternalForces() {
  // Update velocities based on applied external forces
  for (int x= 0; x < nX; x++) {
    for (int y= 0; y < nY; y++) {
      for (int z= 0; z < nZ; z++) {
        if (Solid[x][y][z] || VelBC[x][y][z]) continue;
        VelZ[x][y][z]+= D.UI[TimeStep____].GetF() * D.UI[CoeffGravi__].GetF() * Smok[x][y][z] / fluidDensity;
      }
    }
  }
}


// Project velocity field into a solenoidal/divergence-free field
// 1. Compute RHS based on divergence
// RHS = -(ρ / Δt) × ∇ · vel
// 2. Solve for pressure in pressure Poisson equation
// (-∇²) press = RHS
// 3. Update velocity field by subtracting gradient of pressure
// vel ⇐ vel - (Δt / ρ) × ∇ press
// References for pressure poisson equation and incompressiblity projection
// https://en.wikipedia.org/wiki/Projection_method_(fluid_dynamics)
// https://mycourses.aalto.fi/pluginfile.php/891524/mod_folder/content/0/Lecture03_Pressure.pdf
// https://barbagroup.github.io/essential_skills_RRC/numba/4/#application-pressure-poisson-equation
// http://www.thevisualroom.com/poisson_for_pressure.html
// https://github.com/barbagroup/CFDPython
void CompuFluidDyna::ProjectField(const int iIter, const float iTimeStep,
                                  std::vector<std::vector<std::vector<float>>>& ioVelX,
                                  std::vector<std::vector<std::vector<float>>>& ioVelY,
                                  std::vector<std::vector<std::vector<float>>>& ioVelZ) {
  // Compute divergence for RHS
  ComputeVelocityDivergence();
  // Solve for pressure in the pressure Poisson equation
  if (D.UI[SolvPCG_____].GetB())
    ConjugateGradientSolve(FieldID::IDPres, iIter, iTimeStep, false, 0.0f, Dive, Pres);
  else
    GaussSeidelSolve(FieldID::IDPres, iIter, iTimeStep, false, 0.0f, Dive, Pres);
  // Update velocities based on local pressure gradient
  for (int x= 0; x < nX; x++) {
    for (int y= 0; y < nY; y++) {
      for (int z= 0; z < nZ; z++) {
        if (Solid[x][y][z] || VelBC[x][y][z]) continue;
        // Subtract pressure gradient to remove divergence
        if (x - 1 >= 0 && !Solid[x - 1][y][z]) ioVelX[x][y][z]-= iTimeStep / fluidDensity * (Pres[x][y][z] - Pres[x - 1][y][z]) / (2.0f * voxSize);
        if (y - 1 >= 0 && !Solid[x][y - 1][z]) ioVelY[x][y][z]-= iTimeStep / fluidDensity * (Pres[x][y][z] - Pres[x][y - 1][z]) / (2.0f * voxSize);
        if (z - 1 >= 0 && !Solid[x][y][z - 1]) ioVelZ[x][y][z]-= iTimeStep / fluidDensity * (Pres[x][y][z] - Pres[x][y][z - 1]) / (2.0f * voxSize);
        if (x + 1 < nX && !Solid[x + 1][y][z]) ioVelX[x][y][z]-= iTimeStep / fluidDensity * (Pres[x + 1][y][z] - Pres[x][y][z]) / (2.0f * voxSize);
        if (y + 1 < nY && !Solid[x][y + 1][z]) ioVelY[x][y][z]-= iTimeStep / fluidDensity * (Pres[x][y + 1][z] - Pres[x][y][z]) / (2.0f * voxSize);
        if (z + 1 < nZ && !Solid[x][y][z + 1]) ioVelZ[x][y][z]-= iTimeStep / fluidDensity * (Pres[x][y][z + 1] - Pres[x][y][z]) / (2.0f * voxSize);
      }
    }
  }
}


// Trilinearly interpolate the field value at the given position
float CompuFluidDyna::TrilinearInterpolation(const float iPosX, const float iPosY, const float iPosZ,
                                             const std::vector<std::vector<std::vector<float>>>& iFieldRef) {
  // Get floor and ceil voxel indices
  const int x0= std::min(std::max((int)std::floor(iPosX), 0), nX - 1);
  const int y0= std::min(std::max((int)std::floor(iPosY), 0), nY - 1);
  const int z0= std::min(std::max((int)std::floor(iPosZ), 0), nZ - 1);
  const int x1= std::min(std::max((int)std::ceil(iPosX), 0), nX - 1);
  const int y1= std::min(std::max((int)std::ceil(iPosY), 0), nY - 1);
  const int z1= std::min(std::max((int)std::ceil(iPosZ), 0), nZ - 1);
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


// Apply semi-Lagrangian advection along the velocity field
// vel ⇐ vel - Δt (vel · ∇) vel    TODO check validity of formula
// References for MacCormack backtracking scheme
// https://commons.wikimedia.org/wiki/File:Backtracking_maccormack.png
// https://physbam.stanford.edu/~fedkiw/papers/stanford2006-09.pdf
// https://github.com/NiallHornFX/StableFluids3D-GL/blob/master/src/fluidsolver3d.cpp
void CompuFluidDyna::AdvectField(const int iFieldID, const float iTimeStep,
                                 const std::vector<std::vector<std::vector<float>>>& iVelX,
                                 const std::vector<std::vector<std::vector<float>>>& iVelY,
                                 const std::vector<std::vector<std::vector<float>>>& iVelZ,
                                 std::vector<std::vector<std::vector<float>>>& ioField) {
  // Copy the field values to serve as source in the update step
  std::vector<std::vector<std::vector<float>>> sourceField= ioField;
  // Adjust the source field to make solid voxels have the average smoke value of their non-solid neighbors
  if (D.UI[BCAdvecWall_].GetB() && iFieldID == FieldID::IDSmok) {
    for (int x= 0; x < nX; x++) {
      for (int y= 0; y < nY; y++) {
        for (int z= 0; z < nZ; z++) {
          if (!Solid[x][y][z]) continue;
          int count= 0;
          float sum= 0.0f;
          if (x - 1 >= 0 && !Solid[x - 1][y][z] && ++count) sum+= ioField[x - 1][y][z];
          if (y - 1 >= 0 && !Solid[x][y - 1][z] && ++count) sum+= ioField[x][y - 1][z];
          if (z - 1 >= 0 && !Solid[x][y][z - 1] && ++count) sum+= ioField[x][y][z - 1];
          if (x + 1 < nX && !Solid[x + 1][y][z] && ++count) sum+= ioField[x + 1][y][z];
          if (y + 1 < nY && !Solid[x][y + 1][z] && ++count) sum+= ioField[x][y + 1][z];
          if (z + 1 < nZ && !Solid[x][y][z + 1] && ++count) sum+= ioField[x][y][z + 1];
          sourceField[x][y][z]= (count > 0) ? sum / (float)count : 0.0f;
        }
      }
    }
  }
  // Sweep through the field
  for (int x= 0; x < nX; x++) {
#pragma omp parallel for
    for (int y= 0; y < nY; y++) {
      for (int z= 0; z < nZ; z++) {
        // Skip solid or fixed values
        if (Solid[x][y][z]) continue;
        if (SmoBC[x][y][z] && iFieldID == FieldID::IDSmok) continue;
        if (VelBC[x][y][z] && iFieldID == FieldID::IDVelX) continue;
        if (VelBC[x][y][z] && iFieldID == FieldID::IDVelY) continue;
        if (VelBC[x][y][z] && iFieldID == FieldID::IDVelZ) continue;
        // Find source position for active voxel using naive linear backtracking scheme
        const Math::Vec3f posEnd((float)x, (float)y, (float)z);
        const Math::Vec3f velEnd(iVelX[x][y][z], iVelY[x][y][z], iVelZ[x][y][z]);
        Math::Vec3f posBeg= posEnd - iTimeStep * velEnd / voxSize;
        // Iterative source position correction with 2nd order MacCormack scheme
        int correcMaxIter= std::max(D.UI[CoeffAdvec__].GetI() - 1, 0);
        for (int iter= 0; iter < correcMaxIter; iter++) {
          const float velBegX= TrilinearInterpolation(posBeg[0], posBeg[1], posBeg[2], iVelX);
          const float velBegY= TrilinearInterpolation(posBeg[0], posBeg[1], posBeg[2], iVelY);
          const float velBegZ= TrilinearInterpolation(posBeg[0], posBeg[1], posBeg[2], iVelZ);
          const Math::Vec3f velBeg(velBegX, velBegY, velBegZ);
          const Math::Vec3f vecErr= posEnd - (posBeg + iTimeStep * velBeg / voxSize);
          posBeg= posBeg + vecErr / 2.0f;
        }
        // Save source vector for display
        AdvX[x][y][z]= posBeg[0] - posEnd[0];
        AdvY[x][y][z]= posBeg[1] - posEnd[1];
        AdvZ[x][y][z]= posBeg[2] - posEnd[2];
        // Trilinear interpolation at source position
        ioField[x][y][z]= TrilinearInterpolation(posBeg[0], posBeg[1], posBeg[2], sourceField);
      }
    }
  }
}


// Counteract energy dissipation and introduce turbulent-like behavior by amplifying vorticity on small scales
// https://github.com/awesson/stable-fluids/tree/master
// https://github.com/woeishi/StableFluids/blob/master/StableFluid3d.cpp
// vel ⇐ vel + Δt * TODO write formula
void CompuFluidDyna::VorticityConfinement(const float iTimeStep, const float iVortiCoeff,
                                          std::vector<std::vector<std::vector<float>>>& ioVelX,
                                          std::vector<std::vector<std::vector<float>>>& ioVelY,
                                          std::vector<std::vector<std::vector<float>>>& ioVelZ) {
  // Compute curl and vorticity from the velocity field
  ComputeVelocityCurlVorticity();
  // Amplify non-zero vorticity
  if (iVortiCoeff > 0.0f) {
    for (int x= 0; x < nX; x++) {
      for (int y= 0; y < nY; y++) {
        for (int z= 0; z < nZ; z++) {
          if (Solid[x][y][z] || VelBC[x][y][z]) continue;
          // Gradient of vorticity with zero derivative at solid interface or domain boundary
          Math::Vec3f vortGrad(0.0f, 0.0f, 0.0f);
          if (x - 1 >= 0 && !Solid[x - 1][y][z]) vortGrad[0]+= (Vort[x][y][z] - Vort[x - 1][y][z]) / (2.0f * voxSize);
          if (y - 1 >= 0 && !Solid[x][y - 1][z]) vortGrad[1]+= (Vort[x][y][z] - Vort[x][y - 1][z]) / (2.0f * voxSize);
          if (z - 1 >= 0 && !Solid[x][y][z - 1]) vortGrad[2]+= (Vort[x][y][z] - Vort[x][y][z - 1]) / (2.0f * voxSize);
          if (x + 1 < nX && !Solid[x + 1][y][z]) vortGrad[0]+= (Vort[x + 1][y][z] - Vort[x][y][z]) / (2.0f * voxSize);
          if (y + 1 < nY && !Solid[x][y + 1][z]) vortGrad[1]+= (Vort[x][y + 1][z] - Vort[x][y][z]) / (2.0f * voxSize);
          if (z + 1 < nZ && !Solid[x][y][z + 1]) vortGrad[2]+= (Vort[x][y][z + 1] - Vort[x][y][z]) / (2.0f * voxSize);
          // Amplification of small scale vorticity by following current curl
          if (vortGrad.norm() > 0.0f) {
            const float dVort_dx_scaled= iVortiCoeff * vortGrad[0] / vortGrad.norm();
            const float dVort_dy_scaled= iVortiCoeff * vortGrad[1] / vortGrad.norm();
            const float dVort_dz_scaled= iVortiCoeff * vortGrad[2] / vortGrad.norm();
            ioVelX[x][y][z]+= iTimeStep * (dVort_dy_scaled * CurZ[x][y][z] - dVort_dz_scaled * CurY[x][y][z]);
            ioVelY[x][y][z]+= iTimeStep * (dVort_dz_scaled * CurX[x][y][z] - dVort_dx_scaled * CurZ[x][y][z]);
            ioVelZ[x][y][z]+= iTimeStep * (dVort_dx_scaled * CurY[x][y][z] - dVort_dy_scaled * CurX[x][y][z]);
          }
        }
      }
    }
  }
}


// Compute RHS of pressure poisson equation as negative divergence scaled by density and timestep
// https://en.wikipedia.org/wiki/Projection_method_(fluid_dynamics)
// RHS = -(ρ / Δt) × ∇ · vel
// TODO implement correction to avoid checkerboard due to odd-even decoupling
// References for Rhie Chow correction
// https://youtu.be/yqZ59Xn_aF8 Checkerboard oscillations
// https://youtu.be/PmEUiUB8ETk Deriving the correction
// https://mustafabhotvawala.com/wp-content/uploads/2020/11/MB_rhieChow-1.pdf
void CompuFluidDyna::ComputeVelocityDivergence() {
  // // Precompute pressure gradient for Rhie and Chow correction
  // std::vector<std::vector<std::vector<float>>> PresGradX= Field::AllocField3D(nX, nY, nZ, 0.0f);
  // std::vector<std::vector<std::vector<float>>> PresGradY= Field::AllocField3D(nX, nY, nZ, 0.0f);
  // std::vector<std::vector<std::vector<float>>> PresGradZ= Field::AllocField3D(nX, nY, nZ, 0.0f);
  // for (int x= 0; x < nX; x++) {
  //   for (int y= 0; y < nY; y++) {
  //     for (int z= 0; z < nZ; z++) {
  //       if (Solid[x][y][z]) continue;
  //       // Pressure gradient with zero derivative at solid interface or domain boundary
  //       if (x - 1 >= 0 && !Solid[x - 1][y][z]) PresGradX[x][y][z]+= (Pres[x][y][z] - Pres[x - 1][y][z]) / (2.0f * voxSize);
  //       if (y - 1 >= 0 && !Solid[x][y - 1][z]) PresGradY[x][y][z]+= (Pres[x][y][z] - Pres[x][y - 1][z]) / (2.0f * voxSize);
  //       if (z - 1 >= 0 && !Solid[x][y][z - 1]) PresGradZ[x][y][z]+= (Pres[x][y][z] - Pres[x][y][z - 1]) / (2.0f * voxSize);
  //       if (x + 1 < nX && !Solid[x + 1][y][z]) PresGradX[x][y][z]+= (Pres[x + 1][y][z] - Pres[x][y][z]) / (2.0f * voxSize);
  //       if (y + 1 < nY && !Solid[x][y + 1][z]) PresGradY[x][y][z]+= (Pres[x][y + 1][z] - Pres[x][y][z]) / (2.0f * voxSize);
  //       if (z + 1 < nZ && !Solid[x][y][z + 1]) PresGradZ[x][y][z]+= (Pres[x][y][z + 1] - Pres[x][y][z]) / (2.0f * voxSize);
  //     }
  //   }
  // }
  // Compute divergence of velocity field
  for (int x= 0; x < nX; x++) {
    for (int y= 0; y < nY; y++) {
      for (int z= 0; z < nZ; z++) {
        if (Solid[x][y][z]) Dive[x][y][z]= 0.0f;
        if (PreBC[x][y][z]) Dive[x][y][z]= PresForced[x][y][z];
        if (Solid[x][y][z] || PreBC[x][y][z]) continue;
        // Classical linear interpolation for face velocities with same velocity at domain boundary and zero velocity at solid interface
        float velXN= (x - 1 >= 0) ? ((Solid[x - 1][y][z]) ? (0.0f) : ((VelX[x][y][z] + VelX[x - 1][y][z]) / 2.0f)) : (VelX[x][y][z]);
        float velYN= (y - 1 >= 0) ? ((Solid[x][y - 1][z]) ? (0.0f) : ((VelY[x][y][z] + VelY[x][y - 1][z]) / 2.0f)) : (VelY[x][y][z]);
        float velZN= (z - 1 >= 0) ? ((Solid[x][y][z - 1]) ? (0.0f) : ((VelZ[x][y][z] + VelZ[x][y][z - 1]) / 2.0f)) : (VelZ[x][y][z]);
        float velXP= (x + 1 < nX) ? ((Solid[x + 1][y][z]) ? (0.0f) : ((VelX[x + 1][y][z] + VelX[x][y][z]) / 2.0f)) : (VelX[x][y][z]);
        float velYP= (y + 1 < nY) ? ((Solid[x][y + 1][z]) ? (0.0f) : ((VelY[x][y + 1][z] + VelY[x][y][z]) / 2.0f)) : (VelY[x][y][z]);
        float velZP= (z + 1 < nZ) ? ((Solid[x][y][z + 1]) ? (0.0f) : ((VelZ[x][y][z + 1] + VelZ[x][y][z]) / 2.0f)) : (VelZ[x][y][z]);
        // // Rhie and Chow correction by subtracting pressure gradient minus linear interpolation of pressure gradients
        // velXN-= D.UI[CoeffProj1__].GetF() * ((x - 1 >= 0 && !Solid[x - 1][y][z]) ? ((Pres[x][y][z] - Pres[x - 1][y][z]) / voxSize) : (0.0f));
        // velYN-= D.UI[CoeffProj1__].GetF() * ((y - 1 >= 0 && !Solid[x][y - 1][z]) ? ((Pres[x][y][z] - Pres[x][y - 1][z]) / voxSize) : (0.0f));
        // velZN-= D.UI[CoeffProj1__].GetF() * ((z - 1 >= 0 && !Solid[x][y][z - 1]) ? ((Pres[x][y][z] - Pres[x][y][z - 1]) / voxSize) : (0.0f));
        // velXP-= D.UI[CoeffProj1__].GetF() * ((x + 1 < nX && !Solid[x + 1][y][z]) ? ((Pres[x + 1][y][z] - Pres[x][y][z]) / voxSize) : (0.0f));
        // velYP-= D.UI[CoeffProj1__].GetF() * ((y + 1 < nY && !Solid[x][y + 1][z]) ? ((Pres[x][y + 1][z] - Pres[x][y][z]) / voxSize) : (0.0f));
        // velZP-= D.UI[CoeffProj1__].GetF() * ((z + 1 < nZ && !Solid[x][y][z + 1]) ? ((Pres[x][y][z + 1] - Pres[x][y][z]) / voxSize) : (0.0f));
        // velXN+= D.UI[CoeffProj1__].GetF() * ((x - 1 >= 0) ? ((PresGradX[x][y][z] + PresGradX[x - 1][y][z]) / 2.0f) : (0.0f));
        // velYN+= D.UI[CoeffProj1__].GetF() * ((y - 1 >= 0) ? ((PresGradY[x][y][z] + PresGradY[x][y - 1][z]) / 2.0f) : (0.0f));
        // velZN+= D.UI[CoeffProj1__].GetF() * ((z - 1 >= 0) ? ((PresGradZ[x][y][z] + PresGradZ[x][y][z - 1]) / 2.0f) : (0.0f));
        // velXP+= D.UI[CoeffProj1__].GetF() * ((x + 1 < nX) ? ((PresGradX[x + 1][y][z] + PresGradX[x][y][z]) / 2.0f) : (0.0f));
        // velYP+= D.UI[CoeffProj1__].GetF() * ((y + 1 < nY) ? ((PresGradY[x][y + 1][z] + PresGradY[x][y][z]) / 2.0f) : (0.0f));
        // velZP+= D.UI[CoeffProj1__].GetF() * ((z + 1 < nZ) ? ((PresGradZ[x][y][z + 1] + PresGradZ[x][y][z]) / 2.0f) : (0.0f));
        // Divergence based on face velocities negated and scaled by density and timestep for RHS
        Dive[x][y][z]= -fluidDensity / D.UI[TimeStep____].GetF() * ((velXP - velXN) + (velYP - velYN) + (velZP - velZN)) / voxSize;
      }
    }
  }
}


// Compute curl and vorticity of current velocity field
// curl= ∇ ⨯ vel
// vort= ‖curl‖₂
void CompuFluidDyna::ComputeVelocityCurlVorticity() {
  for (int x= 0; x < nX; x++) {
    for (int y= 0; y < nY; y++) {
      for (int z= 0; z < nZ; z++) {
        CurX[x][y][z]= CurY[x][y][z]= CurZ[x][y][z]= Vort[x][y][z]= 0.0f;
        if (Solid[x][y][z]) continue;
        // Compute velocity cross derivatives considering BC at interface with solid
        float dVely_dx= 0.0f, dVelz_dx= 0.0f, dVelx_dy= 0.0f, dVelz_dy= 0.0f, dVelx_dz= 0.0f, dVely_dz= 0.0f;
        if (x - 1 >= 0 && x + 1 < nX) dVely_dx= ((Solid[x + 1][y][z] ? VelY[x][y][z] : VelY[x + 1][y][z]) - (Solid[x - 1][y][z] ? VelY[x][y][z] : VelY[x - 1][y][z])) / 2.0f;
        if (x - 1 >= 0 && x + 1 < nX) dVelz_dx= ((Solid[x + 1][y][z] ? VelZ[x][y][z] : VelZ[x + 1][y][z]) - (Solid[x - 1][y][z] ? VelZ[x][y][z] : VelZ[x - 1][y][z])) / 2.0f;
        if (y - 1 >= 0 && y + 1 < nY) dVelx_dy= ((Solid[x][y + 1][z] ? VelX[x][y][z] : VelX[x][y + 1][z]) - (Solid[x][y - 1][z] ? VelX[x][y][z] : VelX[x][y - 1][z])) / 2.0f;
        if (y - 1 >= 0 && y + 1 < nY) dVelz_dy= ((Solid[x][y + 1][z] ? VelZ[x][y][z] : VelZ[x][y + 1][z]) - (Solid[x][y - 1][z] ? VelZ[x][y][z] : VelZ[x][y - 1][z])) / 2.0f;
        if (z - 1 >= 0 && z + 1 < nZ) dVelx_dz= ((Solid[x][y][z + 1] ? VelX[x][y][z] : VelX[x][y][z + 1]) - (Solid[x][y][z - 1] ? VelX[x][y][z] : VelX[x][y][z - 1])) / 2.0f;
        if (z - 1 >= 0 && z + 1 < nZ) dVely_dz= ((Solid[x][y][z + 1] ? VelY[x][y][z] : VelY[x][y][z + 1]) - (Solid[x][y][z - 1] ? VelY[x][y][z] : VelY[x][y][z - 1])) / 2.0f;
        // Deduce curl and vorticity
        CurX[x][y][z]= dVelz_dy - dVely_dz;
        CurY[x][y][z]= dVelx_dz - dVelz_dx;
        CurZ[x][y][z]= dVely_dx - dVelx_dy;
        Vort[x][y][z]= std::sqrt(CurX[x][y][z] * CurX[x][y][z] + CurY[x][y][z] * CurY[x][y][z] + CurZ[x][y][z] * CurZ[x][y][z]);
      }
    }
  }
}
