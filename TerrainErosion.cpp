#include "TerrainErosion.hpp"


// Standard lib
#include <array>
#include <cmath>
#include <cstdio>
#include <ctime>
#include <vector>

// GLUT lib
#include <GL/freeglut.h>

// Project lib
#include "Data.hpp"
#include "math/Vectors.hpp"
#include "util/Colormap.hpp"
#include "util/Field.hpp"
#include "util/Random.hpp"


extern Data D;


float InterpolateVal(std::vector<std::vector<float>> const& iField, Math::Vec2f const& iRelPos) {
  int nbX, nbY;
  Field::GetFieldDimensions(iField, nbX, nbY);

  float xFloat= iRelPos[0] / float(nbX - 1);
  float yFloat= iRelPos[1] / float(nbY - 1);

  int x0= std::min(std::max(int(std::floor(xFloat)), 0), nbX - 1);
  int y0= std::min(std::max(int(std::floor(yFloat)), 0), nbY - 1);
  int x1= std::min(std::max(int(std::floor(xFloat)) + 1, 0), nbX - 1);
  int y1= std::min(std::max(int(std::floor(yFloat)) + 1, 0), nbY - 1);

  float xWeight1= xFloat - float(x0);
  float yWeight1= yFloat - float(y0);
  float xWeight0= 1.0 - xWeight1;
  float yWeight0= 1.0 - yWeight1;

  float v00= iField[x0][y0];
  float v01= iField[x0][y1];
  float v10= iField[x1][y0];
  float v11= iField[x1][y1];

  float val= 0.0;
  val+= v00 * (xWeight0 * yWeight0);
  val+= v01 * (xWeight0 * yWeight1);
  val+= v10 * (xWeight1 * yWeight0);
  val+= v11 * (xWeight1 * yWeight1);

  return val;
}


TerrainErosion::TerrainErosion() {
  isInitialized= false;
  isRefreshed= false;
}


