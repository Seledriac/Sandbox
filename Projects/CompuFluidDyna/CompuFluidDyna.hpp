#pragma once

// Standard lib
#include <vector>
#include <tuple>


// Fluid simulation code
// - Eulerian voxel grid
// - Handles 1D, 2D and 3D transparently
// - Linear solve in implicit diffusion step for viscosity and smoke spread/mixing
// - Linear solve in implicit pressure computation and projection to enforce mass conservation
// - Solves all linear systems with a custom matrixless diagonal preconditioned conjugate gradient
// - Semi Lagrangian backtracing for velocity and smoke advection
// - Uses iterative MackCormack backtracking scheme to achieve 2nd order accuracy in advection steps
// - Reinjects dissipated vorticity at smallest scale using vorticity confinement approach
// - Handles arbitrary boundary conditions and obstacles in the simulation domain using boolean flag fields
// - Validated on Re < 2000 in lid-driven cavity flow, Poiseuille, Couette and venturi benchmarks
// - Uses SI units
//
// References for fluid flow photographs, scenarios and visual comparison
// http://courses.washington.edu/me431/handouts/Album-Fluid-Motion-Van-Dyke.pdf
//
// References for "stable fluid" method
// http://graphics.cs.cmu.edu/nsp/course/15-464/Fall09/papers/StamFluidforGames.pdf
// http://www.dgp.toronto.edu/people/stam/reality/Research/zip/CDROM_GDC03.zip
// https://www.dgp.toronto.edu/public_user/stam/reality/Research/pdf/ns.pdf
// https://www.dgp.toronto.edu/public_user/stam/reality/Research/pub.html
// https://fr.wikipedia.org/wiki/Stable-Fluids
// http://www.dgp.utoronto.ca/~stam/reality/Talks/FluidsTalk/FluidsTalkNotes.pdf
// https://www.youtube.com/watch?v=qsYE1wMEMPA theory simple explanation
// https://www.youtube.com/watch?v=iKAVRgIrUOU JS, Matthias Müller, slightly different approach for pressure
// https://www.youtube.com/watch?v=wbYe58NGJJI python
// https://github.com/NiallHornFX/StableFluids3D-GL/blob/master/src/fluidsolver3d.cpp
class CompuFluidDyna
{
  private:
  enum FieldID
  {
    IDSmok,
    IDVelX,
    IDVelY,
    IDVelZ,
    IDPres,
  };

  // Problem dimensions
  int nX;
  int nY;
  int nZ;
  float voxSize;
  float simTime;

  // Pressure drop
  float IPD; // Initial pressure drop
  float RPD; // Relative pressure drop
  float PDD; // Pressure drop delta
  float minRPD; // Minimum pressure drop attained
  int nbIterSinceMinRPDChange; // Number of optimization iterations since the last minimum pressure drop change
  
  // Kinetic energy
  float KE; // Kinetic Energy
  float KED; // Kinetic Energy Delta  

  // Total Density
  float TD;
  int nbIterSinceMaxTDChange; // Number of iterations since the last max total density Change
  float MaxTD; // Max total density (used to find stability in periodic simulations)

  // Volume out of solid voxels
  float VolOOS;
  
  // Geometry surface area
  float SurfArea;

  // Initial geometry "diameter" (volume/surface area)
  float d0;

  // Flush Time
  float FTime;
  bool flushed;

  // Time since the last optimization interation
  float TimeSinceLastIter;

  // Flag for if optimization has started or not 
  bool OptimStarted;
  bool OptimEnded;

  // Fluid properties
  float fluidDensity;

  // Fields for scenario setup
  std::vector<std::vector<std::vector<bool>>> Solid;
  std::vector<std::vector<std::vector<bool>>> VelBC;
  std::vector<std::vector<std::vector<bool>>> PreBC;
  std::vector<std::vector<std::vector<bool>>> SmoBC;
  std::vector<std::vector<std::vector<float>>> VelXForced;
  std::vector<std::vector<std::vector<float>>> VelYForced;
  std::vector<std::vector<std::vector<float>>> VelZForced;
  std::vector<std::vector<std::vector<float>>> PresForced;
  std::vector<std::vector<std::vector<float>>> SmokForced;

  // Fields for scenario run
  std::vector<std::vector<std::vector<float>>> Dum0;
  std::vector<std::vector<std::vector<float>>> Dum1;
  std::vector<std::vector<std::vector<float>>> Dum2;
  std::vector<std::vector<std::vector<float>>> Dum3;
  std::vector<std::vector<std::vector<float>>> Dum4;
  std::vector<std::vector<std::vector<float>>> Vort;
  std::vector<std::vector<std::vector<float>>> Pres;
  std::vector<std::vector<std::vector<float>>> Dive;
  std::vector<std::vector<std::vector<float>>> Smok;
  std::vector<std::vector<std::vector<float>>> VelX;
  std::vector<std::vector<std::vector<float>>> VelY;
  std::vector<std::vector<std::vector<float>>> VelZ;
  std::vector<std::vector<std::vector<float>>> CurX;
  std::vector<std::vector<std::vector<float>>> CurY;
  std::vector<std::vector<std::vector<float>>> CurZ;
  std::vector<std::vector<std::vector<float>>> AdvX;
  std::vector<std::vector<std::vector<float>>> AdvY;
  std::vector<std::vector<std::vector<float>>> AdvZ;

