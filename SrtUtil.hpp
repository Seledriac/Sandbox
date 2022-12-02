#pragma once

// Standard lib
#include <array>
#include <vector>

// Eigen lib
#include "Eigen/Core"


class SrtUtil
{
  public:
  static void DynamicRescaleVectorList(std::vector<Eigen::Vector3d> &ioList);

  static void DynamicRescaleVectorField(std::vector<std::vector<std::vector<Eigen::Vector3d>>> &ioField);

  static void ComputeBasis(Eigen::Vector3d const &iVec, Eigen::Vector3d &oU, Eigen::Vector3d &oV);

  static void UniqueSortInsert(std::vector<int> &ioList, int const iVal);

  static void UniqueSortInsert(std::vector<std::pair<int, int>> &ioList, std::pair<int, int> const iVal);

  static void GetFieldDimensions(
      std::vector<std::vector<std::vector<int>>> const &iField,
      int &oNbX, int &oNbY, int &oNbZ);

  // Get the dimensions of the provided scalar field
  static void GetFieldDimensions(
      std::vector<std::vector<std::vector<double>>> const &iField,
      int &oNbX, int &oNbY, int &oNbZ);

  static void GetFieldDimensions(
      std::vector<std::vector<std::vector<std::array<double, 3>>>> const &iField,
      int &oNbX, int &oNbY, int &oNbZ);

  static void GetFieldDimensions(
      std::vector<std::vector<std::vector<Eigen::Vector3d>>> const &iField,
      int &oNbX, int &oNbY, int &oNbZ);

  static void GetFieldDimensions(
      std::vector<std::vector<std::vector<std::array<double, 9>>>> const &iField,
      int &oNbX, int &oNbY, int &oNbZ);

  static void GetFieldDimensions(
      std::vector<std::vector<std::vector<Eigen::Matrix3d>>> const &iField,
      int &oNbX, int &oNbY, int &oNbZ);

  static void GetVoxelSizes(
      int const iNbX,
      int const iNbY,
      int const iNbZ,
      std::array<double, 3> const &iBBoxMin,
      std::array<double, 3> const &iBBoxMax,
      bool const iCentered,
      double &oVoxSizeX,
      double &oVoxSizeY,
      double &oVoxSizeZ,
      double &oVoxSizeDiag);

  // Get the voxel sizes of the provided scalar field based on the bounding box
  static void GetVoxelSizes(
      int const iNbX,
      int const iNbY,
      int const iNbZ,
      std::array<double, 3> const &iBBoxMin,
      std::array<double, 3> const &iBBoxMax,
      bool const iCentered,
      double &oVoxSizeX,
      double &oVoxSizeY,
      double &oVoxSizeZ);

  // Get the voxel starting position of the provided scalar field based on the bounding box
  static void GetVoxelStart(
      std::array<double, 3> const &iBBoxMin,
      std::array<double, 3> const &iBBoxMax,
      double const iVoxSizeX,
      double const iVoxSizeY,
      double const iVoxSizeZ,
      bool const iCentered,
      double &oStartX,
      double &oStartY,
      double &oStartZ);

  static double GetScalarFieldVal(
      std::vector<std::vector<std::vector<int>>> const &iField,
      Eigen::Vector3d const &iCoord,
      bool const iCentered,
      std::array<double, 3> const &iBBoxMin= {0.0, 0.0, 0.0},
      std::array<double, 3> const &iBBoxMax= {1.0, 1.0, 1.0});

  // Trilinear interpolation of the scalar value at the provided position
  static double GetScalarFieldVal(
      std::vector<std::vector<std::vector<double>>> const &iField,
      Eigen::Vector3d const &iCoord,
      bool const iCentered,
      std::array<double, 3> const &iBBoxMin= {0.0, 0.0, 0.0},
      std::array<double, 3> const &iBBoxMax= {1.0, 1.0, 1.0});

  static Eigen::Vector3d GetVectorFieldVal(
      std::vector<std::vector<std::vector<Eigen::Vector3d>>> const &iField,
      Eigen::Vector3d const &iCoord,
      bool const iCentered,
      std::array<double, 3> const &iBBoxMin= {0.0, 0.0, 0.0},
      std::array<double, 3> const &iBBoxMax= {1.0, 1.0, 1.0});

  // Interpolation of the tensor value at the provided position by considering
  // eigenvectors (rotations) and eigenvalues (scalings) separately
  static Eigen::Matrix3d GetTensorFieldVal(
      std::vector<std::vector<std::vector<Eigen::Matrix3d>>> const &iField,
      Eigen::Vector3d const &iCoord,
      bool const iCentered,
      std::array<double, 3> const &iBBoxMin= {0.0, 0.0, 0.0},
      std::array<double, 3> const &iBBoxMax= {1.0, 1.0, 1.0});
};
