#pragma once

// Standard lib
#include <array>
#include <vector>

// Project lib
#include "../Util/Vector.hpp"


class FractalCurvDev
{
  private:
  std::vector<std::vector<Math::Vec3f>> Nodes;
  std::vector<std::array<Math::Vec3f, 3>> Faces;

  public:
  bool isActivProj;
  bool isAllocated;
  bool isRefreshed;

  FractalCurvDev();

  void SetActiveProject();
  void CheckAlloc();
  void CheckRefresh();
  void Allocate();
  void Refresh();
  void Animate();
  void Draw();
};
