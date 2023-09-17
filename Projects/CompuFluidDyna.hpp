#pragma once

// Standard lib
#include <array>
#include <vector>


class CompuFluidDyna
{
  private:
  // Problem dimensions
  int nbX;
  int nbY;
  int nbZ;
  int maxDim;
  float voxSize;
  std::array<double, 3> boxMin;
  std::array<double, 3> boxMax;

  // Fields for scenario setup
  std::vector<std::vector<std::vector<bool>>> Solid;
  std::vector<std::vector<std::vector<bool>>> Passi;
  std::vector<std::vector<std::vector<bool>>> VelBC;
  std::vector<std::vector<std::vector<bool>>> SmoBC;
  std::vector<std::vector<std::vector<float>>> VelXForced;
  std::vector<std::vector<std::vector<float>>> VelYForced;
  std::vector<std::vector<std::vector<float>>> VelZForced;
  std::vector<std::vector<std::vector<float>>> SmoForced;

  // Fields for scenario run
  std::vector<std::vector<std::vector<float>>> Dumm0;
  std::vector<std::vector<std::vector<float>>> Dumm1;
  std::vector<std::vector<std::vector<float>>> Dumm2;
  std::vector<std::vector<std::vector<float>>> Dumm3;
  std::vector<std::vector<std::vector<float>>> Dumm4;
  std::vector<std::vector<std::vector<float>>> Dumm5;
  std::vector<std::vector<std::vector<float>>> Vorti;
  std::vector<std::vector<std::vector<float>>> Press;
  std::vector<std::vector<std::vector<float>>> Diver;
  std::vector<std::vector<std::vector<float>>> Smoke;
  std::vector<std::vector<std::vector<float>>> VelX;
  std::vector<std::vector<std::vector<float>>> VelY;
  std::vector<std::vector<std::vector<float>>> VelZ;
  std::vector<std::vector<std::vector<float>>> CurX;
  std::vector<std::vector<std::vector<float>>> CurY;
  std::vector<std::vector<std::vector<float>>> CurZ;

  // CFD solver functions
  void ApplyBC(const int iFieldID, std::vector<std::vector<std::vector<float>>>& ioField);
  float ImplicitFieldDotProd(const std::vector<std::vector<std::vector<float>>>& iFieldA,
                             const std::vector<std::vector<std::vector<float>>>& iFieldB);
  void ImplicitFieldAdd(const std::vector<std::vector<std::vector<float>>>& iFieldA,
                        const std::vector<std::vector<std::vector<float>>>& iFieldB,
                        std::vector<std::vector<std::vector<float>>>& oField);
  void ImplicitFieldSub(const std::vector<std::vector<std::vector<float>>>& iFieldA,
                        const std::vector<std::vector<std::vector<float>>>& iFieldB,
                        std::vector<std::vector<std::vector<float>>>& oField);
  void ImplicitFieldScale(const std::vector<std::vector<std::vector<float>>>& iField,
                          const float iVal,
                          std::vector<std::vector<std::vector<float>>>& oField);
  void ImplicitFieldLaplacianMatMult(const int iFieldID, const float iTimeStep,
                                     const bool iDiffuMode, const float iDiffuCoeff,
                                     const std::vector<std::vector<std::vector<float>>>& iField,
                                     std::vector<std::vector<std::vector<float>>>& oField);
  void ConjugateGradientSolve(const int iFieldID, const int iMaxIter, const float iTimeStep,
                              const bool iDiffuMode, const float iDiffuCoeff,
                              const std::vector<std::vector<std::vector<float>>>& iField,
                              std::vector<std::vector<std::vector<float>>>& ioField,
                              std::vector<std::vector<std::vector<float>>>& oResid);
  void GaussSeidelSolve(const int iFieldID, const int iMaxIter, const float iTimeStep,
                        const bool iDiffuMode, const float iDiffuCoeff,
                        const std::vector<std::vector<std::vector<float>>>& iField,
                        std::vector<std::vector<std::vector<float>>>& ioField,
                        std::vector<std::vector<std::vector<float>>>& oResid);
  float TrilinearInterpolation(const float iPosX, const float iPosY, const float iPosZ,
                               const std::vector<std::vector<std::vector<float>>>& iFieldRef);
  void AdvectField(const int iFieldID, const float iTimeStep,
                   const std::vector<std::vector<std::vector<float>>>& iVelX,
                   const std::vector<std::vector<std::vector<float>>>& iVelY,
                   const std::vector<std::vector<std::vector<float>>>& iVelZ,
                   std::vector<std::vector<std::vector<float>>>& ioField);
  void ProjectField(const int iMaxIter, const float iTimeStep,
                    std::vector<std::vector<std::vector<float>>>& ioVelX,
                    std::vector<std::vector<std::vector<float>>>& ioVelY,
                    std::vector<std::vector<std::vector<float>>>& ioVelZ,
    std::vector<std::vector<std::vector<float>>>& oResid);
  void VorticityConfinement(const float iTimeStep, const float iVortiCoeff,
                            std::vector<std::vector<std::vector<float>>>& ioVelX,
                            std::vector<std::vector<std::vector<float>>>& ioVelY,
                            std::vector<std::vector<std::vector<float>>>& ioVelZ);

  public:
  bool isActiveProject;
  bool isInitialized;
  bool isRefreshed;

  CompuFluidDyna();

  void SetActiveProject();
  void CheckInit();
  void CheckRefresh();
  void Initialize();
  void Refresh();
  void Animate();
  void Draw();
};
