#pragma once

// Standard lib
#include <vector>

// Project lib
#include "../math/Vectors.hpp"


class CompuFluidDyn
{
  private:
  int nbX;
  int nbY;
  int nbZ;
  int N;

  float dt;
  int iterMax;
  int iter;

  float diffu;
  float visco;
  float force;
  float input;
  
  float* u;
  float* v;
  float* u_prev;
  float* v_prev;
  float* dens;
  float* dens_prev;
  // std::vector<float> u;
  // std::vector<float> v;
  // std::vector<float> u_prev;
  // std::vector<float> v_prev;
  // std::vector<float> dens;
  // std::vector<float> dens_prev;

  std::vector<std::vector<std::vector<float>>> Press;
  std::vector<std::vector<std::vector<float>>> Solid;
  std::vector<std::vector<std::vector<Math::Vec3f>>> Veloc;
  std::vector<std::vector<std::vector<bool>>> BCond;

  public:
  bool isInitialized;
  bool isRefreshed;

  CompuFluidDyn();

  void Init();
  void Refresh();
  void Animate();
  void Draw();
};


/*
#pragma once

// Standard lib
#include <vector>

// Project lib
#include "../math/Vectors.hpp"


class CompuFluidDyn
{
  private:
  int nbX;
  int nbY;
  int nbZ;
  int simSteps;
  std::vector<std::vector<std::vector<float>>> velX;
  std::vector<std::vector<std::vector<float>>> velY;
  std::vector<std::vector<std::vector<float>>> velZ;
  std::vector<std::vector<std::vector<bool>>> lockX;
  std::vector<std::vector<std::vector<bool>>> lockY;
  std::vector<std::vector<std::vector<bool>>> lockZ;
  std::vector<std::vector<std::vector<float>>> pressure;
  std::vector<std::vector<std::vector<float>>> density;

  Math::Vec3f SampleStaggeredGrids(Math::Vec3f const& pos);

  public:
  bool isInitialized;
  bool isRefreshed;

  CompuFluidDyn();

  void Init();
  void Refresh();
  void Animate();
  void Draw();
};
*/
