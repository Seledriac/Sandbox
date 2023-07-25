
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


#define IX(i, j) ((i) + (N + 2) * (j))
#define SWAP(x0, x) \
  {                 \
    float *tmp= x0; \
    x0= x;          \
    x= tmp;         \
  }
#define FOR_EACH_CELL       \
  for (i= 1; i <= N; i++) { \
    for (j= 1; j <= N; j++) {
#define END_FOR \
  }             \
  }

void add_source(int N, float *x, float *s, float dt) {
  int i, size= (N + 2) * (N + 2);
  for (i= 0; i < size; i++) x[i]+= dt * s[i];
}

void set_bnd(int N, int b, float *x) {
  int i;

  for (i= 1; i <= N; i++) {
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

void lin_solve(int N, int b, float *x, float *x0, float a, float c) {
  int i, j, k;

  for (k= 0; k < 20; k++) {
    FOR_EACH_CELL
    x[IX(i, j)]= (x0[IX(i, j)] + a * (x[IX(i - 1, j)] + x[IX(i + 1, j)] + x[IX(i, j - 1)] + x[IX(i, j + 1)])) / c;
    END_FOR
    set_bnd(N, b, x);
  }
}

void diffuse(int N, int b, float *x, float *x0, float diff, float dt) {
  float a= dt * diff * N * N;
  lin_solve(N, b, x, x0, a, 1 + 4 * a);
}

void advect(int N, int b, float *d, float *d0, float *u, float *v, float dt) {
  int i, j, i0, j0, i1, j1;
  float x, y, s0, t0, s1, t1, dt0;

  dt0= dt * N;
  FOR_EACH_CELL
  x= i - dt0 * u[IX(i, j)];
  y= j - dt0 * v[IX(i, j)];
  if (x < 0.5f) x= 0.5f;
  if (x > N + 0.5f) x= N + 0.5f;
  i0= (int)x;
  i1= i0 + 1;
  if (y < 0.5f) y= 0.5f;
  if (y > N + 0.5f) y= N + 0.5f;
  j0= (int)y;
  j1= j0 + 1;
  s1= x - i0;
  s0= 1 - s1;
  t1= y - j0;
  t0= 1 - t1;
  d[IX(i, j)]= s0 * (t0 * d0[IX(i0, j0)] + t1 * d0[IX(i0, j1)]) +
               s1 * (t0 * d0[IX(i1, j0)] + t1 * d0[IX(i1, j1)]);
  END_FOR
  set_bnd(N, b, d);
}

void project(int N, float *u, float *v, float *p, float *div) {
  int i, j;

  FOR_EACH_CELL
  div[IX(i, j)]= -0.5f * (u[IX(i + 1, j)] - u[IX(i - 1, j)] + v[IX(i, j + 1)] - v[IX(i, j - 1)]) / N;
  p[IX(i, j)]= 0;
  END_FOR
  set_bnd(N, 0, div);
  set_bnd(N, 0, p);

  lin_solve(N, 0, p, div, 1, 4);

  FOR_EACH_CELL
  u[IX(i, j)]-= 0.5f * N * (p[IX(i + 1, j)] - p[IX(i - 1, j)]);
  v[IX(i, j)]-= 0.5f * N * (p[IX(i, j + 1)] - p[IX(i, j - 1)]);
  END_FOR
  set_bnd(N, 1, u);
  set_bnd(N, 2, v);
}

void dens_step(int N, float *x, float *x0, float *u, float *v, float diff, float dt) {
  add_source(N, x, x0, dt);
  SWAP(x0, x);
  diffuse(N, 0, x, x0, diff, dt);
  SWAP(x0, x);
  advect(N, 0, x, x0, u, v, dt);
}

void vel_step(int N, float *u, float *v, float *u0, float *v0, float visc, float dt) {
  add_source(N, u, u0, dt);
  add_source(N, v, v0, dt);
  SWAP(u0, u);
  diffuse(N, 1, u, u0, visc, dt);
  SWAP(v0, v);
  diffuse(N, 2, v, v0, visc, dt);
  project(N, u, v, u0, v0);
  SWAP(u0, u);
  SWAP(v0, v);
  advect(N, 1, u, u0, u0, v0, dt);
  advect(N, 2, v, v0, u0, v0, dt);
  project(N, u, v, u0, v0);
}


extern Data D;

enum ParamType
{
  ResolutionX_________,
  ResolutionY_________,
  ResolutionZ_________,
  TimeStepSize________,
  TimeStepCount_______,
  CoeffDiffusion______,
  CoeffViscosity______,
  CoeffForce__________,
  CoeffSource_________,
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
  D.param.push_back(ParamUI("ResolutionX_________", 1));
  D.param.push_back(ParamUI("ResolutionY_________", 64));
  D.param.push_back(ParamUI("ResolutionZ_________", 64));
  D.param.push_back(ParamUI("TimeStepSize________", 0.1));
  D.param.push_back(ParamUI("TimeStepCount_______", 1));
  D.param.push_back(ParamUI("CoeffDiffusion______", 0.0));
  D.param.push_back(ParamUI("CoeffViscosity______", 0.0));
  D.param.push_back(ParamUI("CoeffForce__________", 5.0));
  D.param.push_back(ParamUI("CoeffSource_________", 100.0));
  D.param.push_back(ParamUI("ScaleFactor_________", 1.0));
  D.param.push_back(ParamUI("ColorFactor_________", 1.0));
}


void CompuFluidDyn::Refresh() {
  if (!isInitialized) return;
  isRefreshed= true;

  // Get UI parameters
  nbX= std::max(int(std::round(D.param[ResolutionX_________].val)), 1);
  nbY= std::max(int(std::round(D.param[ResolutionY_________].val)), 1);
  nbZ= std::max(int(std::round(D.param[ResolutionZ_________].val)), 1);

  nbY= nbZ;
  N= nbZ;

  dt= std::max(D.param[TimeStepSize________].val, 0.0);
  iterMax= std::max(int(std::round(D.param[TimeStepCount_______].val)), 0);
  iter= 0;

  diffu= D.param[CoeffDiffusion______].val;
  visco= D.param[CoeffViscosity______].val;
  force= D.param[CoeffForce__________].val;
  input= D.param[CoeffSource_________].val;

  // Allocate fields
  // u= std::vector<float>(nbX*nbY*nbZ, 0.0f);
  // v= std::vector<float>(nbX*nbY*nbZ, 0.0f);
  // u_prev= std::vector<float>(nbX*nbY*nbZ, 0.0f);
  // v_prev= std::vector<float>(nbX*nbY*nbZ, 0.0f);
  // dens= std::vector<float>(nbX*nbY*nbZ, 0.0f);
  // dens_prev= std::vector<float>(nbX*nbY*nbZ, 0.0f);
  int size= (nbY + 2) * (nbZ + 2);
  u= (float *)malloc(size * sizeof(float));
  v= (float *)malloc(size * sizeof(float));
  u_prev= (float *)malloc(size * sizeof(float));
  v_prev= (float *)malloc(size * sizeof(float));
  dens= (float *)malloc(size * sizeof(float));
  dens_prev= (float *)malloc(size * sizeof(float));

  for (int i= 0; i < size; i++) {
    u[i]= v[i]= u_prev[i]= v_prev[i]= dens[i]= dens_prev[i]= 0.0f;
  }

  for (int i= 1; i < nbY + 2; i++) {
    for (int j= 1; j < nbZ + 2; j++) {
      if (i > 5 && i < 30) {
        if (j > 5 && j < 30) {
          u[IX(i, j)]= v[IX(i, j)]= dens[IX(i, j)]= 0.5f;
        }
      }
    }
  }

  Press= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  Solid= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  Veloc= Field::AllocField3D(nbX, nbY, nbZ, Math::Vec3f(0.0f, 0.0f, 0.0f));
  BCond= Field::AllocField3D(nbX, nbY, nbZ, false);

  // for (int x= 0; x < nbX; x++) {
  //   for (int y= 0; y < nbY; y++) {
  //     for (int z= 0; z < nbZ; z++) {
  //       // Add walls on X and Z faces
  //       if (x == 0 || x == nbX - 1 || z == 0 || z == nbZ - 1) {
  //         BCond[x][y][z]= true;
  //         Veloc[x][y][z]= Math::Vec3f(0.0f, 0.0f, 0.0f);
  //       }

  //       // Set inlet on Y-
  //       if (y == 0 && x > 0 && x < nbX - 1 && z > 0 && z < nbZ - 1) {
  //         BCond[x][y][z]= true;
  //         Veloc[x][y][z]= Math::Vec3f(0.0f, D.param[InletVelocity_______].val, 0.0f);
  //       }

  //       // Set outlet on Y+
  //       if (y == nbY - 1 && x > 0 && x < nbX - 1 && z > 0 && z < nbZ - 1) {
  //         BCond[x][y][z]= false;
  //         Veloc[x][y][z]= Math::Vec3f(0.0f, 0.0f, 0.0f);
  //       }

  //       // Add obstacle
  //       Math::Vec3f posCell((float(x) + 0.5f) / float(nbX), (float(y) + 0.5f) / float(nbY), (float(z) + 0.5f) / float(nbZ));
  //       Math::Vec3f posObstacle(D.param[ObstaclePosX________].val, D.param[ObstaclePosY________].val, D.param[ObstaclePosZ________].val);
  //       if ((posCell - posObstacle).normSquared() <= std::pow(std::max(D.param[ObstacleSize________].val, 0.0), 2.0)) {
  //         Solid[x][y][z]= 1.0f;
  //         BCond[x][y][z]= true;
  //         Veloc[x][y][z]= Math::Vec3f(0.0f, 0.0f, 0.0f);
  //       }
  //     }
  //   }
  // }
}


void CompuFluidDyn::Animate() {
  if (!isInitialized) return;
  if (!isRefreshed) return;

  // Check stopping condition
  if (iter >= iterMax) return;
  iter++;

  vel_step(N, u, v, u_prev, v_prev, visco, dt);
  dens_step(N, dens, dens_prev, u, v, diffu, dt);

  int N= nbZ - 2;
  for (int x= 0; x < nbX; x++) {
    for (int y= 0; y < nbY; y++) {
      for (int z= 0; z < nbZ; z++) {
        Veloc[x][y][z][0]= 0.0f;
        Veloc[x][y][z][1]= u[IX(y, z)];
        Veloc[x][y][z][2]= v[IX(y, z)];
        Press[x][y][z]= dens[IX(y, z)];
      }
    }
  }

  // // Calculate the pressure for display
  // for (int x= 0; x < nbX; x++) {
  //   for (int y= 0; y < nbY; y++) {
  //     for (int z= 0; z < nbZ; z++) {
  //       Press[x][y][z]= 0.0;
  //       if (x - 1 >= 0) Press[x][y][z]-= Veloc[x - 1][y][z][0];
  //       if (x + 1 < nbX) Press[x][y][z]+= Veloc[x + 1][y][z][0];
  //       if (y - 1 >= 0) Press[x][y][z]-= Veloc[x][y - 1][z][1];
  //       if (y + 1 < nbY) Press[x][y][z]+= Veloc[x][y + 1][z][1];
  //       if (z - 1 >= 0) Press[x][y][z]-= Veloc[x][y][z - 1][2];
  //       if (z + 1 < nbZ) Press[x][y][z]+= Veloc[x][y][z + 1][2];
  //       Press[x][y][z]= -Press[x][y][z];
  //     }
  //   }
  // }
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
