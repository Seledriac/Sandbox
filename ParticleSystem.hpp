#pragma once

// Standard lib
#include <vector>

// Project lib
#include "math/Vectors.hpp"


class ParticleSystem
{
  public:
  int NbParticles;
  float BaseRadius;

  std::vector<Math::Vec3f> PosOld;
  std::vector<Math::Vec3f> PosCur;
  std::vector<Math::Vec3f> VelCur;
  std::vector<Math::Vec3f> AccCur;
  std::vector<Math::Vec3f> ForCur;
  std::vector<Math::Vec3f> ColCur;
  std::vector<float> RadCur;
  std::vector<float> MasCur;
  std::vector<float> HotCur;

  bool isInitialized;

  ParticleSystem();

  void Init();
  void Animate();
  void Draw();
};
