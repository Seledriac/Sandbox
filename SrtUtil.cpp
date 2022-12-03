#include "SrtUtil.hpp"

// Standard lib
#include <algorithm>
#include <cmath>
#include <numeric>
#include <vector>

// Eigen lib
#include "Eigen/Dense"

void SrtUtil::DynamicRescaleVectorList(std::vector<Eigen::Vector3d> &ioList) {
  if (ioList.empty()) return;

  double norm= ioList[0].norm();
  double max= norm;
  for (int k= 0; k < (int)ioList.size(); k++) {
    norm= ioList[k].norm();
    if (max < norm) max= norm;
  }
  if (max > 0.0)
    for (int k= 0; k < (int)ioList.size(); k++)
      ioList[k]= ioList[k] / max;
}


void SrtUtil::DynamicRescaleVectorField(std::vector<std::vector<std::vector<Eigen::Vector3d>>> &ioField) {
  if (ioField.empty()) return;
  if (ioField[0].empty()) return;
  if (ioField[0][0].empty()) return;

  double max= ioField[0][0][0].norm();
  for (int k0= 0; k0 < (int)ioField.size(); k0++) {
    for (int k1= 0; k1 < (int)ioField[k0].size(); k1++) {
      for (int k2= 0; k2 < (int)ioField[k0][k1].size(); k2++) {
        double norm= ioField[k0][k1][k2].norm();
        if (max < norm) max= norm;
      }
    }
  }
  if (max > 0.0) {
    for (int k0= 0; k0 < (int)ioField.size(); k0++) {
      for (int k1= 0; k1 < (int)ioField[k0].size(); k1++) {
        for (int k2= 0; k2 < (int)ioField[k0][k1].size(); k2++) {
          ioField[k0][k1][k2]= ioField[k0][k1][k2] / max;
        }
      }
    }
  }
}


void SrtUtil::ComputeBasis(Eigen::Vector3d const &iVec, Eigen::Vector3d &oU, Eigen::Vector3d &oV) {
  oU= (iVec).cross(Eigen::Vector3d(1.0, 0.0, 1.0));
  if (oU.norm() < 1.e-6 * iVec.norm())
    oU= (iVec).cross(Eigen::Vector3d(0.0, 1.0, 0.0));
  oU.normalize();
  oV= (iVec).cross(oU);
  oV.normalize();
}


void SrtUtil::UniqueSortInsert(std::vector<int> &ioList, int const iVal) {
  bool wasAdded= false;
  for (int k= 0; k < (int)ioList.size(); k++) {
    if (ioList[k] < iVal)
      continue;
    else if (ioList[k] == iVal) {
      wasAdded= true;
      break;
    }
    else {
      ioList.insert(ioList.begin() + k, iVal);
      wasAdded= true;
      break;
    }
  }
  if (!wasAdded)
    ioList.push_back(iVal);
}


void SrtUtil::UniqueSortInsert(std::vector<std::pair<int, int>> &ioList, std::pair<int, int> const iVal) {
  bool wasAdded= false;
  for (int k= 0; k < (int)ioList.size(); k++) {
    if (ioList[k].first < iVal.first)
      continue;
    else if (ioList[k].first == iVal.first) {
      wasAdded= true;
      break;
    }
    else {
      ioList.insert(ioList.begin() + k, iVal);
      wasAdded= true;
      break;
    }
  }
  if (!wasAdded)
    ioList.push_back(iVal);
}


void SrtUtil::GetFieldDimensions(
    std::vector<std::vector<std::vector<int>>> const &iField,
    int &oNbX, int &oNbY, int &oNbZ) {
  oNbX= 0;
  oNbY= 0;
  oNbZ= 0;

  oNbX= int(iField.size());
  if (oNbX > 0) {
    oNbY= int(iField[0].size());
    if (oNbY > 0) {
      oNbZ= int(iField[0][0].size());
    }
  }
}


void SrtUtil::GetFieldDimensions(
    std::vector<std::vector<std::vector<double>>> const &iField,
    int &oNbX, int &oNbY, int &oNbZ) {
  oNbX= 0;
  oNbY= 0;
  oNbZ= 0;

  oNbX= int(iField.size());
  if (oNbX > 0) {
    oNbY= int(iField[0].size());
    if (oNbY > 0) {
      oNbZ= int(iField[0][0].size());
    }
  }
}


