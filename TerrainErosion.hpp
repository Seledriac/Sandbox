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
  std::vector<std::vector<float>> terrainVal;
  std::vector<std::vector<float>> terrainRef;
  std::vector<std::vector<Math::Vec3f>> terrainGra;
  std::vector<std::vector<Math::Vec3f>> terrainPos;
  std::vector<std::vector<Math::Vec3f>> terrainNor;

  int dropletNbK;
  int dropletNbS;
  std::vector<std::vector<Math::Vec3f>> dropletPos;
  std::vector<std::vector<Math::Vec3f>> dropletVel;

  bool isInitialized;
  bool isRefreshed;

  TerrainErosion();

  void Init();
  void Refresh();
  void Draw();

  private:
  void ComputeTerrainFractal();
  void ComputeTerrainSmoothing();
  void ComputeTerrainRescaling();
  void ComputeTerrainMesh();
};
