#pragma once


// Standard lib
#include <vector>


namespace math {

  template <class element_type>
  class Field3D
  {
public:
    int nbX, nbY, nbZ;
    std::vector<std::vector<std::vector<element_type>>> multi_vector;

public:
    // Constructors
    Field3D() {
      nbX= 0;
      nbY= 0;
      nbZ= 0;
    }

    Field3D(int const nbX, int const nbY, int const nbZ) {
      if (nbX < 0 || nbY < 0 || nbZ < 0)
        throw;

      this->nbX= nbX;
      this->nbY= nbY;
      this->nbZ= nbZ;

      multi_vector= std::vector<std::vector<std::vector<element_type>>>(nbX, std::vector<std::vector<element_type>>(nbY, std::vector<element_type>(nbZ)));
    }

    Field3D(int const nbX, int const nbY, int const nbZ, element_type const& val) {
      if (nbX < 0 || nbY < 0 || nbZ < 0)
        throw;

      this->nbX= nbX;
      this->nbY= nbY;
      this->nbZ= nbZ;

      multi_vector= std::vector<std::vector<std::vector<element_type>>>(nbX, std::vector<std::vector<element_type>>(nbY, std::vector<element_type>(nbZ, val)));
    }

    Field3D(Field3D const& refField) {
      this->nbX= refField.nbX;
      this->nbY= refField.nbY;
      this->nbZ= refField.nbZ;

      multi_vector= refField;
    }

    // Overloads of operator() to access data
    inline element_type& operator()(int const idxX, int const idxY, int const idxZ) {
      // if (idxX < 0 || idxX >= nbX || idxY < 0 || idxY >= nbY || idxZ < 0 || idxZ >= nbZ)
      //   throw;
      return multi_vector[idxX][idxY][idxZ];
    }
    inline const element_type& operator()(int const idxX, int const idxY, int const idxZ) const {
      // if (idxX < 0 || idxX >= nbX || idxY < 0 || idxY >= nbY || idxZ < 0 || idxZ >= nbZ)
      //   throw;
      return multi_vector[idxX][idxY][idxZ];
    }

    // Overload of operator=
    Field3D& operator=(Field3D const& refField) {
      // Check for self assignment
      if (this == &refField)
        return *this;

      this->nbX= refField.nbX;
      this->nbY= refField.nbY;
      this->nbZ= refField.nbZ;

      multi_vector= refField.multi_vector;

      // Return *this for chained assignment
      return *this;
    }
  };

}  // namespace math
