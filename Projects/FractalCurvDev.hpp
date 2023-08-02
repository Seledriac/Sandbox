#pragma once

// Standard lib
#include <array>
#include <vector>

// Project lib
#include "../math/Vectors.hpp"


class FractalCurvDev
{
  private:
  std::vector<std::vector<Math::Vec3f>> Nodes;
  std::vector<std::array<Math::Vec3f, 3>> Faces;

  public:
  bool isActiveProject;
  bool isInitialized;
  bool isRefreshed;

  FractalCurvDev();

  void SetActiveProject();
  void Initialize();
  void Refresh();
  void Animate();
  void Draw();
};
