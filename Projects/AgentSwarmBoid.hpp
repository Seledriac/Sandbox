#pragma once

// Standard lib
#include <vector>

// Project lib
#include "../Util/Vector.hpp"


class Agent
{
  public:
  Vector::Vec3f p, v, n;
};


class AgentSwarmBoid
{
  private:
  int NbAgents;
  std::vector<Agent> Agents;
  Vector::Vec3f PosFood;
  Vector::Vec3f PosPredator;

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
