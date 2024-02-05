#include "CompuFluidDyna.hpp"


// Standard lib
#include <cstdio>
#include <cstring>

// GLUT lib
#include "../../Libs/freeglut/include/GL/freeglut.h"

// Sandbox lib
#include "../../Util/Colormap.hpp"
#include "../../Util/Field.hpp"
#include "../../Util/Timer.hpp"
#include "../../Util/Vec.hpp"
#include "CompuFluidDynaParam.hpp"


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
    D.UI.push_back(ParamUI("InputFile___", 4));      // BMP file to load
    D.UI.push_back(ParamUI("ResolutionX_", 1));      // Eulerian mesh resolution
    D.UI.push_back(ParamUI("ResolutionY_", 200));    // Eulerian mesh resolution
    D.UI.push_back(ParamUI("ResolutionZ_", 200));    // Eulerian mesh resolution
    D.UI.push_back(ParamUI("VoxelSize___", 1e-2));   // Element size
    D.UI.push_back(ParamUI("TimeStep____", 0.02));   // Simulation time step
    D.UI.push_back(ParamUI("SolvMaxIter_", 32));     // Max number of solver iterations
    D.UI.push_back(ParamUI("SolvType____", 2));      // Flag to use Gauss Seidel (=0), Gradient Descent (=1) or Conjugate Gradient (=2)
    D.UI.push_back(ParamUI("SolvSOR_____", 1.8));    // Overrelaxation coefficient in Gauss Seidel solver
    D.UI.push_back(ParamUI("SolvTolRhs__", 0.0));    // Solver tolerance relative to RHS norm
    D.UI.push_back(ParamUI("SolvTolRel__", 1.e-3));  // Solver tolerance relative to initial guess
    D.UI.push_back(ParamUI("SolvTolAbs__", 0.0));    // Solver tolerance relative absolute value of residual magnitude
    D.UI.push_back(ParamUI("FlagOptim___", 1.0));    // Flag to activate the shape optimizer
    D.UI.push_back(ParamUI("FieldOptimE_", 1));      // Field and mode to use for the voxel sorting for erosion (1 == StrRate DESC, 2 == VelMag DESC, 3 == Vorticity DESC, 4 == StrRate ASC, 5 == VelMag ASC, 6 == Vorticity ASC)
    D.UI.push_back(ParamUI("FieldOptimS_", 4));      // Field and mode to use for the voxel sorting for sedimentation (1 == StrRate DESC, 2 == VelMag DESC, 3 == Vorticity DESC, 4 == StrRate ASC, 5 == VelMag ASC, 6 == Vorticity ASC)
    D.UI.push_back(ParamUI("OptimMFRTol_", 1.e-9));  // Shape optimizer tolerance relative to the mass flow rate
    D.UI.push_back(ParamUI("FlushTol____", 0.1));    // Tolerance relative to the minimum fluid density from which we consider that the fluid flushed  
    D.UI.push_back(ParamUI("KEDTol______", 1.e-0));  // Tolerance relative to Kinetic Energy delta to consider the flow stable
    D.UI.push_back(ParamUI("OptiIterWin_", 1000));   // Max number of iterations without a change of maxMFR before ending the optimization
    D.UI.push_back(ParamUI("SafeZoneRad_", 10));     // Radius of the zone of non optimization around the base case voxels
    D.UI.push_back(ParamUI("FracErosion_", 0.05));   // Fraction of eroded voxels at each optimization step
    D.UI.push_back(ParamUI("CoeffFluTime", 0.1));    // Coefficient applied to the flush time, time window between optimization iterations to reach flow stability
    D.UI.push_back(ParamUI("CoeffGravi__", 0.0));    // Magnitude of gravity in Z- direction
    D.UI.push_back(ParamUI("CoeffAdvec__", 5.0));    // 0= no advection, 1= linear advection, >1 MacCormack correction iterations
    D.UI.push_back(ParamUI("CoeffDiffuS_", 0.0009));  // Diffusion of smoke field, i.e. smoke spread/smear
    D.UI.push_back(ParamUI("CoeffDiffuV_", 0.0009)); // Diffusion of velocity field, i.e. viscosity
    D.UI.push_back(ParamUI("CoeffVorti__", 0.0));    // Vorticity confinement to avoid dissipation of energy in small scale vortices
    D.UI.push_back(ParamUI("CoeffProj___", 1.0));    // Enable incompressibility projection
    D.UI.push_back(ParamUI("BCVelX______", 0.0));    // Velocity value for voxels with enforced velocity
    D.UI.push_back(ParamUI("BCVelY______", 0.0));    // Velocity value for voxels with enforced velocity
    D.UI.push_back(ParamUI("BCVelZ______", 0.0));    // Velocity value for voxels with enforced velocity
    D.UI.push_back(ParamUI("BCPres______", 5.0));    // Pressure value for voxels with enforced pressure
    D.UI.push_back(ParamUI("BCSmok______", 1.0));    // Smoke value for voxels with enforced smoke
    D.UI.push_back(ParamUI("BCSmokTime__", 1.0));    // Period duration for input smoke oscillation
    D.UI.push_back(ParamUI("ObjectPosX__", 0.5));    // Coordinates for objects in hard coded scenarios
    D.UI.push_back(ParamUI("ObjectPosY__", 0.25));   // Coordinates for objects in hard coded scenarios
    D.UI.push_back(ParamUI("ObjectPosZ__", 0.5));    // Coordinates for objects in hard coded scenarios
    D.UI.push_back(ParamUI("ObjectSize0_", 0.5));    // Size for objects in hard coded scenarios
    D.UI.push_back(ParamUI("ObjectSize1_", 0.5));    // Size for objects in hard coded scenarios
    D.UI.push_back(ParamUI("ScaleFactor_", 1.0));    // Scale factor for drawn geometry
    D.UI.push_back(ParamUI("ColorFactor_", 1.0));    // Color factor for drawn geometry
    D.UI.push_back(ParamUI("ColorThresh_", 0.0));    // Color cutoff drawn geometry
    D.UI.push_back(ParamUI("ColorMode___", 1));      // Selector for the scalar field to be drawn
    D.UI.push_back(ParamUI("SliceDim____", 0));      // Enable model slicing along a dimension
    D.UI.push_back(ParamUI("SlicePlotX__", 0.5));    // Positions for the slices
    D.UI.push_back(ParamUI("SlicePlotY__", 0.5));    // Positions for the slices
    D.UI.push_back(ParamUI("SlicePlotZ__", 0.5));    // Positions for the slices
    D.UI.push_back(ParamUI("VerboseSolv_", -0.5));   // Verbose mode for linear solvers
    D.UI.push_back(ParamUI("VerboseTime_", -0.5));   // Verbose mode for linear solvers
    D.UI.push_back(ParamUI("Verbose_____", 0.0));    // Verbose mode
  }

  if (D.UI.size() != Verbose_____ + 1) {
    printf("[ERROR] Invalid parameter count in UI\n");
  }

  D.boxMin= {0.0, 0.0, 0.0};
  D.boxMax= {1.0, 1.0, 1.0};

  isActivProj= true;
  isAllocated= false;
  isRefreshed= false;
}


