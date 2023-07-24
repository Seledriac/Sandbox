
#include "CompuFluidDyn.hpp"


// Standard lib
#include <cstdio>
#include <vector>

// GLUT lib
#include <GL/freeglut.h>

// Project lib
#include "../Data.hpp"
#include "../math/Vectors.hpp"
#include "../util/Colormap.hpp"
#include "../util/Field.hpp"


extern Data D;

enum ParamType
{
  ResolutionX_________,
  ResolutionY_________,
  ResolutionZ_________,
  SimSteps____________,
  DivGaussSeidelIter__,
  DivJacobiIter_______,
  Overrelaxation______,
  TimeStep____________,
  InletVelocity_______,
  ObstacleSize________,
  ObstaclePosX________,
  ObstaclePosY________,
  ObstaclePosZ________,
  ScaleFactor_________,
  ColorFactor_________,
};


CompuFluidDyn::CompuFluidDyn() {
  isInitialized= false;
  isRefreshed= false;
}


void CompuFluidDyn::Init() {
  isInitialized= true;
  isRefreshed= false;

  D.param.clear();
  D.param.push_back(ParamUI("ResolutionX_________", 3));
  D.param.push_back(ParamUI("ResolutionY_________", 20));
  D.param.push_back(ParamUI("ResolutionZ_________", 20));
  D.param.push_back(ParamUI("SimSteps____________", 1));
  D.param.push_back(ParamUI("DivGaussSeidelIter__", 0));
  D.param.push_back(ParamUI("DivJacobiIter_______", 1));
  D.param.push_back(ParamUI("Overrelaxation______", 1.9));
  D.param.push_back(ParamUI("TimeStep____________", 0.01));
  D.param.push_back(ParamUI("InletVelocity_______", 1.0));
  D.param.push_back(ParamUI("ObstacleSize________", 0.2));
  D.param.push_back(ParamUI("ObstaclePosX________", 0.5));
  D.param.push_back(ParamUI("ObstaclePosY________", 0.3));
  D.param.push_back(ParamUI("ObstaclePosZ________", 0.7));
  D.param.push_back(ParamUI("ScaleFactor_________", 1.0));
  D.param.push_back(ParamUI("ColorFactor_________", 1.0));
}


void CompuFluidDyn::Refresh() {
  if (!isInitialized) return;
  isRefreshed= true;

  nbX= std::max(int(std::round(D.param[ResolutionX_________].val)), 3);
  nbY= std::max(int(std::round(D.param[ResolutionY_________].val)), 3);
  nbZ= std::max(int(std::round(D.param[ResolutionZ_________].val)), 3);
  simSteps= 0;

  Press= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  Solid= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  Veloc= Field::AllocField3D(nbX, nbY, nbZ, Math::Vec3f(0.0f, 0.0f, 0.0f));
  BCond= Field::AllocField3D(nbX, nbY, nbZ, false);

  for (int x= 0; x < nbX; x++) {
    for (int y= 0; y < nbY; y++) {
      for (int z= 0; z < nbZ; z++) {
        // Add walls on X and Z faces
        if (x == 0 || x == nbX - 1 || z == 0 || z == nbZ - 1) {
          BCond[x][y][z]= true;
          Veloc[x][y][z]= Math::Vec3f(0.0f, 0.0f, 0.0f);
        }

        // Set inlet on Y-
        if (y == 0 && x > 0 && x < nbX - 1 && z > 0 && z < nbZ - 1) {
          BCond[x][y][z]= true;
          Veloc[x][y][z]= Math::Vec3f(0.0f, D.param[InletVelocity_______].val, 0.0f);
        }

        // Set outlet on Y+
        if (y == nbY - 1 && x > 0 && x < nbX - 1 && z > 0 && z < nbZ - 1) {
          BCond[x][y][z]= false;
          Veloc[x][y][z]= Math::Vec3f(0.0f, 0.0f, 0.0f);
        }

        // Add obstacle
        Math::Vec3f posCell((float(x) + 0.5f) / float(nbX), (float(y) + 0.5f) / float(nbY), (float(z) + 0.5f) / float(nbZ));
        Math::Vec3f posObstacle(D.param[ObstaclePosX________].val, D.param[ObstaclePosY________].val, D.param[ObstaclePosZ________].val);
        if ((posCell - posObstacle).normSquared() <= std::pow(std::max(D.param[ObstacleSize________].val, 0.0), 2.0)) {
          Solid[x][y][z]= 1.0f;
          BCond[x][y][z]= true;
          Veloc[x][y][z]= Math::Vec3f(0.0f, 0.0f, 0.0f);
        }
      }
    }
  }
}


