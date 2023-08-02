#pragma once

// Standard lib
#include <array>
#include <vector>

// Project lib
#include "../math/Vectors.hpp"


class CompuFluidDyna
{
  private:
  int nbX;
  int nbY;
  int nbZ;

  std::vector<std::vector<std::vector<int>>> Bound;
  std::vector<std::vector<std::vector<int>>> Force;
  std::vector<std::vector<std::vector<int>>> Sourc;
  std::vector<std::vector<std::vector<float>>> DensAdd;
  std::vector<std::vector<std::vector<float>>> DensCur;
  std::vector<std::vector<std::vector<float>>> VelXAdd;
  std::vector<std::vector<std::vector<float>>> VelYAdd;
  std::vector<std::vector<std::vector<float>>> VelZAdd;
  std::vector<std::vector<std::vector<float>>> VelXCur;
  std::vector<std::vector<std::vector<float>>> VelYCur;
  std::vector<std::vector<std::vector<float>>> VelZCur;
  std::vector<std::vector<std::array<float, 3>>> loadedImage;

  void AddSource(const std::vector<std::vector<std::vector<float>>>& iSource, const float iTimestep,
                 std::vector<std::vector<std::vector<float>>>& ioField);
  void ApplyBC(const std::vector<std::vector<std::vector<int>>>& iType, const bool iMirror,
               std::vector<std::vector<std::vector<float>>>& ioField);
  void GaussSeidelSolve(const std::vector<std::vector<std::vector<int>>>& iType, const bool iMirror,
                        const int iIter, const bool iAdvancedMode, const float iMultip,
                        std::vector<std::vector<std::vector<float>>>& ioField);
  void DiffuseField(const std::vector<std::vector<std::vector<int>>>& iType, const bool iMirror,
                    const int iIter, const float iTimeStep, const float iDiffusionCoeff,
                    std::vector<std::vector<std::vector<float>>>& ioField);
  float TrilinearInterpolation(const float iPosX, const float iPosY, const float iPosZ,
                               const std::vector<std::vector<std::vector<float>>>& iFieldRef);
  void AdvectField(const std::vector<std::vector<std::vector<int>>>& iType,
                   const bool iMirror, const float iTimeStep,
                   const std::vector<std::vector<std::vector<float>>>& iVelX,
                   const std::vector<std::vector<std::vector<float>>>& iVelY,
                   const std::vector<std::vector<std::vector<float>>>& iVelZ,
                   std::vector<std::vector<std::vector<float>>>& ioField);
  void ProjectField(const std::vector<std::vector<std::vector<int>>>& iType, const int iIter,
                    std::vector<std::vector<std::vector<float>>>& ioVelX,
                    std::vector<std::vector<std::vector<float>>>& ioVelY,
                    std::vector<std::vector<std::vector<float>>>& ioVelZ);
  void DensityStep(const std::vector<std::vector<std::vector<int>>>& iType,
                   const int iIter, const float iTimeStep, const float iDiffusionCoeff,
                   const std::vector<std::vector<std::vector<float>>>& iVelX,
                   const std::vector<std::vector<std::vector<float>>>& iVelY,
                   const std::vector<std::vector<std::vector<float>>>& iVelZ,
                   const std::vector<std::vector<std::vector<float>>>& ioDensAdd,
                   std::vector<std::vector<std::vector<float>>>& ioDensCur);
  void VelocityStep(const std::vector<std::vector<std::vector<int>>>& iType,
                    const int iIter, const float iTimeStep, const float iDiffusionCoeff,
                    const std::vector<std::vector<std::vector<float>>>& iVelXAdd,
                    const std::vector<std::vector<std::vector<float>>>& iVelYAdd,
                    const std::vector<std::vector<std::vector<float>>>& iVelZAdd,
                    std::vector<std::vector<std::vector<float>>>& ioVelXCur,
                    std::vector<std::vector<std::vector<float>>>& ioVelYCur,
                    std::vector<std::vector<std::vector<float>>>& ioVelZCur);

  public:
  bool isActiveProject;
  bool isInitialized;
  bool isRefreshed;

  CompuFluidDyna();

  void SetActiveProject();
  void Initialize();
  void Refresh();
  void Animate();
  void Draw();
};
