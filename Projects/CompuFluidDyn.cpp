
#include "CompuFluidDyn.hpp"


// Standard lib
#include <cstdio>
#include <vector>

// GLUT lib
#include <GL/freeglut.h>

// Project lib
#include "../Data.hpp"
#include "../math/Fields.hpp"
#include "../math/Vectors.hpp"
#include "../util/Colormap.hpp"
#include "../util/Field.hpp"


#define IX(i, j) ((i) + (N + 2) * (j))
#define SWAP(x0, x) \
  {                 \
    float* tmp= x0; \
    x0= x;          \
    x= tmp;         \
  }

void add_source(int N, float* x, float* s, float dt) {
  for (int i= 0; i < (N + 2) * (N + 2); i++)
    x[i]+= dt * s[i];
}

void set_bnd(int N, int b, float* x) {
  for (int i= 1; i <= N; i++) {
    x[IX(0, i)]= b == 1 ? -x[IX(1, i)] : x[IX(1, i)];
    x[IX(N + 1, i)]= b == 1 ? -x[IX(N, i)] : x[IX(N, i)];
    x[IX(i, 0)]= b == 2 ? -x[IX(i, 1)] : x[IX(i, 1)];
    x[IX(i, N + 1)]= b == 2 ? -x[IX(i, N)] : x[IX(i, N)];
  }
  x[IX(0, 0)]= 0.5f * (x[IX(1, 0)] + x[IX(0, 1)]);
  x[IX(0, N + 1)]= 0.5f * (x[IX(1, N + 1)] + x[IX(0, N)]);
  x[IX(N + 1, 0)]= 0.5f * (x[IX(N, 0)] + x[IX(N + 1, 1)]);
  x[IX(N + 1, N + 1)]= 0.5f * (x[IX(N, N + 1)] + x[IX(N + 1, N)]);
}

void lin_solve(int N, int b, float* x, float* x0, float a, float c) {
  for (int k= 0; k < 20; k++) {
    for (int i= 1; i <= N; i++) {
      for (int j= 1; j <= N; j++) {
        x[IX(i, j)]= (x0[IX(i, j)] + a * (x[IX(i - 1, j)] + x[IX(i + 1, j)] + x[IX(i, j - 1)] + x[IX(i, j + 1)])) / c;
      }
    }
    set_bnd(N, b, x);
  }
}

void diffuse(int N, int b, float* x, float* x0, float diff, float dt) {
  float a= dt * diff * N * N;
  lin_solve(N, b, x, x0, a, 1 + 4 * a);
}

void advect(int N, int b, float* d, float* d0, float* u, float* v, float dt) {
  float dt0= dt * N;
  for (int i= 1; i <= N; i++) {
    for (int j= 1; j <= N; j++) {
      float x= i - dt0 * u[IX(i, j)];
      float y= j - dt0 * v[IX(i, j)];
      if (x < 0.5f) x= 0.5f;
      if (x > N + 0.5f) x= N + 0.5f;
      int i0= (int)x;
      int i1= i0 + 1;
      if (y < 0.5f) y= 0.5f;
      if (y > N + 0.5f) y= N + 0.5f;
      int j0= (int)y;
      int j1= j0 + 1;
      float s1= x - i0;
      float s0= 1 - s1;
      float t1= y - j0;
      float t0= 1 - t1;
      d[IX(i, j)]= s0 * (t0 * d0[IX(i0, j0)] + t1 * d0[IX(i0, j1)]) + s1 * (t0 * d0[IX(i1, j0)] + t1 * d0[IX(i1, j1)]);
    }
  }

  set_bnd(N, b, d);
}

void project(int N, float* u, float* v, float* p, float* div) {
  for (int i= 1; i <= N; i++) {
    for (int j= 1; j <= N; j++) {
      div[IX(i, j)]= -0.5f * (u[IX(i + 1, j)] - u[IX(i - 1, j)] + v[IX(i, j + 1)] - v[IX(i, j - 1)]) / N;
      p[IX(i, j)]= 0;
    }
  }
  set_bnd(N, 0, div);
  set_bnd(N, 0, p);

  lin_solve(N, 0, p, div, 1, 4);

  for (int i= 1; i <= N; i++) {
    for (int j= 1; j <= N; j++) {
      u[IX(i, j)]-= 0.5f * N * (p[IX(i + 1, j)] - p[IX(i - 1, j)]);
      v[IX(i, j)]-= 0.5f * N * (p[IX(i, j + 1)] - p[IX(i, j - 1)]);
    }
  }
  set_bnd(N, 1, u);
  set_bnd(N, 2, v);
}

