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
  std::vector<std::vector<std::vector<float>>> Press;
  std::vector<std::vector<std::vector<float>>> Smoke;
  std::vector<std::vector<std::vector<float>>> VelX;
  std::vector<std::vector<std::vector<float>>> VelY;
  std::vector<std::vector<std::vector<float>>> VelZ;

  // CFD solver functions
  void ApplyBC(const int iFieldID, std::vector<std::vector<std::vector<float>>>& ioField);
  void GaussSeidelSolve(const int iFieldID, const int iMaxIter, const float iTimeStep,
                        const bool iDiffuMode, const float iDiffuCoeff,
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
