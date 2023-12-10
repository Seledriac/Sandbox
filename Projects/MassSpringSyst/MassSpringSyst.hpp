#pragma once

// Standard lib
#include <vector>

// Project lib
#include "../../Util/Vec.hpp"


// Mass-spring soft body simulation
// - TODO
//
// Reference mass spring
// https://www.cs.rpi.edu/~cutler/classes/advancedgraphics/S17/lectures/06_mass_spring_systems.pdf
//
// Reference implicit solve
// http://www.cs.cmu.edu/~baraff/sigcourse/notese.pdf
// http://www.cs.cmu.edu/~baraff/sigcourse/slidese.pdf
// https://blog.mmacklin.com/2012/05/04/implicitsprings/
// https://stackoverflow.com/questions/3897424/implementing-semi-implicit-backward-euler-in-a-1-dof-mass-spring-system?rq=4
// https://hugi.scene.org/online/hugi28/hugi%2028%20-%20coding%20corner%20uttumuttu%20implementing%20the%20implicit%20euler%20method%20for%20mass-spring%20systems.htm
class MassSpringSyst
{
  private:
  int N;
  std::vector<std::vector<int>> Adj;
  std::vector<Vec::Vec3f> Ref;
  std::vector<Vec::Vec3f> Pos;
  std::vector<Vec::Vec3f> Vel;
  std::vector<Vec::Vec3f> Acc;
  std::vector<Vec::Vec3f> For;
  std::vector<Vec::Vec3f> Ext;
  std::vector<Vec::Vec3f> Fix;
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
  bool CheckAlloc();
  bool CheckRefresh();
  void Allocate();
  void KeyPress(const unsigned char key);
  void Refresh();
  void Animate();
  void Draw();
};
