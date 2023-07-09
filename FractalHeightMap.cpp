#include "FractalHeightMap.hpp"


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


FractalHeightMap::FractalHeightMap() {
  isInitialized= false;
  isRefreshed= false;
}


void FractalHeightMap::Init() {
  isInitialized= true;
  isRefreshed= false;
}


void FractalHeightMap::Refresh() {
  if (!isInitialized) return;
  isRefreshed= true;

  // Get UI parameters
  mapNbX= std::max(2, int(std::round(D.param[testVar0____________].val)));
  mapNbY= std::max(2, int(std::round(D.param[testVar1____________].val)));
  D.param[testVar0____________].val= mapNbX;
  D.param[testVar1____________].val= mapNbY;

  mapZoom= std::max(0.0, double(D.param[testVar2____________].val));
  D.param[testVar2____________].val= mapZoom;

  mapNbIter= std::max(1, int(std::round(D.param[testVar3____________].val)));
  D.param[testVar3____________].val= mapNbIter;

  mapFocus= Math::Vec2d(D.param[testVar4____________].val, D.param[testVar5____________].val);
  mapConst= Math::Vec2d(D.param[testVar6____________].val, D.param[testVar7____________].val);

  mapDivThresh= D.param[testVar8____________].val;

  // Allocate data
  mapPos= Field::AllocField2D(mapNbX, mapNbY, Math::Vec3f(0.0f, 0.0f, 0.0f));
  mapNor= Field::AllocField2D(mapNbX, mapNbY, Math::Vec3f(0.0f, 0.0f, 1.0f));
  mapCol= Field::AllocField2D(mapNbX, mapNbY, Math::Vec3f(0.5f, 0.5f, 0.5f));

  // Compute positions
  for (int x= 0; x < mapNbX; x++) {
    for (int y= 0; y < mapNbY; y++) {
      mapPos[x][y][0]= float(x) / float(mapNbX - 1);
      mapPos[x][y][1]= float(y) / float(mapNbY - 1);

      Math::Vec2d z= mapFocus + Math::Vec2d(2.0 * double(x) / double(mapNbX - 1) - 1.0, 2.0 * double(y) / double(mapNbY - 1) - 1.0) / mapZoom;
      int idxIter= 0;
      while (idxIter < mapNbIter && z.normSquared() < mapDivThresh) {
        const double zr= z[0] * z[0] - z[1] * z[1];
        const double zi= 2.0 * z[0] * z[1];
        z= Math::Vec2d(zr, zi) + mapConst;
        idxIter++;
      }
      double val= double(idxIter) / double(mapNbIter-1);
      // double val= double(idxIter) - std::log2(std::max(std::log2(z.normSquared()), 1.0));
      // if (val != 0.0) val= std::log2(std::max(std::log2(val), 1.0));

      // mapPos[x][y][2]= float(z.norm());
      // if (mapPos[x][y][2] != mapPos[x][y][2]) mapPos[x][y][2]= D.param[testVar8____________].val;
      Colormap::RatioToJetSmooth(float(val), mapCol[x][y][0], mapCol[x][y][1], mapCol[x][y][2]);

      mapPos[x][y][2]= 0.5f + 0.1f * std::min(std::max(float(val), 0.0f), 1.0f);
    }
  }

  // Compute normals
  for (int x= 0; x < mapNbX; x++) {
    for (int y= 0; y < mapNbY; y++) {
      mapNor[x][y].set(0.0f, 0.0f, 0.0f);
      if (x > 0 && y > 0)
        mapNor[x][y]+= ((mapPos[x - 1][y] - mapPos[x][y]).cross(mapPos[x][y - 1] - mapPos[x][y])).normalized();
      if (x < mapNbX - 1 && y > 0)
        mapNor[x][y]+= ((mapPos[x][y - 1] - mapPos[x][y]).cross(mapPos[x + 1][y] - mapPos[x][y])).normalized();
      if (x < mapNbX - 1 && y < mapNbY - 1)
        mapNor[x][y]+= ((mapPos[x + 1][y] - mapPos[x][y]).cross(mapPos[x][y + 1] - mapPos[x][y])).normalized();
      if (x > 0 && y < mapNbY - 1)
        mapNor[x][y]+= ((mapPos[x][y + 1] - mapPos[x][y]).cross(mapPos[x - 1][y] - mapPos[x][y])).normalized();
      mapNor[x][y].normalize();
    }
  }
}


void FractalHeightMap::Draw() {
  if (!isInitialized) return;
  if (!isRefreshed) return;

  // Draw the map
  glEnable(GL_LIGHTING);
  glBegin(GL_QUADS);
  for (int x= 0; x < mapNbX - 1; x++) {
    for (int y= 0; y < mapNbY - 1; y++) {
      Math::Vec3f flatNormal= (mapNor[x][y] + mapNor[x + 1][y] + mapNor[x + 1][y + 1] + mapNor[x][y + 1]).normalized();
      Math::Vec3f flatColor= (mapCol[x][y] + mapCol[x + 1][y] + mapCol[x + 1][y + 1] + mapCol[x][y + 1]) / 4.0f;
      glColor3fv((flatColor / 2.0f).array());
      glNormal3fv(flatNormal.array());
      glVertex3fv(mapPos[x][y].array());
      glVertex3fv(mapPos[x + 1][y].array());
      glVertex3fv(mapPos[x + 1][y + 1].array());
      glVertex3fv(mapPos[x][y + 1].array());
    }
  }
  glEnd();
  glDisable(GL_LIGHTING);
}
