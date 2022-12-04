#pragma once


// Standard lib
#include <vector>
#include <array>

// Project lib
#include "math/Vectors.hpp"


class SpaceTimeWorld
{
  public:
  int worldNbT;
  int worldNbX;
  int worldNbY;
  int worldNbZ;
  std::array<double, 3> worldBBoxMin;
  std::array<double, 3> worldBBoxMax;
  std::vector<std::vector<std::vector<std::vector<bool>>>> worldSolid;
  std::vector<std::vector<std::vector<std::vector<math::Vec3>>>> worldColor;
  std::vector<std::vector<std::vector<std::vector<math::Vec3>>>> worldFlow;

  int screenNbH;
  int screenNbV;
  int screenNbS;
  std::vector<std::vector<math::Vec3>> screenCol;
  std::vector<std::vector<int>> screenCount;

  std::vector<std::vector<std::vector<math::Vec3>>> photonPos;
  std::vector<std::vector<std::vector<math::Vec3>>> photonVel;

  SpaceTimeWorld();

  void draw();
};
