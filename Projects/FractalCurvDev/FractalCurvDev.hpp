#pragma once

// Standard lib
#include <array>
#include <vector>

// Sandbox lib
#include "../../Util/Vec.hpp"


// Generate 3D surfaces from fractal curves
// - Extrude fractals in the out-of-plane rirection to create surfaces
// - Progressively increase fractal depth along the extrusion direction
// - Written to generate 3D printable math shapes
//
// Example for reference
// https://naml.us/paper/fractal/
// https://naml.us/paper/irving_segerman2012_fractal.pdf
class FractalCurvDev
{
  private:
  std::vector<std::vector<Vec::Vec3f>> Nodes;
  std::vector<std::array<Vec::Vec3f, 3>> Faces;

  public:
  bool isActivProj;
  bool isAllocated;
  bool isRefreshed;

  FractalCurvDev();

  void SetActiveProject();
  bool CheckAlloc();
  bool CheckRefresh();
  void Allocate();
  void KeyPress(const unsigned char key);
  void Refresh();
  void Animate();
  void Draw();
};
