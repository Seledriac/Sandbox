#pragma once

// Standard lib
#include <vector>

// Project lib
#include "../math/Vectors.hpp"


class SpaceTimeWorld
{
  public:
  int worldNbT;
  int worldNbX;
  int worldNbY;
  int worldNbZ;
  std::vector<std::vector<std::vector<std::vector<bool>>>> worldSolid;
  std::vector<std::vector<std::vector<std::vector<bool>>>> worldIsFix;
  std::vector<std::vector<std::vector<std::vector<float>>>> worldMasss;
  std::vector<std::vector<std::vector<std::vector<Math::Vec3f>>>> worldColor;
  std::vector<std::vector<std::vector<std::vector<Math::Vec4f>>>> worldFlows;

  int screenNbH;
  int screenNbV;
  int screenNbS;
  std::vector<std::vector<Math::Vec3f>> screenColor;
  std::vector<std::vector<int>> screenCount;
  std::vector<std::vector<std::vector<Math::Vec4f>>> photonPos;
  std::vector<std::vector<std::vector<Math::Vec4f>>> photonVel;

  bool isInitialized;
  bool isRefreshed;

  SpaceTimeWorld();

  void Init();
  void Refresh();
  void Animate();
  void Draw();
};
