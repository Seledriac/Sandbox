#pragma once


// Standard lib
#include <cstring>
#include <cstdio>
#include <algorithm>


// Choose 0 or 1 to disable or enable bound checking in the () operator
#define FIELDS_ARRAY_BOUNDS_CHECK 1


namespace math {

  // Sample code
  //
  // math::Field2D<double> densityField(nbX, nbY);
  // for (int x= 0; x < nbX; x++) {
  //   for (int y= 0; y < nbY; y++) {
  //     densityField(x, y)= 0.5 + 0.5*std::sin(3.0*double(y)/double(nbY));
  //   }
  // }
  //
  template <class array_element_type>
  class Field2D
  {
private:
    int sizeX, sizeY, sizeTotal;
    int C1;
    array_element_type* internal_array;

public:
    // Get raw data
    inline array_element_type* data(void) const { return internal_array; }

    // Get dimensions
    inline int nbX(void) const { return sizeX; }
    inline int nbY(void) const { return sizeY; }
    inline int nbTotal(void) const { return sizeTotal; }

    // Constructors
    Field2D() {
      sizeX= 0;
      sizeY= 0;
      sizeTotal= 0;

      C1= 0;

      internal_array= nullptr;
    }
    Field2D(int const nbX, int const nbY) {
      if ((nbX < 0) || (nbY < 0)) {
        printf("[Error] Invalid sizes for initialized field (%d;%d)\n", nbX, nbY);
        throw;
      }

      sizeX= nbX;
      sizeY= nbY;
      sizeTotal= sizeX * sizeY;

      C1= sizeY;

      internal_array= nullptr;
      if (sizeTotal > 0)
        internal_array= new array_element_type[sizeTotal];
    }
    Field2D(int const nbX, int const nbY, array_element_type const& val) {
      if ((nbX < 0) || (nbY < 0)) {
        printf("[Error] Invalid sizes for initialized field (%d;%d)\n", nbX, nbY);
        throw;
      }

      sizeX= nbX;
      sizeY= nbY;
      sizeTotal= sizeX * sizeY;

      C1= sizeY;

      internal_array= nullptr;
      if (sizeTotal > 0) {
        internal_array= new array_element_type[sizeTotal];
        std::fill_n(internal_array, sizeTotal, val);
      }
    }
    Field2D(Field2D const& refField) {
      sizeX= refField.nbX();
      sizeY= refField.nbY();
      sizeTotal= refField.nbTotal();

      C1= sizeY;

      internal_array= nullptr;
      if (sizeTotal > 0) {
        internal_array= new array_element_type[sizeTotal];
        memcpy(internal_array, refField.internal_array, sizeTotal * sizeof(array_element_type));
      }
    }

    // Destructor
    ~Field2D() { delete[] internal_array; }

    // Overloads of operator() to access data
    inline array_element_type& operator()(int const idxX, int const idxY) {
#if FIELDS_ARRAY_BOUNDS_CHECK == 1
      if ((idxX < 0) || (idxX >= sizeX) || (idxY < 0) || (idxY >= sizeY)) {
        printf("[Error] Index out of bounds while accessing (%d;%d) in %dx%d array\n", idxX, idxY, sizeX, sizeY);
        throw;
      }
#endif
      return internal_array[idxX * C1 + idxY];
    }
    inline const array_element_type& operator()(int const idxX, int const idxY) const {
#if FIELDS_ARRAY_BOUNDS_CHECK == 1
      if ((idxX < 0) || (idxX >= sizeX) || (idxY < 0) || (idxY >= sizeY)) {
        printf("[Error] Index out of bounds while accessing (%d;%d) in %dx%d array\n", idxX, idxY, sizeX, sizeY);
        throw;
      }
#endif
      return internal_array[idxX * C1 + idxY];
    }
    inline array_element_type& operator()(int const idx) {
#if FIELDS_ARRAY_BOUNDS_CHECK == 1
      if ((idx < 0) || (idx >= sizeTotal)) {
        printf("[Error] Index out of bounds while accessing %d in %dx%d = %d array\n", idx, sizeX, sizeY, sizeTotal);
        throw;
      }
#endif
      return internal_array[idx];
    }
    inline const array_element_type& operator()(int const idx) const {
#if FIELDS_ARRAY_BOUNDS_CHECK == 1
      if ((idx < 0) || (idx >= sizeTotal)) {
        printf("[Error] Index out of bounds while accessing %d in %dx%d = %d array\n", idx, sizeX, sizeY, sizeTotal);
        throw;
      }
#endif
      return internal_array[idx];
    }

