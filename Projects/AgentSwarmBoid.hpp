#pragma once

// Standard lib
#include <vector>

// Project lib
#include "../Util/Vector.hpp"


class AgentSwarmBoid
{
  private:
  int NbAgents;
  std::vector<Math::Vec3f> Pos;
  std::vector<Math::Vec3f> Vel;
  std::vector<Math::Vec3f> Nor;
  Math::Vec3f PosFood;
  Math::Vec3f PosPredator;

  public:
  bool isActivProj;
  bool isAllocated;
  bool isRefreshed;

  AgentSwarmBoid();

  void SetActiveProject();
  void CheckAlloc();
  void CheckRefresh();
  void Allocate();
  void Refresh();
  void Animate();
  void Draw();
};
