#pragma once

// Standard lib
#include <vector>

// Project lib
#include "../math/Vectors.hpp"


class CompuFluidDyn
{
  private:
  int nbX;
  int nbY;
  int nbZ;

  float* VelXNew;
  float* VelYNew;
  float* VelZNew;
  float* VelXOld;
  float* VelYOld;
  float* VelZOld;
  float* DensNew;
  float* DensOld;

  std::vector<std::vector<std::vector<float>>> OSDensi;
  std::vector<std::vector<std::vector<float>>> OSPress;
  std::vector<std::vector<std::vector<float>>> OSDiver;
  std::vector<std::vector<std::vector<int>>> OSSolid;
  std::vector<std::vector<std::vector<int>>> OSForce;
  std::vector<std::vector<std::vector<Math::Vec3f>>> OSVelCu;

  void AllocateInitializeFields();
  void DeallocateFields();
  void CheckNeedRefresh();

  void AddSource(std::vector<std::vector<std::vector<float>>> const& iSource, float const iTimestep,
                 std::vector<std::vector<std::vector<float>>>& ioField);
  void ApplyBC(std::vector<std::vector<std::vector<int>>> const& iType, bool const iMirror, bool const iAverage,
               std::vector<std::vector<std::vector<float>>>& ioField);
  void GaussSeidelSolve(std::vector<std::vector<std::vector<int>>> const& iType, bool const iMirror, bool const iAverage,
                        int const iIter, bool const iAdvancedMode, float const iMultip,
                        std::vector<std::vector<std::vector<float>>> const& iFieldRef,
                        std::vector<std::vector<std::vector<float>>>& ioField);
  void DiffuseField(std::vector<std::vector<std::vector<int>>> const& iType, bool const iMirror, bool const iAverage,
                    int const iIter, float const iTimeStep, float const iDiffusionCoeff,
                    std::vector<std::vector<std::vector<float>>> const& iFieldRef,
                    std::vector<std::vector<std::vector<float>>>& ioField);
  void AdvectField(std::vector<std::vector<std::vector<int>>> const& iType,
                   bool const iMirror, bool const iAverage, float const iTimeStep,
                   std::vector<std::vector<std::vector<float>>> const& iVelX,
                   std::vector<std::vector<std::vector<float>>> const& iVelY,
                   std::vector<std::vector<std::vector<float>>> const& iVelZ,
                   std::vector<std::vector<std::vector<float>>> const& iFieldRef,
                   std::vector<std::vector<std::vector<float>>>& ioField);

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
