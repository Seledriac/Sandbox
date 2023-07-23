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
#include "../Data.hpp"
#include "../math/Vectors.hpp"
#include "../util/Colormap.hpp"
#include "../util/Field.hpp"
#include "../util/Random.hpp"


extern Data D;


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
  terrainNor= Field::AllocField2D(terrainNbX, terrainNbY, Math::Vec3f(0.0f, 0.0f, 1.0f));
  terrainCol= Field::AllocField2D(terrainNbX, terrainNbY, Math::Vec3f(0.5f, 0.5f, 0.5f));
  terrainChg= Field::AllocField2D(terrainNbX, terrainNbY, 0.0f);

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
      terrainPos[x][y][0]= float(x) / float(terrainNbX - 1);
      terrainPos[x][y][1]= float(y) / float(terrainNbY - 1);
      terrainPos[x][y][2]= 0.0f;
      for (int iter= 0; iter < terrainNbCuts; iter++) {
        Math::Vec2f pos(terrainPos[x][y][0], terrainPos[x][y][1]);
        if ((pos - cutPiv[iter]).dot(cutVec[iter]) < 0.0f)
          terrainPos[x][y][2]+= 1.0f;
        else
          terrainPos[x][y][2]-= 1.0f;
      }
    }
  }

  // Smooth the terrain
  for (int iter= 0; iter < std::max(terrainNbX, terrainNbY) / 64; iter++) {
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
  float terrainMinTarg= 0.3f;
  float terrainMaxTarg= 0.7f;
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

  float dt= D.param[TE_SimuTimestep_____].val;
  float velocityDecay= std::min(std::max(D.param[TE_VelocityDecay____].val, 0.0), 1.0);
  Math::Vec3f gravity(0.0f, 0.0f, -0.5f);

  // Respawn dead droplets
  for (int k= 0; k < dropletNbK; k++) {
    if (dropletIsDead[k]) {
      dropletPosCur[k].set(Random::Val(0.0f, 1.0f), Random::Val(0.0f, 1.0f), Random::Val(0.7f, 1.0f));
      dropletPosOld[k]= dropletPosCur[k];
      dropletColCur[k].set(0.5f, 0.5f, 1.0f);
      dropletMasCur[k]= 1.0f;
      dropletSatCur[k]= 0.01f;
      dropletRadCur[k]= std::max(D.param[TE_DropletRad_______].val, 1.e-6);
      dropletIsDead[k]= false;
    }
  }

  // Reset forces
  for (int k= 0; k < dropletNbK; k++)
    dropletForCur[k].set(0.0f, 0.0f, 0.0f);

  // Add gravity forces
  for (int k= 0; k < dropletNbK; k++)
    dropletForCur[k]+= gravity * dropletMasCur[k];

  // Reset terrain change
  for (int x= 0; x < terrainNbX; x++)
    for (int y= 0; y < terrainNbY; y++)
      terrainChg[x][y]= 0.0f;

  // Compute terrain erosion and sedimentation
  for (int k= 0; k < dropletNbK; k++) {
    int xRef= std::min(std::max(int(std::round(dropletPosCur[k][0] * float(terrainNbX - 1))), 0), terrainNbX - 1);
    int yRef= std::min(std::max(int(std::round(dropletPosCur[k][1] * float(terrainNbY - 1))), 0), terrainNbY - 1);
    int xRad= int(std::ceil(dropletRadCur[k] * 2.0f / (1.0f / float(terrainNbX))));
    int yRad= int(std::ceil(dropletRadCur[k] * 2.0f / (1.0f / float(terrainNbY))));
    for (int xOff= std::max(xRef - xRad, 0); xOff <= std::min(xRef + xRad, terrainNbX - 1); xOff++) {
      for (int yOff= std::max(yRef - yRad, 0); yOff <= std::min(yRef + yRad, terrainNbY - 1); yOff++) {
        float weight= std::max(dropletRadCur[k] * 2.0f - (dropletPosCur[k] - terrainPos[xOff][yOff]).norm(), 0.0f);
        terrainChg[xOff][yOff]-= weight * D.param[TE_ErosionCoeff_____].val;
      }
    }
  }

  // Resolve droplet-terrain collisions
  for (int k= 0; k < dropletNbK; k++) {
    float xFloat= dropletPosCur[k][0] * float(terrainNbX - 1);
    float yFloat= dropletPosCur[k][1] * float(terrainNbY - 1);

    int x0= std::min(std::max(int(std::floor(xFloat)), 0), terrainNbX - 2);
    int y0= std::min(std::max(int(std::floor(yFloat)), 0), terrainNbY - 2);
    int x1= x0 + 1;
    int y1= y0 + 1;

    float xWeight1= xFloat - float(x0);
    float yWeight1= yFloat - float(y0);
    float xWeight0= 1.0 - xWeight1;
    float yWeight0= 1.0 - yWeight1;

    float interpoVal= 0.0;
    interpoVal+= terrainPos[x0][y0][2] * (xWeight0 * yWeight0);
    interpoVal+= terrainPos[x0][y1][2] * (xWeight0 * yWeight1);
    interpoVal+= terrainPos[x1][y0][2] * (xWeight1 * yWeight0);
    interpoVal+= terrainPos[x1][y1][2] * (xWeight1 * yWeight1);

    if (dropletPosCur[k][2] - dropletRadCur[k] < interpoVal) {
      Math::Vec3f interpoNor(0.0f, 0.0f, 0.0f);
      interpoNor+= terrainNor[x0][y0] * (xWeight0 * yWeight0);
      interpoNor+= terrainNor[x0][y1] * (xWeight0 * yWeight1);
      interpoNor+= terrainNor[x1][y0] * (xWeight1 * yWeight0);
      interpoNor+= terrainNor[x1][y1] * (xWeight1 * yWeight1);
      dropletPosCur[k]+= (interpoVal + dropletRadCur[k] - dropletPosCur[k][2]) * interpoNor.normalized();
    }
  }

  // Resolve droplet-droplet collisions
  // todo spatial partition
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
  for (int k= 0; k < dropletNbK; k++)
    dropletVelCur[k]= (dropletPosCur[k] - dropletPosOld[k]) / dt;

  // Apply explicit velocity damping
  for (int k= 0; k < dropletNbK; k++)
    dropletVelCur[k]= dropletVelCur[k] * std::pow(velocityDecay, dt);

  // Update positions
  dropletPosOld= dropletPosCur;
  for (int k= 0; k < dropletNbK; k++) {
    dropletAccCur[k]= dropletForCur[k] / dropletMasCur[k];
    dropletPosCur[k]= dropletPosCur[k] + dropletVelCur[k] * dt + dropletAccCur[k] * dt * dt;
  }

  // Kill particles out of bounds
  for (int k= 0; k < dropletNbK; k++) {
    if (dropletPosCur[k][0] < 0.0f || dropletPosCur[k][0] > 1.0f) dropletIsDead[k]= true;
    if (dropletPosCur[k][1] < 0.0f || dropletPosCur[k][1] > 1.0f) dropletIsDead[k]= true;
    if (dropletPosCur[k][2] < 0.0f || dropletPosCur[k][2] > 1.0f) dropletIsDead[k]= true;
  }

  // Apply terrain change
  for (int x= 0; x < terrainNbX; x++) {
    for (int y= 0; y < terrainNbY; y++) {
      float minNeighbor= terrainPos[x][y][2];
      float maxNeighbor= terrainPos[x][y][2];
      for (int xOff= std::max(x - 1, 0); xOff <= std::min(x + 1, terrainNbX - 1); xOff++) {
        for (int yOff= std::max(y - 1, 0); yOff <= std::min(y + 1, terrainNbY - 1); yOff++) {
          if (minNeighbor > terrainPos[xOff][yOff][2]) minNeighbor= terrainPos[xOff][yOff][2];
          if (maxNeighbor < terrainPos[xOff][yOff][2]) maxNeighbor= terrainPos[xOff][yOff][2];
        }
      }
      terrainPos[x][y][2]= std::min(std::max(terrainPos[x][y][2] + terrainChg[x][y], minNeighbor), maxNeighbor);
    }
  }

  // Smooth the terrain
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
      terrainPos[x][y][2]= D.param[TE_SmoothResist_____].val * terrainPosOld[x][y][2] + (1.0f - D.param[TE_SmoothResist_____].val) * terrainPos[x][y][2];
    }
  }

  // Recompute terrain normals
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


