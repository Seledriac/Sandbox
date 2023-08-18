#pragma once

// Standard lib
#include <vector>

// Project lib
#include "../Util/Vector.hpp"


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
