#pragma once

// Standard lib
#include <array>
#include <vector>

// Project lib
#include "Vec.hpp"


class MarchingCubes
{
  private:
  static void Interpolate(
      float const& iIsoval,
      float const& iEpsilon,
      Vec::Vec3f const& p1,
      Vec::Vec3f const& p2,
      float const& v1,
      float const& v2,
      Vec::Vec3f& oVert);

  public:
  static void ComputeMarchingCubes(
      std::vector<std::vector<std::vector<float>>> const& iField,
      float const iIsoval,
      std::vector<Vec::Vec3f>& oVertices,
      std::vector<std::array<int, 3>>& oTriangles);
};
