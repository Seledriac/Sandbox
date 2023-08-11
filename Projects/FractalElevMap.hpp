#pragma once

// Standard lib
#include <vector>

// Project lib
#include "../util/Vector.hpp"


class FractalElevMap
{
  private:
  int mapNbX;
  int mapNbY;
  int mapNbIter;
  double mapDivThresh;
  double mapZoom;
  Vector::Vec2d mapFocus;
  Vector::Vec2d mapConst;

  std::vector<std::vector<Vector::Vec3f>> mapPos;
  std::vector<std::vector<Vector::Vec3f>> mapNor;
  std::vector<std::vector<Vector::Vec3f>> mapCol;

  public:
  bool isActiveProject;
  bool isInitialized;
  bool isRefreshed;

  FractalElevMap();

  void SetActiveProject();
  void Initialize();
  void Refresh();
  void Animate();
  void Draw();

};