    // Overload of operator=
    Field2D& operator=(Field2D const& refField) {
      // Check for self assignment
      if (this == &refField)
        return *this;

      // Reallocate if necessary
      if (sizeTotal != refField.sizeTotal) {
        delete[] internal_array;
        internal_array= new array_element_type[refField.sizeTotal];
      }

      // Set dimension variables
      sizeX= refField.sizeX;
      sizeY= refField.sizeY;
      sizeTotal= refField.sizeTotal;

      C1= sizeY;

      // Copy data
      memcpy(internal_array, refField.internal_array, sizeTotal * sizeof(array_element_type));

      // for (int k= 0; k < sizeTotal; k++)
      //   internal_array[k]= refField.internal_array[k];

      // Return *this for chained assignment
      return *this;
    }
  };


  // Sample code
  //
  // math::Field3D<double> densityField(nbX, nbY, nbZ);
  // for (int x= 0; x < nbX; x++) {
  //   for (int y= 0; y < nbY; y++) {
  //     for (int z= 0; z < nbZ; z++) {
  //       densityField(x, y, z)= 0.5 + 0.5*std::sin(3.0*double(z)/double(nbZ));
  //     }
  //   }
  // }
  //
  template <class array_element_type>
  class Field3D
  {
private:
    int sizeX, sizeY, sizeZ, sizeTotal;
    int C1, C2;
    array_element_type* internal_array;

public:
    // Get raw data
    inline array_element_type* data(void) const { return internal_array; }

    // Get dimensions
    inline int nbX(void) const { return sizeX; }
    inline int nbY(void) const { return sizeY; }
    inline int nbZ(void) const { return sizeZ; }
    inline int nbTotal(void) const { return sizeTotal; }

    // Constructors
    Field3D() {
      sizeX= 0;
      sizeY= 0;
      sizeZ= 0;
      sizeTotal= 0;

      C1= 0;
      C2= 0;

      internal_array= nullptr;
    }
    Field3D(int const nbX, int const nbY, int const nbZ) {
      if ((nbX < 0) || (nbY < 0) || (nbZ < 0)) {
        printf("[Error] Invalid sizes for initialized field (%d;%d;%d)\n", nbX, nbY, nbZ);
        throw;
      }

      sizeX= nbX;
      sizeY= nbY;
      sizeZ= nbZ;
      sizeTotal= sizeX * sizeY * sizeZ;

      C1= sizeY * sizeZ;
      C2= sizeZ;

      internal_array= nullptr;
      if (sizeTotal > 0)
        internal_array= new array_element_type[sizeTotal];
    }
    Field3D(int const nbX, int const nbY, int const nbZ, array_element_type const& val) {
      if ((nbX < 0) || (nbY < 0) || (nbZ < 0)) {
        printf("[Error] Invalid sizes for initialized field (%d;%d;%d)\n", nbX, nbY, nbZ);
        throw;
      }

      sizeX= nbX;
      sizeY= nbY;
      sizeZ= nbZ;
      sizeTotal= sizeX * sizeY * sizeZ;

      C1= sizeY * sizeZ;
      C2= sizeZ;

      internal_array= nullptr;
      if (sizeTotal > 0) {
        internal_array= new array_element_type[sizeTotal];
        std::fill_n(internal_array, sizeTotal, val);
      }
    }
    Field3D(Field3D const& refField) {
      sizeX= refField.nbX();
      sizeY= refField.nbY();
      sizeZ= refField.nbZ();
      sizeTotal= refField.nbTotal();

      C1= sizeY * sizeZ;
      C2= sizeZ;

      internal_array= nullptr;
      if (sizeTotal > 0) {
        internal_array= new array_element_type[sizeTotal];
        memcpy(internal_array, refField.internal_array, sizeTotal * sizeof(array_element_type));
      }
    }

    // Destructor
    ~Field3D() { delete[] internal_array; }

