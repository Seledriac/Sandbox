#pragma once

// Standard lib
#include <vector>


// Fluid simulation code based on the "Stable Fluid" method popularized by Jos Stam in 1999
// - Eulerian voxel grid
// - Linear solve in implicit diffusion step for viscosity and smoke spread/mixing
// - Linear solve in implicit pressure computation and projection to enforce mass conservation
// - Semi Lagrangian backtracing for velocity and smoke advection
//
// Improvements over standard Stable Fluids implementations
// - Handles both 2D and 3D
// - Handles arbitrary boundary conditions and obstacles in the simulation domain
// - Solves all linear systems with a custom matrixless diagonal preconditioned conjugate gradient
// - Uses MackCormack backtracking scheme to achieve 2nd order accuracy in advection steps
// - Validated on low and high Reynolds lid driven cavity, Poiseuille, Couette and venturi benchmarks
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
//
// References for fluid flow photographs, scenarios and visual comparison
// http://courses.washington.edu/me431/handouts/Album-Fluid-Motion-Van-Dyke.pdf
//
// References for MacCormack backtracking scheme
// https://commons.wikimedia.org/wiki/File:Backtracking_maccormack.png
// https://physbam.stanford.edu/~fedkiw/papers/stanford2006-09.pdf
// https://github.com/NiallHornFX/StableFluids3D-GL/blob/master/src/fluidsolver3d.cpp
//
// References for vorticity confinement implem
// https://github.com/awesson/stable-fluids/tree/master
// https://github.com/woeishi/StableFluids/blob/master/StableFluid3d.cpp
//
// References for linear solvers and particularily PCG
// https://www.cs.cmu.edu/~quake-papers/painless-conjugate-gradient.pdf
// https://services.math.duke.edu/~holee/math361-2020/lectures/Conjugate_gradients.pdf
// https://www3.nd.edu/~zxu2/acms60212-40212-S12/final_project/Linear_solvers_GPU.pdf
// https://github.com/awesson/stable-fluids/tree/master
// https://en.wikipedia.org/wiki/Conjugate_gradient_method
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
  int nbX;
  int nbY;
  int nbZ;
  float voxSize;

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
  void ApplyBC(const int iFieldID, std::vector<std::vector<std::vector<float>>>& ioField);
  void ImplicitFieldAdd(const std::vector<std::vector<std::vector<float>>>& iFieldA,
                        const std::vector<std::vector<std::vector<float>>>& iFieldB,
                        std::vector<std::vector<std::vector<float>>>& oField);
  void ImplicitFieldSub(const std::vector<std::vector<std::vector<float>>>& iFieldA,
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

  public:
  bool isActivProj;
  bool isAllocated;
  bool isRefreshed;

  CompuFluidDyna();

  void SetActiveProject();
  void CheckAlloc();
  void CheckRefresh();
  void Allocate();
  void Refresh();
  void Animate();
  void Draw();
};
