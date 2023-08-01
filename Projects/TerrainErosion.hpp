#pragma once

// Standard lib
#include <vector>

// Project lib
#include "../math/Vectors.hpp"


class TerrainErosion
{
  private:
  int terrainNbX;
  int terrainNbY;
  int terrainNbC;
  std::vector<std::vector<Math::Vec3f>> terrainPos;
  std::vector<std::vector<Math::Vec3f>> terrainNor;
  std::vector<std::vector<Math::Vec3f>> terrainCol;
  std::vector<std::vector<float>> terrainChg;

  int dropletNbK;
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

  public:
  bool isActiveProject;
  bool isInitialized;

  TerrainErosion();

  void SetActiveProject();
  void Initialize();
  void Animate();
  void Draw();
};