void dens_step(int N, float* x, float* x0, float* u, float* v, float diff, float dt) {
  add_source(N, x, x0, dt);
  SWAP(x0, x);
  diffuse(N, 0, x, x0, diff, dt);
  SWAP(x0, x);
  advect(N, 0, x, x0, u, v, dt);
}

void vel_step(int N, float* u, float* v, float* u0, float* v0, float visc, float dt) {
  add_source(N, u, u0, dt);
  add_source(N, v, v0, dt);
  SWAP(u0, u);
  SWAP(v0, v);
  diffuse(N, 1, u, u0, visc, dt);
  diffuse(N, 2, v, v0, visc, dt);
  project(N, u, v, u0, v0);
  SWAP(u0, u);
  SWAP(v0, v);
  advect(N, 1, u, u0, u0, v0, dt);
  advect(N, 2, v, v0, u0, v0, dt);
  project(N, u, v, u0, v0);
}

void CompuFluidDyn::AllocateInitializeFields() {
  int size= (nbZ + 2) * (nbZ * 2);
  VelXNew= new float[size];
  VelYNew= new float[size];
  VelZNew= new float[size];
  VelXOld= new float[size];
  VelYOld= new float[size];
  VelZOld= new float[size];
  DensNew= new float[size];
  DensOld= new float[size];
  for (int k= 0; k < (nbZ + 2) * (nbZ * 2); k++) {
    VelXNew[k]= VelYNew[k]= VelZNew[k]= 0.0f;
    VelXOld[k]= VelYOld[k]= VelZOld[k]= 0.0f;
    DensNew[k]= 0.0f;
    DensOld[k]= 0.0f;
  }
}

void CompuFluidDyn::DeallocateFields() {
  if (VelXNew != NULL) delete[] VelXNew;
  if (VelYNew != NULL) delete[] VelYNew;
  if (VelZNew != NULL) delete[] VelZNew;
  if (VelXOld != NULL) delete[] VelXOld;
  if (VelYOld != NULL) delete[] VelYOld;
  if (VelZOld != NULL) delete[] VelZOld;
  if (DensNew != NULL) delete[] DensNew;
  if (DensOld != NULL) delete[] DensOld;
}


extern Data D;

enum ParamType
{
  ResolutionX_________,
  ResolutionY_________,
  ResolutionZ_________,
  TimeStepSize________,
  CoeffDiffusion______,
  CoeffViscosity______,
  CoeffForceX_________,
  CoeffForceY_________,
  CoeffForceZ_________,
  CoeffSource_________,
  ObstaclePosX________,
  ObstaclePosY________,
  ObstaclePosZ________,
  ObstacleSize________,
  ScaleFactor_________,
  ColorFactor_________,
};


CompuFluidDyn::CompuFluidDyn() {
  isInitialized= false;
  isRefreshed= false;
}


CompuFluidDyn::~CompuFluidDyn() {
  DeallocateFields();
  isInitialized= false;
  isRefreshed= false;
}


void CompuFluidDyn::Init() {
  isInitialized= true;
  isRefreshed= false;

  D.param.clear();
  D.param.push_back(ParamUI("ResolutionX_________", 1));
  D.param.push_back(ParamUI("ResolutionY_________", 100));
  D.param.push_back(ParamUI("ResolutionZ_________", 100));
  D.param.push_back(ParamUI("TimeStepSize________", 0.1));
  D.param.push_back(ParamUI("CoeffDiffusion______", 0.0));
  D.param.push_back(ParamUI("CoeffViscosity______", 0.0));
  D.param.push_back(ParamUI("CoeffForceX_________", 0.0));
  D.param.push_back(ParamUI("CoeffForceY_________", 0.5));
  D.param.push_back(ParamUI("CoeffForceZ_________", 0.2));
  D.param.push_back(ParamUI("CoeffSource_________", 1.0));
  D.param.push_back(ParamUI("ObstaclePosX________", 0.5));
  D.param.push_back(ParamUI("ObstaclePosY________", 0.2));
  D.param.push_back(ParamUI("ObstaclePosZ________", 0.6));
  D.param.push_back(ParamUI("ObstacleSize________", 0.1));
  D.param.push_back(ParamUI("ScaleFactor_________", 100.0));
  D.param.push_back(ParamUI("ColorFactor_________", 1.0));
}


