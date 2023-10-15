#pragma once

// Standard lib
#include <array>
#include <vector>

// Project lib
#include "../Util/Vector.hpp"


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

  std::vector<Math::Vec3f> PosOld;
  std::vector<Math::Vec3f> PosCur;
  std::vector<Math::Vec3f> VelCur;
  std::vector<Math::Vec3f> AccCur;
  std::vector<Math::Vec3f> ForCur;
  std::vector<Math::Vec3f> ColCur;
  std::vector<float> RadCur;
  std::vector<float> MasCur;
  std::vector<float> HotCur;

  public:
  bool isActivProj;
  bool isAllocated;
  bool isRefreshed;

  PosiBasedDynam();

  void SetActiveProject();
  void CheckAlloc();
  void CheckRefresh();
  void Allocate();
  void Refresh();
  void Animate();
  void Draw();
};
