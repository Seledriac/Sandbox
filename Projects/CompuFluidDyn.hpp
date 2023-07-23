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
  std::vector<std::vector<std::vector<float>>> velX;
  std::vector<std::vector<std::vector<float>>> velY;
  std::vector<std::vector<std::vector<float>>> velZ;
  std::vector<std::vector<std::vector<bool>>> lockX;
  std::vector<std::vector<std::vector<bool>>> lockY;
  std::vector<std::vector<std::vector<bool>>> lockZ;
  std::vector<std::vector<std::vector<float>>> divergence;
  std::vector<std::vector<std::vector<float>>> density;

  public:
  bool isInitialized;
  bool isRefreshed;

  CompuFluidDyn();

  void Init();
  void Refresh();
  void Animate();
  void Draw();
};
