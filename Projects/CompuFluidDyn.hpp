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

  void AllocateFields();
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


/*
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
  int simSteps;
  std::vector<std::vector<std::vector<float>>> velX;
  std::vector<std::vector<std::vector<float>>> velY;
  std::vector<std::vector<std::vector<float>>> velZ;
  std::vector<std::vector<std::vector<bool>>> lockX;
  std::vector<std::vector<std::vector<bool>>> lockY;
  std::vector<std::vector<std::vector<bool>>> lockZ;
  std::vector<std::vector<std::vector<float>>> pressure;
  std::vector<std::vector<std::vector<float>>> density;

  Math::Vec3f SampleStaggeredGrids(Math::Vec3f const& pos);

  public:
  bool isInitialized;
  bool isRefreshed;

  CompuFluidDyn();

  void Init();
  void Refresh();
  void Animate();
  void Draw();
};
*/
