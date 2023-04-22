#pragma once


// Standard lib
#include <array>
#include <vector>


namespace Util {

  // Allocation of fields
  template <class element_type>
  inline std::vector<std::vector<element_type>> AllocField2D(int const iNbX, int const iNbY, element_type const& val) {
    return std::vector<std::vector<element_type>>(iNbX, std::vector<element_type>(iNbY, val));
  }
  template <class element_type>
  inline std::vector<std::vector<std::vector<element_type>>> AllocField3D(int const iNbX, int const iNbY, int const iNbZ, element_type const& val) {
    return std::vector<std::vector<std::vector<element_type>>>(iNbX, std::vector<std::vector<element_type>>(iNbY, std::vector<element_type>(iNbZ, val)));
  }
  template <class element_type>
  inline std::vector<std::vector<std::vector<std::vector<element_type>>>> AllocField4D(int const iNbX, int const iNbY, int const iNbZ, int const iNbW, element_type const& val) {
    return std::vector<std::vector<std::vector<std::vector<element_type>>>>(iNbX, std::vector<std::vector<std::vector<element_type>>>(iNbY, std::vector<std::vector<element_type>>(iNbZ, std::vector<element_type>(iNbW, val))));
  }

  // Get dimensions of fields
  template <class element_type>
  inline void GetFieldDimensions(std::vector<std::vector<element_type>> const& iField, int& oNbX, int& oNbY) {
    oNbX= 0;
    oNbY= 0;

    oNbX= int(iField.size());
    if (oNbX > 0) {
      oNbY= int(iField[0].size());
    }
  }
  template <class element_type>
  inline void GetFieldDimensions(std::vector<std::vector<std::vector<element_type>>> const& iField, int& oNbX, int& oNbY, int& oNbZ) {
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
  template <class element_type>
  inline void GetFieldDimensions(std::vector<std::vector<std::vector<std::vector<element_type>>>> const& iField, int& oNbX, int& oNbY, int& oNbZ, int& oNbW) {
    oNbX= 0;
    oNbY= 0;
    oNbZ= 0;
    oNbW= 0;

    oNbX= int(iField.size());
    if (oNbX > 0) {
      oNbY= int(iField[0].size());
      if (oNbY > 0) {
        oNbZ= int(iField[0][0].size());
        if (oNbZ > 0) {
          oNbW= int(iField[0][0][0].size());
        }
      }
    }
  }


  // Check dimensions of fields
  template <class element_type>
  inline bool CheckFieldDimensions(std::vector<std::vector<element_type>> const& iField, int const& iNbX, int const& iNbY) {
    if (int(iField.size()) != iNbX) return false;
    if (iNbX > 0 && int(iField[0].size()) != iNbY) return false;
    if (iNbX > 0 && iNbY > 0 && int(iField[0].size()) != iNbY) return false;
    return true;
  }
  template <class element_type>
  inline bool CheckFieldDimensions(std::vector<std::vector<std::vector<element_type>>> const& iField, int const& iNbX, int const& iNbY, int const& iNbZ) {
    if (int(iField.size()) != iNbX) return false;
    if (iNbX > 0 && int(iField[0].size()) != iNbY) return false;
    if (iNbX > 0 && iNbY > 0 && int(iField[0].size()) != iNbY) return false;
    if (iNbX > 0 && iNbY > 0 && iNbZ > 0 && int(iField[0][0].size()) != iNbZ) return false;
    return true;
  }
  template <class element_type>
  inline bool CheckFieldDimensions(std::vector<std::vector<std::vector<std::vector<element_type>>>> const& iField, int const& iNbX, int const& iNbY, int const& iNbZ, int const& iNbW) {
    if (int(iField.size()) != iNbX) return false;
    if (iNbX > 0 && int(iField[0].size()) != iNbY) return false;
    if (iNbX > 0 && iNbY > 0 && int(iField[0].size()) != iNbY) return false;
    if (iNbX > 0 && iNbY > 0 && iNbZ > 0 && int(iField[0][0].size()) != iNbZ) return false;
    if (iNbX > 0 && iNbY > 0 && iNbZ > 0 && iNbW > 0 && int(iField[0][0][0].size()) != iNbW) return false;
    return true;
  }

}  // namespace Util
