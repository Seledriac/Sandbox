#pragma once

// Standard lib
#include <vector>

// Project lib
#include "../util/Vector.hpp"


class TerrainErosion
{
  private:
  int terrainNbX;
  int terrainNbY;
  int terrainNbC;
  std::vector<std::vector<Vector::Vec3f>> terrainPos;
  std::vector<std::vector<Vector::Vec3f>> terrainNor;
  std::vector<std::vector<Vector::Vec3f>> terrainCol;
  std::vector<std::vector<float>> terrainChg;

  int dropletNbK;
  std::vector<Vector::Vec3f> dropletPosOld;
  std::vector<Vector::Vec3f> dropletPosCur;
  std::vector<Vector::Vec3f> dropletVelCur;
  std::vector<Vector::Vec3f> dropletAccCur;
  std::vector<Vector::Vec3f> dropletForCur;
  std::vector<Vector::Vec3f> dropletColCur;
  std::vector<float> dropletMasCur;
  std::vector<float> dropletRadCur;
  std::vector<float> dropletSatCur;
  std::vector<bool> dropletIsDead;

  public:
  bool isActiveProject;
  bool isInitialized;
  bool isRefreshed;

  TerrainErosion();

  void SetActiveProject();
  void Initialize();
  void Refresh();
  void Animate();
  void Draw();
};
