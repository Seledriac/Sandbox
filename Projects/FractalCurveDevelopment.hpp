#pragma once

// Standard lib
#include <array>
#include <vector>

// Project lib
#include "../math/Vectors.hpp"


class FractalCurveDevelopment
{
  public:
  std::vector<std::vector<Math::Vec3f>> Nodes;
  std::vector<std::array<Math::Vec3f, 3>> Faces;

  bool isInitialized;
  bool isRefreshed;

  FractalCurveDevelopment();

  void Init();
  void Refresh();
  void Animate();
  void Draw();
};
