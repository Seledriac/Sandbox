#pragma once

// Standard lib
#include <array>
#include <vector>


class CompuFluidDyna
{
  private:
  int nbX;
  int nbY;
  int nbZ;

  // Handles boundary conditions on velocity field
  // Bound > 0  Block velocities
  // Bound == 0 Simulate veolcities
  // Bound < 0  Freely absorb velocities
  std::vector<std::vector<std::vector<int>>> Bound;

  // Flag if a voxel has a forced velocity, negative values flip the sign of velocity
  std::vector<std::vector<std::vector<int>>> Force;

  // Flag if a voxel has a forced density, negative values flip the sign of density
  std::vector<std::vector<std::vector<int>>> Sourc;

  std::vector<std::vector<std::vector<float>>> SmokCur;
  std::vector<std::vector<std::vector<float>>> VelXCur;
  std::vector<std::vector<std::vector<float>>> VelYCur;
  std::vector<std::vector<std::vector<float>>> VelZCur;

  std::vector<std::vector<std::array<float, 3>>> loadedImage;


  void ApplyBC(const int iFieldID, std::vector<std::vector<std::vector<float>>>& ioField);
  void GaussSeidelSolve(const int iFieldID, const int iIter, const float iTimeStep,
                        const bool iDiffuMode, const float iDiffuCoeff,
                        std::vector<std::vector<std::vector<float>>>& ioField);
  float TrilinearInterpolation(const float iPosX, const float iPosY, const float iPosZ,
                               const std::vector<std::vector<std::vector<float>>>& iFieldRef);
  void AdvectField(const int iFieldID, const float iTimeStep,
                   const std::vector<std::vector<std::vector<float>>>& iVelX,
                   const std::vector<std::vector<std::vector<float>>>& iVelY,
                   const std::vector<std::vector<std::vector<float>>>& iVelZ,
                   std::vector<std::vector<std::vector<float>>>& ioField);
  void ProjectField(const int iIter, const float iTimeStep,
                    std::vector<std::vector<std::vector<float>>>& ioVelX,
                    std::vector<std::vector<std::vector<float>>>& ioVelY,
                    std::vector<std::vector<std::vector<float>>>& ioVelZ);

  public:
  bool isActiveProject;
  bool isInitialized;
  bool isRefreshed;

  CompuFluidDyna();

  void SetActiveProject();
  void Initialize();
  void Refresh();
  void Animate();
  void Draw();
};
