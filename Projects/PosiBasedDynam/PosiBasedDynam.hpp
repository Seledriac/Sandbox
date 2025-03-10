#pragma once

// Standard lib
#include <vector>

// Sandbox lib
#include "../../Util/Vec.hpp"


// Particle system simulation with a Position-Based Dynamics (PBD) approach
// - Particle positions are iteratively updated with explicit time integration
// - Collision constraints are resolved by correcting positions of particles with a Gauss Seidel relaxation
// - Velocities are deduced from changed positions
//
// Reference
// https://www.youtube.com/watch?v=jrociOAYqxA
// https://www.youtube.com/watch?v=lS_qeBy3aQI
class PosiBasedDynam
{
  private:
  int N;

  std::vector<Vec::Vec3<float>> PosOld;
  std::vector<Vec::Vec3<float>> PosCur;
  std::vector<Vec::Vec3<float>> VelCur;
  std::vector<Vec::Vec3<float>> AccCur;
  std::vector<Vec::Vec3<float>> ForCur;
  std::vector<Vec::Vec3<float>> ColCur;
  std::vector<float> RadCur;
  std::vector<float> MasCur;
  std::vector<float> HotCur;

  public:
  bool isActivProj;
  bool isAllocated;
  bool isRefreshed;

  PosiBasedDynam();

  void SetActiveProject();
  bool CheckAlloc();
  bool CheckRefresh();
  void Allocate();
  void KeyPress(const unsigned char key);
  void Refresh();
  void Animate();
  void Draw();
};
