#pragma once

// Standard lib
#include <array>
#include <vector>

// Project lib
#include "../Util/Vector.hpp"


class FractalCurvDev
{
  private:
  std::vector<std::vector<Vector::Vec3f>> Nodes;
  std::vector<std::array<Vector::Vec3f, 3>> Faces;

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