void CompuFluidDyn::Animate() {
  if (!isInitialized) return;
  if (!isRefreshed) return;

  if (simSteps >= int(std::round(D.param[SimSteps____________].val)))
    return;
  simSteps++;


  // // Build staggered grids
  // std::vector<std::vector<std::vector<float>>> velX= Field::AllocField3D(nbX + 1, nbY, nbZ, 0.0f);
  // std::vector<std::vector<std::vector<float>>> velY= Field::AllocField3D(nbX, nbY + 1, nbZ, 0.0f);
  // std::vector<std::vector<std::vector<float>>> velZ= Field::AllocField3D(nbX, nbY, nbZ + 1, 0.0f);
  // std::vector<std::vector<std::vector<bool>>> lockX= Field::AllocField3D(nbX + 1, nbY, nbZ, false);
  // std::vector<std::vector<std::vector<bool>>> lockY= Field::AllocField3D(nbX, nbY + 1, nbZ, false);
  // std::vector<std::vector<std::vector<bool>>> lockZ= Field::AllocField3D(nbX, nbY, nbZ + 1, false);
  // for (int x= 0; x < nbX; x++) {
  //   for (int y= 0; y < nbY; y++) {
  //     for (int z= 0; z < nbZ; z++) {
  //       if (Solid[x][y][z] > 0.5f || BCond[x][y][z]) {
  //         lockX[x + 0][y][z]= true;
  //         lockX[x + 1][y][z]= true;
  //         lockY[x][y + 0][z]= true;
  //         lockY[x][y + 1][z]= true;
  //         lockZ[x][y][z + 0]= true;
  //         lockZ[x][y][z + 1]= true;
  //       }
  //       float weightX= (x > 0 && x < nbX - 1) ? 0.5f : 1.0f;
  //       float weightY= (y > 0 && y < nbY - 1) ? 0.5f : 1.0f;
  //       float weightZ= (z > 0 && z < nbZ - 1) ? 0.5f : 1.0f;
  //       velX[x + 0][y][z]+= weightX * Veloc[x][y][z][0];
  //       velX[x + 1][y][z]+= weightX * Veloc[x][y][z][0];
  //       velY[x][y + 0][z]+= weightY * Veloc[x][y][z][1];
  //       velY[x][y + 1][z]+= weightY * Veloc[x][y][z][1];
  //       velZ[x][y][z + 0]+= weightZ * Veloc[x][y][z][2];
  //       velZ[x][y][z + 1]+= weightZ * Veloc[x][y][z][2];
  //     }
  //   }
  // }

  // // Solve for incompressibility via Gauss Seidel
  // for (int k= 0; k < int(std::round(D.param[DivGaussSeidelIter__].val)); k++) {
  //   std::vector<std::vector<std::vector<float>>> divergence= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  //   for (int x= 0; x < nbX; x++) {
  //     for (int y= 0; y < nbY; y++) {
  //       for (int z= 0; z < nbZ; z++) {
  //         divergence[x][y][z]= 0.0;
  //         divergence[x][y][z]-= velX[x + 0][y][z];
  //         divergence[x][y][z]+= velX[x + 1][y][z];
  //         divergence[x][y][z]-= velY[x][y + 0][z];
  //         divergence[x][y][z]+= velY[x][y + 1][z];
  //         divergence[x][y][z]-= velZ[x][y][z + 0];
  //         divergence[x][y][z]+= velZ[x][y][z + 1];

  //         int count= 0;
  //         if (!lockX[x + 0][y][z]) count++;
  //         if (!lockX[x + 1][y][z]) count++;
  //         if (!lockY[x][y + 0][z]) count++;
  //         if (!lockY[x][y + 1][z]) count++;
  //         if (!lockZ[x][y][z + 0]) count++;
  //         if (!lockZ[x][y][z + 1]) count++;

  //         if (count > 0) {
  //           if (!lockX[x + 0][y][z]) velX[x + 0][y][z]+= D.param[Overrelaxation______].val * divergence[x][y][z] / float(count);
  //           if (!lockX[x + 1][y][z]) velX[x + 1][y][z]-= D.param[Overrelaxation______].val * divergence[x][y][z] / float(count);
  //           if (!lockY[x][y + 0][z]) velY[x][y + 0][z]+= D.param[Overrelaxation______].val * divergence[x][y][z] / float(count);
  //           if (!lockY[x][y + 1][z]) velY[x][y + 1][z]-= D.param[Overrelaxation______].val * divergence[x][y][z] / float(count);
  //           if (!lockZ[x][y][z + 0]) velZ[x][y][z + 0]+= D.param[Overrelaxation______].val * divergence[x][y][z] / float(count);
  //           if (!lockZ[x][y][z + 1]) velZ[x][y][z + 1]-= D.param[Overrelaxation______].val * divergence[x][y][z] / float(count);
  //         }
  //       }
  //     }
  //   }
  // }

  // // Solve for incompressibility via Jacobi
  // for (int k= 0; k < int(std::round(D.param[DivJacobiIter_______].val)); k++) {
  //   std::vector<std::vector<std::vector<float>>> divergence= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  //   for (int x= 0; x < nbX; x++) {
  //     for (int y= 0; y < nbY; y++) {
  //       for (int z= 0; z < nbZ; z++) {
  //         divergence[x][y][z]= 0.0;
  //         divergence[x][y][z]-= velX[x + 0][y][z];
  //         divergence[x][y][z]+= velX[x + 1][y][z];
  //         divergence[x][y][z]-= velY[x][y + 0][z];
  //         divergence[x][y][z]+= velY[x][y + 1][z];
  //         divergence[x][y][z]-= velZ[x][y][z + 0];
  //         divergence[x][y][z]+= velZ[x][y][z + 1];

  //         Press[x][y][z]= -divergence[x][y][z];
  //       }
  //     }
  //   }
  //   for (int x= 0; x < nbX; x++) {
  //     for (int y= 0; y < nbY; y++) {
  //       for (int z= 0; z < nbZ; z++) {
  //         int count= 0;
  //         if (!lockX[x + 0][y][z]) count++;
  //         if (!lockX[x + 1][y][z]) count++;
  //         if (!lockY[x][y + 0][z]) count++;
  //         if (!lockY[x][y + 1][z]) count++;
  //         if (!lockZ[x][y][z + 0]) count++;
  //         if (!lockZ[x][y][z + 1]) count++;

  //         if (count > 0) {
  //           if (!lockX[x + 0][y][z]) velX[x + 0][y][z]+= divergence[x][y][z] / float(count);
  //           if (!lockX[x + 1][y][z]) velX[x + 1][y][z]-= divergence[x][y][z] / float(count);
  //           if (!lockY[x][y + 0][z]) velY[x][y + 0][z]+= divergence[x][y][z] / float(count);
  //           if (!lockY[x][y + 1][z]) velY[x][y + 1][z]-= divergence[x][y][z] / float(count);
  //           if (!lockZ[x][y][z + 0]) velZ[x][y][z + 0]+= divergence[x][y][z] / float(count);
  //           if (!lockZ[x][y][z + 1]) velZ[x][y][z + 1]-= divergence[x][y][z] / float(count);
  //         }
  //       }
  //     }
  //   }
  // }

  // // Update velocities
  // for (int x= 0; x < nbX; x++) {
  //   for (int y= 0; y < nbY; y++) {
  //     for (int z= 0; z < nbZ; z++) {
  //       Veloc[x][y][z][0]= 0.5f * (velX[x + 0][y][z] + velX[x + 1][y][z]);
  //       Veloc[x][y][z][1]= 0.5f * (velY[x][y + 0][z] + velY[x][y + 1][z]);
  //       Veloc[x][y][z][2]= 0.5f * (velZ[x][y][z + 0] + velZ[x][y][z + 1]);
  //     }
  //   }
  // }

  // Solve for incompressibility via Jacobi
  for (int k= 0; k < int(std::round(D.param[DivJacobiIter_______].val)); k++) {
    std::vector<std::vector<std::vector<float>>> divergence= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          divergence[x][y][z]= 0.0;
          if (x - 1 >=  0) divergence[x][y][z]-= Veloc[x - 1][y][z][0];
          if (x + 1 < nbX) divergence[x][y][z]+= Veloc[x + 1][y][z][0];
          if (y - 1 >=  0) divergence[x][y][z]-= Veloc[x][y - 1][z][1];
          if (y + 1 < nbY) divergence[x][y][z]+= Veloc[x][y + 1][z][1];
          if (z - 1 >=  0) divergence[x][y][z]-= Veloc[x][y][z - 1][2];
          if (z + 1 < nbZ) divergence[x][y][z]+= Veloc[x][y][z + 1][2];
        }
      }
    }
    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          if (x - 1 >= 0  && !BCond[x - 1][y][z]) Veloc[x - 1][y][z][0]+= divergence[x][y][z];
          if (x + 1 < nbX && !BCond[x + 1][y][z]) Veloc[x + 1][y][z][0]-= divergence[x][y][z];
          if (y - 1 >= 0  && !BCond[x][y - 1][z]) Veloc[x][y - 1][z][1]+= divergence[x][y][z];
          if (y + 1 < nbY && !BCond[x][y + 1][z]) Veloc[x][y + 1][z][1]-= divergence[x][y][z];
          if (z - 1 >= 0  && !BCond[x][y][z - 1]) Veloc[x][y][z - 1][2]+= divergence[x][y][z];
          if (z + 1 < nbZ && !BCond[x][y][z + 1]) Veloc[x][y][z + 1][2]-= divergence[x][y][z];
        }
      }
    }
    // for (int x= 1; x < nbX - 1; x++) {
    //   for (int y= 1; y < nbY - 1; y++) {
    //     for (int z= 1; z < nbZ - 1; z++) {
    //       int count= 0;
    //       if (!BCond[x - 1][y][z]) count++;
    //       if (!BCond[x + 1][y][z]) count++;
    //       if (!BCond[x][y - 1][z]) count++;
    //       if (!BCond[x][y + 1][z]) count++;
    //       if (!BCond[x][y][z - 1]) count++;
    //       if (!BCond[x][y][z + 1]) count++;

    //       if (count > 0) {
    //         if (!BCond[x - 1][y][z]) Veloc[x - 1][y][z][0]+= divergence[x][y][z] / float(count);
    //         if (!BCond[x + 1][y][z]) Veloc[x + 1][y][z][0]-= divergence[x][y][z] / float(count);
    //         if (!BCond[x][y - 1][z]) Veloc[x][y - 1][z][1]+= divergence[x][y][z] / float(count);
    //         if (!BCond[x][y + 1][z]) Veloc[x][y + 1][z][1]-= divergence[x][y][z] / float(count);
    //         if (!BCond[x][y][z - 1]) Veloc[x][y][z - 1][2]+= divergence[x][y][z] / float(count);
    //         if (!BCond[x][y][z + 1]) Veloc[x][y][z + 1][2]-= divergence[x][y][z] / float(count);
    //       }
    //     }
    //   }
    // }
  }

  // Calculate the pressure for display
  for (int x= 0; x < nbX; x++) {
    for (int y= 0; y < nbY; y++) {
      for (int z= 0; z < nbZ; z++) {
        Press[x][y][z]= 0.0;
        if (x - 1 >= 0) Press[x][y][z]-= Veloc[x - 1][y][z][0];
        if (x + 1 < nbX) Press[x][y][z]+= Veloc[x + 1][y][z][0];
        if (y - 1 >= 0) Press[x][y][z]-= Veloc[x][y - 1][z][1];
        if (y + 1 < nbY) Press[x][y][z]+= Veloc[x][y + 1][z][1];
        if (z - 1 >= 0) Press[x][y][z]-= Veloc[x][y][z - 1][2];
        if (z + 1 < nbZ) Press[x][y][z]+= Veloc[x][y][z + 1][2];
        Press[x][y][z]= -Press[x][y][z];
      }
    }
  }
}


