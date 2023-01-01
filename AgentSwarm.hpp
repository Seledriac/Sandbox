#pragma once


// Standard lib
#include <vector>

// Project lib
#include "math/Vectors.hpp"

class Agent
{
  public:
  Math::Vec3 p, v, n;
};


class AgentSwarm
{
  public:
  Math::Vec3 PosFood;
  Math::Vec3 PosPredator;

  int NbAgents;
  double SizeAgent;
  std::vector<Agent> Agents;

  bool isInitialized;

  AgentSwarm();

  void Init();
  void Draw();
  void Animate();
};
