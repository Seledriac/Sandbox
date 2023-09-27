#pragma once

// Standard lib
#include <vector>

// Project lib
#include "../Util/Vector.hpp"


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
  std::vector<Math::Vec3f> Pos;
  std::vector<Math::Vec3f> Vel;
  std::vector<Math::Vec3f> Nor;
  Math::Vec3f PosFood;
  Math::Vec3f PosPred;

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