void TerrainErosion::Init() {
  isInitialized= true;
  isRefreshed= false;

  // Get terrain parameters
  terrainNbX= std::max(2, int(std::round(D.param[TE_TerrainNbX_______].val)));
  terrainNbY= std::max(2, int(std::round(D.param[TE_TerrainNbY_______].val)));
  terrainNbCuts= std::max(0, int(std::round(D.param[TE_TerrainNbCuts____].val)));

  // Allocate terrain data
  terrainPos= Field::AllocField2D(terrainNbX, terrainNbY, Math::Vec3f(0.0f, 0.0f, 0.0f));
  terrainNor= Field::AllocField2D(terrainNbX, terrainNbY, Math::Vec3f(0.0f, 0.0f, 0.0f));
  terrainWet= Field::AllocField2D(terrainNbX, terrainNbY, 0.0f);

  // Compute terrain mesh vertex positions
  for (int x= 0; x < terrainNbX; x++)
    for (int y= 0; y < terrainNbY; y++)
      terrainPos[x][y].set(float(x) / float(terrainNbX - 1), float(y) / float(terrainNbY - 1), 0.0f);

  // Precompute cut planes
  srand(0);
  std::vector<Math::Vec2f> cutPiv(terrainNbCuts);
  std::vector<Math::Vec2f> cutVec(terrainNbCuts);
  for (int iter= 0; iter < terrainNbCuts; iter++) {
    cutPiv[iter].set(Random::Val(0.0f, 1.0f), Random::Val(0.0f, 1.0f));
    do {
      cutVec[iter].set(Random::Val(-1.0f, 1.0f), Random::Val(-1.0f, 1.0f));
    } while (cutVec[iter].normSquared() > 1.0f);
    cutVec[iter].normalize();
  }

  // Compute random terrain through iterative cutting
  for (int x= 0; x < terrainNbX; x++) {
    for (int y= 0; y < terrainNbY; y++) {
      terrainPos[x][y][2]= 0.0f;
      for (int iter= 0; iter < terrainNbCuts; iter++) {
        Math::Vec2f pos(terrainPos[x][y][0], terrainPos[x][y][1]);
        terrainPos[x][y][2]+= ((pos - cutPiv[iter]).dot(cutVec[iter]) < 0.0f) ? 1.0f : -1.0f;
      }
    }
  }

  // Smooth the terrain
  for (int iter= 0; iter < std::max(terrainNbX, terrainNbY) / 32; iter++) {
    std::vector<std::vector<Math::Vec3f>> terrainPosOld= terrainPos;
    for (int x= 0; x < terrainNbX; x++) {
      for (int y= 0; y < terrainNbY; y++) {
        int count= 0;
        terrainPos[x][y][2]= 0.0;
        for (int xOff= std::max(x - 1, 0); xOff <= std::min(x + 1, terrainNbX - 1); xOff++) {
          for (int yOff= std::max(y - 1, 0); yOff <= std::min(y + 1, terrainNbY - 1); yOff++) {
            terrainPos[x][y][2]+= terrainPosOld[xOff][yOff][2];
            count++;
          }
        }
        terrainPos[x][y][2]/= float(count);
      }
    }
  }

  // Rescale terrain elevation
  float terrainMinTarg= 0.35f;
  float terrainMaxTarg= 0.65f;
  float terrainMinVal= terrainPos[0][0][2];
  float terrainMaxVal= terrainPos[0][0][2];
  for (int x= 0; x < terrainNbX; x++) {
    for (int y= 0; y < terrainNbY; y++) {
      if (terrainMinVal > terrainPos[x][y][2]) terrainMinVal= terrainPos[x][y][2];
      if (terrainMaxVal < terrainPos[x][y][2]) terrainMaxVal= terrainPos[x][y][2];
    }
  }
  for (int x= 0; x < terrainNbX; x++)
    for (int y= 0; y < terrainNbY; y++)
      terrainPos[x][y][2]= terrainMinTarg + (terrainMaxTarg - terrainMinTarg) * (terrainPos[x][y][2] - terrainMinVal) / (terrainMaxVal - terrainMinVal);

  // Compute terrain mesh vertex normals
  for (int x= 0; x < terrainNbX; x++) {
    for (int y= 0; y < terrainNbY; y++) {
      terrainNor[x][y].set(0.0f, 0.0f, 0.0f);
      if (x > 0 && y > 0)
        terrainNor[x][y]+= ((terrainPos[x - 1][y] - terrainPos[x][y]).cross(terrainPos[x][y - 1] - terrainPos[x][y])).normalized();
      if (x < terrainNbX - 1 && y > 0)
        terrainNor[x][y]+= ((terrainPos[x][y - 1] - terrainPos[x][y]).cross(terrainPos[x + 1][y] - terrainPos[x][y])).normalized();
      if (x < terrainNbX - 1 && y < terrainNbY - 1)
        terrainNor[x][y]+= ((terrainPos[x + 1][y] - terrainPos[x][y]).cross(terrainPos[x][y + 1] - terrainPos[x][y])).normalized();
      if (x > 0 && y < terrainNbY - 1)
        terrainNor[x][y]+= ((terrainPos[x][y + 1] - terrainPos[x][y]).cross(terrainPos[x - 1][y] - terrainPos[x][y])).normalized();
      terrainNor[x][y].normalize();
    }
  }

  // Get droplet parameters
  dropletNbK= std::max(1, int(std::round(D.param[TE_DropletNbK_______].val)));

  // Allocate droplet data
  dropletPosOld= std::vector<Math::Vec3f>(dropletNbK, Math::Vec3f(0.0f, 0.0f, 0.0f));
  dropletPosCur= std::vector<Math::Vec3f>(dropletNbK, Math::Vec3f(0.0f, 0.0f, 0.0f));
  dropletVelCur= std::vector<Math::Vec3f>(dropletNbK, Math::Vec3f(0.0f, 0.0f, 0.0f));
  dropletAccCur= std::vector<Math::Vec3f>(dropletNbK, Math::Vec3f(0.0f, 0.0f, 0.0f));
  dropletForCur= std::vector<Math::Vec3f>(dropletNbK, Math::Vec3f(0.0f, 0.0f, 0.0f));
  dropletColCur= std::vector<Math::Vec3f>(dropletNbK, Math::Vec3f(0.0f, 0.0f, 0.0f));
  dropletMasCur= std::vector<float>(dropletNbK, 0.0f);
  dropletRadCur= std::vector<float>(dropletNbK, 0.0f);
  dropletSatCur= std::vector<float>(dropletNbK, 0.0f);
  dropletIsDead= std::vector<bool>(dropletNbK, true);
}


void TerrainErosion::Refresh() {
  if (!isInitialized) return;
  isRefreshed= true;
}


