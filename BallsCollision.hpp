#pragma once


// Standard lib
#include <vector>

// Project lib
#include "math/Vectors.hpp"


class BallsCollision
{
  public:
  int nbBalls;
  std::vector<Math::Vec3> Pos;
  std::vector<Math::Vec3> Vel;
  std::vector<Math::Vec3> Acc;
  std::vector<Math::Vec3> For;
  std::vector<Math::Vec3> Col;
  std::vector<double> Rad;
  std::vector<double> Hot;
  
  bool isInitialized;

  BallsCollision();

  void Init();
  void Draw();
  void Animate();
};