    // Overloads of operator() to access data
    inline array_element_type& operator()(int const idxX, int const idxY, int const idxZ) {
#if FIELDS_ARRAY_BOUNDS_CHECK == 1
      if ((idxX < 0) || (idxX >= sizeX) || (idxY < 0) || (idxY >= sizeY) || (idxZ < 0) || (idxZ >= sizeZ)) {
        printf("[Error] Index out of bounds while accessing (%d;%d;%d) in %dx%dx%d array\n", idxX, idxY, idxZ, sizeX, sizeY, sizeZ);
        throw;
      }
#endif
      return internal_array[idxX * C1 + idxY * C2 + idxZ];
    }
    inline const array_element_type& operator()(int const idxX, int const idxY, int const idxZ) const {
#if FIELDS_ARRAY_BOUNDS_CHECK == 1
      if ((idxX < 0) || (idxX >= sizeX) || (idxY < 0) || (idxY >= sizeY) || (idxZ < 0) || (idxZ >= sizeZ)) {
        printf("[Error] Index out of bounds while accessing (%d;%d;%d) in %dx%dx%d array\n", idxX, idxY, idxZ, sizeX, sizeY, sizeZ);
        throw;
      }
#endif
      return internal_array[idxX * C1 + idxY * C2 + idxZ];
    }
    inline array_element_type& operator()(int const idx) {
#if FIELDS_ARRAY_BOUNDS_CHECK == 1
      if ((idx < 0) || (idx >= sizeTotal)) {
        printf("[Error] Index out of bounds while accessing %d in %dx%dx%d = %d array\n", idx, sizeX, sizeY, sizeZ, sizeTotal);
        throw;
      }
#endif
      return internal_array[idx];
    }
    inline const array_element_type& operator()(int const idx) const {
#if FIELDS_ARRAY_BOUNDS_CHECK == 1
      if ((idx < 0) || (idx >= sizeTotal)) {
        printf("[Error] Index out of bounds while accessing %d in %dx%dx%d = %d array\n", idx, sizeX, sizeY, sizeZ, sizeTotal);
        throw;
      }
#endif
      return internal_array[idx];
    }

    // Overload of operator=
    Field3D& operator=(Field3D const& refField) {
      // Check for self assignment
      if (this == &refField)
        return *this;

      // Reallocate if necessary
      if (sizeTotal != refField.sizeTotal) {
        delete[] internal_array;
        internal_array= new array_element_type[refField.sizeTotal];
      }

      // Set dimension variables
      sizeX= refField.sizeX;
      sizeY= refField.sizeY;
      sizeZ= refField.sizeZ;
      sizeTotal= refField.sizeTotal;

      C1= sizeY * sizeZ;
      C2= sizeZ;

      // Copy data
      memcpy(internal_array, refField.internal_array, sizeTotal * sizeof(array_element_type));

      // for (int k= 0; k < sizeTotal; k++)
      //   internal_array[k]= refField.internal_array[k];

      // Return *this for chained assignment
      return *this;
    }
  };


  // Sample code
  //
  // math::Field4D<double> densityField(nbX, nbY, nbZ);
  // for (int t= 0; t < nbT; t++) {
  //   for (int x= 0; x < nbX; x++) {
  //     for (int y= 0; y < nbY; y++) {
  //       for (int z= 0; z < nbZ; z++) {
  //         densityField(t, x, y, z)= 0.5 + 0.5*std::sin(3.0*double(z)/double(nbZ));
  //       }
  //     }
  //   }
  // }
  //
  template <class array_element_type>
  class Field4D
  {
private:
    int sizeT, sizeX, sizeY, sizeZ, sizeTotal;
    int C1, C2, C3;
    array_element_type* internal_array;

public:
    // Get raw data
    inline array_element_type* data(void) const { return internal_array; }

    // Get dimensions
    inline int nbT(void) const { return sizeT; }
    inline int nbX(void) const { return sizeX; }
    inline int nbY(void) const { return sizeY; }
    inline int nbZ(void) const { return sizeZ; }
    inline int nbTotal(void) const { return sizeTotal; }

    // Constructors
    Field4D() {
      sizeT= 0;
      sizeX= 0;
      sizeY= 0;
      sizeZ= 0;
      sizeTotal= 0;

      C1= 0;
      C2= 0;
      C3= 0;

      internal_array= nullptr;
    }
    Field4D(int const nbT, int const nbX, int const nbY, int const nbZ) {
      if ((nbT < 0) || (nbX < 0) || (nbY < 0) || (nbZ < 0)) {
        printf("[Error] Invalid sizes for initialized field (%d;%d;%d;%d)\n", nbT, nbX, nbY, nbZ);
        throw;
      }

      sizeT= nbT;
      sizeX= nbX;
      sizeY= nbY;
      sizeZ= nbZ;
      sizeTotal= sizeT * sizeX * sizeY * sizeZ;

      C1= sizeX * sizeY * sizeZ;
      C2= sizeY * sizeZ;
      C3= sizeZ;

      internal_array= nullptr;
      if (sizeTotal > 0)
        internal_array= new array_element_type[sizeTotal];
    }
    Field4D(int const nbT, int const nbX, int const nbY, int const nbZ, array_element_type const& val) {
      if ((nbT < 0) || (nbX < 0) || (nbY < 0) || (nbZ < 0)) {
        printf("[Error] Invalid sizes for initialized field (%d;%d;%d;%d)\n", nbT, nbX, nbY, nbZ);
        throw;
      }

      sizeT= nbT;
      sizeX= nbX;
      sizeY= nbY;
      sizeZ= nbZ;
      sizeTotal= sizeT * sizeX * sizeY * sizeZ;

      C1= sizeX * sizeY * sizeZ;
      C2= sizeY * sizeZ;
      C3= sizeZ;

      internal_array= nullptr;
      if (sizeTotal > 0) {
        internal_array= new array_element_type[sizeTotal];
        std::fill_n(internal_array, sizeTotal, val);
      }
    }
    Field4D(Field4D const& refField) {
      sizeT= refField.nbT();
      sizeX= refField.nbX();
      sizeY= refField.nbY();
      sizeZ= refField.nbZ();
      sizeTotal= refField.nbTotal();

      C1= sizeX * sizeY * sizeZ;
      C2= sizeY * sizeZ;
      C3= sizeZ;

      internal_array= nullptr;
      if (sizeTotal > 0) {
        internal_array= new array_element_type[sizeTotal];
        memcpy(internal_array, refField.internal_array, sizeTotal * sizeof(array_element_type));
      }
    }