void TerrainErosion::Draw() {
  if (!isInitialized) return;
  if (!isRefreshed) return;

  // Set the terrain colors
  for (int x= 0; x < terrainNbX; x++) {
    for (int y= 0; y < terrainNbY; y++) {
      if (D.displayMode1) {
        Colormap::RatioToJetSmooth(terrainPos[x][y][2] * 2.0f - 0.5f, terrainCol[x][y][0], terrainCol[x][y][1], terrainCol[x][y][2]);
      }
      else if (D.displayMode2) {
        terrainCol[x][y][0]= 0.5f + terrainNor[x][y][0] / 2.0f;
        terrainCol[x][y][1]= 0.5f + terrainNor[x][y][1] / 2.0f;
        terrainCol[x][y][2]= 0.5f + terrainNor[x][y][2] / 2.0f;
      }
      else if (D.displayMode3) {
        if (terrainNor[x][y].dot(Math::Vec3f(0.0f, 0.0f, 1.0f)) < D.param[testVar6____________].val) {
          terrainCol[x][y][0]= 0.7f;
          terrainCol[x][y][1]= 0.6f;
          terrainCol[x][y][2]= 0.3f;
        }
        else {
          terrainCol[x][y][0]= 0.5f;
          terrainCol[x][y][1]= 0.9f;
          terrainCol[x][y][2]= 0.5f;
        }
      }
    }
  }

  // Draw the terrain
  if (D.displayMode1 || D.displayMode2 || D.displayMode3) {
    glEnable(GL_LIGHTING);
    glBegin(GL_QUADS);
    for (int x= 0; x < terrainNbX - 1; x++) {
      for (int y= 0; y < terrainNbY - 1; y++) {
        Math::Vec3f flatNormal= (terrainNor[x][y] + terrainNor[x + 1][y] + terrainNor[x + 1][y + 1] + terrainNor[x][y + 1]).normalized();
        Math::Vec3f flatColor= (terrainCol[x][y] + terrainCol[x + 1][y] + terrainCol[x + 1][y + 1] + terrainCol[x][y + 1]) / 4.0f;
        glColor3fv((flatColor / 2.0f).array());
        glNormal3fv(flatNormal.array());
        glVertex3fv(terrainPos[x][y].array());
        glVertex3fv(terrainPos[x + 1][y].array());
        glVertex3fv(terrainPos[x + 1][y + 1].array());
        glVertex3fv(terrainPos[x][y + 1].array());
      }
    }
    glEnd();
    glDisable(GL_LIGHTING);
  }

  // Draw the terrain normals
  if (D.displayMode4) {
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
  if (D.displayMode5) {
    for (int k= 0; k < dropletNbK; k++) {
      glPushMatrix();
      glTranslatef(dropletPosCur[k][0], dropletPosCur[k][1], dropletPosCur[k][2]);
      glScalef(dropletRadCur[k], dropletRadCur[k], dropletRadCur[k]);
      float r, g, b;
      Colormap::RatioToJetSmooth(dropletVelCur[k].norm(), r, g, b);
      glColor3f(r, g, b);
      // glColor4f(r, g, b, 0.2f);
      glutSolidSphere(1.0, 32, 16);
      glPopMatrix();
    }
  }
}
