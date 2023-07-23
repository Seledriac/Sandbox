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


class AgentSwarm
{
  public:
  Math::Vec3f PosFood;
  Math::Vec3f PosPredator;

  int NbAgents;
  double SizeAgent;
  std::vector<Agent> Agents;

  bool isInitialized;
  bool isRefreshed;

  AgentSwarm();

  void Init();
  void Refresh();
  void Animate();
  void Draw();
};