void CompuFluidDyn::CheckNeedRefresh() {
  if (nbX != std::max(int(std::round(D.param[ParamType::ResolutionX_________].val)), 1)) isRefreshed= false;
  if (nbY != std::max(int(std::round(D.param[ParamType::ResolutionY_________].val)), 1)) isRefreshed= false;
  if (nbZ != std::max(int(std::round(D.param[ParamType::ResolutionZ_________].val)), 1)) isRefreshed= false;

  nbX= std::max(int(std::round(D.param[ParamType::ResolutionX_________].val)), 1);
  nbY= std::max(int(std::round(D.param[ParamType::ResolutionY_________].val)), 1);
  nbZ= std::max(int(std::round(D.param[ParamType::ResolutionZ_________].val)), 1);
}


void CompuFluidDyn::Refresh() {
  if (!isInitialized) return;
  CheckNeedRefresh();
  if (isRefreshed) return;

  // Allocated fields
  Press= Math::Field3D<float>(nbX, nbY, nbZ, 0.0f);
  Densi= Math::Field3D<float>(nbX, nbY, nbZ, 0.0f);
  Solid= Math::Field3D<int>(nbX, nbY, nbZ, 0);
  Force= Math::Field3D<int>(nbX, nbY, nbZ, 0);
  VelCu= Math::Field3D<Math::Vec3f>(nbX, nbY, nbZ, Math::Vec3f(0.0f, 0.0f, 0.0f));

  // Deallocate fields if they already exist
  DeallocateFields();

  // Allocate and initialize new fields with correct size
  AllocateInitializeFields();

  isRefreshed= true;
}


