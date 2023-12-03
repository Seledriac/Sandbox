#pragma once

// Standard lib
#include <array>
#include <vector>

// Project lib
#include "../../Util/Vector.hpp"


// String Art Generator
// - Algorithmic solver for the string art optimization problem
// - Reproduce the appearance of a target image by wrapping a string around a set of fixed pegs
class StringArtOptim
{
  private:
  // Problem dimensions
  int nW;
  int nH;

  // Fields for scenario setup
  std::vector<std::vector<Math::Vec3f>> ImRef;
  std::vector<std::vector<Math::Vec3f>> ImCur;
  std::vector<std::array<int, 2>> Pegs;
  std::vector<int> PegsCount;
  std::vector<std::vector<int>> Lines;
  std::vector<Math::Vec3f> Colors;

  bool AddLineStep();

  public:
  bool isActivProj;
  bool isAllocated;
  bool isRefreshed;

  StringArtOptim();

  void SetActiveProject();
  void CheckAlloc();
  void CheckRefresh();
  void Allocate();
  void Refresh();
  void Animate();
  void Draw();
};
