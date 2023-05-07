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


void TerrainErosion::ComputeTerrainFractal() {
  // Precompute cut planes
  srand(0);
  int nbCuts= std::round(D.param[testVar1____________].val);
  std::vector<Math::Vec2f> piv(nbCuts);
  std::vector<Math::Vec2f> vec(nbCuts);
  for (int iter= 0; iter < nbCuts; iter++) {
    piv[iter].set(Random::Val(0.0f, 1.0f), Random::Val(0.0f, 1.0f));
    do {
      vec[iter].set(Random::Val(-1.0f, 1.0f), Random::Val(-1.0f, 1.0f));
    } while (vec[iter].normSquared() > 1.0f);
    vec[iter].normalize();
  }

  // Compute random terrain through iterative cutting
  for (int x= 0; x < terrainNbX; x++) {
    for (int y= 0; y < terrainNbY; y++) {
      terrainRef[x][y]= 0.0f;
      for (int iter= 0; iter < nbCuts; iter++) {
        Math::Vec2f pos(float(x) / float(terrainNbX - 1), float(y) / float(terrainNbY - 1));
        terrainRef[x][y]+= ((pos - piv[iter]).dot(vec[iter]) < 0.0f) ? 1.0f : -1.0f;
      }
    }
  }
}


void TerrainErosion::ComputeTerrainSmoothing() {
  // Smooth the terrain
  for (int iter= 0; iter < std::max(terrainNbX, terrainNbY) / 50; iter++) {
    std::vector<std::vector<float>> terrainRefOld= terrainRef;
    for (int x= 0; x < terrainNbX; x++) {
      for (int y= 0; y < terrainNbY; y++) {
        int count= 0;
        terrainRef[x][y]= 0.0;
        for (int xOff= std::max(x - 1, 0); xOff <= std::min(x + 1, terrainNbX - 1); xOff++) {
          for (int yOff= std::max(y - 1, 0); yOff <= std::min(y + 1, terrainNbY - 1); yOff++) {
            terrainRef[x][y]+= terrainRefOld[xOff][yOff];
            count++;
          }
        }
        terrainRef[x][y]/= float(count);
      }
    }
  }
}


void TerrainErosion::ComputeTerrainRescaling() {
  // Rescale terrain elevation
  float minVal= terrainRef[0][0];
  float maxVal= terrainRef[0][0];
  for (int x= 0; x < terrainNbX; x++) {
    for (int y= 0; y < terrainNbY; y++) {
      if (minVal > terrainRef[x][y]) minVal= terrainRef[x][y];
      if (maxVal < terrainRef[x][y]) maxVal= terrainRef[x][y];
    }
  }
  for (int x= 0; x < terrainNbX; x++)
    for (int y= 0; y < terrainNbY; y++)
      terrainRef[x][y]= 0.25f + 0.5f * (terrainRef[x][y] - minVal) / (maxVal - minVal);
}


void TerrainErosion::ComputeTerrainMesh() {
  for (int x= 0; x < terrainNbX; x++) {
    for (int y= 0; y < terrainNbY; y++) {
      terrainPos[x][y].set(float(x) / float(terrainNbX - 1), float(y) / float(terrainNbY - 1), terrainVal[x][y]);
    }
  }
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
}


TerrainErosion::TerrainErosion() {
  isInitialized= false;
  isRefreshed= false;
}


void TerrainErosion::Init() {
  isInitialized= true;
  isRefreshed= false;
}