void CompuFluidDyn::Draw() {
  if (!isInitialized) return;
  if (!isRefreshed) return;

  int maxDim= std::max(std::max(nbX, nbY), nbZ);
  float voxSize= 1.0 / float(maxDim);

  // Draw the solid voxels
  if (D.displayMode1) {
    glPushMatrix();
    glTranslatef(0.5f - 0.5f * float(nbX) / float(maxDim), 0.5f - 0.5f * float(nbY) / float(maxDim), 0.5f - 0.5f * float(nbZ) / float(maxDim));
    glScalef(voxSize, voxSize, voxSize);
    glTranslatef(0.5f, 0.5f, 0.5f);
    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          if (nbX == 3 && (x == 0 || x == 2)) continue;
          if (nbY == 3 && (y == 0 || y == 2)) continue;
          if (nbZ == 3 && (z == 0 || z == 2)) continue;
          if (Solid[x][y][z] > 0.5f) {
            glPushMatrix();
            glTranslatef(float(x), float(y), float(z));
            glColor3f(0.5f, 0.5f, 0.5f);
            glutSolidCube(1.0);
            glPopMatrix();
          }
        }
      }
    }
    glPopMatrix();
  }

  // Draw the cell velocity field
  if (D.displayMode2) {
    glPushMatrix();
    glTranslatef(0.5f - 0.5f * float(nbX) / float(maxDim), 0.5f - 0.5f * float(nbY) / float(maxDim), 0.5f - 0.5f * float(nbZ) / float(maxDim));
    glScalef(voxSize, voxSize, voxSize);
    glTranslatef(0.5f, 0.5f, 0.5f);
    glBegin(GL_LINES);
    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          if (nbX == 3 && (x == 0 || x == 2)) continue;
          if (nbY == 3 && (y == 0 || y == 2)) continue;
          if (nbZ == 3 && (z == 0 || z == 2)) continue;
          Math::Vec3f vec= Veloc[x][y][z];
          float r= 0.0f, g= 0.0f, b= 0.0f;
          if (vec.normSquared() > 0.0)
            Colormap::RatioToJetBrightSmooth(vec.norm() * std::max(D.param[ColorFactor_________].val, 0.0), r, g, b);
          glColor3f(r, g, b);
          Math::Vec3f pos= Math::Vec3f(float(x), float(y), float(z));
          glVertex3fv(pos.array());
          glVertex3fv(pos + vec * std::max(D.param[ScaleFactor_________].val, 0.0));
        }
      }
    }
    glEnd();
    glPopMatrix();
  }

  // Draw the pressure field
  if (D.displayMode3) {
    glPointSize(3.0f);
    glPushMatrix();
    glTranslatef(0.5f - 0.5f * float(nbX) / float(maxDim), 0.5f - 0.5f * float(nbY) / float(maxDim), 0.5f - 0.5f * float(nbZ) / float(maxDim));
    glScalef(voxSize, voxSize, voxSize);
    glTranslatef(0.5f, 0.5f, 0.5f);
    glBegin(GL_POINTS);
    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          if (nbX == 3 && (x == 0 || x == 2)) continue;
          if (nbY == 3 && (y == 0 || y == 2)) continue;
          if (nbZ == 3 && (z == 0 || z == 2)) continue;
          float r= 0.0f, g= 0.0f, b= 0.0f;
          Colormap::RatioToJetBrightSmooth(0.5f + Press[x][y][z] * std::max(D.param[ColorFactor_________].val, 0.0), r, g, b);
          glColor3f(r, g, b);
          glVertex3f(float(x), float(y), float(z));
        }
      }
    }
    glEnd();
    glPopMatrix();
    glPointSize(1.0f);
  }

  // // Draw the staggered grid coefficients
  // if (D.displayMode4) {
  //   glPointSize(3.0f);
  //   glPushMatrix();
  //   glTranslatef(0.5f - 0.5f * float(nbX) / float(maxDim), 0.5f - 0.5f * float(nbY) / float(maxDim), 0.5f - 0.5f * float(nbZ) / float(maxDim));
  //   glScalef(voxSize, voxSize, voxSize);
  //   glBegin(GL_POINTS);
  //   for (int x= 0; x < nbX + 1; x++) {
  //     for (int y= 0; y < nbY + 1; y++) {
  //       for (int z= 0; z < nbZ + 1; z++) {
  //         float r, g, b;

  //         if (y < nbY && z < nbZ) {
  //           Colormap::RatioToJetBrightSmooth(velX[x][y][z] * std::max(D.param[ColorFactor_________].val, 0.0), r, g, b);
  //           glColor3f(r, g, b);
  //           glVertex3f(float(x), float(y) + 0.5f, float(z) + 0.5f);
  //         }

  //         if (x < nbX && z < nbZ) {
  //           Colormap::RatioToJetBrightSmooth(velY[x][y][z] * std::max(D.param[ColorFactor_________].val, 0.0), r, g, b);
  //           glColor3f(r, g, b);
  //           glVertex3f(float(x) + 0.5f, float(y), float(z) + 0.5f);
  //         }

  //         if (x < nbX && y < nbY) {
  //           Colormap::RatioToJetBrightSmooth(velZ[x][y][z] * std::max(D.param[ColorFactor_________].val, 0.0), r, g, b);
  //           glColor3f(r, g, b);
  //           glVertex3f(float(x) + 0.5f, float(y) + 0.5f, float(z));
  //         }
  //       }
  //     }
  //   }
  //   glEnd();
  //   glPopMatrix();
  //   glPointSize(1.0f);
  // }
}