    // Destructor
    ~Field4D() { delete[] internal_array; }

    // Overloads of operator() to access data
    inline array_element_type& operator()(int const idxT, int const idxX, int const idxY, int const idxZ) {
#if FIELDS_ARRAY_BOUNDS_CHECK == 1
      if ((idxT < 0) || (idxT >= sizeT) || (idxT < 0) || (idxX >= sizeX) || (idxY < 0) || (idxY >= sizeY) || (idxZ < 0) || (idxZ >= sizeZ)) {
        printf("[Error] Index out of bounds while accessing (%d;%d;%d;%d) in %dx%dx%dx%d array\n", idxT, idxX, idxY, idxZ, sizeT, sizeX, sizeY, sizeZ);
        throw;
      }
#endif
      return internal_array[idxT * C1 + idxX * C2 + idxY * C3 + idxZ];
    }
    inline const array_element_type& operator()(int const idxT, int const idxX, int const idxY, int const idxZ) const {
#if FIELDS_ARRAY_BOUNDS_CHECK == 1
      if ((idxT < 0) || (idxT >= sizeT) || (idxT < 0) || (idxX >= sizeX) || (idxY < 0) || (idxY >= sizeY) || (idxZ < 0) || (idxZ >= sizeZ)) {
        printf("[Error] Index out of bounds while accessing (%d;%d;%d;%d) in %dx%dx%dx%d array\n", idxT, idxX, idxY, idxZ, sizeT, sizeX, sizeY, sizeZ);
        throw;
      }
#endif
      return internal_array[idxT * C1 + idxX * C2 + idxY * C3 + idxZ];
    }
    inline array_element_type& operator()(int const idx) {
#if FIELDS_ARRAY_BOUNDS_CHECK == 1
      if ((idx < 0) || (idx >= sizeTotal)) {
        printf("[Error] Index out of bounds while accessing %d in %dx%dx%dx%d = %d array\n", idx, sizeT, sizeX, sizeY, sizeZ, sizeTotal);
        throw;
      }
#endif
      return internal_array[idx];
    }
    inline const array_element_type& operator()(int const idx) const {
#if FIELDS_ARRAY_BOUNDS_CHECK == 1
      if ((idx < 0) || (idx >= sizeTotal)) {
        printf("[Error] Index out of bounds while accessing %d in %dx%dx%dx%d = %d array\n", idx, sizeT, sizeX, sizeY, sizeZ, sizeTotal);
        throw;
      }
#endif
      return internal_array[idx];
    }

    // Overload of operator=
    Field4D& operator=(Field4D const& refField) {
      // Check for self assignment
      if (this == &refField)
        return *this;

      // Reallocate if necessary
      if (sizeTotal != refField.sizeTotal) {
        delete[] internal_array;
        internal_array= new array_element_type[refField.sizeTotal];
      }

      // Set dimension variables
      sizeT= refField.sizeT;
      sizeX= refField.sizeX;
      sizeY= refField.sizeY;
      sizeZ= refField.sizeZ;
      sizeTotal= refField.sizeTotal;

      C1= sizeX * sizeY * sizeZ;
      C2= sizeY * sizeZ;
      C3= sizeZ;

      // Copy data
      memcpy(internal_array, refField.internal_array, sizeTotal * sizeof(array_element_type));

      // for (int k= 0; k < sizeTotal; k++)
      //   internal_array[k]= refField.internal_array[k];

      // Return *this for chained assignment
      return *this;
    }
  };

}  // namespace math