void SrtUtil::GetFieldDimensions(
    std::vector<std::vector<std::vector<std::array<double, 3>>>> const &iField,
    int &oNbX, int &oNbY, int &oNbZ) {
  oNbX= 0;
  oNbY= 0;
  oNbZ= 0;

  oNbX= int(iField.size());
  if (oNbX > 0) {
    oNbY= int(iField[0].size());
    if (oNbY > 0) {
      oNbZ= int(iField[0][0].size());
    }
  }
}


void SrtUtil::GetFieldDimensions(
    std::vector<std::vector<std::vector<Eigen::Vector3d>>> const &iField,
    int &oNbX, int &oNbY, int &oNbZ) {
  oNbX= 0;
  oNbY= 0;
  oNbZ= 0;

  oNbX= int(iField.size());
  if (oNbX > 0) {
    oNbY= int(iField[0].size());
    if (oNbY > 0) {
      oNbZ= int(iField[0][0].size());
    }
  }
}


void SrtUtil::GetFieldDimensions(
    std::vector<std::vector<std::vector<std::array<double, 9>>>> const &iField,
    int &oNbX, int &oNbY, int &oNbZ) {
  oNbX= 0;
  oNbY= 0;
  oNbZ= 0;

  oNbX= int(iField.size());
  if (oNbX > 0) {
    oNbY= int(iField[0].size());
    if (oNbY > 0) {
      oNbZ= int(iField[0][0].size());
    }
  }
}


void SrtUtil::GetFieldDimensions(
    std::vector<std::vector<std::vector<Eigen::Matrix3d>>> const &iField,
    int &oNbX, int &oNbY, int &oNbZ) {
  oNbX= 0;
  oNbY= 0;
  oNbZ= 0;

  oNbX= int(iField.size());
  if (oNbX > 0) {
    oNbY= int(iField[0].size());
    if (oNbY > 0) {
      oNbZ= int(iField[0][0].size());
    }
  }
}


void SrtUtil::GetVoxelSizes(
    int const iNbX,
    int const iNbY,
    int const iNbZ,
    std::array<double, 3> const &iBBoxMin,
    std::array<double, 3> const &iBBoxMax,
    bool const iCentered,
    double &oVoxSizeX,
    double &oVoxSizeY,
    double &oVoxSizeZ,
    double &oVoxSizeDiag) {
  if (iCentered) {
    oVoxSizeX= (iBBoxMax[0] - iBBoxMin[0]) / double(iNbX);
    oVoxSizeY= (iBBoxMax[1] - iBBoxMin[1]) / double(iNbY);
    oVoxSizeZ= (iBBoxMax[2] - iBBoxMin[2]) / double(iNbZ);
  }
  else {
    oVoxSizeX= (iBBoxMax[0] - iBBoxMin[0]) / double(iNbX - 1);
    oVoxSizeY= (iBBoxMax[1] - iBBoxMin[1]) / double(iNbY - 1);
    oVoxSizeZ= (iBBoxMax[2] - iBBoxMin[2]) / double(iNbZ - 1);
  }
  oVoxSizeDiag= std::sqrt(oVoxSizeX * oVoxSizeX + oVoxSizeY * oVoxSizeY + oVoxSizeZ * oVoxSizeZ);
}


void SrtUtil::GetVoxelSizes(
    int const iNbX,
    int const iNbY,
    int const iNbZ,
    std::array<double, 3> const &iBBoxMin,
    std::array<double, 3> const &iBBoxMax,
    bool const iCentered,
    double &oVoxSizeX,
    double &oVoxSizeY,
    double &oVoxSizeZ) {
  if (iCentered) {
    oVoxSizeX= (iBBoxMax[0] - iBBoxMin[0]) / double(iNbX);
    oVoxSizeY= (iBBoxMax[1] - iBBoxMin[1]) / double(iNbY);
    oVoxSizeZ= (iBBoxMax[2] - iBBoxMin[2]) / double(iNbZ);
  }
  else {
    oVoxSizeX= (iBBoxMax[0] - iBBoxMin[0]) / double(iNbX - 1);
    oVoxSizeY= (iBBoxMax[1] - iBBoxMin[1]) / double(iNbY - 1);
    oVoxSizeZ= (iBBoxMax[2] - iBBoxMin[2]) / double(iNbZ - 1);
  }
}


