#pragma once

// Standard lib
#include <array>
#include <vector>

// Project lib
#include "math/Vectors.hpp"


class SpaceTimeWorld
{
  public:
  int worldNbT;
  int worldNbX;
  int worldNbY;
  int worldNbZ;
  std::vector<std::vector<std::vector<std::vector<bool>>>> worldSolid;
  std::vector<std::vector<std::vector<std::vector<bool>>>> worldIsFix;
  std::vector<std::vector<std::vector<std::vector<double>>>> worldCurva;
  std::vector<std::vector<std::vector<std::vector<Math::Vec3>>>> worldColor;
  std::vector<std::vector<std::vector<std::vector<Math::Vec4>>>> worldFlows;

  int screenNbH;
  int screenNbV;
  int screenNbS;
  std::vector<std::vector<Math::Vec3>> screenColor;
  std::vector<std::vector<int>> screenCount;
  std::vector<std::vector<std::vector<Math::Vec4>>> photonPos;
  std::vector<std::vector<std::vector<Math::Vec4>>> photonVel;

  bool isInitialized;
  bool isRefreshed;

  SpaceTimeWorld();

  void Init();
  void Refresh();
  void Draw();
};
