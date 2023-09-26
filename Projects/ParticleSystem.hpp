#pragma once

// Standard lib
#include <vector>

// Project lib
#include "../Util/Vector.hpp"


class ParticleSystem
{
  private:
  int NbParticles;

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

  ParticleSystem();

  void SetActiveProject();
  void CheckAlloc();
  void CheckRefresh();
  void Allocate();
  void Refresh();
  void Animate();
  void Draw();
};