void TerrainErosion::Animate() {
  if (!isInitialized) return;
  if (!isRefreshed) return;

  int nbSubstep= 1;
  float dt= 0.02f / float(nbSubstep);
  float velocityDecay= (1.0f - 0.8f * dt);
  Math::Vec3f gravity(0.0f, 0.0f, -0.5f);

  for (int idxStep= 0; idxStep < nbSubstep; idxStep++) {
    // Initialize invalid droplets with random properties
    for (int k0= 0; k0 < dropletNbK; k0++) {
      if (dropletIsDead[k0]) {
        dropletPosCur[k0].set(Random::Val(0.0f, 1.0f), Random::Val(0.0f, 1.0f), Random::Val(0.9f, 1.0f));
        dropletPosOld[k0]= dropletPosCur[k0];
        dropletColCur[k0].set(0.5f, 0.5f, 1.0f);
        dropletMasCur[k0]= 1.0f;
        dropletSatCur[k0]= 0.01f;
        dropletRadCur[k0]= std::max(D.param[TE_DropletRad_______].val, 1.e-6f);
        dropletIsDead[k0]= false;
      }
    }

    // Reset forces
    for (int k0= 0; k0 < dropletNbK; k0++)
      dropletForCur[k0].set(0.0f, 0.0f, 0.0f);

    // Add gravity forces
    for (int k0= 0; k0 < dropletNbK; k0++)
      dropletForCur[k0]+= gravity * dropletMasCur[k0];

    // Apply boundary constraint
    for (int k0= 0; k0 < dropletNbK; k0++) {
      float xFloat= dropletPosCur[k0][0] / (1.0f / float(terrainNbX - 1));
      float yFloat= dropletPosCur[k0][1] / (1.0f / float(terrainNbY - 1));

      int x0= std::min(std::max(int(std::floor(xFloat)), 0), terrainNbX - 1);
      int y0= std::min(std::max(int(std::floor(yFloat)), 0), terrainNbY - 1);
      int x1= std::min(std::max(int(std::floor(xFloat)) + 1, 0), terrainNbX - 1);
      int y1= std::min(std::max(int(std::floor(yFloat)) + 1, 0), terrainNbY - 1);

      float xWeight1= xFloat - float(x0);
      float yWeight1= yFloat - float(y0);
      float xWeight0= 1.0 - xWeight1;
      float yWeight0= 1.0 - yWeight1;

      float interpoVal= 0.0;
      interpoVal+= terrainPos[x0][y0][2] * (xWeight0 * yWeight0);
      interpoVal+= terrainPos[x0][y1][2] * (xWeight0 * yWeight1);
      interpoVal+= terrainPos[x1][y0][2] * (xWeight1 * yWeight0);
      interpoVal+= terrainPos[x1][y1][2] * (xWeight1 * yWeight1);

      if (dropletPosCur[k0][2] - dropletRadCur[k0] < interpoVal) {
        Math::Vec3f interpoNor(0.0f, 0.0f, 0.0f);
        interpoNor+= terrainNor[x0][y0] * (xWeight0 * yWeight0);
        interpoNor+= terrainNor[x0][y1] * (xWeight0 * yWeight1);
        interpoNor+= terrainNor[x1][y0] * (xWeight1 * yWeight0);
        interpoNor+= terrainNor[x1][y1] * (xWeight1 * yWeight1);
        dropletPosCur[k0]+= (interpoVal + dropletRadCur[k0] - dropletPosCur[k0][2]) * interpoNor.normalized();

        terrainWet[x0][y0]+= 0.1f * (xWeight0 * yWeight0);
        terrainWet[x0][y1]+= 0.1f * (xWeight0 * yWeight1);
        terrainWet[x1][y0]+= 0.1f * (xWeight1 * yWeight0);
        terrainWet[x1][y1]+= 0.1f * (xWeight1 * yWeight1);
      }
    }

    // Apply collision constraint (Gauss Seidel)
    for (int k0= 0; k0 < dropletNbK; k0++) {
      for (int k1= k0 + 1; k1 < dropletNbK; k1++) {
        if ((dropletPosCur[k1] - dropletPosCur[k0]).normSquared() <= (dropletRadCur[k0] + dropletRadCur[k1]) * (dropletRadCur[k0] + dropletRadCur[k1])) {
          Math::Vec3f val= (dropletPosCur[k1] - dropletPosCur[k0]).normalized() * 0.5f * ((dropletRadCur[k0] + dropletRadCur[k1]) - (dropletPosCur[k1] - dropletPosCur[k0]).norm());
          dropletPosCur[k0]-= val;
          dropletPosCur[k1]+= val;
        }
      }
    }

    // Deduce velocities
    for (int k0= 0; k0 < dropletNbK; k0++)
      dropletVelCur[k0]= (dropletPosCur[k0] - dropletPosOld[k0]) / dt;

    // Apply explicit velocity damping
    for (int k0= 0; k0 < dropletNbK; k0++)
      dropletVelCur[k0]= dropletVelCur[k0] * velocityDecay;

    // Update positions
    dropletPosOld= dropletPosCur;
    for (int k0= 0; k0 < dropletNbK; k0++) {
      dropletAccCur[k0]= dropletForCur[k0] / dropletMasCur[k0];
      dropletPosCur[k0]= dropletPosCur[k0] + dropletVelCur[k0] * dt + dropletAccCur[k0] * dt * dt;
    }

    // Kill particles out of bounds
    for (int k0= 0; k0 < dropletNbK; k0++) {
      if (dropletPosCur[k0][0] < 0.0f || dropletPosCur[k0][0] > 1.0f) dropletIsDead[k0]= true;
      if (dropletPosCur[k0][1] < 0.0f || dropletPosCur[k0][1] > 1.0f) dropletIsDead[k0]= true;
      if (dropletPosCur[k0][2] < 0.0f || dropletPosCur[k0][2] > 1.0f) dropletIsDead[k0]= true;
    }
  }

  // Decrease terrain wetness
  for (int x= 0; x < terrainNbX; x++)
    for (int y= 0; y < terrainNbY; y++)
      terrainWet[x][y]= std::min(std::max(terrainWet[x][y] * 0.99f, 0.0f), 1.0f);

  // Plot some values
  D.plotData.resize(3);
  D.plotData[0].first= "valX";
  D.plotData[1].first= "valY";
  D.plotData[2].first= "valZ";
  if (D.plotData[0].second.size() < 1000) D.plotData[0].second.push_back(dropletPosCur[0][0]);
  if (D.plotData[1].second.size() < 1000) D.plotData[1].second.push_back(dropletPosCur[0][1]);
  if (D.plotData[2].second.size() < 1000) D.plotData[2].second.push_back(dropletPosCur[0][2]);
}