// Check if parameter changes should trigger an allocation
bool CompuFluidDyna::CheckAlloc() {
  if (D.UI[Scenario____].hasChanged()) isAllocated= false;
  if (D.UI[InputFile___].hasChanged()) isAllocated= false;
  if (D.UI[ResolutionX_].hasChanged()) isAllocated= false;
  if (D.UI[ResolutionY_].hasChanged()) isAllocated= false;
  if (D.UI[ResolutionZ_].hasChanged()) isAllocated= false;
  if (D.UI[VoxelSize___].hasChanged()) isAllocated= false;
  return isAllocated;
}


// Check if parameter changes should trigger a refresh
bool CompuFluidDyna::CheckRefresh() {
  if (D.UI[BCVelX______].hasChanged()) isRefreshed= false;
  if (D.UI[BCVelY______].hasChanged()) isRefreshed= false;
  if (D.UI[BCVelZ______].hasChanged()) isRefreshed= false;
  if (D.UI[BCPres______].hasChanged()) isRefreshed= false;
  if (D.UI[BCSmok______].hasChanged()) isRefreshed= false;
  if (D.UI[ObjectPosX__].hasChanged()) isRefreshed= false;
  if (D.UI[ObjectPosY__].hasChanged()) isRefreshed= false;
  if (D.UI[ObjectPosZ__].hasChanged()) isRefreshed= false;
  if (D.UI[ObjectSize0_].hasChanged()) isRefreshed= false;
  if (D.UI[ObjectSize1_].hasChanged()) isRefreshed= false;
  return isRefreshed;
}

