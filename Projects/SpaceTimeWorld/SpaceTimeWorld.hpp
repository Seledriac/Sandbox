#pragma once

// Standard lib
#include <vector>

// Project lib
#include "../../Util/Vec.hpp"


// Improvised curved space-time rendering
// - 4D Eulerian representation of the world
// - Local curvature evaluated from mass distribution
// - Explicit integration of photons backtracing in the curved fields from the screen to the source
class SpaceTimeWorld
{
  private:
  int worldNbT;
  int worldNbX;
  int worldNbY;
  int worldNbZ;
  std::vector<std::vector<std::vector<std::vector<bool>>>> worldSolid;
  std::vector<std::vector<std::vector<std::vector<bool>>>> worldIsFix;
  std::vector<std::vector<std::vector<std::vector<float>>>> worldMasss;
  std::vector<std::vector<std::vector<std::vector<Vec::Vec3f>>>> worldColor;
  std::vector<std::vector<std::vector<std::vector<Vec::Vec4f>>>> worldFlows;

  int screenNbH;
  int screenNbV;
  int screenNbS;
  std::vector<std::vector<Vec::Vec3f>> screenColor;
  std::vector<std::vector<int>> screenCount;
  std::vector<std::vector<std::vector<Vec::Vec4f>>> photonPos;
  std::vector<std::vector<std::vector<Vec::Vec4f>>> photonVel;

  public:
  bool isActivProj;
  bool isAllocated;
  bool isRefreshed;

  SpaceTimeWorld();

  void SetActiveProject();
  void CheckAlloc();
  void CheckRefresh();
  void Allocate();
  void Refresh();
  void Animate();
  void Draw();
};
