#pragma once

// Choose 0 or 1 to disable or enable bound checking in the () operator
#define SRTFIELD_ARRAY_BOUNDS_CHECK 0


namespace SrtField {

  // Sample code
  //
  // SrtField::Field3D<double> densityField(nbDim1, nbDim2, nbDim3);
  // for (int x= 0; x < nbDim1; x++) {
  //   for (int y= 0; y < nbDim2; y++) {
  //     for (int z= 0; z < nbDim3; z++) {
  //       densityField(x, y, z)= 0.5 + 0.5*std::sin(3.0*double(z)/double(nbDim3));
  //     }
  //   }
  // }
  //
  template <class array_element_type> class Field3D {

  private:
    int size1, size2, size3, sizeTotal;
    int C1, C2;
    array_element_type* internal_array;

  public:

    // Get raw data
    inline array_element_type* data(void) const { return internal_array; }

    // Get dimensions
    inline int nbDim1(void) const { return size1; }
    inline int nbDim2(void) const { return size2; }
    inline int nbDim3(void) const { return size3; }
    inline int nbTotal(void) const { return sizeTotal; }

    // Constructors
    Field3D() {
      size1 = 0;
      size2 = 0;
      size3 = 0;
      sizeTotal = 0;

      C1 = 0;
      C2 = 0;

      internal_array = nullptr;
    }
    Field3D(int const nbDim1, int const nbDim2, int const nbDim3) {
      if ((nbDim1 < 0) || (nbDim2 < 0) || (nbDim3 < 0)) {
        printf("[Error] Invalid initialization sizes (%d;%d;%d)\n", nbDim1, nbDim2, nbDim3);
        throw;
      }

      size1 = nbDim1;
      size2 = nbDim2;
      size3 = nbDim3;
      sizeTotal = size1*size2*size3;

      C1 = size2*size3;
      C2 = size3;

      internal_array= nullptr;
      if (sizeTotal > 0)
        internal_array = new array_element_type[sizeTotal];
    }
    Field3D(int const nbDim1, int const nbDim2, int const nbDim3, array_element_type const& val) {
      if ((nbDim1 < 0) || (nbDim2 < 0) || (nbDim3 < 0)) {
        printf("[Error] Invalid initialization sizes (%d;%d;%d)\n", nbDim1, nbDim2, nbDim3);
        throw;
      }

      size1 = nbDim1;
      size2 = nbDim2;
      size3 = nbDim3;
      sizeTotal = size1*size2*size3;

      C1 = size2*size3;
      C2 = size3;

      internal_array= nullptr;
      if (sizeTotal > 0) {
        internal_array = new array_element_type[sizeTotal];
        std::fill_n(internal_array, sizeTotal, val);
      }
    }
    Field3D(Field3D const& refField) {
      size1 = refField.nbDim1();
      size2 = refField.nbDim2();
      size3 = refField.nbDim3();
      sizeTotal = refField.nbTotal();

      C1 = size2*size3;
      C2 = size3;

      internal_array= nullptr;
      if (sizeTotal > 0) {
        internal_array = new array_element_type[sizeTotal];
        memcpy(internal_array, refField.internal_array, sizeTotal*sizeof(array_element_type));
      }
    }

    // Destructor
    ~Field3D() { delete[] internal_array; }

    // Overloads of operator() to access data
    inline array_element_type& operator() (int const idxX, int const idxY, int const idxZ) {
    #if SRTFIELD_ARRAY_BOUNDS_CHECK == 1
      if ((idxX < 0) || (idxX >= size1) || (idxY < 0) || (idxY >= size2) || (idxZ < 0) || (idxZ >= size3)) {
        printf("[Error] Index out of bounds while accessing (%d;%d;%d) in %dx%dx%d array\n", idxX, idxY, idxZ, size1, size2, size3);
        throw;
      }
    #endif
      return internal_array[idxX*C1 + idxY*C2 + idxZ];
    }
    inline const array_element_type& operator() (int const idxX, int const idxY, int const idxZ) const {
    #if SRTFIELD_ARRAY_BOUNDS_CHECK == 1
      if ((idxX < 0) || (idxX >= size1) || (idxY < 0) || (idxY >= size2) || (idxZ < 0) || (idxZ >= size3)) {
        printf("[Error] Index out of bounds while accessing (%d;%d;%d) in %dx%dx%d array\n", idxX, idxY, idxZ, size1, size2, size3);
        throw;
      }
    #endif
      return internal_array[idxX*C1 + idxY*C2 + idxZ];
    }
    inline array_element_type& operator() (int const idx) {
    #if SRTFIELD_ARRAY_BOUNDS_CHECK == 1
      if ((idx < 0) || (idx >= sizeTotal)) {
        printf("[Error] Index out of bounds while accessing %d in %dx%dx%d=%d array\n", idx, size1, size2, size3, sizeTotal);
        throw;
      }
    #endif
      return internal_array[idx];
    }
    inline const array_element_type& operator() (int const idx) const {
    #if SRTFIELD_ARRAY_BOUNDS_CHECK == 1
      if ((idx < 0) || (idx >= sizeTotal)) {
        printf("[Error] Index out of bounds while accessing %d in %dx%dx%d=%d array\n", idx, size1, size2, size3, sizeTotal);
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
      if (sizeTotal != refField.len()) {
        delete[] internal_array;
        internal_array = new array_element_type[refField.len()];
      }

      // Set dimension variables
      size1 = refField.size1;
      size2 = refField.size2;
      size3 = refField.size3;
      sizeTotal = refField.sizeTotal;

      C1 = size2*size3;
      C2 = size3;

      // Copy data
      memcpy(internal_array, refField.internal_array, sizeTotal*sizeof(array_element_type));

      // Return *this for chained assignment
      return *this;
    }
  };

}
