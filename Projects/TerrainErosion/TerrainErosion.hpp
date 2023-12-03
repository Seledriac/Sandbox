#pragma once

// Standard lib
#include <vector>

// Project lib
#include "../../Util/Vector.hpp"


// Terrain generation and erosion simulation 
// - Representation as height map
// - Initial terrain created with iterative random cut planes
// - Particles dropped and collide with the terrain using Position Based Dynamics scheme
// - Erosion and sedimentation handled by heuristic rules
//
// Reference
// https://www.youtube.com/watch?v=eaXk97ujbPQ
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
  bool isActivProj;
  bool isAllocated;
  bool isRefreshed;

  TerrainErosion();

  void SetActiveProject();
  void CheckAlloc();
  void CheckRefresh();
  void Allocate();
  void Refresh();
  void Animate();
  void Draw();
};
