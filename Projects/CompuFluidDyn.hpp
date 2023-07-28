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

  Math::Field3D<float> OSDensi;
  Math::Field3D<float> OSPress;
  Math::Field3D<float> OSDiver;
  Math::Field3D<int> OSSolid;
  Math::Field3D<int> OSForce;
  Math::Field3D<Math::Vec3f> OSVelCu;

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
