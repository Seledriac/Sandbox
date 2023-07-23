#pragma once

// Standard lib
#include <vector>

// Project lib
#include "../math/Vectors.hpp"


class ProjectTemplate
{
  public:
  bool isInitialized;
  bool isRefreshed;

  ProjectTemplate();

  void Init();
  void Refresh();
  void Animate();
  void Draw();
};
