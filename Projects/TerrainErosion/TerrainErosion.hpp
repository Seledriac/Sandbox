#pragma once

// Standard lib
#include <vector>

// Sandbox lib
#include "../../Util/Vec.hpp"


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
  std::vector<std::vector<Vec::Vec3<float>>> terrainPos;
  std::vector<std::vector<Vec::Vec3<float>>> terrainNor;
  std::vector<std::vector<Vec::Vec3<float>>> terrainCol;
  std::vector<std::vector<float>> terrainChg;

  int dropletNbK;
  std::vector<Vec::Vec3<float>> dropletPosOld;
  std::vector<Vec::Vec3<float>> dropletPosCur;
  std::vector<Vec::Vec3<float>> dropletVelCur;
  std::vector<Vec::Vec3<float>> dropletAccCur;
  std::vector<Vec::Vec3<float>> dropletForCur;
  std::vector<Vec::Vec3<float>> dropletColCur;
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
  bool CheckAlloc();
  bool CheckRefresh();
  void Allocate();
  void KeyPress(const unsigned char key);
  void Refresh();
  void Animate();
  void Draw();
};
