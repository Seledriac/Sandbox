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
