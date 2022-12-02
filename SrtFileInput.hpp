#pragma once

// Standard lib
#include <array>
#include <string>
#include <vector>

// Eigen lib
#include "Eigen/Core"

class SrtFileInput
{
  public:
  static void LoadScalarFieldBinaryFile(
      std::string const iFullpath,
      int const iNbX,
      int const iNbY,
      int const iNbZ,
      std::vector<std::vector<std::vector<double>>>& oField,
      bool const iVerbose);

  static void LoadVectorFieldBinaryFile(
      std::string const iFullpath,
      int const iNbX,
      int const iNbY,
      int const iNbZ,
      std::vector<std::vector<std::vector<Eigen::Vector3d>>>& oField,
      bool const iVerbose);

  static void LoadBoxTXTFile(
      std::string const iFullpath,
      std::array<double, 3>& oBBoxMin,
      std::array<double, 3>& oBBoxMax,
      bool const iVerbose);

  static void LoadScalarFieldTXTFile(
      std::string const iFullpath,
      std::vector<std::vector<std::vector<double>>>& oField,
      bool const iVerbose);

  static void LoadScalarFieldAsciiVTIFile(
      std::string const iFullpath,
      std::array<double, 3>& oBBoxMin,
      std::array<double, 3>& oBBoxMax,
      std::vector<std::vector<std::vector<double>>>& oField,
      bool const iVerbose);

  static void LoadScalarFieldRawVTIFile(
      std::string const iFullpath,
      std::array<double, 3>& oBBoxMin,
      std::array<double, 3>& oBBoxMax,
      std::vector<std::vector<std::vector<double>>>& oField,
      bool const iVerbose);

  static void LoadVectorFieldRawVTIFile(
      std::string const iFullpath,
      std::array<double, 3>& oBBoxMin,
      std::array<double, 3>& oBBoxMax,
      std::vector<std::vector<std::vector<Eigen::Vector3d>>>& oField,
      bool const iVerbose);

  static void LoadScalarFieldTXTFile(
      std::string const iFullpath,
      std::vector<std::vector<std::vector<int>>>& oField,
      bool const iVerbose);

  static void LoadScalarFieldImagePNGFile(
      std::string const iFullpath,
      std::vector<std::vector<std::vector<double>>>& oField,
      bool const iVerbose);

  static void LoadRGBAFieldImagePNGFile(
      std::string const iFullpath,
      std::vector<std::vector<std::vector<double>>>& oRField,
      std::vector<std::vector<std::vector<double>>>& oGField,
      std::vector<std::vector<std::vector<double>>>& oBField,
      std::vector<std::vector<std::vector<double>>>& oAField,
      bool const iVerbose);

  static void LoadVectorFieldTXTFile(
      std::string const iFullpath,
      std::vector<std::vector<std::vector<Eigen::Vector3d>>>& oField,
      bool const iVerbose);

  static void LoadVectorFieldTXTFile(
      std::string const iFullpath,
      std::vector<std::vector<std::vector<std::array<bool, 3>>>>& oField,
      bool const iVerbose);

  static void LoadVectorFieldTXTFile(
      std::string const iFullpath,
      std::vector<std::vector<std::vector<std::array<double, 3>>>>& oField,
      bool const iVerbose);

  static void LoadTensorFieldTXTFile(
      std::string const iFullpath,
      std::vector<std::vector<std::vector<std::array<double, 9>>>>& oField,
      bool const iVerbose);

  static void LoadTensorFieldTXTFile(
      std::string const iFullpath,
      std::vector<std::vector<std::vector<Eigen::Matrix3d>>>& oField,
      bool const iVerbose);

  static void LoadMeshOBJFile(
      std::string const iFullpath,
      std::vector<Eigen::Vector3d>& oPoints,
      std::vector<Eigen::Vector3d>& oColors,
      std::vector<Eigen::Vector3i>& oTriangles,
      bool const iVerbose);
};
