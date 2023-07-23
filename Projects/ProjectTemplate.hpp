#pragma once

// Standard lib
#include <vector>

// Project lib
#include "../math/Vectors.hpp"


class ProjectTemplate
{
  private:
  int nbX;
  
  public:
  bool isInitialized;
  bool isRefreshed;

  ProjectTemplate();

  void Init();
  void Refresh();
  void Animate();
  void Draw();
};