void CompuFluidDyn::Animate() {
  if (!isInitialized) return;
  if (!isRefreshed) return;

  // Initialize problem
  for (int x= 0; x < nbX; x++) {
    for (int y= 0; y < nbY; y++) {
      for (int z= 0; z < nbZ; z++) {
        Solid(x, y, z)= 0;
        Force(x, y, z)= 0;

        // // Add walls on X and Z faces
        // if (x == 0 || x == nbX - 1 || z == 0 || z == nbZ - 1) {
        //   Solid(x, y, z)= 1;
        // }

        // // Set inlet on Y-
        // if (y == 0 && x > 0 && x < nbX - 1 && z > 0 && z < nbZ - 1) {
        //   Force(x, y, z)= 1;
        // }

        // Add obstacle
        {
          Math::Vec3f posCell((float(x) + 0.5f) / float(nbX), (float(y) + 0.5f) / float(nbY), (float(z) + 0.5f) / float(nbZ));
          Math::Vec3f posObstacle(D.param[ParamType::ObstaclePosX________].val, D.param[ParamType::ObstaclePosY________].val, D.param[ParamType::ObstaclePosZ________].val);
          if ((posCell - posObstacle).normSquared() <= std::pow(std::max(D.param[ParamType::ObstacleSize________].val, 0.0), 2.0)) {
            Force(x, y, z)= 1;
          }
        }

        // Add obstacle
        {
          Math::Vec3f posCell((float(x) + 0.5f) / float(nbX), (float(y) + 0.5f) / float(nbY), (float(z) + 0.5f) / float(nbZ));
          Math::Vec3f posObstacle(1.0 - D.param[ParamType::ObstaclePosX________].val, 1.0 - D.param[ParamType::ObstaclePosY________].val, 1.0 - D.param[ParamType::ObstaclePosZ________].val);
          if ((posCell - posObstacle).normSquared() <= std::pow(std::max(D.param[ParamType::ObstacleSize________].val, 0.0), 2.0)) {
            Force(x, y, z)= -1;
          }
        }

        // Add obstacle
        {
          Math::Vec3f posCell((float(x) + 0.5f) / float(nbX), (float(y) + 0.5f) / float(nbY), (float(z) + 0.5f) / float(nbZ));
          Math::Vec3f posObstacle(0.5, 0.5, 0.5);
          if ((posCell - posObstacle).normSquared() <= std::pow(std::max(D.param[ParamType::ObstacleSize________].val, 0.0), 2.0)) {
            Solid(x, y, z)= 1;
          }
        }
      }
    }
  }

  for (int k= 0; k < (nbZ + 2) * (nbZ * 2); k++) {
    VelXOld[k]= VelYOld[k]= VelZOld[k]= 0.0f;
    DensOld[k]= 0.0f;
  }

  for (int y= 0; y < nbY; y++) {
    for (int z= 0; z < nbZ; z++) {
      int N= nbZ;

      if (Solid(0, y, z) != 0) {
        VelXNew[IX(y + 1, z + 1)]= 0.0f;
        VelYNew[IX(y + 1, z + 1)]= 0.0f;
        VelZNew[IX(y + 1, z + 1)]= 0.0f;
        DensNew[IX(y + 1, z + 1)]= 0.0f;
      }

      if (Force(0, y, z) != 0) {
        VelXOld[IX(y + 1, z + 1)]= float(Force(0, y, z)) * D.param[ParamType::CoeffForceX_________].val;
        VelYOld[IX(y + 1, z + 1)]= float(Force(0, y, z)) * D.param[ParamType::CoeffForceY_________].val;
        VelZOld[IX(y + 1, z + 1)]= float(Force(0, y, z)) * D.param[ParamType::CoeffForceZ_________].val;
        DensOld[IX(y + 1, z + 1)]= float(Force(0, y, z)) * D.param[ParamType::CoeffSource_________].val;
      }
    }
  }

  vel_step(nbZ, VelYNew, VelZNew, VelYOld, VelZOld, D.param[ParamType::CoeffViscosity______].val, D.param[ParamType::TimeStepSize________].val);
  dens_step(nbZ, DensNew, DensOld, VelYNew, VelZNew, D.param[ParamType::CoeffDiffusion______].val, D.param[ParamType::TimeStepSize________].val);

  for (int k= 0; k < (nbZ + 2) * (nbZ * 2); k++) {
    if (std::isnan(VelXNew[k])) throw;
    if (std::isnan(VelYNew[k])) throw;
    if (std::isnan(VelZNew[k])) throw;
    if (std::isnan(VelXOld[k])) throw;
    if (std::isnan(VelYOld[k])) throw;
    if (std::isnan(VelZOld[k])) throw;
    if (std::isnan(DensNew[k])) throw;
    if (std::isnan(DensOld[k])) throw;
  }

  for (int y= 0; y < nbY; y++) {
    for (int z= 0; z < nbZ; z++) {
      int N= nbZ;
      VelCu(0, y, z)[0]= 0.0f;
      VelCu(0, y, z)[1]= VelYNew[IX(y + 1, z + 1)];
      VelCu(0, y, z)[2]= VelZNew[IX(y + 1, z + 1)];
      Press(0, y, z)= DensNew[IX(y + 1, z + 1)];
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
          if (Solid(x, y, z) != 0) {
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
    glLineWidth(2.0f);
    glPushMatrix();
    glTranslatef(0.5f - 0.5f * float(nbX) / float(maxDim), 0.5f - 0.5f * float(nbY) / float(maxDim), 0.5f - 0.5f * float(nbZ) / float(maxDim));
    glScalef(voxSize, voxSize, voxSize);
    glTranslatef(0.5f, 0.5f, 0.5f);
    glBegin(GL_LINES);
    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          Math::Vec3f vec= VelCu(x, y, z);
          float r= 0.0f, g= 0.0f, b= 0.0f;
          if (vec.normSquared() > 0.0)
            Colormap::RatioToJetBrightSmooth(vec.norm() * D.param[ParamType::ColorFactor_________].val, r, g, b);
          glColor3f(r, g, b);
          Math::Vec3f pos= Math::Vec3f(float(x), float(y), float(z));
          glVertex3fv(pos.array());
          glVertex3fv(pos + vec * D.param[ParamType::ScaleFactor_________].val);
        }
      }
    }
    glEnd();
    glPopMatrix();
    glLineWidth(1.0f);
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
          float r= 0.0f, g= 0.0f, b= 0.0f;
          Colormap::RatioToJetBrightSmooth(0.5f + 0.5f * Press(x, y, z) * D.param[ParamType::ColorFactor_________].val, r, g, b);
          glColor3f(r, g, b);
          glVertex3f(float(x), float(y), float(z));
        }
      }
    }
    glEnd();
    glPopMatrix();
    glPointSize(1.0f);
  }
}
