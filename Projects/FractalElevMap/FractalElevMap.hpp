#pragma once

// Standard lib
#include <vector>

// Project lib
#include "../../Util/Vector.hpp"


// Generate 2.5D surface based on the Julia Set fractal
// - Representation with height map
// - Interactive depth, coefficients and zoom factor as UI parameters
//
// https://en.wikipedia.org/wiki/Julia_set
class FractalElevMap
{
  private:
  int mapNbX;
  int mapNbY;
  int mapNbIter;
  double mapDivThresh;
  double mapZoom;
  Math::Vec2d mapFocus;
  Math::Vec2d mapConst;

  std::vector<std::vector<Math::Vec3f>> mapPos;
  std::vector<std::vector<Math::Vec3f>> mapNor;
  std::vector<std::vector<Math::Vec3f>> mapCol;

  public:
  bool isActivProj;
  bool isAllocated;
  bool isRefreshed;

  FractalElevMap();

  void SetActiveProject();
  void CheckAlloc();
  void CheckRefresh();
  void Allocate();
  void Refresh();
  void Animate();
  void Draw();
};
