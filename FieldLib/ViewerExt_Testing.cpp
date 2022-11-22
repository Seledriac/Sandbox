
#include "ViewerExt_Testing.h"

// Standard lib
#include <vector>
#include <chrono>

// Project
#include "SrtUtil.h"

// TESTING multiarray
#include "SrtField.h"

// Eigen lib
#include <Eigen/Core>


using std::vector;


void ViewerExt_Testing::keyPressEventExt(Data* D, QKeyEvent* e, Qt::KeyboardModifiers const& modifiers, bool& handled) {
  if ((e->key() == Qt::Key_T) && (modifiers == Qt::NoButton)) {
    printf("nestedvecto : ");

    int nbX, nbY, nbZ;
    SrtUtil::GetFieldDimensions(D->densityField, nbX, nbY, nbZ);
    auto start= std::chrono::high_resolution_clock::now();

    vector<vector<vector<double>>> densityField(nbX, vector<vector<double>>(nbY, vector<double>(nbZ)));
    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          densityField[x][y][z]= 0.5*D->densityField[x][y][z];
        }
      }
    }


    start= std::chrono::high_resolution_clock::now();
    vector<vector<vector<double>>> field0(nbX, vector<vector<double>>(nbY, vector<double>(nbZ)));
    printf("Alloc %8.6f ", std::chrono::duration<double>(std::chrono::high_resolution_clock::now()-start).count());


    start= std::chrono::high_resolution_clock::now();
    double count= 0.0;
    double increment= 1.0/(nbX*nbY*nbZ);
    for (int k= 0; k < 100; k++) {
      for (int x= 0; x < nbX; x++) {
        for (int y= 0; y < nbY; y++) {
          for (int z= 0; z < nbZ; z++) {
            field0[x][y][z]= count;
            count+= increment;
          }
        }
      }
    }
    printf("SeqSet %8.6f ", std::chrono::duration<double>(std::chrono::high_resolution_clock::now()-start).count());


    start= std::chrono::high_resolution_clock::now();
    double sum= 0.0;
    for (int k= 0; k < 100; k++) {
      for (int x= 0; x < nbX; x++) {
        for (int y= 0; y < nbY; y++) {
          for (int z= 0; z < nbZ; z++) {
            sum+= field0[x][y][z];
          }
        }
      }
    }
    printf("SeqSum %8.6f ", std::chrono::duration<double>(std::chrono::high_resolution_clock::now()-start).count());
    printf("val %f ", sum);


    start= std::chrono::high_resolution_clock::now();
    for (int k= 0; k < 100; k++) {
      for (int z= 0; z < nbZ; z++) {
        for (int y= 0; y < nbY; y++) {
          for (int x= 0; x < nbX; x++) {
            sum+= field0[x][y][z];
          }
        }
      }
    }
    printf("BakSeqSum %8.6f ", std::chrono::duration<double>(std::chrono::high_resolution_clock::now()-start).count());
    printf("val %f ", sum);


    vector<vector<vector<double>>> field1(nbX, vector<vector<double>>(nbY, vector<double>(nbZ)));
    start= std::chrono::high_resolution_clock::now();
    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          field1[x][y][z]= 2.0*densityField[x][y][z];
        }
      }
    }
    printf("SeqRW %8.6f ", std::chrono::duration<double>(std::chrono::high_resolution_clock::now()-start).count());


    start= std::chrono::high_resolution_clock::now();
  #pragma omp parallel for
    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          field1[x][y][z]= 2.0*densityField[x][y][z];
        }
      }
    }
    printf("ParRW %8.6f ", std::chrono::duration<double>(std::chrono::high_resolution_clock::now()-start).count());


    vector<vector<vector<double>>> field2(nbX, vector<vector<double>>(nbY, vector<double>(nbZ)));
    start= std::chrono::high_resolution_clock::now();
    int filterRad= 4;
    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          field2[x][y][z]= 0.0;
          int count= 0;
          for (int xOff= std::max(x-filterRad, 0); xOff <= std::min(x+filterRad, nbX-1); xOff++) {
            for (int yOff= std::max(y-filterRad, 0); yOff <= std::min(y+filterRad, nbY-1); yOff++) {
              for (int zOff= std::max(z-filterRad, 0); zOff <= std::min(z+filterRad, nbZ-1); zOff++) {
                field2[x][y][z]+= field1[xOff][yOff][zOff];
                count++;
              }
            }
          }
          field2[x][y][z]/= double(count);
        }
      }
    }
    printf("SeqAvg %8.6f ", std::chrono::duration<double>(std::chrono::high_resolution_clock::now()-start).count());


    start= std::chrono::high_resolution_clock::now();
  #pragma omp parallel for
    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          field2[x][y][z]= 0.0;
          int count= 0;
          for (int xOff= std::max(x-filterRad, 0); xOff <= std::min(x+filterRad, nbX-1); xOff++) {
            for (int yOff= std::max(y-filterRad, 0); yOff <= std::min(y+filterRad, nbY-1); yOff++) {
              for (int zOff= std::max(z-filterRad, 0); zOff <= std::min(z+filterRad, nbZ-1); zOff++) {
                field2[x][y][z]+= field1[xOff][yOff][zOff];
                count++;
              }
            }
          }
          field2[x][y][z]/= double(count);
        }
      }
    }
    printf("ParAvg %8.6f ", std::chrono::duration<double>(std::chrono::high_resolution_clock::now()-start).count());


    start= std::chrono::high_resolution_clock::now();
    vector<vector<vector<double>>> field3= field2;
    printf("Copy %8.6f ", std::chrono::duration<double>(std::chrono::high_resolution_clock::now()-start).count());


    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          D->densityField[x][y][z]= field3[x][y][z];
        }
      }
    }
    printf("\n");

    handled= true;
  }
  else if ((e->key() == Qt::Key_T) && (modifiers == Qt::SHIFT)) {
    printf("customclass : ");

    int nbX, nbY, nbZ;
    SrtUtil::GetFieldDimensions(D->densityField, nbX, nbY, nbZ);
    auto start= std::chrono::high_resolution_clock::now();

    SrtField::Field3D<double> densityField(nbX, nbY, nbZ);
    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          densityField(x, y, z)= 0.5*D->densityField[x][y][z];
        }
      }
    }


    start= std::chrono::high_resolution_clock::now();
    SrtField::Field3D<double> field0(nbX, nbY, nbZ);
    printf("Alloc %8.6f ", std::chrono::duration<double>(std::chrono::high_resolution_clock::now()-start).count());


    start= std::chrono::high_resolution_clock::now();
    double count= 0.0;
    double increment= 1.0/(nbX*nbY*nbZ);
    for (int k= 0; k < 100; k++) {
      for (int x= 0; x < nbX; x++) {
        for (int y= 0; y < nbY; y++) {
          for (int z= 0; z < nbZ; z++) {
            field0(x, y, z)= count;
            count+= increment;
          }
        }
      }
    }
    printf("SeqSet %8.6f ", std::chrono::duration<double>(std::chrono::high_resolution_clock::now()-start).count());


    start= std::chrono::high_resolution_clock::now();
    double sum= 0.0;
    for (int k= 0; k < 100; k++) {
      for (int x= 0; x < nbX; x++) {
        for (int y= 0; y < nbY; y++) {
          for (int z= 0; z < nbZ; z++) {
            sum+= field0(x, y, z);
          }
        }
      }
    }
    printf("SeqSum %8.6f ", std::chrono::duration<double>(std::chrono::high_resolution_clock::now()-start).count());
    printf("val %f ", sum);


    start= std::chrono::high_resolution_clock::now();
    for (int k= 0; k < 100; k++) {
      for (int z= 0; z < nbZ; z++) {
        for (int y= 0; y < nbY; y++) {
          for (int x= 0; x < nbX; x++) {
            sum+= field0(x, y, z);
          }
        }
      }
    }
    printf("BakSeqSum %8.6f ", std::chrono::duration<double>(std::chrono::high_resolution_clock::now()-start).count());
    printf("val %f ", sum);


    SrtField::Field3D<double> field1(nbX, nbY, nbZ);
    start= std::chrono::high_resolution_clock::now();
    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          field1(x, y, z)= 2.0*densityField(x, y, z);
        }
      }
    }
    printf("SeqRW %8.6f ", std::chrono::duration<double>(std::chrono::high_resolution_clock::now()-start).count());


    start= std::chrono::high_resolution_clock::now();
  #pragma omp parallel for
    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          field1(x, y, z)= 2.0*densityField(x, y, z);
        }
      }
    }
    printf("ParRW %8.6f ", std::chrono::duration<double>(std::chrono::high_resolution_clock::now()-start).count());


    SrtField::Field3D<double> field2(nbX, nbY, nbZ);
    start= std::chrono::high_resolution_clock::now();
    int filterRad= 4;
    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          field2(x, y, z)= 0.0;
          int count= 0;
          for (int xOff= std::max(x-filterRad, 0); xOff <= std::min(x+filterRad, nbX-1); xOff++) {
            for (int yOff= std::max(y-filterRad, 0); yOff <= std::min(y+filterRad, nbY-1); yOff++) {
              for (int zOff= std::max(z-filterRad, 0); zOff <= std::min(z+filterRad, nbZ-1); zOff++) {
                field2(x, y, z)+= field1(xOff, yOff, zOff);
                count++;
              }
            }
          }
          field2(x, y, z)/= double(count);
        }
      }
    }
    printf("SeqAvg %8.6f ", std::chrono::duration<double>(std::chrono::high_resolution_clock::now()-start).count());


    start= std::chrono::high_resolution_clock::now();
  #pragma omp parallel for
    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          field2(x, y, z)= 0.0;
          int count= 0;
          for (int xOff= std::max(x-filterRad, 0); xOff <= std::min(x+filterRad, nbX-1); xOff++) {
            for (int yOff= std::max(y-filterRad, 0); yOff <= std::min(y+filterRad, nbY-1); yOff++) {
              for (int zOff= std::max(z-filterRad, 0); zOff <= std::min(z+filterRad, nbZ-1); zOff++) {
                field2(x, y, z)+= field1(xOff, yOff, zOff);
                count++;
              }
            }
          }
          field2(x, y, z)/= double(count);
        }
      }
    }
    printf("ParAvg %8.6f ", std::chrono::duration<double>(std::chrono::high_resolution_clock::now()-start).count());


    start= std::chrono::high_resolution_clock::now();
    SrtField::Field3D<double> field3= field2;
    printf("Copy %8.6f ", std::chrono::duration<double>(std::chrono::high_resolution_clock::now()-start).count());


    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          D->densityField[x][y][z]= field3(x, y, z);
        }
      }
    }
    printf("\n");

    handled= true;
  }
  else if ((e->key() == Qt::Key_T) && (modifiers == Qt::CTRL)) {
    printf("simplevecto : ");

    int nbX, nbY, nbZ;
    SrtUtil::GetFieldDimensions(D->densityField, nbX, nbY, nbZ);
    auto start= std::chrono::high_resolution_clock::now();

    vector<double> densityField(nbX*nbY*nbZ);
    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          densityField[x*nbY*nbZ + y*nbZ + z]= 0.5*D->densityField[x][y][z];
        }
      }
    }


    start= std::chrono::high_resolution_clock::now();
    vector<double> field0(nbX*nbY*nbZ);
    printf("Alloc %8.6f ", std::chrono::duration<double>(std::chrono::high_resolution_clock::now()-start).count());


    start= std::chrono::high_resolution_clock::now();
    double count= 0.0;
    double increment= 1.0/(nbX*nbY*nbZ);
    for (int k= 0; k < 100; k++) {
      for (int x= 0; x < nbX; x++) {
        for (int y= 0; y < nbY; y++) {
          for (int z= 0; z < nbZ; z++) {
            field0[x*nbY*nbZ + y*nbZ + z]= count;
            count+= increment;
          }
        }
      }
    }
    printf("SeqSet %8.6f ", std::chrono::duration<double>(std::chrono::high_resolution_clock::now()-start).count());


    start= std::chrono::high_resolution_clock::now();
    double sum= 0.0;
    for (int k= 0; k < 100; k++) {
      for (int x= 0; x < nbX; x++) {
        for (int y= 0; y < nbY; y++) {
          for (int z= 0; z < nbZ; z++) {
            sum+= field0[x*nbY*nbZ + y*nbZ + z];
          }
        }
      }
    }
    printf("SeqSum %8.6f ", std::chrono::duration<double>(std::chrono::high_resolution_clock::now()-start).count());
    printf("val %f ", sum);


    start= std::chrono::high_resolution_clock::now();
    for (int k= 0; k < 100; k++) {
      for (int z= 0; z < nbZ; z++) {
        for (int y= 0; y < nbY; y++) {
          for (int x= 0; x < nbX; x++) {
            sum+= field0[x*nbY*nbZ + y*nbZ + z];
          }
        }
      }
    }
    printf("BakSeqSum %8.6f ", std::chrono::duration<double>(std::chrono::high_resolution_clock::now()-start).count());
    printf("val %f ", sum);


    vector<double> field1(nbX*nbY*nbZ);
    start= std::chrono::high_resolution_clock::now();
    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          field1[x*nbY*nbZ + y*nbZ + z]= 2.0*densityField[x*nbY*nbZ + y*nbZ + z];
        }
      }
    }
    printf("SeqRW %8.6f ", std::chrono::duration<double>(std::chrono::high_resolution_clock::now()-start).count());


    start= std::chrono::high_resolution_clock::now();
  #pragma omp parallel for
    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          field1[x*nbY*nbZ + y*nbZ + z]= 2.0*densityField[x*nbY*nbZ + y*nbZ + z];
        }
      }
    }
    printf("ParRW %8.6f ", std::chrono::duration<double>(std::chrono::high_resolution_clock::now()-start).count());


    vector<double> field2(nbX*nbY*nbZ);
    start= std::chrono::high_resolution_clock::now();
    int filterRad= 4;
    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          field2[x*nbY*nbZ + y*nbZ + z]= 0.0;
          int count= 0;
          for (int xOff= std::max(x-filterRad, 0); xOff <= std::min(x+filterRad, nbX-1); xOff++) {
            for (int yOff= std::max(y-filterRad, 0); yOff <= std::min(y+filterRad, nbY-1); yOff++) {
              for (int zOff= std::max(z-filterRad, 0); zOff <= std::min(z+filterRad, nbZ-1); zOff++) {
                field2[x*nbY*nbZ + y*nbZ + z]+= field1[xOff*nbY*nbZ + yOff*nbZ + zOff];
                count++;
              }
            }
          }
          field2[x*nbY*nbZ + y*nbZ + z]/= double(count);
        }
      }
    }
    printf("SeqAvg %8.6f ", std::chrono::duration<double>(std::chrono::high_resolution_clock::now()-start).count());


    start= std::chrono::high_resolution_clock::now();
  #pragma omp parallel for
    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          field2[x*nbY*nbZ + y*nbZ + z]= 0.0;
          int count= 0;
          for (int xOff= std::max(x-filterRad, 0); xOff <= std::min(x+filterRad, nbX-1); xOff++) {
            for (int yOff= std::max(y-filterRad, 0); yOff <= std::min(y+filterRad, nbY-1); yOff++) {
              for (int zOff= std::max(z-filterRad, 0); zOff <= std::min(z+filterRad, nbZ-1); zOff++) {
                field2[x*nbY*nbZ + y*nbZ + z]+= field1[xOff*nbY*nbZ + yOff*nbZ + zOff];
                count++;
              }
            }
          }
          field2[x*nbY*nbZ + y*nbZ + z]/= double(count);
        }
      }
    }
    printf("ParAvg %8.6f ", std::chrono::duration<double>(std::chrono::high_resolution_clock::now()-start).count());


    start= std::chrono::high_resolution_clock::now();
    vector<double> field3= field2;
    printf("Copy %8.6f ", std::chrono::duration<double>(std::chrono::high_resolution_clock::now()-start).count());


    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          D->densityField[x][y][z]= field3[x*nbY*nbZ + y*nbZ + z];
        }
      }
    }
    printf("\n");

    handled= true;
  }
  else if ((e->key() == Qt::Key_T) && (modifiers == Qt::ALT)) {
    printf("mallocvecto : ");

    int nbX, nbY, nbZ;
    SrtUtil::GetFieldDimensions(D->densityField, nbX, nbY, nbZ);
    auto start= std::chrono::high_resolution_clock::now();

    double* densityField= new double[nbX*nbY*nbZ];
    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          densityField[x*nbY*nbZ + y*nbZ + z]= 0.5*D->densityField[x][y][z];
        }
      }
    }


    start= std::chrono::high_resolution_clock::now();
    double* field0= new double[nbX*nbY*nbZ];
    printf("Alloc %8.6f ", std::chrono::duration<double>(std::chrono::high_resolution_clock::now()-start).count());


    start= std::chrono::high_resolution_clock::now();
    double count= 0.0;
    double increment= 1.0/(nbX*nbY*nbZ);
    for (int k= 0; k < 100; k++) {
      for (int x= 0; x < nbX; x++) {
        for (int y= 0; y < nbY; y++) {
          for (int z= 0; z < nbZ; z++) {
            field0[x*nbY*nbZ + y*nbZ + z]= count;
            count+= increment;
          }
        }
      }
    }
    printf("SeqSet %8.6f ", std::chrono::duration<double>(std::chrono::high_resolution_clock::now()-start).count());


    start= std::chrono::high_resolution_clock::now();
    double sum= 0.0;
    for (int k= 0; k < 100; k++) {
      for (int x= 0; x < nbX; x++) {
        for (int y= 0; y < nbY; y++) {
          for (int z= 0; z < nbZ; z++) {
            sum+= field0[x*nbY*nbZ + y*nbZ + z];
          }
        }
      }
    }
    printf("SeqSum %8.6f ", std::chrono::duration<double>(std::chrono::high_resolution_clock::now()-start).count());
    printf("val %f ", sum);


    start= std::chrono::high_resolution_clock::now();
    for (int k= 0; k < 100; k++) {
      for (int z= 0; z < nbZ; z++) {
        for (int y= 0; y < nbY; y++) {
          for (int x= 0; x < nbX; x++) {
            sum+= field0[x*nbY*nbZ + y*nbZ + z];
          }
        }
      }
    }
    printf("BakSeqSum %8.6f ", std::chrono::duration<double>(std::chrono::high_resolution_clock::now()-start).count());
    printf("val %f ", sum);


    double* field1= new double[nbX*nbY*nbZ];
    start= std::chrono::high_resolution_clock::now();
    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          field1[x*nbY*nbZ + y*nbZ + z]= 2.0*densityField[x*nbY*nbZ + y*nbZ + z];
        }
      }
    }
    printf("SeqRW %8.6f ", std::chrono::duration<double>(std::chrono::high_resolution_clock::now()-start).count());


    start= std::chrono::high_resolution_clock::now();
  #pragma omp parallel for
    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          field1[x*nbY*nbZ + y*nbZ + z]= 2.0*densityField[x*nbY*nbZ + y*nbZ + z];
        }
      }
    }
    printf("ParRW %8.6f ", std::chrono::duration<double>(std::chrono::high_resolution_clock::now()-start).count());


    double* field2= new double[nbX*nbY*nbZ];
    start= std::chrono::high_resolution_clock::now();
    int filterRad= 4;
    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          field2[x*nbY*nbZ + y*nbZ + z]= 0.0;
          int count= 0;
          for (int xOff= std::max(x-filterRad, 0); xOff <= std::min(x+filterRad, nbX-1); xOff++) {
            for (int yOff= std::max(y-filterRad, 0); yOff <= std::min(y+filterRad, nbY-1); yOff++) {
              for (int zOff= std::max(z-filterRad, 0); zOff <= std::min(z+filterRad, nbZ-1); zOff++) {
                field2[x*nbY*nbZ + y*nbZ + z]+= field1[xOff*nbY*nbZ + yOff*nbZ + zOff];
                count++;
              }
            }
          }
          field2[x*nbY*nbZ + y*nbZ + z]/= double(count);
        }
      }
    }
    printf("SeqAvg %8.6f ", std::chrono::duration<double>(std::chrono::high_resolution_clock::now()-start).count());


    start= std::chrono::high_resolution_clock::now();
  #pragma omp parallel for
    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          field2[x*nbY*nbZ + y*nbZ + z]= 0.0;
          int count= 0;
          for (int xOff= std::max(x-filterRad, 0); xOff <= std::min(x+filterRad, nbX-1); xOff++) {
            for (int yOff= std::max(y-filterRad, 0); yOff <= std::min(y+filterRad, nbY-1); yOff++) {
              for (int zOff= std::max(z-filterRad, 0); zOff <= std::min(z+filterRad, nbZ-1); zOff++) {
                field2[x*nbY*nbZ + y*nbZ + z]+= field1[xOff*nbY*nbZ + yOff*nbZ + zOff];
                count++;
              }
            }
          }
          field2[x*nbY*nbZ + y*nbZ + z]/= double(count);
        }
      }
    }
    printf("ParAvg %8.6f ", std::chrono::duration<double>(std::chrono::high_resolution_clock::now()-start).count());


    start= std::chrono::high_resolution_clock::now();
    double* field3= new double[nbX*nbY*nbZ];
    memcpy(field3, field2, nbX*nbY*nbZ*sizeof(double));
    printf("Copy %8.6f ", std::chrono::duration<double>(std::chrono::high_resolution_clock::now()-start).count());


    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          D->densityField[x][y][z]= field3[x*nbY*nbZ + y*nbZ + z];
        }
      }
    }
    printf("\n");

    handled= true;
  }
}
