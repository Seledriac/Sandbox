#pragma once


// Standard lib
#include <vector>

// Project lib
#include "math/Vec.hpp"


class SpaceTimeWorld
{
  public:
  int worldNbT;
  int worldNbX;
  int worldNbY;
  int worldNbZ;
  std::vector<std::vector<std::vector<std::vector<bool>>>> worldSolid;
  std::vector<std::vector<std::vector<std::vector<math::Vec3>>>> worldColor;
  std::vector<std::vector<std::vector<std::vector<math::Vec3>>>> worldGravi;

  int screenNbH;
  int screenNbV;
  int screenNbS;
  std::vector<std::vector<bool>> screenSet;
  std::vector<std::vector<math::Vec3>> screenCol;

  std::vector<std::vector<std::vector<math::Vec3>>> photonPos;
  std::vector<std::vector<std::vector<math::Vec3>>> photonVel;

  SpaceTimeWorld(int const iWorldNbT, int const iWorldNbX, int const iWorldNbY, int const iWorldNbZ,
                 int const iScreenNbH, int const iScreenNbV, int const iScreenNbS);

  void draw();
  void animate(double const iTimestep);
};
