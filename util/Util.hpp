#pragma once


// Standard lib
#include <array>
#include <vector>


namespace Util {

  // Allocation of fields
  template <class element_type>
  inline std::vector<std::vector<element_type>> AllocField2D(int const iNbA, int const iNbB, element_type const& val) {
    return std::vector<std::vector<element_type>>(iNbA, std::vector<element_type>(iNbB, val));
  }
  template <class element_type>
  inline std::vector<std::vector<std::vector<element_type>>> AllocField3D(int const iNbA, int const iNbB, int const iNbC, element_type const& val) {
    return std::vector<std::vector<std::vector<element_type>>>(iNbA, std::vector<std::vector<element_type>>(iNbB, std::vector<element_type>(iNbC, val)));
  }
  template <class element_type>
  inline std::vector<std::vector<std::vector<std::vector<element_type>>>> AllocField4D(int const iNbA, int const iNbB, int const iNbC, int const iNbD, element_type const& val) {
    return std::vector<std::vector<std::vector<std::vector<element_type>>>>(iNbA, std::vector<std::vector<std::vector<element_type>>>(iNbB, std::vector<std::vector<element_type>>(iNbC, std::vector<element_type>(iNbD, val))));
  }
  template <class element_type>
  inline std::vector<std::vector<std::vector<std::vector<std::vector<element_type>>>>> AllocField5D(int const iNbA, int const iNbB, int const iNbC, int const iNbD, int const iNbE, element_type const& val) {
    return std::vector<std::vector<std::vector<std::vector<std::vector<element_type>>>>>(iNbA, std::vector<std::vector<std::vector<std::vector<element_type>>>>(iNbB, std::vector<std::vector<std::vector<element_type>>>(iNbC, std::vector<std::vector<element_type>>(iNbD, std::vector<element_type>(iNbE, val)))));
  }

  // Get dimensions of fields
  template <class element_type>
  inline void GetFieldDimensions(std::vector<std::vector<element_type>> const& iField, int& oNbA, int& oNbB) {
    oNbA= 0;
    oNbB= 0;

    oNbA= int(iField.size());
    if (oNbA > 0) {
      oNbB= int(iField[0].size());
    }
  }
  template <class element_type>
  inline void GetFieldDimensions(std::vector<std::vector<std::vector<element_type>>> const& iField, int& oNbA, int& oNbB, int& oNbC) {
    oNbA= 0;
    oNbB= 0;
    oNbC= 0;

    oNbA= int(iField.size());
    if (oNbA > 0) {
      oNbB= int(iField[0].size());
      if (oNbB > 0) {
        oNbC= int(iField[0][0].size());
      }
    }
  }
  template <class element_type>
  inline void GetFieldDimensions(std::vector<std::vector<std::vector<std::vector<element_type>>>> const& iField, int& oNbA, int& oNbB, int& oNbC, int& oNbD) {
    oNbA= 0;
    oNbB= 0;
    oNbC= 0;
    oNbD= 0;

    oNbA= int(iField.size());
    if (oNbA > 0) {
      oNbB= int(iField[0].size());
      if (oNbB > 0) {
        oNbC= int(iField[0][0].size());
        if (oNbC > 0) {
          oNbD= int(iField[0][0][0].size());
        }
      }
    }
  }
  template <class element_type>
  inline void GetFieldDimensions(std::vector<std::vector<std::vector<std::vector<std::vector<element_type>>>>> const& iField, int& oNbA, int& oNbB, int& oNbC, int& oNbD, int& oNbE) {
    oNbA= 0;
    oNbB= 0;
    oNbC= 0;
    oNbD= 0;
    oNbE= 0;

    oNbA= int(iField.size());
    if (oNbA > 0) {
      oNbB= int(iField[0].size());
      if (oNbB > 0) {
        oNbC= int(iField[0][0].size());
        if (oNbC > 0) {
          oNbD= int(iField[0][0][0].size());
          if (oNbD > 0) {
            oNbE= int(iField[0][0][0][0].size());
          }
        }
      }
    }
  }

  // Check dimensions of fields
  template <class element_type>
  inline bool CheckFieldDimensions(std::vector<std::vector<element_type>> const& iField, int const& iNbA, int const& iNbB) {
    if (int(iField.size()) != iNbA) return false;
    if (iNbA > 0 && int(iField[0].size()) != iNbB) return false;
    return true;
  }
  template <class element_type>
  inline bool CheckFieldDimensions(std::vector<std::vector<std::vector<element_type>>> const& iField, int const& iNbA, int const& iNbB, int const& iNbC) {
    if (int(iField.size()) != iNbA) return false;
    if (iNbA > 0 && int(iField[0].size()) != iNbB) return false;
    if (iNbA > 0 && iNbB > 0 && int(iField[0][0].size()) != iNbC) return false;
    return true;
  }
  template <class element_type>
  inline bool CheckFieldDimensions(std::vector<std::vector<std::vector<std::vector<element_type>>>> const& iField, int const& iNbA, int const& iNbB, int const& iNbC, int const& iNbD) {
    if (int(iField.size()) != iNbA) return false;
    if (iNbA > 0 && int(iField[0].size()) != iNbB) return false;
    if (iNbA > 0 && iNbB > 0 && int(iField[0][0].size()) != iNbC) return false;
    if (iNbA > 0 && iNbB > 0 && iNbC > 0 && int(iField[0][0][0].size()) != iNbD) return false;
    return true;
  }
  template <class element_type>
  inline bool CheckFieldDimensions(std::vector<std::vector<std::vector<std::vector<std::vector<element_type>>>>> const& iField, int const& iNbA, int const& iNbB, int const& iNbC, int const& iNbD, int const& iNbE) {
    if (int(iField.size()) != iNbA) return false;
    if (iNbA > 0 && int(iField[0].size()) != iNbB) return false;
    if (iNbA > 0 && iNbB > 0 && int(iField[0][0].size()) != iNbC) return false;
    if (iNbA > 0 && iNbB > 0 && iNbC > 0 && int(iField[0][0][0].size()) != iNbD) return false;
    if (iNbA > 0 && iNbB > 0 && iNbC > 0 && iNbD > 0 && int(iField[0][0][0][0].size()) != iNbE) return false;
    return true;
  }

}  // namespace Util
