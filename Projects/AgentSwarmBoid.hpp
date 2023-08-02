#pragma once

// Standard lib
#include <vector>

// Project lib
#include "../math/Vectors.hpp"


class Agent
{
  public:
  Math::Vec3f p, v, n;
};


class AgentSwarmBoid
{
  private:
  int NbAgents;
  std::vector<Agent> Agents;
  Math::Vec3f PosFood;
  Math::Vec3f PosPredator;

  public:
  bool isActiveProject;
  bool isInitialized;
  bool isRefreshed;

  AgentSwarmBoid();

  void SetActiveProject();
  void Initialize();
  void Refresh();
  void Animate();
  void Draw();
};
