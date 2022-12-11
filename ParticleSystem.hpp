// #pragma once

#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

// Standard lib
#include <vector>

// Project lib
#include "math/Vectors.hpp"


class ParticleSystem
{
  public:
  int NbParticles;

  std::vector<Math::Vec3> PosOld;
  std::vector<Math::Vec3> PosCur;
  std::vector<Math::Vec3> VelCur;
  std::vector<Math::Vec3> AccCur;
  std::vector<Math::Vec3> ForCur;
  std::vector<Math::Vec3> ColCur;
  std::vector<double> RadCur;
  std::vector<double> MasCur;
  std::vector<double> HotCur;

  bool isInitialized;

  ParticleSystem();

  void Init();
  void Draw();
  void Animate();
};

#endif
