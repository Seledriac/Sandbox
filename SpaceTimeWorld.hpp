#pragma once


// Standard lib
#include <vector>

// Project lib
#include "math/Fields.hpp"
#include "math/Vectors.hpp"


class SpaceTimeWorld
{
  public:
  int worldNbT;
  int worldNbX;
  int worldNbY;
  int worldNbZ;

  math::Field4D<bool> fieldSolid;
  math::Field4D<math::Vec3> fieldColor;
  math::Field4D<math::Vec3> fieldGravi;

  int screenNbH;
  int screenNbV;
  int screenNbS;
  math::Field2D<bool> screenSet;
  math::Field2D<math::Vec3> screenCol;
  math::Field3D<math::Vec3> photonPos;
  math::Field3D<math::Vec3> photonVel;

  SpaceTimeWorld(int const iWorldNbT, int const iWorldNbX, int const iWorldNbY, int const iWorldNbZ,
                 int const iScreenNbH, int const iScreenNbV, int const iScreenNbS);

  void draw();
  void animate(double const iTimestep);
};
