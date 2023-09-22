#pragma once

// Standard lib
#include <array>
#include <vector>


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
  int maxDim;
  float voxSize;
  std::array<double, 3> boxMin;
  std::array<double, 3> boxMax;

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
  void GaussSeidelSolve(const int iFieldID, const int iMaxIter, const float iTimeStep,
                        const bool iDiffuMode, const float iDiffuCoeff,
                        const std::vector<std::vector<std::vector<float>>>& iField,
                        std::vector<std::vector<std::vector<float>>>& ioField);
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
                    std::vector<std::vector<std::vector<float>>>& ioVelZ);
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
