#pragma once

// Standard lib
#include <vector>

// Project lib
#include "../../Util/Vec.hpp"


// Simple implementation of Reynolds Boids to produce emergent swarm intelligence
// - Each boid is an agent with an evolving position and velocity vector
// - Agent velocities evolve through explicit time integration
// - The forces are a combination of separation, aligment, cohesion and target attraction/repulsion behaviors
//
// References for "Boids" concept
// https://en.wikipedia.org/wiki/Boids
class AgentSwarmBoid
{
  private:
  int NbAgents;
  int NbTypes;
  std::vector<Vec::Vec3f> Pos;
  std::vector<Vec::Vec3f> Vel;
  std::vector<int> Typ;

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
