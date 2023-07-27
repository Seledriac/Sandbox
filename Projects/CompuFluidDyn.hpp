#pragma once

// Standard lib
#include <vector>

// Project lib
#include "../math/Fields.hpp"
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

  Math::Field3D<float> Press;
  Math::Field3D<float> Densi;
  Math::Field3D<int> Solid;
  Math::Field3D<int> Force;
  Math::Field3D<Math::Vec3f> VelCu;

  void AllocateInitializeFields();
  void DeallocateFields();
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
