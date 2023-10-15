#pragma once

// Standard lib
#include <vector>

// Project lib
#include "../Util/Vector.hpp"


// Mass-spring soft body simulation
// - TODO
//
// Reference mass spring
// https://www.cs.rpi.edu/~cutler/classes/advancedgraphics/S17/lectures/06_mass_spring_systems.pdf
//
// Reference implicit solve
// http://www.cs.cmu.edu/~baraff/sigcourse/notese.pdf
// http://www.cs.cmu.edu/~baraff/sigcourse/slidese.pdf
class MassSpringSyst
{
  private:
  int N;
  std::vector<std::vector<int>> Adj;
  std::vector<Math::Vec3f> Ref;
  std::vector<Math::Vec3f> Pos;
  std::vector<Math::Vec3f> Vel;
  std::vector<Math::Vec3f> Acc;
  std::vector<Math::Vec3f> For;
  std::vector<Math::Vec3f> Ext;
  std::vector<Math::Vec3f> Fix;
  std::vector<float> Mas;

  void ComputeForces();
  void StepForwardInTime();
  void ApplyBCPos();
  void ApplyBCFor();

  public:
  bool isActivProj;
  bool isAllocated;
  bool isRefreshed;

  MassSpringSyst();

  void SetActiveProject();
  void CheckAlloc();
  void CheckRefresh();
  void Allocate();
  void Refresh();
  void Animate();
  void Draw();
};