void SrtUtil::GetVoxelStart(
    std::array<double, 3> const &iBBoxMin,
    double const iVoxSizeX,
    double const iVoxSizeY,
    double const iVoxSizeZ,
    bool const iCentered,
    double &oStartX,
    double &oStartY,
    double &oStartZ) {
  if (iCentered) {
    oStartX= 0.5 * iVoxSizeX + iBBoxMin[0];
    oStartY= 0.5 * iVoxSizeY + iBBoxMin[1];
    oStartZ= 0.5 * iVoxSizeZ + iBBoxMin[2];
  }
  else {
    oStartX= iBBoxMin[0];
    oStartY= iBBoxMin[1];
    oStartZ= iBBoxMin[2];
  }
}


double SrtUtil::GetScalarFieldVal(
    std::vector<std::vector<std::vector<int>>> const &iField,
    Eigen::Vector3d const &iCoord,
    bool const iCentered,
    std::array<double, 3> const &iBBoxMin,
    std::array<double, 3> const &iBBoxMax) {
  int nbX, nbY, nbZ;
  SrtUtil::GetFieldDimensions(iField, nbX, nbY, nbZ);
  if (nbX == 0 || nbY == 0 || nbZ == 0) return 0;
  double stepX, stepY, stepZ, startX, startY, startZ;
  SrtUtil::GetVoxelSizes(nbX, nbY, nbZ, iBBoxMin, iBBoxMax, iCentered, stepX, stepY, stepZ);
  SrtUtil::GetVoxelStart(iBBoxMin, stepX, stepY, stepZ, iCentered, startX, startY, startZ);

  int x= std::min(std::max(int(std::round((iCoord[0] - startX) / stepX)), 0), nbX - 1);
  int y= std::min(std::max(int(std::round((iCoord[1] - startY) / stepY)), 0), nbY - 1);
  int z= std::min(std::max(int(std::round((iCoord[2] - startZ) / stepZ)), 0), nbZ - 1);

  return iField[x][y][z];
}


double SrtUtil::GetScalarFieldVal(
    std::vector<std::vector<std::vector<double>>> const &iField,
    Eigen::Vector3d const &iCoord,
    bool const iCentered,
    std::array<double, 3> const &iBBoxMin,
    std::array<double, 3> const &iBBoxMax) {
  int nbX, nbY, nbZ;
  SrtUtil::GetFieldDimensions(iField, nbX, nbY, nbZ);
  if (nbX == 0 || nbY == 0 || nbZ == 0) return 0.0;
  double stepX, stepY, stepZ, startX, startY, startZ;
  SrtUtil::GetVoxelSizes(nbX, nbY, nbZ, iBBoxMin, iBBoxMax, iCentered, stepX, stepY, stepZ);
  SrtUtil::GetVoxelStart(iBBoxMin, stepX, stepY, stepZ, iCentered, startX, startY, startZ);

  double xFloat= (iCoord[0] - startX) / stepX;
  double yFloat= (iCoord[1] - startY) / stepY;
  double zFloat= (iCoord[2] - startZ) / stepZ;

  int x0= std::min(std::max(int(std::floor(xFloat)), 0), nbX - 1);
  int y0= std::min(std::max(int(std::floor(yFloat)), 0), nbY - 1);
  int z0= std::min(std::max(int(std::floor(zFloat)), 0), nbZ - 1);
  int x1= std::min(std::max(int(std::floor(xFloat)) + 1, 0), nbX - 1);
  int y1= std::min(std::max(int(std::floor(yFloat)) + 1, 0), nbY - 1);
  int z1= std::min(std::max(int(std::floor(zFloat)) + 1, 0), nbZ - 1);

  double xWeight1= xFloat - double(x0);
  double yWeight1= yFloat - double(y0);
  double zWeight1= zFloat - double(z0);
  double xWeight0= 1.0 - xWeight1;
  double yWeight0= 1.0 - yWeight1;
  double zWeight0= 1.0 - zWeight1;

  double v000= iField[x0][y0][z0];
  double v001= iField[x0][y0][z1];
  double v010= iField[x0][y1][z0];
  double v011= iField[x0][y1][z1];
  double v100= iField[x1][y0][z0];
  double v101= iField[x1][y0][z1];
  double v110= iField[x1][y1][z0];
  double v111= iField[x1][y1][z1];

  double val= 0.0;
  val+= v000 * (xWeight0 * yWeight0 * zWeight0);
  val+= v001 * (xWeight0 * yWeight0 * zWeight1);
  val+= v010 * (xWeight0 * yWeight1 * zWeight0);
  val+= v011 * (xWeight0 * yWeight1 * zWeight1);
  val+= v100 * (xWeight1 * yWeight0 * zWeight0);
  val+= v101 * (xWeight1 * yWeight0 * zWeight1);
  val+= v110 * (xWeight1 * yWeight1 * zWeight0);
  val+= v111 * (xWeight1 * yWeight1 * zWeight1);

  return val;
}


