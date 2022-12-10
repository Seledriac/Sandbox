#pragma once


// Standard lib
#include <vector>


namespace Math {

  template <class element_type>
  class Field2D
  {
public:
    int nbX, nbY;
    std::vector<std::vector<element_type>> multi_vector;

public:
    // Constructors
    Field2D() {
      nbX= 0;
      nbY= 0;
    }

    Field2D(int const nbX, int const nbY) {
      if (nbX < 0 || nbY < 0) throw;

      this->nbX= nbX;
      this->nbY= nbY;

      multi_vector= std::vector<std::vector<element_type>>(nbX, std::vector<element_type>(nbY));
    }

    Field2D(int const nbX, int const nbY, element_type const& val) {
      if (nbX < 0 || nbY < 0) throw;

      this->nbX= nbX;
      this->nbY= nbY;

      multi_vector= std::vector<std::vector<element_type>>(nbX, std::vector<element_type>(nbY, val));
    }

    Field2D(Field2D const& refField) {
      this->nbX= refField.nbX;
      this->nbY= refField.nbY;

      multi_vector= refField;
    }

    // Overloads of operator() to access data
    inline element_type& operator()(int const idxX, int const idxY) {
      // if (idxX < 0 || idxX >= nbX || idxY < 0 || idxY >= nbY) throw;
      return multi_vector[idxX][idxY];
    }
    inline const element_type& operator()(int const idxX, int const idxY) const {
      // if (idxX < 0 || idxX >= nbX || idxY < 0 || idxY >= nbY) throw;
      return multi_vector[idxX][idxY];
    }

    // Overload of operator=
    Field2D& operator=(Field2D const& refField) {
      // Check for self assignment
      if (this == &refField)
        return *this;

      this->nbX= refField.nbX;
      this->nbY= refField.nbY;

      multi_vector= refField.multi_vector;

      // Return *this for chained assignment
      return *this;
    }
  };


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
      if (nbX < 0 || nbY < 0 || nbZ < 0) throw;

      this->nbX= nbX;
      this->nbY= nbY;
      this->nbZ= nbZ;

      multi_vector= std::vector<std::vector<std::vector<element_type>>>(nbX, std::vector<std::vector<element_type>>(nbY, std::vector<element_type>(nbZ)));
    }

    Field3D(int const nbX, int const nbY, int const nbZ, element_type const& val) {
      if (nbX < 0 || nbY < 0 || nbZ < 0) throw;

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
      // if (idxX < 0 || idxX >= nbX || idxY < 0 || idxY >= nbY || idxZ < 0 || idxZ >= nbZ) throw;
      return multi_vector[idxX][idxY][idxZ];
    }
    inline const element_type& operator()(int const idxX, int const idxY, int const idxZ) const {
      // if (idxX < 0 || idxX >= nbX || idxY < 0 || idxY >= nbY || idxZ < 0 || idxZ >= nbZ) throw;
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


  template <class element_type>
  class Field4D
  {
public:
    int nbX, nbY, nbZ, nbW;
    std::vector<std::vector<std::vector<std::vector<element_type>>>> multi_vector;

public:
    // Constructors
    Field4D() {
      nbX= 0;
      nbY= 0;
      nbZ= 0;
      nbW= 0;
    }

    Field4D(int const nbX, int const nbY, int const nbZ, int const nbW) {
      if (nbX < 0 || nbY < 0 || nbZ < 0 || nbW < 0) throw;

      this->nbX= nbX;
      this->nbY= nbY;
      this->nbZ= nbZ;
      this->nbW= nbW;

      multi_vector= std::vector<std::vector<std::vector<std::vector<element_type>>>>(nbX, std::vector<std::vector<std::vector<element_type>>>(nbY, std::vector<std::vector<element_type>>(nbZ, std::vector<element_type>(nbW))));
    }

    Field4D(int const nbX, int const nbY, int const nbZ, int const nbW, element_type const& val) {
      if (nbX < 0 || nbY < 0 || nbZ < 0 || nbW < 0) throw;

      this->nbX= nbX;
      this->nbY= nbY;
      this->nbZ= nbZ;
      this->nbW= nbW;

      multi_vector= std::vector<std::vector<std::vector<std::vector<element_type>>>>(nbX, std::vector<std::vector<std::vector<element_type>>>(nbY, std::vector<std::vector<element_type>>(nbZ, std::vector<element_type>(nbW, val))));
    }

    Field4D(Field4D const& refField) {
      this->nbX= refField.nbX;
      this->nbY= refField.nbY;
      this->nbZ= refField.nbZ;
      this->nbW= refField.nbW;

      multi_vector= refField;
    }

    // Overloads of operator() to access data
    inline element_type& operator()(int const idxX, int const idxY, int const idxZ, int const idxW) {
      // if (idxX < 0 || idxX >= nbX || idxY < 0 || idxY >= nbY || idxZ < 0 || idxZ >= nbZ || idxW < 0 || idxW >= nbW) throw;
      return multi_vector[idxX][idxY][idxZ][idxW];
    }
    inline const element_type& operator()(int const idxX, int const idxY, int const idxZ, int const idxW) const {
      // if (idxX < 0 || idxX >= nbX || idxY < 0 || idxY >= nbY || idxZ < 0 || idxZ >= nbZ || idxW < 0 || idxW >= nbW) throw;
      return multi_vector[idxX][idxY][idxZ][idxW];
    }

    // Overload of operator=
    Field4D& operator=(Field4D const& refField) {
      // Check for self assignment
      if (this == &refField)
        return *this;

      this->nbX= refField.nbX;
      this->nbY= refField.nbY;
      this->nbZ= refField.nbZ;
      this->nbW= refField.nbW;

      multi_vector= refField.multi_vector;

      // Return *this for chained assignment
      return *this;
    }
  };

}  // namespace Math