void TerrainErosion::Refresh() {
  if (!isInitialized) return;
  isRefreshed= true;

  // Get dimensions
  terrainNbX= std::max(2, int(std::round(D.param[TE_TerrainNbX_______].val)));
  terrainNbY= std::max(2, int(std::round(D.param[TE_TerrainNbY_______].val)));

  // Initialize
  terrainRef= Field::AllocField2D(terrainNbX, terrainNbY, 0.0f);

  // Generate the terrain
  ComputeTerrainFractal();
  ComputeTerrainSmoothing();
  ComputeTerrainRescaling();

  // Set the initial terrain before erosoion
  terrainVal= terrainRef;

  // Get dimensions
  dropletNbK= std::max(1, int(std::round(D.param[TE_DropletNbK_______].val)));
  dropletNbS= std::max(1, int(std::round(D.param[TE_DropletNbS_______].val)));

  // Initialize
  terrainGra= Field::AllocField2D(terrainNbX, terrainNbY, Math::Vec3f(0.0f, 0.0f, 0.0f));
  terrainPos= Field::AllocField2D(terrainNbX, terrainNbY, Math::Vec3f(0.0f, 0.0f, 0.0f));
  terrainNor= Field::AllocField2D(terrainNbX, terrainNbY, Math::Vec3f(0.0f, 0.0f, 0.0f));
  dropletPos= Field::AllocField2D(dropletNbK, dropletNbS, Math::Vec3f(0.0f, 0.0f, 0.0f));
  dropletVel= Field::AllocField2D(dropletNbK, dropletNbS, Math::Vec3f(0.0f, 0.0f, 0.0f));

  // Precompute terrain gradient
  for (int x= 0; x < terrainNbX; x++) {
    for (int y= 0; y < terrainNbY; y++) {
      if (x > 0) terrainGra[x][y][0]= terrainVal[x - 1][y] - terrainVal[x][y];
      if (y > 0) terrainGra[x][y][1]= terrainVal[x][y - 1] - terrainVal[x][y];
      if (x < terrainNbX - 1) terrainGra[x][y][0]= terrainVal[x][y] - terrainVal[x + 1][y];
      if (y < terrainNbY - 1) terrainGra[x][y][1]= terrainVal[x][y] - terrainVal[x][y + 1];
      if (x > 0 && x < terrainNbX - 1) terrainGra[x][y][0]/= 2.0f;
      if (y > 0 && y < terrainNbY - 1) terrainGra[x][y][1]/= 2.0f;
    }
  }

  // Compute mesh for visualization
  ComputeTerrainMesh();

  // Compute the droplet paths
  for (int k= 0; k < dropletNbK; k++) {
    dropletPos[k][0]= Math::Vec3f(Random::Val(0.0f, 1.0f), Random::Val(0.0f, 1.0f), 0.0f);
    dropletVel[k][0]= Math::Vec3f(Random::Val(-0.01f, +0.01f), Random::Val(-0.01f, +0.01f), 0.0f);
    for (int s= 0; s < dropletNbS; s++) {
      dropletPos[k][s][0]= std::min(std::max(dropletPos[k][s][0], 0.0f), 1.0f);
      dropletPos[k][s][1]= std::min(std::max(dropletPos[k][s][1], 0.0f), 1.0f);
      int idxX= std::min(std::max(int(std::floor(dropletPos[k][s - 1][0] * float(terrainNbX))), 0), terrainNbX - 1);
      int idxY= std::min(std::max(int(std::floor(dropletPos[k][s - 1][1] * float(terrainNbY))), 0), terrainNbY - 1);
      dropletPos[k][s][2]= terrainPos[idxX][idxY][2] + 0.01f;

      if (s < dropletNbS - 1) {
        dropletPos[k][s + 1]= dropletPos[k][s] + dropletVel[k][s];
        dropletVel[k][s + 1]= D.param[testVar2____________].val * dropletVel[k][s] + D.param[testVar3____________].val * terrainGra[idxX][idxY];
      }
    }
  }
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
          Colormap::RatioToJetBrightSmooth(terrainVal[x][y] * 2.0f - 0.5f, r, g, b);
        else
          Colormap::RatioToJetBrightSmooth((1.0f - std::abs(terrainNor[x][y][2])) * D.param[testVar0____________].val, r, g, b);
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
        Colormap::RatioToJetBrightSmooth((1.0f - terrainNor[x][y][2] * terrainNor[x][y][2]) * D.param[testVar0____________].val, r, g, b);
        glColor3f(r, g, b);
        glVertex3fv(terrainPos[x][y].array());
        glVertex3fv((terrainPos[x][y] + 0.02f * terrainNor[x][y] * D.param[testVar0____________].val).array());
      }
    }
    glEnd();
  }

  // Draw the droplet paths
  if (D.displayMode4) {
    glBegin(GL_LINES);
    for (int k= 0; k < dropletNbK; k++) {
      for (int s= 0; s < dropletNbS - 1; s++) {
        float r, g, b;
        // Colormap::RatioToJetBrightSmooth(dropletVel[k][s].norm() * D.param[testVar0____________].val, r, g, b);
        Colormap::RatioToJetBrightSmooth(float(s) / float(dropletNbS - 1), r, g, b);
        glColor3f(r, g, b);
        glVertex3fv(dropletPos[k][s].array());
        glVertex3fv(dropletPos[k][s + 1].array());
      }
    }
    glEnd();
  }
}
