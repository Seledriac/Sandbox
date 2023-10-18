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
  CoeffGravi__,
  CoeffAdvec__,
  CoeffDiffuS_,
  CoeffDiffuV_,
  CoeffVorti__,
  CoeffProj___,
  CoeffProj2__,
  CoeffProj3__,
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
    D.UI.push_back(ParamUI("SolvMaxIter_", 30));     // Max number of solver iterations
    D.UI.push_back(ParamUI("SolvTolRhs__", 1.e-9));  // Solver tolerance relative to RHS norm
    D.UI.push_back(ParamUI("SolvTolRel__", 1.e-3));  // Solver tolerance relative to initial guess
    D.UI.push_back(ParamUI("CoeffGravi__", 0.0));    // Magnitude of gravity in Z- direction
    D.UI.push_back(ParamUI("CoeffAdvec__", 5.0));    // 0= no advection, 1= linear advection, >1 MacCormack correction iterations
    D.UI.push_back(ParamUI("CoeffDiffuS_", 1.e-4));  // Diffusion of smoke field, i.e. smoke spread/smear
    D.UI.push_back(ParamUI("CoeffDiffuV_", 1.e-3));  // Diffusion of velocity field, i.e. viscosity
    D.UI.push_back(ParamUI("CoeffVorti__", 0.0));    // Vorticity confinement to avoid dissipation of energy in small scale vortices
    D.UI.push_back(ParamUI("CoeffProj___", 1.0));    // 0 = No correction, 1= incompressiblity correction, 2= time dependant correction
    D.UI.push_back(ParamUI("CoeffProj2__", 0.5));    // TODO test coeff for Rhie Chow correction of voxel face velocities
    D.UI.push_back(ParamUI("CoeffProj3__", 0.5));    // TODO test coeff for Rhie Chow correction of voxel face velocities
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
  ApplyBC(FieldID::IDSmok, Smok);

  // Incompressible Navier Stokes
  // ∂vel/∂t + (vel · ∇) vel = − 1/ρ ∇press + visco ∇²vel + f
  // ∇ · vel = 0

  // Advection steps
  if (D.UI[CoeffAdvec__].GetB()) {
    // smo ⇐ smo{pos-vel}
    // smo ⇐ smo - Δt (vel · ∇) smo
    AdvectField(FieldID::IDSmok, timestep, VelX, VelY, VelZ, Smok);
  }
  if (D.UI[CoeffAdvec__].GetB()) {
    // vel ⇐ vel{pos-vel}
    // vel ⇐ vel - Δt (vel · ∇) vel
    std::vector<std::vector<std::vector<float>>> oldVelX= VelX;
    std::vector<std::vector<std::vector<float>>> oldVelY= VelY;
    std::vector<std::vector<std::vector<float>>> oldVelZ= VelZ;
    if (nX > 1) AdvectField(FieldID::IDVelX, timestep, oldVelX, oldVelY, oldVelZ, VelX);
    if (nY > 1) AdvectField(FieldID::IDVelY, timestep, oldVelX, oldVelY, oldVelZ, VelY);
    if (nZ > 1) AdvectField(FieldID::IDVelZ, timestep, oldVelX, oldVelY, oldVelZ, VelZ);
  }

  // Diffusion steps
  if (D.UI[CoeffDiffuS_].GetB()) {
    // (Id - diffu Δt ∇²) smo = smo
    std::vector<std::vector<std::vector<float>>> oldSmoke= Smok;
    ConjugateGradientSolve(FieldID::IDSmok, maxIter, timestep, true, coeffDiffu, oldSmoke, Smok);
  }
  if (D.UI[CoeffDiffuV_].GetB()) {
    // (Id - visco Δt ∇²) vel = vel
    std::vector<std::vector<std::vector<float>>> oldVelX= VelX;
    std::vector<std::vector<std::vector<float>>> oldVelY= VelY;
    std::vector<std::vector<std::vector<float>>> oldVelZ= VelZ;
    if (nX > 1) ConjugateGradientSolve(FieldID::IDVelX, maxIter, timestep, true, coeffVisco, oldVelX, VelX);
    if (nY > 1) ConjugateGradientSolve(FieldID::IDVelY, maxIter, timestep, true, coeffVisco, oldVelY, VelY);
    if (nZ > 1) ConjugateGradientSolve(FieldID::IDVelZ, maxIter, timestep, true, coeffVisco, oldVelZ, VelZ);
  }

  // Vorticity step
  if (D.UI[CoeffVorti__].GetB()) {
    // curl= ∇ ⨯ vel
    // vort= ‖curl‖₂
    // vel ⇐ vel + Δt * TODO write formula
    VorticityConfinement(timestep, coeffVorti, VelX, VelY, VelZ);
  }

  // External forces
  // vel ⇐ vel + Δt * F / m
  if (D.UI[CoeffGravi__].GetB()) {
    ExternalForces();
  }

  // Projection step
  if (D.UI[CoeffProj___].GetB()) {
    // ∇² press = ρ (∇ · vel)
    // vel ⇐ vel - ∇ press
    ProjectField(maxIter, timestep, VelX, VelY, VelZ);
  }

  // Compute field data for display
  ComputeVelocityDivergence();
  ComputeVelocityCurlVorticity();

  // TODO Compute fluid density to check if constant as it should be in incompressible case

  // TODO Test heuristic optimization of solid regions

  CompuFluidDyna::SetUpUIData();
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
              Colormap::RatioToJetBrightSmooth(vec.norm(), r, g, b);
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
      // D.scatData[0].push_back(std::array<double, 2>({(double)y / (double)(nY - 1), VelZ[nX / 2][y][zCursor] + (double)zCursor / (double)(nZ - 1)}));
      // D.scatData[2].push_back(std::array<double, 2>({(double)y / (double)(nY - 1), Pres[nX / 2][y][zCursor] + (double)zCursor / (double)(nZ - 1)}));
    }
  }
  if (nY > 1) {
    for (int z= 0; z < nZ; z++) {
      D.scatData[1].push_back(std::array<double, 2>({VelY[nX / 2][yCursor][z], (double)z / (double)(nZ - 1)}));
      D.scatData[3].push_back(std::array<double, 2>({Pres[nX / 2][yCursor][z], (double)z / (double)(nZ - 1)}));
      // D.scatData[1].push_back(std::array<double, 2>({VelY[nX / 2][yCursor][z] + (double)yCursor / (double)(nY - 1), (double)z / (double)(nZ - 1)}));
      // D.scatData[3].push_back(std::array<double, 2>({Pres[nX / 2][yCursor][z] + (double)yCursor / (double)(nY - 1), (double)z / (double)(nZ - 1)}));
    }
  }

  // Add hard coded experimental values for lid driven cavity flow benchmark
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
      D.scatData[2].push_back(std::array<double, 2>({GhiaData0X[k], GhiaData0Y[k]}));
      D.scatData[3].push_back(std::array<double, 2>({GhiaData1X[k], GhiaData1Y[k]}));
      // D.scatData[2].push_back(std::array<double, 2>({GhiaData0X[k], GhiaData0Y[k] + 0.5f}));
      // D.scatData[3].push_back(std::array<double, 2>({GhiaData1X[k] + 0.5f, GhiaData1Y[k]}));
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
      D.scatData[4].push_back(std::array<double, 2>({ErtuData0X[k], ErtuData0Y[k]}));
      D.scatData[5].push_back(std::array<double, 2>({ErtuData1X[k], ErtuData1Y[k]}));
      // D.scatData[4].push_back(std::array<double, 2>({ErtuData0X[k], ErtuData0Y[k] + 0.5f}));
      // D.scatData[5].push_back(std::array<double, 2>({ErtuData1X[k] + 0.5f, ErtuData1Y[k]}));
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
        // D.scatData[4].push_back(std::array<double, 2>({analyVelY + (double)yCursor / (double)(nY - 1), (double)z / (double)(nZ - 1)}));
      }
    }
    if (nZ > 1) {
      for (int y= 0; y < nY; y++) {
        const float analyP= press0 + (press1 - press0) * (float)y / (float)(nY - 1);
        D.scatData[5].push_back(std::array<double, 2>({(double)y / (double)(nY - 1), analyP}));
        // D.scatData[5].push_back(std::array<double, 2>({(double)y / (double)(nY - 1), analyP + (double)zCursor / (double)(nZ - 1)}));
      }
    }
  }

  if (!D.UI[VerboseSolv_].GetB()) {
    // Draw the plot data
    D.plotData.resize(8);
    D.plotLegend.resize(8);
    D.plotLegend[0]= "VelMag";
    D.plotLegend[1]= "Smok";
    D.plotLegend[2]= "SmokAbs";
    D.plotLegend[3]= "Pres";
    D.plotLegend[4]= "PresAbs";
    D.plotLegend[5]= "Vorti";
    if (D.plotData[0].size() < 500) {
      for (int k= 0; k < (int)D.plotLegend.size(); k++)
        D.plotData[k].push_back(0.0f);
      for (int x= 0; x < nX; x++) {
        for (int y= 0; y < nY; y++) {
          for (int z= 0; z < nZ; z++) {
            D.plotData[0][D.plotData[0].size() - 1]+= std::sqrt(VelX[x][y][z] * VelX[x][y][z] + VelY[x][y][z] * VelY[x][y][z] + VelZ[x][y][z] * VelZ[x][y][z]);
            D.plotData[1][D.plotData[1].size() - 1]+= Smok[x][y][z];
            D.plotData[2][D.plotData[2].size() - 1]+= std::abs(Smok[x][y][z]);
            D.plotData[3][D.plotData[3].size() - 1]+= Pres[x][y][z];
            D.plotData[4][D.plotData[4].size() - 1]+= std::abs(Pres[x][y][z]);
            D.plotData[5][D.plotData[5].size() - 1]+= Vort[x][y][z];
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
    if (inputFile == 0)
      FileInput::LoadImageBMPFile("Resources/CFD_TeslaValveTwinSharp.bmp", imageRGBA, false);
    else if (inputFile == 1)
      FileInput::LoadImageBMPFile("Resources/CFD_Venturi.bmp", imageRGBA, false);
    else if (inputFile == 2)
      FileInput::LoadImageBMPFile("Resources/CFD_Wing.bmp", imageRGBA, false);
    else if (inputFile == 3)
      FileInput::LoadImageBMPFile("Resources/CFD_Nozzle.bmp", imageRGBA, false);
    else if (inputFile == 4)
      FileInput::LoadImageBMPFile("Resources/CFD_Wall.bmp", imageRGBA, false);
    else
      FileInput::LoadImageBMPFile("Resources/CFD_Pipe.bmp", imageRGBA, false);
  }

  // Set scenario values
  for (int x= 0; x < nX; x++) {
    for (int y= 0; y < nY; y++) {
      for (int z= 0; z < nZ; z++) {
        // Scenario from loaded BMP file
        if (scenarioType == 0) {
          // Get pixel colors
          const float posW= (float)(imageRGBA.size() - 1) * ((float)y + 0.5f) / (float)nY;
          const float posH= (float)(imageRGBA[0].size() - 1) * ((float)z + 0.5f) / (float)nZ;
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
          // Force zero velocity for no-slip condition on cavity walls
          if (y == 0 || y == nY - 1 || z == 0) {
            Solid[x][y][z]= true;
          }
          // Force tangential velocity on cavity lid
          else if (z == nZ - 1) {
            VelBC[x][y][z]= true;
            VelXForced[x][y][z]= D.UI[BCVelX______].GetF();
            VelYForced[x][y][z]= D.UI[BCVelY______].GetF();
            VelZForced[x][y][z]= D.UI[BCVelZ______].GetF();
          }
          // Add smoke source for visualization
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
          if ((nX > 1 && (x == 0 || x == nX - 1)) ||
              (nY > 1 && (y == 0 || y == nY - 1)) ||
              (nZ > 1 && (z == 0 || z == nZ - 1))) {
            Solid[x][y][z]= true;
          }
          else if (((nX == 1) != (std::min(x, nX - 1 - x) > nX / 3)) &&
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
        if (SmoBC[x][y][z] && iFieldID == FieldID::IDSmok) ioField[x][y][z]= SmokForced[x][y][z] * std::cos(simTime * M_PI / D.UI[BCSmokTime__].GetF());
        if (VelBC[x][y][z] && iFieldID == FieldID::IDVelX) ioField[x][y][z]= VelXForced[x][y][z];
        if (VelBC[x][y][z] && iFieldID == FieldID::IDVelY) ioField[x][y][z]= VelYForced[x][y][z];
        if (VelBC[x][y][z] && iFieldID == FieldID::IDVelZ) ioField[x][y][z]= VelZForced[x][y][z];
        if (PreBC[x][y][z] && iFieldID == FieldID::IDPres) ioField[x][y][z]= PresForced[x][y][z];
      }
    }
  }
}


void CompuFluidDyna::ImplicitFieldAdd(const std::vector<std::vector<std::vector<float>>>& iFieldA,
                                      const std::vector<std::vector<std::vector<float>>>& iFieldB,
                                      std::vector<std::vector<std::vector<float>>>& oField) {
  for (int x= 0; x < nX; x++)
    for (int y= 0; y < nY; y++)
      for (int z= 0; z < nZ; z++)
        oField[x][y][z]= iFieldA[x][y][z] + iFieldB[x][y][z];
}


void CompuFluidDyna::ImplicitFieldSub(const std::vector<std::vector<std::vector<float>>>& iFieldA,
                                      const std::vector<std::vector<std::vector<float>>>& iFieldB,
                                      std::vector<std::vector<std::vector<float>>>& oField) {
  for (int x= 0; x < nX; x++)
    for (int y= 0; y < nY; y++)
      for (int z= 0; z < nZ; z++)
        oField[x][y][z]= iFieldA[x][y][z] - iFieldB[x][y][z];
}


void CompuFluidDyna::ImplicitFieldScale(const float iVal,
                                        const std::vector<std::vector<std::vector<float>>>& iField,
                                        std::vector<std::vector<std::vector<float>>>& oField) {
  for (int x= 0; x < nX; x++)
    for (int y= 0; y < nY; y++)
      for (int z= 0; z < nZ; z++)
        oField[x][y][z]= iField[x][y][z] * iVal;
}


float CompuFluidDyna::ImplicitFieldDotProd(const std::vector<std::vector<std::vector<float>>>& iFieldA,
                                           const std::vector<std::vector<std::vector<float>>>& iFieldB) {
  float val= 0.0f;
  for (int x= 0; x < nX; x++)
    for (int y= 0; y < nY; y++)
      for (int z= 0; z < nZ; z++)
        val+= iFieldA[x][y][z] * iFieldB[x][y][z];
  return val;
}


void CompuFluidDyna::ImplicitFieldLaplacianMatMult(const int iFieldID, const float iTimeStep,
                                                   const bool iDiffuMode, const float iDiffuCoeff, const bool iPrecondMode,
                                                   const std::vector<std::vector<std::vector<float>>>& iField,
                                                   std::vector<std::vector<std::vector<float>>>& oField) {
  // Mask encoding neighborhood
  constexpr int MaskSize= 6;
  constexpr int Mask[MaskSize][3]=
      {{+1, +0, +0},
       {-1, +0, +0},
       {+0, +1, +0},
       {+0, -1, +0},
       {+0, +0, +1},
       {+0, +0, -1}};

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
        int count= 0;
        float sum= 0.0f;
        for (int k= 0; k < MaskSize; k++) {
          const int xOff= x + Mask[k][0];
          const int yOff= y + Mask[k][1];
          const int zOff= z + Mask[k][2];
          if (xOff < 0 || xOff >= nX || yOff < 0 || yOff >= nY || zOff < 0 || zOff >= nZ) continue;
          // Add neighbor contribution considering BC (fixed value, zero deriv, mirror)
          if (!Solid[xOff][yOff][zOff]) sum+= iField[xOff][yOff][zOff];
          else if (iFieldID == FieldID::IDSmok) sum+= iField[x][y][z];
          else if (iFieldID == FieldID::IDPres) sum+= iField[x][y][z];
          else if (iFieldID == FieldID::IDVelX && xOff != x) sum+= -iField[x][y][z];
          else if (iFieldID == FieldID::IDVelY && yOff != y) sum+= -iField[x][y][z];
          else if (iFieldID == FieldID::IDVelZ && zOff != z) sum+= -iField[x][y][z];
          count++;
        }
        // Apply linear expression
        if (iDiffuMode) {
          if (iPrecondMode)
            oField[x][y][z]= 1.0f / (1.0f + diffuVal * (float)count) * iField[x][y][z];             //               [   -D*dt/(h*h)]
          else                                                                                      // [-D*dt/(h*h)] [1+4*D*dt/(h*h)] [-D*dt/(h*h)]
            oField[x][y][z]= -diffuVal * sum + (1.0f + diffuVal * (float)count) * iField[x][y][z];  //               [   -D*dt/(h*h)]
        }
        else {
          if (iPrecondMode)
            oField[x][y][z]= ((voxSize * voxSize) / (float)count) * iField[x][y][z];        //            [ 1/(h*h)]
          else                                                                              // [ 1/(h*h)] [-4/(h*h)] [ 1/(h*h)]
            oField[x][y][z]= (sum - (float)count * iField[x][y][z]) / (voxSize * voxSize);  //            [ 1/(h*h)]
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
  float errBeg= errNew;
  float errOld= 0.0f;

  // Iterate to solve
  for (int k= 0; k < iMaxIter; k++) {
    if (errNew / normRHS < D.UI[SolvTolRhs__].GetF()) break;
    if (errNew / errBeg < D.UI[SolvTolRel__].GetF()) break;
    if (errNew == 0.0f) break;

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
    errOld= errNew;
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


void CompuFluidDyna::ExternalForces() {
  // Sweep through the field
  for (int x= 0; x < nX; x++) {
    for (int y= 0; y < nY; y++) {
      for (int z= 0; z < nZ; z++) {
        // Skip solid or fixed values
        if (Solid[x][y][z]) continue;
        if (VelBC[x][y][z]) continue;
        // Add gravity effect to velocity
        VelZ[x][y][z]+= D.UI[TimeStep____].GetF() * D.UI[CoeffGravi__].GetF() * Smok[x][y][z];
      }
    }
  }
  ApplyBC(FieldID::IDVelZ, VelZ);
}


void CompuFluidDyna::ProjectField(const int iIter, const float iTimeStep,
                                  std::vector<std::vector<std::vector<float>>>& ioVelX,
                                  std::vector<std::vector<std::vector<float>>>& ioVelY,
                                  std::vector<std::vector<std::vector<float>>>& ioVelZ) {
  // Compute divergence for RHS
  ComputeVelocityDivergence();

  if (D.UI[CoeffProj___].GetI() == 3)
    Pres= Field::AllocField3D(nX, nY, nZ, 0.0f);

  // Solve for pressure in the pressure Poisson equation
  ConjugateGradientSolve(FieldID::IDPres, iIter, iTimeStep, false, 0.0f, Dive, Pres);

  // Update velocities based on local pressure gradient
  for (int x= 0; x < nX; x++) {
    for (int y= 0; y < nY; y++) {
      for (int z= 0; z < nZ; z++) {
        if (Solid[x][y][z] || VelBC[x][y][z]) continue;
        // Subtract pressure gradient with zero slope at interface to remove divergence
        const float timeDependance= (D.UI[CoeffProj___].GetI() == 2) ? iTimeStep * D.UI[CoeffProj2__].GetF() : 1.0f;
        if (x - 1 >= 0) ioVelX[x][y][z]-= (Solid[x - 1][y][z]) ? 0.0f : timeDependance * (Pres[x][y][z] - Pres[x - 1][y][z]) / (2.0f * voxSize);
        if (y - 1 >= 0) ioVelY[x][y][z]-= (Solid[x][y - 1][z]) ? 0.0f : timeDependance * (Pres[x][y][z] - Pres[x][y - 1][z]) / (2.0f * voxSize);
        if (z - 1 >= 0) ioVelZ[x][y][z]-= (Solid[x][y][z - 1]) ? 0.0f : timeDependance * (Pres[x][y][z] - Pres[x][y][z - 1]) / (2.0f * voxSize);
        if (x + 1 < nX) ioVelX[x][y][z]-= (Solid[x + 1][y][z]) ? 0.0f : timeDependance * (Pres[x + 1][y][z] - Pres[x][y][z]) / (2.0f * voxSize);
        if (y + 1 < nY) ioVelY[x][y][z]-= (Solid[x][y + 1][z]) ? 0.0f : timeDependance * (Pres[x][y + 1][z] - Pres[x][y][z]) / (2.0f * voxSize);
        if (z + 1 < nZ) ioVelZ[x][y][z]-= (Solid[x][y][z + 1]) ? 0.0f : timeDependance * (Pres[x][y][z + 1] - Pres[x][y][z]) / (2.0f * voxSize);
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


void CompuFluidDyna::AdvectField(const int iFieldID, const float iTimeStep,
                                 const std::vector<std::vector<std::vector<float>>>& iVelX,
                                 const std::vector<std::vector<std::vector<float>>>& iVelY,
                                 const std::vector<std::vector<std::vector<float>>>& iVelZ,
                                 std::vector<std::vector<std::vector<float>>>& ioField) {
  // Sweep through field and apply semi Lagrangian advection
  std::vector<std::vector<std::vector<float>>> oldField= ioField;

  // Adjust the source field to have non-zero values on the wall interface for continuity
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
          oldField[x][y][z]= (count > 0) ? sum / (float)count : 0.0f;
        }
      }
    }
  }

  for (int x= 0; x < nX; x++) {
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
        int correcMaxIter= 0;
        if (iFieldID == FieldID::IDSmok) correcMaxIter= std::max(D.UI[CoeffAdvec__].GetI() - 1, 0);
        if (iFieldID == FieldID::IDVelX) correcMaxIter= std::max(D.UI[CoeffAdvec__].GetI() - 1, 0);
        if (iFieldID == FieldID::IDVelY) correcMaxIter= std::max(D.UI[CoeffAdvec__].GetI() - 1, 0);
        if (iFieldID == FieldID::IDVelZ) correcMaxIter= std::max(D.UI[CoeffAdvec__].GetI() - 1, 0);
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
        ioField[x][y][z]= TrilinearInterpolation(posBeg[0], posBeg[1], posBeg[2], oldField);
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
  // Compute curl and vorticity from the velocity field
  ComputeVelocityCurlVorticity();

  // Amplify non-zero vorticity
  if (iVortiCoeff > 0.0f) {
    for (int x= 0; x < nX; x++) {
      for (int y= 0; y < nY; y++) {
        for (int z= 0; z < nZ; z++) {
          if (Solid[x][y][z] || VelBC[x][y][z] || PreBC[x][y][z]) continue;
          Math::Vec3f vortGrad(0.0f, 0.0f, 0.0f);
          if (x - 1 >= 0 && !Solid[x - 1][y][z]) vortGrad[0]+= (Vort[x][y][z] - Vort[x - 1][y][z]) / (2.0f * voxSize);
          if (y - 1 >= 0 && !Solid[x][y - 1][z]) vortGrad[1]+= (Vort[x][y][z] - Vort[x][y - 1][z]) / (2.0f * voxSize);
          if (z - 1 >= 0 && !Solid[x][y][z - 1]) vortGrad[2]+= (Vort[x][y][z] - Vort[x][y][z - 1]) / (2.0f * voxSize);
          if (x + 1 < nX && !Solid[x + 1][y][z]) vortGrad[0]+= (Vort[x + 1][y][z] - Vort[x][y][z]) / (2.0f * voxSize);
          if (y + 1 < nY && !Solid[x][y + 1][z]) vortGrad[1]+= (Vort[x][y + 1][z] - Vort[x][y][z]) / (2.0f * voxSize);
          if (z + 1 < nZ && !Solid[x][y][z + 1]) vortGrad[2]+= (Vort[x][y][z + 1] - Vort[x][y][z]) / (2.0f * voxSize);
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

    // Reapply BC to maintain consistency
    ApplyBC(FieldID::IDVelX, ioVelX);
    ApplyBC(FieldID::IDVelY, ioVelY);
    ApplyBC(FieldID::IDVelZ, ioVelZ);
  }
}


void CompuFluidDyna::ComputeVelocityDivergence() {
  if (D.UI[CoeffProj___].GetI() == 4) {
    // Precompute pressure gradient
    std::vector<std::vector<std::vector<float>>> PresGradX= Field::AllocField3D(nX, nY, nZ, 0.0f);
    std::vector<std::vector<std::vector<float>>> PresGradY= Field::AllocField3D(nX, nY, nZ, 0.0f);
    std::vector<std::vector<std::vector<float>>> PresGradZ= Field::AllocField3D(nX, nY, nZ, 0.0f);
    for (int x= 0; x < nX; x++) {
      for (int y= 0; y < nY; y++) {
        for (int z= 0; z < nZ; z++) {
          if (Solid[x][y][z]) continue;
          // Pressure gradient with zero derivative at solid interface
          if (x - 1 >= 0 && !Solid[x - 1][y][z]) PresGradX[x][y][z]+= (Pres[x][y][z] - Pres[x - 1][y][z]) / (2.0f * voxSize);
          if (y - 1 >= 0 && !Solid[x][y - 1][z]) PresGradY[x][y][z]+= (Pres[x][y][z] - Pres[x][y - 1][z]) / (2.0f * voxSize);
          if (z - 1 >= 0 && !Solid[x][y][z - 1]) PresGradZ[x][y][z]+= (Pres[x][y][z] - Pres[x][y][z - 1]) / (2.0f * voxSize);
          if (x + 1 < nX && !Solid[x + 1][y][z]) PresGradX[x][y][z]+= (Pres[x + 1][y][z] - Pres[x][y][z]) / (2.0f * voxSize);
          if (y + 1 < nY && !Solid[x][y + 1][z]) PresGradY[x][y][z]+= (Pres[x][y + 1][z] - Pres[x][y][z]) / (2.0f * voxSize);
          if (z + 1 < nZ && !Solid[x][y][z + 1]) PresGradZ[x][y][z]+= (Pres[x][y][z + 1] - Pres[x][y][z]) / (2.0f * voxSize);
        }
      }
    }
    // Compute divergence of velocity field using Rhie Chow interpolation correction
    for (int x= 0; x < nX; x++) {
      for (int y= 0; y < nY; y++) {
        for (int z= 0; z < nZ; z++) {
          Dive[x][y][z]= 0.0f;
          if (PreBC[x][y][z])
            Dive[x][y][z]= PresForced[x][y][z];
          if (Solid[x][y][z] || PreBC[x][y][z]) continue;
          // Classical linear interpolation for face velocities
          float velXN= (x - 1 >= 0) ? ((Solid[x - 1][y][z]) ? (0.0f) : ((VelX[x][y][z] + VelX[x - 1][y][z]) / 2.0f)) : (VelX[x][y][z]);
          float velYN= (y - 1 >= 0) ? ((Solid[x][y - 1][z]) ? (0.0f) : ((VelY[x][y][z] + VelY[x][y - 1][z]) / 2.0f)) : (VelY[x][y][z]);
          float velZN= (z - 1 >= 0) ? ((Solid[x][y][z - 1]) ? (0.0f) : ((VelZ[x][y][z] + VelZ[x][y][z - 1]) / 2.0f)) : (VelZ[x][y][z]);
          float velXP= (x + 1 < nX) ? ((Solid[x + 1][y][z]) ? (0.0f) : ((VelX[x + 1][y][z] + VelX[x][y][z]) / 2.0f)) : (VelX[x][y][z]);
          float velYP= (y + 1 < nY) ? ((Solid[x][y + 1][z]) ? (0.0f) : ((VelY[x][y + 1][z] + VelY[x][y][z]) / 2.0f)) : (VelY[x][y][z]);
          float velZP= (z + 1 < nZ) ? ((Solid[x][y][z + 1]) ? (0.0f) : ((VelZ[x][y][z + 1] + VelZ[x][y][z]) / 2.0f)) : (VelZ[x][y][z]);
          // Rhie and Chow correction by subtracting pressure gradient minus linear interpolation of pressure gradients
          velXN-= D.UI[CoeffProj2__].GetF() * ((x - 1 >= 0 && !Solid[x - 1][y][z]) ? ((Pres[x][y][z] - Pres[x - 1][y][z]) / voxSize) : (0.0f));
          velYN-= D.UI[CoeffProj2__].GetF() * ((y - 1 >= 0 && !Solid[x][y - 1][z]) ? ((Pres[x][y][z] - Pres[x][y - 1][z]) / voxSize) : (0.0f));
          velZN-= D.UI[CoeffProj2__].GetF() * ((z - 1 >= 0 && !Solid[x][y][z - 1]) ? ((Pres[x][y][z] - Pres[x][y][z - 1]) / voxSize) : (0.0f));
          velXP-= D.UI[CoeffProj2__].GetF() * ((x + 1 < nX && !Solid[x + 1][y][z]) ? ((Pres[x + 1][y][z] - Pres[x][y][z]) / voxSize) : (0.0f));
          velYP-= D.UI[CoeffProj2__].GetF() * ((y + 1 < nY && !Solid[x][y + 1][z]) ? ((Pres[x][y + 1][z] - Pres[x][y][z]) / voxSize) : (0.0f));
          velZP-= D.UI[CoeffProj2__].GetF() * ((z + 1 < nZ && !Solid[x][y][z + 1]) ? ((Pres[x][y][z + 1] - Pres[x][y][z]) / voxSize) : (0.0f));
          velXN+= D.UI[CoeffProj3__].GetF() * ((x - 1 >= 0) ? ((PresGradX[x][y][z] + PresGradX[x - 1][y][z]) / 2.0f) : (0.0f));
          velYN+= D.UI[CoeffProj3__].GetF() * ((y - 1 >= 0) ? ((PresGradY[x][y][z] + PresGradY[x][y - 1][z]) / 2.0f) : (0.0f));
          velZN+= D.UI[CoeffProj3__].GetF() * ((z - 1 >= 0) ? ((PresGradZ[x][y][z] + PresGradZ[x][y][z - 1]) / 2.0f) : (0.0f));
          velXP+= D.UI[CoeffProj3__].GetF() * ((x + 1 < nX) ? ((PresGradX[x + 1][y][z] + PresGradX[x][y][z]) / 2.0f) : (0.0f));
          velYP+= D.UI[CoeffProj3__].GetF() * ((y + 1 < nY) ? ((PresGradY[x][y + 1][z] + PresGradY[x][y][z]) / 2.0f) : (0.0f));
          velZP+= D.UI[CoeffProj3__].GetF() * ((z + 1 < nZ) ? ((PresGradZ[x][y][z + 1] + PresGradZ[x][y][z]) / 2.0f) : (0.0f));
          // Divergence based on face velocities
          Dive[x][y][z]= ((velXP - velXN) + (velYP - velYN) + (velZP - velZN)) / voxSize;
        }
      }
    }
  }
  else {
    // Compute divergence of velocity field using simple linear interpolation
    for (int x= 0; x < nX; x++) {
      for (int y= 0; y < nY; y++) {
        for (int z= 0; z < nZ; z++) {
          Dive[x][y][z]= 0.0f;
          if (PreBC[x][y][z])
            Dive[x][y][z]= PresForced[x][y][z];
          if (Solid[x][y][z] || PreBC[x][y][z]) continue;
          // Divergence with mirrored velocities at solid interface
          if (x - 1 >= 0) Dive[x][y][z]+= (Solid[x - 1][y][z]) ? (+2.0f * VelX[x][y][z]) : (VelX[x][y][z] - VelX[x - 1][y][z]);
          if (y - 1 >= 0) Dive[x][y][z]+= (Solid[x][y - 1][z]) ? (+2.0f * VelY[x][y][z]) : (VelY[x][y][z] - VelY[x][y - 1][z]);
          if (z - 1 >= 0) Dive[x][y][z]+= (Solid[x][y][z - 1]) ? (+2.0f * VelZ[x][y][z]) : (VelZ[x][y][z] - VelZ[x][y][z - 1]);
          if (x + 1 < nX) Dive[x][y][z]+= (Solid[x + 1][y][z]) ? (-2.0f * VelX[x][y][z]) : (VelX[x + 1][y][z] - VelX[x][y][z]);
          if (y + 1 < nY) Dive[x][y][z]+= (Solid[x][y + 1][z]) ? (-2.0f * VelY[x][y][z]) : (VelY[x][y + 1][z] - VelY[x][y][z]);
          if (z + 1 < nZ) Dive[x][y][z]+= (Solid[x][y][z + 1]) ? (-2.0f * VelZ[x][y][z]) : (VelZ[x][y][z + 1] - VelZ[x][y][z]);
          Dive[x][y][z]/= (2.0f * voxSize);
        }
      }
    }
  }
}


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
        CurX[x][y][z]= dVelz_dy - dVely_dz;
        CurY[x][y][z]= dVelx_dz - dVelz_dx;
        CurZ[x][y][z]= dVely_dx - dVelx_dy;
        Vort[x][y][z]= std::sqrt(CurX[x][y][z] * CurX[x][y][z] + CurY[x][y][z] * CurY[x][y][z] + CurZ[x][y][z] * CurZ[x][y][z]);
      }
    }
  }
}