  // CFD solver functions
  void SetUpUIData();
  void InitializeScenario();
  void ApplyBC(const int iFieldID, std::vector<std::vector<std::vector<float>>>& ioField);
  void ImplicitFieldAdd(const std::vector<std::vector<std::vector<float>>>& iFieldA,
                        const std::vector<std::vector<std::vector<float>>>& iFieldB,
                        std::vector<std::vector<std::vector<float>>>& oField);
  void ImplicitFieldSub(const std::vector<std::vector<std::vector<float>>>& iFieldA,
                        const std::vector<std::vector<std::vector<float>>>& iFieldB,
                        std::vector<std::vector<std::vector<float>>>& oField);
  void ImplicitFieldMult(const std::vector<std::vector<std::vector<float>>>& iFieldA,
                        const std::vector<std::vector<std::vector<float>>>& iFieldB,
                        std::vector<std::vector<std::vector<float>>>& oField);
  void ImplicitFieldScale(const float iVal,
                          const std::vector<std::vector<std::vector<float>>>& iField,
                          std::vector<std::vector<std::vector<float>>>& oField);
  float ImplicitFieldDotProd(const std::vector<std::vector<std::vector<float>>>& iFieldA,
                             const std::vector<std::vector<std::vector<float>>>& iFieldB);
  void ImplicitFieldLaplacianMatMult(const int iFieldID, const float iTimeStep,
                                     const bool iDiffuMode, const float iDiffuCoeff, const bool iPrecondMode,
                                     const std::vector<std::vector<std::vector<float>>>& iField,
                                     std::vector<std::vector<std::vector<float>>>& oField);
  void ConjugateGradientSolve(const int iFieldID, const int iMaxIter, const float iTimeStep,
                              const bool iDiffuMode, const float iDiffuCoeff,
                              const std::vector<std::vector<std::vector<float>>>& iField,
                              std::vector<std::vector<std::vector<float>>>& ioField);
  void GradientDescentSolve(const int iFieldID, const int iMaxIter, const float iTimeStep,
                            const bool iDiffuMode, const float iDiffuCoeff,
                            const std::vector<std::vector<std::vector<float>>>& iField,
                            std::vector<std::vector<std::vector<float>>>& ioField);
  void GaussSeidelSolve(const int iFieldID, const int iMaxIter, const float iTimeStep,
                        const bool iDiffuMode, const float iDiffuCoeff,
                        const std::vector<std::vector<std::vector<float>>>& iField,
                        std::vector<std::vector<std::vector<float>>>& ioField);
  void ExternalForces();
  void ProjectField(const int iMaxIter, const float iTimeStep,
                    std::vector<std::vector<std::vector<float>>>& ioVelX,
                    std::vector<std::vector<std::vector<float>>>& ioVelY,
                    std::vector<std::vector<std::vector<float>>>& ioVelZ);
  float TrilinearInterpolation(const float iPosX, const float iPosY, const float iPosZ,
                               const std::vector<std::vector<std::vector<float>>>& iFieldRef);
  void AdvectField(const int iFieldID, const float iTimeStep,
                   const std::vector<std::vector<std::vector<float>>>& iVelX,
                   const std::vector<std::vector<std::vector<float>>>& iVelY,
                   const std::vector<std::vector<std::vector<float>>>& iVelZ,
                   std::vector<std::vector<std::vector<float>>>& ioField);
  void VorticityConfinement(const float iTimeStep, const float iVortiCoeff,
                            std::vector<std::vector<std::vector<float>>>& ioVelX,
                            std::vector<std::vector<std::vector<float>>>& ioVelY,
                            std::vector<std::vector<std::vector<float>>>& ioVelZ);
  std::vector<std::tuple<int,int,int,float>> SortVoxels(std::vector<std::vector<std::vector<float>>>& ioField, 
                                                        bool iReverse);
  void ComputeVelocityDivergence();
  void ComputeVelocityCurlVorticity();
  float ComputePressureDrop(const bool iMode);
  void ComputeKineticEnergy();
  void ComputeTotalDensity();
  void ComputeVolumeOutOfSolid();
  void ComputeGeometrySurfaceArea();
  void HeuristicOptimization();
  

  public:
  bool isActivProj;
  bool isAllocated;
  bool isRefreshed;

  CompuFluidDyna();

  void SetActiveProject();
  bool CheckAlloc();
  bool CheckRefresh();
  void CheckFlushed();
  void Allocate();
  void KeyPress(const unsigned char key);
  void Refresh();
  void Animate();
  void Draw();
};
