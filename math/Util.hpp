#pragma once


// Standard lib
#include <array>
#include <vector>


namespace Util {

  // Allocation of fields
  template <class element_type>
  inline std::vector<std::vector<element_type>> AllocField2D(int const nbX, int const nbY, element_type const& val) {
    return std::vector<std::vector<element_type>>(nbX, std::vector<element_type>(nbY, val));
  }
  template <class element_type>
  inline std::vector<std::vector<std::vector<element_type>>> AllocField3D(int const nbX, int const nbY, int const nbZ, element_type const& val) {
    return std::vector<std::vector<std::vector<element_type>>>(nbX, std::vector<std::vector<element_type>>(nbY, std::vector<element_type>(nbZ, val)));
  }
  template <class element_type>
  inline std::vector<std::vector<std::vector<std::vector<element_type>>>> AllocField4D(int const nbX, int const nbY, int const nbZ, int const nbW, element_type const& val) {
    return std::vector<std::vector<std::vector<std::vector<element_type>>>>(nbX, std::vector<std::vector<std::vector<element_type>>>(nbY, std::vector<std::vector<element_type>>(nbZ, std::vector<element_type>(nbW, val))));
  }

}  // namespace Util