// Check if the fluid has flushed through the whole geometry
void CompuFluidDyna::CheckFlushed() {
  float sumSmo = 0.0f;
  int nbSmo = 0;
  for (int x= 0; x < nX; x++) {
    for (int y= 0; y < nY; y++) {
      for (int z= 0; z < nZ; z++) {
        if (PreBC[x][y][z] && Pres[x][y][z] <= 0.0f) { // If it is an outlet voxel 
          sumSmo += Smok[x][y][z];
          nbSmo++;
        }
      }
    }
  }
  if ( (sumSmo/nbSmo) >= D.UI[FlushTol____].GetF())
    flushed = true;
}

// Allocate the project data
void CompuFluidDyna::Allocate() {
  if (!isActivProj) return;
  if (CheckAlloc()) return;
  isRefreshed= false;
  isAllocated= true;

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
  Vmag= Field::AllocField3D(nX, nY, nZ, 0.0f);
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

  StrRate= Field::AllocField3D(nX, nY, nZ, 0.0f);
}


// Refresh the project
void CompuFluidDyna::Refresh() {
  if (!isActivProj) return;
  if (!CheckAlloc()) Allocate();
  if (CheckRefresh()) return;
  isRefreshed= true;

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

  // Initialize optimization variables
  // RPD = 1.0f;
  // minRPD = RPD;
  // nbIterSinceMinRPDChange = 0;

  ComputeMassFlowRates(true);
  MaxMFR = MFR[0];
  nbIterSinceMaxMFRChange = 0;

  ComputeKineticEnergy();
  KED = INFINITY;

  flushed = false;
  FTime = 0.0f;

  TimeSinceLastIter = INFINITY;

  OptimStarted = false;
  OptimEnded = false;

  ComputeVolumeOutOfSolid();
  ComputeGeometrySurfaceArea();
  d0 = VolOOS / SurfArea;
  // printf("d0 : %f\n", d0);
  
}


// Handle keypress
void CompuFluidDyna::KeyPress(const unsigned char key) {
  if (!isActivProj) return;
  if (!CheckAlloc()) Allocate();
  (void)key;  // Disable warning unused variable
}


