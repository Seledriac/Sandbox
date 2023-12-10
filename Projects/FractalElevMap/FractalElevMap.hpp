#pragma once

// Standard lib
#include <vector>

// Project lib
#include "../../Util/Vec.hpp"


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
  Vec::Vec2d mapFocus;
  Vec::Vec2d mapConst;

  std::vector<std::vector<Vec::Vec3f>> mapPos;
  std::vector<std::vector<Vec::Vec3f>> mapNor;
  std::vector<std::vector<Vec::Vec3f>> mapCol;

  public:
  bool isActivProj;
  bool isAllocated;
  bool isRefreshed;

  FractalElevMap();

  void SetActiveProject();
  bool CheckAlloc();
  bool CheckRefresh();
  void Allocate();
  void KeyPress(const unsigned char key);
  void Refresh();
  void Animate();
  void Draw();
};
