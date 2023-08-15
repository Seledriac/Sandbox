#pragma once

// Standard lib
#include <array>
#include <vector>


class MarkovProcGene
{
  private:
  int nbX;
  int nbY;
  int nbZ;

  int activeSet;
  int activeRul;

  std::vector<std::vector<std::vector<int>>> Field;
  std::vector<std::vector<std::array<std::vector<std::vector<std::vector<int>>>, 2>>> Dict;

  void FillRuleBox(
      std::array<std::vector<std::vector<std::vector<int>>>, 2>& ioRule,
      const int iMinX, const int iMinY, const int iMinZ,
      const int iMaxX, const int iMaxY, const int iMaxZ,
      const int iVal, const bool iFillI, const bool iFillO);

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