void TerrainErosion::Draw() {
  if (!isInitialized) return;
  if (!isRefreshed) return;

  // Draw the terrain
  if (D.displayMode1 || D.displayMode2) {
    glEnable(GL_LIGHTING);
    glBegin(GL_QUADS);
    for (int x= 0; x < terrainNbX - 1; x++) {
      for (int y= 0; y < terrainNbY - 1; y++) {
        Math::Vec3f flatNormal= (terrainNor[x][y] + terrainNor[x + 1][y] + terrainNor[x + 1][y + 1] + terrainNor[x][y + 1]).normalized();
        float r, g, b;
        if (D.displayMode1)
          Colormap::RatioToJetSmooth(terrainPos[x][y][2] * 2.0f - 0.5f, r, g, b);
        if (D.displayMode2)
          Colormap::RatioToJetSmooth(1.0 - terrainWet[x][y] * D.param[testVar1____________].val, r, g, b);
        glColor3f(r / 2.0f, g / 2.0f, b / 2.0f);
        glNormal3fv(flatNormal.array());
        glVertex3fv(terrainPos[x][y].array());
        glNormal3fv(flatNormal.array());
        glVertex3fv(terrainPos[x + 1][y].array());
        glNormal3fv(flatNormal.array());
        glVertex3fv(terrainPos[x + 1][y + 1].array());
        glNormal3fv(flatNormal.array());
        glVertex3fv(terrainPos[x][y + 1].array());
      }
    }
    glEnd();
    glDisable(GL_LIGHTING);
  }

  // Draw the terrain normals
  if (D.displayMode3) {
    glBegin(GL_LINES);
    for (int x= 0; x < terrainNbX; x++) {
      for (int y= 0; y < terrainNbY; y++) {
        float r, g, b;
        Colormap::RatioToJetSmooth(1.0f - terrainNor[x][y][2] * terrainNor[x][y][2], r, g, b);
        glColor3f(r, g, b);
        glVertex3fv(terrainPos[x][y].array());
        glVertex3fv((terrainPos[x][y] + 0.02f * terrainNor[x][y]).array());
      }
    }
    glEnd();
  }

  // Draw the droplets
  if (D.displayMode4) {
    for (int k= 0; k < dropletNbK; k++) {
      glPushMatrix();
      glTranslatef(dropletPosCur[k][0], dropletPosCur[k][1], dropletPosCur[k][2]);
      glScalef(dropletRadCur[k], dropletRadCur[k], dropletRadCur[k]);
      float r, g, b;
      Colormap::RatioToJetSmooth(dropletVelCur[k].norm(), r, g, b);
      glColor3f(r, g, b);
      glutSolidSphere(1.0, 32, 16);
      glPopMatrix();
    }
  }
}