Eigen::Vector3d SrtUtil::GetVectorFieldVal(
    std::vector<std::vector<std::vector<Eigen::Vector3d>>> const &iField,
    Eigen::Vector3d const &iCoord,
    bool const iCentered,
    std::array<double, 3> const &iBBoxMin,
    std::array<double, 3> const &iBBoxMax) {
  int nbX, nbY, nbZ;
  SrtUtil::GetFieldDimensions(iField, nbX, nbY, nbZ);
  if (nbX == 0 || nbY == 0 || nbZ == 0) return Eigen::Vector3d::Zero();
  double stepX, stepY, stepZ, startX, startY, startZ;
  SrtUtil::GetVoxelSizes(nbX, nbY, nbZ, iBBoxMin, iBBoxMax, iCentered, stepX, stepY, stepZ);
  SrtUtil::GetVoxelStart(iBBoxMin, stepX, stepY, stepZ, iCentered, startX, startY, startZ);

  double xFloat= (iCoord[0] - startX) / stepX;
  double yFloat= (iCoord[1] - startY) / stepY;
  double zFloat= (iCoord[2] - startZ) / stepZ;

  int x0= std::min(std::max(int(std::floor(xFloat)), 0), nbX - 1);
  int y0= std::min(std::max(int(std::floor(yFloat)), 0), nbY - 1);
  int z0= std::min(std::max(int(std::floor(zFloat)), 0), nbZ - 1);
  int x1= std::min(std::max(int(std::floor(xFloat)) + 1, 0), nbX - 1);
  int y1= std::min(std::max(int(std::floor(yFloat)) + 1, 0), nbY - 1);
  int z1= std::min(std::max(int(std::floor(zFloat)) + 1, 0), nbZ - 1);

  double xWeight1= xFloat - double(x0);
  double yWeight1= yFloat - double(y0);
  double zWeight1= zFloat - double(z0);
  double xWeight0= 1.0 - xWeight1;
  double yWeight0= 1.0 - yWeight1;
  double zWeight0= 1.0 - zWeight1;

  Eigen::Vector3d v000= iField[x0][y0][z0];
  Eigen::Vector3d v001= iField[x0][y0][z1];
  Eigen::Vector3d v010= iField[x0][y1][z0];
  Eigen::Vector3d v011= iField[x0][y1][z1];
  Eigen::Vector3d v100= iField[x1][y0][z0];
  Eigen::Vector3d v101= iField[x1][y0][z1];
  Eigen::Vector3d v110= iField[x1][y1][z0];
  Eigen::Vector3d v111= iField[x1][y1][z1];

  Eigen::Vector3d val(0.0, 0.0, 0.0);
  val+= v000 * (xWeight0 * yWeight0 * zWeight0);
  val+= v001 * (xWeight0 * yWeight0 * zWeight1);
  val+= v010 * (xWeight0 * yWeight1 * zWeight0);
  val+= v011 * (xWeight0 * yWeight1 * zWeight1);
  val+= v100 * (xWeight1 * yWeight0 * zWeight0);
  val+= v101 * (xWeight1 * yWeight0 * zWeight1);
  val+= v110 * (xWeight1 * yWeight1 * zWeight0);
  val+= v111 * (xWeight1 * yWeight1 * zWeight1);

  return val;
}
