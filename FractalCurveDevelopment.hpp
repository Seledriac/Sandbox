#pragma once

// Standard lib
#include <vector>
#include <array>

// Project lib
#include "math/Vectors.hpp"


class FractalCurveDevelopment
{
  public:
  std::vector<std::vector<Math::Vec3>> Nodes;
  std::vector<std::array<Math::Vec3, 3>> Faces;

  bool isInitialized;

  FractalCurveDevelopment();

  void Init();
  void Draw();
};
