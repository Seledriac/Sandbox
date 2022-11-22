#pragma once

// Choose 0 or 1 to disable or enable bound checking in the () operator
#define SRTFIELD_ARRAY_BOUNDS_CHECK 0


namespace SrtField {

  // Sample code
  //
  // SrtField::Field3D<double> densityField(nbX, nbY, nbZ);
  // for (int x= 0; x < nbX; x++) {
  //   for (int y= 0; y < nbY; y++) {
  //     for (int z= 0; z < nbZ; z++) {
  //       densityField(x, y, z)= 0.5 + 0.5*std::sin(3.0*double(z)/double(nbZ));
  //     }
  //   }
  // }
  //
  template <class array_element_type> class Field3D {

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
      sizeX = 0;
      sizeY = 0;
      sizeZ = 0;
      sizeTotal = 0;

      C1 = 0;
      C2 = 0;

      internal_array = nullptr;
    }
    Field3D(int const nbX, int const nbY, int const nbZ) {
      if ((nbX < 0) || (nbY < 0) || (nbZ < 0)) {
        printf("[Error] Invalid sizes for initialized field (%d;%d;%d)\n", nbX, nbY, nbZ);
        throw;
      }

      sizeX = nbX;
      sizeY = nbY;
      sizeZ = nbZ;
      sizeTotal = sizeX*sizeY*sizeZ;

      C1 = sizeY*sizeZ;
      C2 = sizeZ;

      internal_array= nullptr;
      if (sizeTotal > 0)
        internal_array = new array_element_type[sizeTotal];
    }
    Field3D(int const nbX, int const nbY, int const nbZ, array_element_type const& val) {
      if ((nbX < 0) || (nbY < 0) || (nbZ < 0)) {
        printf("[Error] Invalid sizes for initialized field (%d;%d;%d)\n", nbX, nbY, nbZ);
        throw;
      }

      sizeX = nbX;
      sizeY = nbY;
      sizeZ = nbZ;
      sizeTotal = sizeX*sizeY*sizeZ;

      C1 = sizeY*sizeZ;
      C2 = sizeZ;

      internal_array= nullptr;
      if (sizeTotal > 0) {
        internal_array = new array_element_type[sizeTotal];
        std::fill_n(internal_array, sizeTotal, val);
      }
    }
    Field3D(Field3D const& refField) {
      sizeX = refField.nbX();
      sizeY = refField.nbY();
      sizeZ = refField.nbZ();
      sizeTotal = refField.nbTotal();

      C1 = sizeY*sizeZ;
      C2 = sizeZ;

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
      if ((idxX < 0) || (idxX >= sizeX) || (idxY < 0) || (idxY >= sizeY) || (idxZ < 0) || (idxZ >= sizeZ)) {
        printf("[Error] Index out of bounds while accessing (%d;%d;%d) in %dx%dx%d array\n", idxX, idxY, idxZ, sizeX, sizeY, sizeZ);
        throw;
      }
    #endif
      return internal_array[idxX*C1 + idxY*C2 + idxZ];
    }
    inline const array_element_type& operator() (int const idxX, int const idxY, int const idxZ) const {
    #if SRTFIELD_ARRAY_BOUNDS_CHECK == 1
      if ((idxX < 0) || (idxX >= sizeX) || (idxY < 0) || (idxY >= sizeY) || (idxZ < 0) || (idxZ >= sizeZ)) {
        printf("[Error] Index out of bounds while accessing (%d;%d;%d) in %dx%dx%d array\n", idxX, idxY, idxZ, sizeX, sizeY, sizeZ);
        throw;
      }
    #endif
      return internal_array[idxX*C1 + idxY*C2 + idxZ];
    }
    inline array_element_type& operator() (int const idx) {
    #if SRTFIELD_ARRAY_BOUNDS_CHECK == 1
      if ((idx < 0) || (idx >= sizeTotal)) {
        printf("[Error] Index out of bounds while accessing %d in %dx%dx%d=%d array\n", idx, sizeX, sizeY, sizeZ, sizeTotal);
        throw;
      }
    #endif
      return internal_array[idx];
    }
    inline const array_element_type& operator() (int const idx) const {
    #if SRTFIELD_ARRAY_BOUNDS_CHECK == 1
      if ((idx < 0) || (idx >= sizeTotal)) {
        printf("[Error] Index out of bounds while accessing %d in %dx%dx%d=%d array\n", idx, sizeX, sizeY, sizeZ, sizeTotal);
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
      sizeX = refField.sizeX;
      sizeY = refField.sizeY;
      sizeZ = refField.sizeZ;
      sizeTotal = refField.sizeTotal;

      C1 = sizeY*sizeZ;
      C2 = sizeZ;

      // Copy data
      memcpy(internal_array, refField.internal_array, sizeTotal*sizeof(array_element_type));

      //for (int k= 0; k < sizeTotal; k++)
      //  internal_array[k]= refField.internal_array[k];

      // Return *this for chained assignment
      return *this;
    }
  };

}
