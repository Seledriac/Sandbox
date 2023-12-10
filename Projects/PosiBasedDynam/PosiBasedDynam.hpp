#pragma once

// Standard lib
#include <vector>

// Project lib
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

  std::vector<Vec::Vec3f> PosOld;
  std::vector<Vec::Vec3f> PosCur;
  std::vector<Vec::Vec3f> VelCur;
  std::vector<Vec::Vec3f> AccCur;
  std::vector<Vec::Vec3f> ForCur;
  std::vector<Vec::Vec3f> ColCur;
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