// Animate the project
void CompuFluidDyna::Animate() {
  if (!isActivProj) return;
  if (!CheckAlloc()) Allocate();
  if (!CheckRefresh()) Refresh();

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
  // ∂vel/∂t = - (vel · ∇) vel + visco ∇²vel − 1/ρ ∇press + f
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
    if (D.UI[SolvType____].GetI() == 0) {
      GaussSeidelSolve(FieldID::IDSmok, maxIter, timestep, true, coeffDiffu, oldSmoke, Smok);
    }
    else if (D.UI[SolvType____].GetI() == 1) {
      GradientDescentSolve(FieldID::IDSmok, maxIter, timestep, true, coeffDiffu, oldSmoke, Smok);
    }
    else {
      ConjugateGradientSolve(FieldID::IDSmok, maxIter, timestep, true, coeffDiffu, oldSmoke, Smok);
    }
  }
  if (D.UI[CoeffDiffuV_].GetB()) {
    // (Id - visco Δt ∇²) vel = vel
    std::vector<std::vector<std::vector<float>>> oldVelX= VelX;
    std::vector<std::vector<std::vector<float>>> oldVelY= VelY;
    std::vector<std::vector<std::vector<float>>> oldVelZ= VelZ;
    if (D.UI[SolvType____].GetI() == 0) {
      if (nX > 1) GaussSeidelSolve(FieldID::IDVelX, maxIter, timestep, true, coeffVisco, oldVelX, VelX);
      if (nY > 1) GaussSeidelSolve(FieldID::IDVelY, maxIter, timestep, true, coeffVisco, oldVelY, VelY);
      if (nZ > 1) GaussSeidelSolve(FieldID::IDVelZ, maxIter, timestep, true, coeffVisco, oldVelZ, VelZ);
    }
    else if (D.UI[SolvType____].GetI() == 1) {
      if (nX > 1) GradientDescentSolve(FieldID::IDVelX, maxIter, timestep, true, coeffVisco, oldVelX, VelX);
      if (nY > 1) GradientDescentSolve(FieldID::IDVelY, maxIter, timestep, true, coeffVisco, oldVelY, VelY);
      if (nZ > 1) GradientDescentSolve(FieldID::IDVelZ, maxIter, timestep, true, coeffVisco, oldVelZ, VelZ);
    }
    else {
      if (nX > 1) ConjugateGradientSolve(FieldID::IDVelX, maxIter, timestep, true, coeffVisco, oldVelX, VelX);
      if (nY > 1) ConjugateGradientSolve(FieldID::IDVelY, maxIter, timestep, true, coeffVisco, oldVelY, VelY);
      if (nZ > 1) ConjugateGradientSolve(FieldID::IDVelZ, maxIter, timestep, true, coeffVisco, oldVelZ, VelZ);
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

  // Display data on 2D graphs
  if (D.UI[VerboseTime_].GetB()) Timer::PushTimer();
  CompuFluidDyna::SetUpUIData();
  if (D.UI[VerboseTime_].GetB()) printf("%f T SetUpUIData\n", Timer::PopTimer());

  // TODO Compute fluid density to check if constant as it should be in incompressible case

  // Test heuristic optimization criterion method
  // https://open-research-europe.ec.europa.eu/articles/3-156  
  if (D.UI[FlagOptim___].GetB()) {
    TimeSinceLastIter += timestep;
    // Determine flush time through the geometry
    if (!flushed)
      CheckFlushed();
    if (flushed && !FTime) {
      FTime = simTime;
      printf("%f T flushTime\n", FTime);
    }
    // printf("%f KED\n", KED);
    if (KED >= D.UI[KEDTol______].GetF()) { // If the fluid is not in a steady state yet
      const float oldKE = KE;
      ComputeKineticEnergy();
      KED = std::abs(KE - oldKE);
    } else if (flushed && !OptimEnded) { // If it is stable (it should have flushed at this time)
      if (TimeSinceLastIter >= FTime * D.UI[CoeffFluTime].GetF()) {
        // Optimization based on pressure drop
        // if (!OptimStarted) {
        //   IPD = ComputePressureDrop(false); printf("IPD : %f\n", IPD);
        // }
        // printf("%f T stableTime\n", simTime);
        // const float oldRPD = RPD;
        // RPD = ComputePressureDrop(true);
        // PDD = std::abs(RPD - oldRPD);
        // printf("\nPDD after last opti : %f\n", PDD);
        // if (RPD < minRPD) {
        //   minRPD = RPD;
        //   nbIterSinceMinRPDChange = 0;
        // } else {
        //   nbIterSinceMinRPDChange++;
        // }
        // float PD = ComputePressureDrop(false);
        // Optimization based on mass flow rate
        if (!OptimStarted) {
          ComputeMassFlowRates(false);
          printf("Starting MFR : %f\n", MFR[0]);
          MFRD = 0.0f;
        } else {
          ComputeMassFlowRates(false);
          MFRD = std::abs(MFR[0] - MFRtmp);                  
          // printf("\nMFR delta after last opti : %f\n", MFRD);
          if (MFR[0] > MaxMFR) {
            MaxMFR = MFR[0];
            nbIterSinceMaxMFRChange = 0;
          } else {
            nbIterSinceMaxMFRChange++;
          }
        }
        if (nbIterSinceMaxMFRChange > D.UI[OptiIterWin_].GetI()
        || (OptimStarted && MFRD < std::max(D.UI[OptimMFRTol_].GetF(), 0.0f))) { // Optimization ends
          OptimEnded = true;
          printf("Final MFR : %f\n", MFR[0]);
        } else {
          OptimStarted = true;
          // printf("MFR before next opti : %f\n", MFR[0]);
          MFRtmp = MFR[0];
          HeuristicOptimizationStep();
          // printf("Opti\n");
        }
        TimeSinceLastIter = 0.0f;
      }
    }
  }

  // TODO Introduce solid interface normals calculations to better handle BC on sloped geometry ?
  // TODO Test with flow separation scenarios ?

  if (D.UI[VerboseTime_].GetB()) printf("\n");
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
    for (int x= 0; x < nX; x++) {
      for (int y= 0; y < nY; y++) {
        for (int z= 0; z < nZ; z++) {
          if (D.UI[SliceDim____].GetI() == 1 && x != (int)std::round(D.UI[SlicePlotX__].GetF() * nX)) continue;
          if (D.UI[SliceDim____].GetI() == 2 && y != (int)std::round(D.UI[SlicePlotY__].GetF() * nY)) continue;
          if (D.UI[SliceDim____].GetI() == 3 && z != (int)std::round(D.UI[SlicePlotZ__].GetF() * nZ)) continue;
          // Set the voxel color components
          float r= 0.4f, g= 0.4f, b= 0.4f;
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
          if (D.UI[SliceDim____].GetI() == 1 && x != (int)std::round(D.UI[SlicePlotX__].GetF() * nX)) continue;
          if (D.UI[SliceDim____].GetI() == 2 && y != (int)std::round(D.UI[SlicePlotY__].GetF() * nY)) continue;
          if (D.UI[SliceDim____].GetI() == 3 && z != (int)std::round(D.UI[SlicePlotZ__].GetF() * nZ)) continue;
          if (Solid[x][y][z] && D.UI[ColorThresh_].GetF() == 0.0) continue;
          float r= 0.0f, g= 0.0f, b= 0.0f;
          // Color by smoke
          if (D.UI[ColorMode___].GetI() == 1) {
            if (std::abs(Smok[x][y][z]) < D.UI[ColorThresh_].GetF()) continue;
            Colormap::RatioToPlasma(0.5f + 0.5f * Smok[x][y][z] * D.UI[ColorFactor_].GetF(), r, g, b);
          }
          // Color by velocity magnitude
          if (D.UI[ColorMode___].GetI() == 2) {
            Vec::Vec3<float> vec(VelX[x][y][z], VelY[x][y][z], VelZ[x][y][z]);
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
          // Color by vel in X
          if (D.UI[ColorMode___].GetI() == 6) {
            if (std::abs(VelX[x][y][z]) < D.UI[ColorThresh_].GetF()) continue;
            Colormap::RatioToJetBrightSmooth(0.5f + 0.5f * VelX[x][y][z] * D.UI[ColorFactor_].GetF(), r, g, b);
          }
          // Color by vel in Y
          if (D.UI[ColorMode___].GetI() == 7) {
            if (std::abs(VelY[x][y][z]) < D.UI[ColorThresh_].GetF()) continue;
            Colormap::RatioToJetBrightSmooth(0.5f + 0.5f * VelY[x][y][z] * D.UI[ColorFactor_].GetF(), r, g, b);
          }
          // Color by vel in Z
          if (D.UI[ColorMode___].GetI() == 8) {
            if (std::abs(VelZ[x][y][z]) < D.UI[ColorThresh_].GetF()) continue;
            Colormap::RatioToJetBrightSmooth(0.5f + 0.5f * VelZ[x][y][z] * D.UI[ColorFactor_].GetF(), r, g, b);
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
          glutSolidCube(1.0);
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
            if (D.UI[SliceDim____].GetI() == 1 && x != (int)std::round(D.UI[SlicePlotX__].GetF() * nX)) continue;
            if (D.UI[SliceDim____].GetI() == 2 && y != (int)std::round(D.UI[SlicePlotY__].GetF() * nY)) continue;
            if (D.UI[SliceDim____].GetI() == 3 && z != (int)std::round(D.UI[SlicePlotZ__].GetF() * nZ)) continue;
            if (Solid[x][y][z] && D.UI[ColorThresh_].GetF() == 0.0) continue;
            // Draw the velocity field
            Vec::Vec3<float> vec(VelX[x][y][z], VelY[x][y][z], VelZ[x][y][z]);
            if (std::abs(D.UI[SliceDim____].GetI()) == 1) vec[0]= 0.0f;
            if (std::abs(D.UI[SliceDim____].GetI()) == 2) vec[1]= 0.0f;
            if (std::abs(D.UI[SliceDim____].GetI()) == 3) vec[2]= 0.0f;
            if (vec.normSquared() > 0.0f) {
              float r= 0.0f, g= 0.0f, b= 0.0f;
              Colormap::RatioToJetBrightSmooth(vec.norm() * D.UI[ColorFactor_].GetF(), r, g, b);
              glColor3f(r, g, b);
              Vec::Vec3<float> pos((float)x, (float)y, (float)z);
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
          if (D.UI[SliceDim____].GetI() == 1 && x != (int)std::round(D.UI[SlicePlotX__].GetF() * nX)) continue;
          if (D.UI[SliceDim____].GetI() == 2 && y != (int)std::round(D.UI[SlicePlotY__].GetF() * nY)) continue;
          if (D.UI[SliceDim____].GetI() == 3 && z != (int)std::round(D.UI[SlicePlotZ__].GetF() * nZ)) continue;
          // Draw the velocity field
          Vec::Vec3<float> vec(AdvX[x][y][z], AdvY[x][y][z], AdvZ[x][y][z]);
          if (std::abs(D.UI[SliceDim____].GetI()) == 1) vec[0]= 0.0f;
          if (std::abs(D.UI[SliceDim____].GetI()) == 2) vec[1]= 0.0f;
          if (std::abs(D.UI[SliceDim____].GetI()) == 3) vec[2]= 0.0f;
          if (vec.normSquared() > 0.0f) {
            const float r= 0.5f - vec[0];
            const float g= 0.5f - vec[1];
            const float b= 0.5f - vec[2];
            glColor3f(r, g, b);
            Vec::Vec3<float> pos((float)x, (float)y, (float)z);
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
