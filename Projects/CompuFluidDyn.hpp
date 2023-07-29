#pragma once

// Standard lib
#include <array>
#include <vector>

// Project lib
#include "../math/Vectors.hpp"

#define NEW_IMPLEM

class CompuFluidDyn
{
  private:
  int nbX;
  int nbY;
  int nbZ;

#ifdef NEW_IMPLEM
#else
#endif

#ifdef NEW_IMPLEM
  std::vector<std::vector<std::vector<float>>> DensOld;
  std::vector<std::vector<std::vector<float>>> DensNew;
  std::vector<std::vector<std::vector<float>>> VelXOld;
  std::vector<std::vector<std::vector<float>>> VelYOld;
  std::vector<std::vector<std::vector<float>>> VelZOld;
  std::vector<std::vector<std::vector<float>>> VelXNew;
  std::vector<std::vector<std::vector<float>>> VelYNew;
  std::vector<std::vector<std::vector<float>>> VelZNew;
#else
  float* VelXNew;
  float* VelYNew;
  float* VelZNew;
  float* VelXOld;
  float* VelYOld;
  float* VelZOld;
  float* DensNew;
  float* DensOld;
#endif

  std::vector<std::vector<std::array<float, 4>>> loadedImage;

  std::vector<std::vector<std::vector<float>>> OSDensi;
  std::vector<std::vector<std::vector<float>>> OSPress;
  std::vector<std::vector<std::vector<float>>> OSDiver;
  std::vector<std::vector<std::vector<int>>> OSSolid;
  std::vector<std::vector<std::vector<int>>> OSForce;
  std::vector<std::vector<std::vector<Math::Vec3f>>> OSVelCu;

#ifdef NEW_IMPLEM
  void AddSource(const std::vector<std::vector<std::vector<float>>>& iSource, const float iTimestep,
                 std::vector<std::vector<std::vector<float>>>& ioField);
  void ApplyBC(const std::vector<std::vector<std::vector<int>>>& iType, const bool iMirror, const bool iAverage,
               std::vector<std::vector<std::vector<float>>>& ioField);
  void GaussSeidelSolve(const std::vector<std::vector<std::vector<int>>>& iType, const bool iMirror, const bool iAverage,
                        const int iIter, const bool iAdvancedMode, const float iMultip,
                        const std::vector<std::vector<std::vector<float>>>& iFieldRef,
                        std::vector<std::vector<std::vector<float>>>& ioField);
  void DiffuseField(const std::vector<std::vector<std::vector<int>>>& iType, const bool iMirror, const bool iAverage,
                    const int iIter, const float iTimeStep, const float iDiffusionCoeff,
                    const std::vector<std::vector<std::vector<float>>>& iFieldRef,
                    std::vector<std::vector<std::vector<float>>>& ioField);
  float TrilinearInterpolation(const float iPosX, const float iPosY, const float iPosZ,
                               const std::vector<std::vector<std::vector<float>>>& iFieldRef);
  void AdvectField(const std::vector<std::vector<std::vector<int>>>& iType,
                   const bool iMirror, const bool iAverage, const float iTimeStep,
                   const std::vector<std::vector<std::vector<float>>>& iVelX,
                   const std::vector<std::vector<std::vector<float>>>& iVelY,
                   const std::vector<std::vector<std::vector<float>>>& iVelZ,
                   std::vector<std::vector<std::vector<float>>>& ioField);
  void ProjectField(const std::vector<std::vector<std::vector<int>>>& iType, const int iIter,
                    std::vector<std::vector<std::vector<float>>>& ioVelX,
                    std::vector<std::vector<std::vector<float>>>& ioVelY,
                    std::vector<std::vector<std::vector<float>>>& ioVelZ);
  void SwapFields(std::vector<std::vector<std::vector<float>>>& ioFieldA,
                  std::vector<std::vector<std::vector<float>>>& ioFieldB);
  void DensityStep(const std::vector<std::vector<std::vector<int>>>& iType,
                   const int iIter, const float iTimeStep, const float iDiffusionCoeff,
                   const std::vector<std::vector<std::vector<float>>>& iVelX,
                   const std::vector<std::vector<std::vector<float>>>& iVelY,
                   const std::vector<std::vector<std::vector<float>>>& iVelZ,
                   std::vector<std::vector<std::vector<float>>>& ioDensOld,
                   std::vector<std::vector<std::vector<float>>>& ioDensNew);
  void VelocityStep(const std::vector<std::vector<std::vector<int>>>& iType,
                    const int iIter, const float iTimeStep, const float iDiffusionCoeff,
                    std::vector<std::vector<std::vector<float>>>& ioVelXOld,
                    std::vector<std::vector<std::vector<float>>>& ioVelYOld,
                    std::vector<std::vector<std::vector<float>>>& ioVelZOld,
                    std::vector<std::vector<std::vector<float>>>& ioVelXNew,
                    std::vector<std::vector<std::vector<float>>>& ioVelYNew,
                    std::vector<std::vector<std::vector<float>>>& ioVelZNew);
#else
  void AllocateInitializeFields();
  void DeallocateFields();
#endif

  void CheckNeedRefresh();

  public:
  bool isInitialized;
  bool isRefreshed;

  CompuFluidDyn();
  ~CompuFluidDyn();

  void Init();
  void Refresh();
  void Animate();
  void Draw();
};