/*
#include "CompuFluidDyn.hpp"


// Standard lib
#include <cstdio>
#include <vector>

// GLUT lib
#include <GL/freeglut.h>

// Project lib
#include "../Data.hpp"
#include "../math/Vectors.hpp"
#include "../util/Colormap.hpp"
#include "../util/Field.hpp"
#include "../util/Sampling.hpp"


extern Data D;

enum ParamType
{
  ResolutionX_________,
  ResolutionY_________,
  ResolutionZ_________,
  SimSteps____________,
  DivGaussSeidelIter__,
  DivJacobiIter_______,
  Overrelaxation______,
  TimeStep____________,
  InletVelocity_______,
  ObstacleSize________,
  ObstaclePosX________,
  ObstaclePosY________,
  ObstaclePosZ________,
  ScaleFactor_________,
  ColorFactor_________,
};

// Math::Vec3f CompuFluidDyn::SampleStaggeredGrids(Math::Vec3f const &posNode, Math::Vec3f const &posStart) {
//   if (posNode[0] < 0.0f || posNode[0] > float(nbX)) printf("fuckup\n");
//   if (posNode[1] < 0.0f || posNode[1] > float(nbY)) printf("fuckup\n");
//   if (posNode[2] < 0.0f || posNode[2] > float(nbZ)) printf("fuckup\n");

//   Math::Vec3f vel(0.0f, 0.0f, 0.0f);
//   for (int dim= 0; dim < 3; dim++) {
//     int x0= (dim == 0) ? int(std::floor(posNode[0])) : int(std::floor(posNode[0] - 0.5f));
//     int y0= (dim == 1) ? int(std::floor(posNode[1])) : int(std::floor(posNode[1] - 0.5f));
//     int z0= (dim == 2) ? int(std::floor(posNode[2])) : int(std::floor(posNode[2] - 0.5f));
//     int x1= x0 + 1;
//     int y1= y0 + 1;
//     int z1= z0 + 1;

//     x0= std::max(x0, 0);
//     y0= std::max(y0, 0);
//     z0= std::max(z0, 0);
//     x1= (dim == 0) ? std::min(x1, nbX) : std::min(x1, nbX - 1);
//     y1= (dim == 1) ? std::min(y1, nbY) : std::min(y1, nbY - 1);
//     z1= (dim == 2) ? std::min(z1, nbZ) : std::min(z1, nbZ - 1);

//     float v000= velX[x0][y0][z0]

//     // vel[0]+=

//     // if ()
//     //   int count= 0;
//     // for (int x= std::max(x0, 0); x <= std::min(x1, nbX); x++) {
//     //   for (int y= std::max(y0, 0); y <= std::min(y1, nbY); y++) {
//     //     for (int z= std::max(z0, 0); z <= std::min(z1, nbZ); z++) {
//     //       if (dim == 0) vel[dim]+= velX[x][y][z];
//     //       if (dim == 1) vel[dim]+= velY[x][y][z];
//     //       if (dim == 2) vel[dim]+= velZ[x][y][z];
//     //       count++;
//     //     }
//     //   }
//     // }
//   }
//   return vel;
// }


CompuFluidDyn::CompuFluidDyn() {
  isInitialized= false;
  isRefreshed= false;
}


void CompuFluidDyn::Init() {
  isInitialized= true;
  isRefreshed= false;

  D.param.clear();
  D.param.push_back(ParamUI("ResolutionX_________", 1));
  D.param.push_back(ParamUI("ResolutionY_________", 20));
  D.param.push_back(ParamUI("ResolutionZ_________", 20));
  D.param.push_back(ParamUI("SimSteps____________", 50));
  D.param.push_back(ParamUI("DivGaussSeidelIter__", 100));
  D.param.push_back(ParamUI("DivJacobiIter_______", 100));
  D.param.push_back(ParamUI("Overrelaxation______", 1.9));
  D.param.push_back(ParamUI("TimeStep____________", 0.0));
  D.param.push_back(ParamUI("InletVelocity_______", 1.0));
  D.param.push_back(ParamUI("ObstacleSize________", 0.2));
  D.param.push_back(ParamUI("ObstaclePosX________", 0.5));
  D.param.push_back(ParamUI("ObstaclePosY________", 0.3));
  D.param.push_back(ParamUI("ObstaclePosZ________", 0.7));
  D.param.push_back(ParamUI("ScaleFactor_________", 1.0));
  D.param.push_back(ParamUI("ColorFactor_________", 1.0));

  D.displayMode3= false;
  D.displayMode4= false;
}


void CompuFluidDyn::Refresh() {
  if (!isInitialized) return;
  isRefreshed= true;

  nbX= std::max(int(std::round(D.param[ResolutionX_________].val)), 1);
  nbY= std::max(int(std::round(D.param[ResolutionY_________].val)), 1);
  nbZ= std::max(int(std::round(D.param[ResolutionZ_________].val)), 1);
  simSteps= 0;

  density= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  velX= Field::AllocField3D(nbX + 1, nbY, nbZ, 0.0f);
  velY= Field::AllocField3D(nbX, nbY + 1, nbZ, 0.0f);
  velZ= Field::AllocField3D(nbX, nbY, nbZ + 1, 0.0f);
  lockX= Field::AllocField3D(nbX + 1, nbY, nbZ, false);
  lockY= Field::AllocField3D(nbX, nbY + 1, nbZ, false);
  lockZ= Field::AllocField3D(nbX, nbY, nbZ + 1, false);

  for (int x= 0; x < nbX; x++) {
    for (int y= 0; y < nbY; y++) {
      for (int z= 0; z < nbZ; z++) {
        // Lock velocities leaving the domain
        if (x == 0) {
          velX[x + 0][y][z]= 0.0f;
          lockX[x + 0][y][z]= true;
        }
        if (x == nbX - 1) {
          velX[x + 1][y][z]= 0.0f;
          lockX[x + 1][y][z]= true;
        }
        if (y == 0) {
          velY[x][y + 0][z]= 0.0f;
          lockY[x][y + 0][z]= true;
        }
        if (y == nbY - 1) {
          velY[x][y + 1][z]= 0.0f;
          lockY[x][y + 1][z]= true;
        }
        if (z == 0) {
          velZ[x][y][z + 0]= 0.0f;
          lockZ[x][y][z + 0]= true;
        }
        if (z == nbZ - 1) {
          velZ[x][y][z + 1]= 0.0f;
          lockZ[x][y][z + 1]= true;
        }
        // Add locked inlet on Y- side
        if (y == 0) {
          velY[x][y + 0][z]= D.param[InletVelocity_______].val;
          lockY[x][y + 0][z]= true;
        }
        // Add unlocked outlet on Y+ side
        if (y == nbY - 1) {
          velY[x][y + 1][z]= 0.0;
          lockY[x][y + 1][z]= false;
        }
        // Add locked obstacle
        Math::Vec3f posCell((float(x) + 0.5f) / float(nbX), (float(y) + 0.5f) / float(nbY), (float(z) + 0.5f) / float(nbZ));
        Math::Vec3f posObstacle(D.param[ObstaclePosX________].val, D.param[ObstaclePosY________].val, D.param[ObstaclePosZ________].val);
        if ((posCell - posObstacle).normSquared() <= std::pow(std::max(D.param[ObstacleSize________].val, 0.0), 2.0)) {
          density[x][y][z]= 1.0f;
          velX[x + 0][y][z]= 0.0f;
          velX[x + 1][y][z]= 0.0f;
          lockX[x + 0][y][z]= true;
          lockX[x + 1][y][z]= true;
          velY[x][y + 0][z]= 0.0f;
          velY[x][y + 1][z]= 0.0f;
          lockY[x][y + 0][z]= true;
          lockY[x][y + 1][z]= true;
          velZ[x][y][z + 0]= 0.0f;
          velZ[x][y][z + 1]= 0.0f;
          lockZ[x][y][z + 0]= true;
          lockZ[x][y][z + 1]= true;
        }
      }
    }
  }
}


void CompuFluidDyn::Animate() {
  if (!isInitialized) return;
  if (!isRefreshed) return;

  if (simSteps >= int(std::round(D.param[SimSteps____________].val)))
    return;
  simSteps++;

  // Solve for incompressibility via Gauss Seidel
  for (int k= 0; k < int(std::round(D.param[DivGaussSeidelIter__].val)); k++) {
    std::vector<std::vector<std::vector<float>>> divergence= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          divergence[x][y][z]= 0.0;
          divergence[x][y][z]-= velX[x + 0][y][z];
          divergence[x][y][z]+= velX[x + 1][y][z];
          divergence[x][y][z]-= velY[x][y + 0][z];
          divergence[x][y][z]+= velY[x][y + 1][z];
          divergence[x][y][z]-= velZ[x][y][z + 0];
          divergence[x][y][z]+= velZ[x][y][z + 1];

          int count= 0;
          if (!lockX[x + 0][y][z]) count++;
          if (!lockX[x + 1][y][z]) count++;
          if (!lockY[x][y + 0][z]) count++;
          if (!lockY[x][y + 1][z]) count++;
          if (!lockZ[x][y][z + 0]) count++;
          if (!lockZ[x][y][z + 1]) count++;

          if (count > 0) {
            if (!lockX[x + 0][y][z]) velX[x + 0][y][z]+= D.param[Overrelaxation______].val * divergence[x][y][z] / float(count);
            if (!lockX[x + 1][y][z]) velX[x + 1][y][z]-= D.param[Overrelaxation______].val * divergence[x][y][z] / float(count);
            if (!lockY[x][y + 0][z]) velY[x][y + 0][z]+= D.param[Overrelaxation______].val * divergence[x][y][z] / float(count);
            if (!lockY[x][y + 1][z]) velY[x][y + 1][z]-= D.param[Overrelaxation______].val * divergence[x][y][z] / float(count);
            if (!lockZ[x][y][z + 0]) velZ[x][y][z + 0]+= D.param[Overrelaxation______].val * divergence[x][y][z] / float(count);
            if (!lockZ[x][y][z + 1]) velZ[x][y][z + 1]-= D.param[Overrelaxation______].val * divergence[x][y][z] / float(count);
          }
        }
      }
    }
  }
  // Solve for incompressibility via Jacobi
  for (int k= 0; k < int(std::round(D.param[DivJacobiIter_______].val)); k++) {
    std::vector<std::vector<std::vector<float>>> divergence= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          divergence[x][y][z]= 0.0;
          divergence[x][y][z]-= velX[x + 0][y][z];
          divergence[x][y][z]+= velX[x + 1][y][z];
          divergence[x][y][z]-= velY[x][y + 0][z];
          divergence[x][y][z]+= velY[x][y + 1][z];
          divergence[x][y][z]-= velZ[x][y][z + 0];
          divergence[x][y][z]+= velZ[x][y][z + 1];
        }
      }
    }
    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          int count= 0;
          if (!lockX[x + 0][y][z]) count++;
          if (!lockX[x + 1][y][z]) count++;
          if (!lockY[x][y + 0][z]) count++;
          if (!lockY[x][y + 1][z]) count++;
          if (!lockZ[x][y][z + 0]) count++;
          if (!lockZ[x][y][z + 1]) count++;

          if (count > 0) {
            if (!lockX[x + 0][y][z]) velX[x + 0][y][z]+= divergence[x][y][z] / float(count);
            if (!lockX[x + 1][y][z]) velX[x + 1][y][z]-= divergence[x][y][z] / float(count);
            if (!lockY[x][y + 0][z]) velY[x][y + 0][z]+= divergence[x][y][z] / float(count);
            if (!lockY[x][y + 1][z]) velY[x][y + 1][z]-= divergence[x][y][z] / float(count);
            if (!lockZ[x][y][z + 0]) velZ[x][y][z + 0]+= divergence[x][y][z] / float(count);
            if (!lockZ[x][y][z + 1]) velZ[x][y][z + 1]-= divergence[x][y][z] / float(count);
          }
        }
      }
    }
  }

  // Save the pressure for display
  pressure= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  for (int x= 0; x < nbX; x++) {
    for (int y= 0; y < nbY; y++) {
      for (int z= 0; z < nbZ; z++) {
        pressure[x][y][z]= 0.0;
        pressure[x][y][z]-= velX[x + 0][y][z];
        pressure[x][y][z]+= velX[x + 1][y][z];
        pressure[x][y][z]-= velY[x][y + 0][z];
        pressure[x][y][z]+= velY[x][y + 1][z];
        pressure[x][y][z]-= velZ[x][y][z + 0];
        pressure[x][y][z]+= velZ[x][y][z + 1];

        pressure[x][y][z]= -pressure[x][y][z];
      }
    }
  }

  // Compute velocity advection
  if (D.param[TimeStep____________].val >= 0.0) {
    std::vector<std::vector<std::vector<float>>> velXold= velX;
    std::vector<std::vector<std::vector<float>>> velYold= velY;
    std::vector<std::vector<std::vector<float>>> velZold= velZ;

    // for (int x= 0; x < nbX + 1; x++) {
    //   for (int y= 0; y < nbY + 1; y++) {
    //     for (int z= 0; z < nbZ + 1; z++) {
    //       if (y < nbY && z < nbZ) {
    //         Math::Vec3f posNode(float(x), float(y) + 0.5f, float(z) + 0.5f);
    //         Math::Vec3f velNode(velXold[x][y][z], 0.0f, 0.0f);
    //         int count= 0;
    //         if (x - 1 >= 0) {
    //           velNode[1]+= 0.5 * (velYold[x - 1][y][z] + velYold[x - 1][y + 1][z]);
    //           velNode[2]+= 0.5 * (velZold[x - 1][y][z] + velZold[x - 1][y][z + 1]);
    //           count++;
    //         }
    //         if (x <= nbX - 1) {
    //           velNode[1]+= 0.5 * (velYold[x][y][z] + velYold[x][y + 1][z]);
    //           velNode[2]+= 0.5 * (velZold[x][y][z] + velZold[x][y][z + 1]);
    //           count++;
    //         }
    //         velNode[1]/= float(count);
    //         velNode[2]/= float(count);
    //         Math::Vec3f posNodeOff= posNode - velNode * D.param[TimeStep____________].val;

    //         int xOff= int(std::floor(posNodeOff[0]));
    //         int yOff= int(std::floor(posNodeOff[1]));
    //         int zOff= int(std::floor(posNodeOff[2]));
    //         if (xOff < 0 || xOff >= nbX || yOff < 0 || yOff >= nbY || zOff < 0 || zOff >= nbZ) continue;
    //         float relXOff= posNodeOff[0] - std::floor(posNodeOff[0]);
    //         float relYOff= posNodeOff[1] - std::floor(posNodeOff[1]);
    //         float relZOff= posNodeOff[2] - std::floor(posNodeOff[2]);

    //         if (!lockX[x][y][z]) velX[x][y][z]= (1.0f - relXOff) * velXold[xOff + 0][yOff][zOff] + (relXOff + 0.0f) * velXold[xOff + 1][yOff][zOff];
    //       }

    //       if (x < nbX && z < nbZ) {
    //       }

    //       if (x < nbX && y < nbY) {
    //       }
    //     }
    //   }
    // }

    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          Math::Vec3f posCell(float(x) + 0.5f, float(y) + 0.5f, float(z) + 0.5f);
          Sampling::Get3DScalarFieldVal(
              velXold,
              std::array<double, 3>{float(x), float(y) + 0.5f, float(z) + 0.5f}, false,
              std::array<double, 3>{},
              std::array<double, 3>{});

          Math::Vec3f velCell(
              0.5f * (velXold[x][y][z] + velXold[x + 1][y][z]),
              0.5f * (velYold[x][y][z] + velYold[x][y + 1][z]),
              0.5f * (velZold[x][y][z] + velZold[x][y][z + 1]));
          Math::Vec3f posCellOff= posCell - velCell * D.param[TimeStep____________].val;

          int xOff= int(std::floor(posCellOff[0]));
          int yOff= int(std::floor(posCellOff[1]));
          int zOff= int(std::floor(posCellOff[2]));
          if (xOff < 0 || xOff >= nbX || yOff < 0 || yOff >= nbY || zOff < 0 || zOff >= nbZ) continue;
          float relXOff= posCellOff[0] - std::floor(posCellOff[0]);
          float relYOff= posCellOff[1] - std::floor(posCellOff[1]);
          float relZOff= posCellOff[2] - std::floor(posCellOff[2]);
          Math::Vec3f velCellOff;
          velCellOff[0]= (1.0f - relXOff) * velXold[xOff + 0][yOff][zOff] + (relXOff + 0.0f) * velXold[xOff + 1][yOff][zOff];
          velCellOff[1]= (1.0f - relYOff) * velXold[xOff][yOff + 0][zOff] + (relYOff + 0.0f) * velXold[xOff][yOff + 1][zOff];
          velCellOff[2]= (1.0f - relZOff) * velXold[xOff][yOff][zOff + 0] + (relZOff + 0.0f) * velXold[xOff][yOff][zOff + 1];

          if (!lockX[x + 0][y][z]) velX[x + 0][y][z]= velCellOff[0];
          if (!lockX[x + 1][y][z]) velX[x + 1][y][z]= velCellOff[0];
          if (!lockY[x][y + 0][z]) velY[x][y + 0][z]= velCellOff[1];
          if (!lockY[x][y + 1][z]) velY[x][y + 1][z]= velCellOff[1];
          if (!lockZ[x][y][z + 0]) velZ[x][y][z + 0]= velCellOff[2];
          if (!lockZ[x][y][z + 1]) velZ[x][y][z + 1]= velCellOff[2];
        }
      }
    }
  }
}


void CompuFluidDyn::Draw() {
  if (!isInitialized) return;
  if (!isRefreshed) return;

  int maxDim= std::max(std::max(nbX, nbY), nbZ);
  float voxSize= 1.0 / float(maxDim);

  // Draw the solid voxels
  if (D.displayMode1) {
    glPushMatrix();
    glTranslatef(0.5f - 0.5f * float(nbX) / float(maxDim), 0.5f - 0.5f * float(nbY) / float(maxDim), 0.5f - 0.5f * float(nbZ) / float(maxDim));
    glScalef(voxSize, voxSize, voxSize);
    glTranslatef(0.5f, 0.5f, 0.5f);
    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          if (density[x][y][z] > 0.5f) {
            glPushMatrix();
            glTranslatef(float(x), float(y), float(z));
            glColor3f(0.5f, 0.5f, 0.5f);
            glutSolidCube(1.0);
            glPopMatrix();
          }
        }
      }
    }
    glPopMatrix();
  }

  // Draw the cell velocity field
  if (D.displayMode2) {
    glPushMatrix();
    glTranslatef(0.5f - 0.5f * float(nbX) / float(maxDim), 0.5f - 0.5f * float(nbY) / float(maxDim), 0.5f - 0.5f * float(nbZ) / float(maxDim));
    glScalef(voxSize, voxSize, voxSize);
    glTranslatef(0.5f, 0.5f, 0.5f);
    glBegin(GL_LINES);
    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          Math::Vec3f vec(velX[x][y][z] + velX[x + 1][y][z], velY[x][y][z] + velY[x][y + 1][z], velZ[x][y][z] + velZ[x][y][z + 1]);
          float r= 0.0f, g= 0.0f, b= 0.0f;
          if (vec.normSquared() > 0.0)
            Colormap::RatioToJetBrightSmooth(vec.norm() * std::max(D.param[ColorFactor_________].val, 0.0), r, g, b);
          glColor3f(r, g, b);
          Math::Vec3f pos= Math::Vec3f(float(x), float(y), float(z));
          glVertex3fv(pos.array());
          glVertex3fv(pos + vec * std::max(D.param[ScaleFactor_________].val, 0.0));
        }
      }
    }
    glEnd();
    glPopMatrix();
  }

  // Draw the pressure field
  if (D.displayMode3) {
    glPushMatrix();
    glTranslatef(0.5f - 0.5f * float(nbX) / float(maxDim), 0.5f - 0.5f * float(nbY) / float(maxDim), 0.5f - 0.5f * float(nbZ) / float(maxDim));
    glScalef(voxSize, voxSize, voxSize);
    glTranslatef(0.5f, 0.5f, 0.5f);
    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          glPushMatrix();
          glTranslatef(float(x), float(y), float(z));
          float r= 0.0f, g= 0.0f, b= 0.0f;
          Colormap::RatioToJetBrightSmooth(0.5f + 0.5f * pressure[x][y][z] * std::max(D.param[ColorFactor_________].val, 0.0), r, g, b);
          glColor3f(r, g, b);
          glutSolidCube(1.0);
          glPopMatrix();
        }
      }
    }
    glPopMatrix();
  }

  // Draw the staggered grid coefficients
  if (D.displayMode4) {
    glPointSize(3.0f);
    glPushMatrix();
    glTranslatef(0.5f - 0.5f * float(nbX) / float(maxDim), 0.5f - 0.5f * float(nbY) / float(maxDim), 0.5f - 0.5f * float(nbZ) / float(maxDim));
    glScalef(voxSize, voxSize, voxSize);
    glBegin(GL_POINTS);
    for (int x= 0; x < nbX + 1; x++) {
      for (int y= 0; y < nbY + 1; y++) {
        for (int z= 0; z < nbZ + 1; z++) {
          float r, g, b;

          if (y < nbY && z < nbZ) {
            Colormap::RatioToJetBrightSmooth(velX[x][y][z] * std::max(D.param[ColorFactor_________].val, 0.0), r, g, b);
            glColor3f(r, g, b);
            glVertex3f(float(x), float(y) + 0.5f, float(z) + 0.5f);
          }

          if (x < nbX && z < nbZ) {
            Colormap::RatioToJetBrightSmooth(velY[x][y][z] * std::max(D.param[ColorFactor_________].val, 0.0), r, g, b);
            glColor3f(r, g, b);
            glVertex3f(float(x) + 0.5f, float(y), float(z) + 0.5f);
          }

          if (x < nbX && y < nbY) {
            Colormap::RatioToJetBrightSmooth(velZ[x][y][z] * std::max(D.param[ColorFactor_________].val, 0.0), r, g, b);
            glColor3f(r, g, b);
            glVertex3f(float(x) + 0.5f, float(y) + 0.5f, float(z));
          }
        }
      }
    }
    glEnd();
    glPopMatrix();
    glPointSize(1.0f);
  }
}
*/