#pragma once

// Standard lib
#include <vector>

// Project lib
#include "../util/Vector.hpp"


class ParticleSystem
{
  private:
  int NbParticles;

  std::vector<Vector::Vec3f> PosOld;
  std::vector<Vector::Vec3f> PosCur;
  std::vector<Vector::Vec3f> VelCur;
  std::vector<Vector::Vec3f> AccCur;
  std::vector<Vector::Vec3f> ForCur;
  std::vector<Vector::Vec3f> ColCur;
  std::vector<float> RadCur;
  std::vector<float> MasCur;
  std::vector<float> HotCur;

  public:
  bool isActiveProject;
  bool isInitialized;
  bool isRefreshed;

  ParticleSystem();

  void SetActiveProject();
  void Initialize();
  void Refresh();
  void Animate();
  void Draw();
};
