#pragma once

// Standard lib
#include <vector>

// Project lib
#include "math/Vectors.hpp"


class FractalCurveDevelopment
{
  public:
  std::vector<std::vector<Math::Vec3>> Nodes;

  bool isInitialized;

  FractalCurveDevelopment();

  void Init();
  void Draw();
};
