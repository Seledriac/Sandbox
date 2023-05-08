#pragma once

// Standard lib
#include <vector>

// Project lib
#include "math/Vectors.hpp"


class TerrainErosion
{
  public:
  int terrainNbX;
  int terrainNbY;
  int terrainNbCuts;
  std::vector<std::vector<Math::Vec3f>> terrainPos;
  std::vector<std::vector<Math::Vec3f>> terrainNor;

  int dropletNbK;
  int dropletNbS;
  std::vector<Math::Vec3f> dropletPosOld;
  std::vector<Math::Vec3f> dropletPosCur;
  std::vector<Math::Vec3f> dropletVelCur;
  std::vector<Math::Vec3f> dropletAccCur;
  std::vector<Math::Vec3f> dropletForCur;
  std::vector<Math::Vec3f> dropletColCur;
  std::vector<float> dropletMasCur;
  std::vector<float> dropletRadCur;
  std::vector<float> dropletSatCur;
  std::vector<bool> dropletIsDead;


  bool isInitialized;
  bool isRefreshed;

  TerrainErosion();

  void Init();
  void Refresh();
  void Animate();
  void Draw();
};
