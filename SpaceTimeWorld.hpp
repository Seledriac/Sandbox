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
  std::vector<std::vector<std::vector<std::vector<Math::Vec3>>>> worldColor;
  std::vector<std::vector<std::vector<std::vector<Math::Vec3>>>> worldFlows;

  int screenNbH;
  int screenNbV;
  int screenNbS;
  std::vector<std::vector<Math::Vec3>> screenColor;
  std::vector<std::vector<int>> screenCount;
  std::vector<std::vector<std::vector<Math::Vec3>>> photonPos;
  std::vector<std::vector<std::vector<Math::Vec3>>> photonVel;

  bool isInitialized;
  
  SpaceTimeWorld();

  void Init();
  void Draw();
};
