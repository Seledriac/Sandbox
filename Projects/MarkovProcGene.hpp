#pragma once

// Standard lib
#include <array>
#include <vector>

// Project lib
#include "../math/Vectors.hpp"


class MarkovProcGene
{
  private:
  int nbX;
  int nbY;
  int nbZ;

  int idxSubsti;
  int activeSeq;

  std::vector<std::vector<std::vector<int>>> Field;
  std::vector<std::vector<std::array<std::vector<std::vector<std::vector<int>>>, 2>>> Dict;

  public:
  bool isActiveProject;
  bool isInitialized;
  bool isRefreshed;

  MarkovProcGene();

  void SetActiveProject();
  void Initialize();
  void Refresh();
  void Animate();
  void Draw();
};
