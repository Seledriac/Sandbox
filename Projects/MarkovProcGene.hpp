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

  int activeSeq;
  int activeRul;

  std::vector<std::vector<std::vector<int>>> FieldOcclu;
  std::vector<std::vector<std::vector<int>>> Field;
  std::vector<std::vector<std::array<std::vector<std::vector<std::vector<int>>>, 2>>> Dict;

  std::array<std::vector<std::vector<std::vector<int>>>, 2> BuildColorSwap(
      const int iOldColor, const int iNewColor,
      const std::array<std::vector<std::vector<std::vector<int>>>, 2>& iRule);

  std::array<std::vector<std::vector<std::vector<int>>>, 2> BuildSymmetric(
      const int iDim1, const int iDim2, const int iDim3,
      const std::array<std::vector<std::vector<std::vector<int>>>, 2>& iRule);

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
