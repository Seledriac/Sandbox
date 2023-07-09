#pragma once

// Standard lib
#include <array>
#include <vector>

// Project lib
#include "math/Vectors.hpp"


class FractalHeightMap
{
  public:
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

  bool isInitialized;
  bool isRefreshed;

  FractalHeightMap();

  void Init();
  void Refresh();
  void Draw();
};
