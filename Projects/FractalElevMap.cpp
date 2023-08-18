#include "FractalElevMap.hpp"


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
#include "../Util/Colormap.hpp"
#include "../Util/Field.hpp"
#include "../Util/Random.hpp"
#include "../Util/Vector.hpp"


extern Data D;

enum ParamType
{
  testVar0____,
  testVar1____,
  testVar2____,
  testVar3____,
  testVar4____,
  testVar5____,
  testVar6____,
  testVar7____,
  testVar8____,
  testVar9____,
};


FractalElevMap::FractalElevMap() {
  D.param.clear();
  D.plotData.clear();
  isActiveProject= false;
  isInitialized= false;
  isRefreshed= false;
}


void FractalElevMap::SetActiveProject() {
  if (!isActiveProject) {
    D.param.push_back(ParamUI("testVar0____", 500.0));
    D.param.push_back(ParamUI("testVar1____", 500.0));
    D.param.push_back(ParamUI("testVar2____", 0.5));
    D.param.push_back(ParamUI("testVar3____", 40.0));
    D.param.push_back(ParamUI("testVar4____", 0.365242));
    D.param.push_back(ParamUI("testVar5____", 0.534752));
    D.param.push_back(ParamUI("testVar6____", -0.8350));
    D.param.push_back(ParamUI("testVar7____", -0.2241));
    D.param.push_back(ParamUI("testVar8____", 32.0));
    D.param.push_back(ParamUI("testVar9____", 1.0));
  }

  isActiveProject= true;
  isInitialized= false;
  isRefreshed= false;
  Initialize();
}


void FractalElevMap::Initialize() {
  // Check if need to skip
  if (!isActiveProject) return;
  if (D.param[testVar0____].hasChanged()) isInitialized= false;
  if (D.param[testVar1____].hasChanged()) isInitialized= false;
  if (D.param[testVar2____].hasChanged()) isInitialized= false;
  if (D.param[testVar3____].hasChanged()) isInitialized= false;
  if (D.param[testVar4____].hasChanged()) isInitialized= false;
  if (D.param[testVar5____].hasChanged()) isInitialized= false;
  if (D.param[testVar6____].hasChanged()) isInitialized= false;
  if (D.param[testVar7____].hasChanged()) isInitialized= false;
  if (D.param[testVar8____].hasChanged()) isInitialized= false;
  if (D.param[testVar9____].hasChanged()) isInitialized= false;
  if (isInitialized) return;
  isInitialized= true;

  // Get UI parameters
  mapNbX= std::max(2, int(std::round(D.param[testVar0____].Get())));
  mapNbY= std::max(2, int(std::round(D.param[testVar1____].Get())));

  // Allocate data
  mapPos= Field::AllocField2D(mapNbX, mapNbY, Math::Vec3f(0.0f, 0.0f, 0.0f));
  mapNor= Field::AllocField2D(mapNbX, mapNbY, Math::Vec3f(0.0f, 0.0f, 1.0f));
  mapCol= Field::AllocField2D(mapNbX, mapNbY, Math::Vec3f(0.5f, 0.5f, 0.5f));

  // Force refresh
  isRefreshed= false;
  Refresh();
}


void FractalElevMap::Refresh() {
  if (!isActiveProject) return;
  if (!isInitialized) return;
  if (isRefreshed) return;
  isRefreshed= true;


  mapZoom= std::max(1.e-6, double(D.param[testVar2____].Get()));

  mapNbIter= std::max(1, int(std::round(D.param[testVar3____].Get())));

  mapFocus= Math::Vec2d(D.param[testVar4____].Get(), D.param[testVar5____].Get());
  mapConst= Math::Vec2d(D.param[testVar6____].Get(), D.param[testVar7____].Get());

  mapDivThresh= std::max(0.0, double(D.param[testVar8____].Get()));


  // Compute positions
#pragma omp parallel for
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

      // double val= double(idxIter) / double(mapNbIter-1);
      double val= (double(idxIter) - std::log2(std::max(std::log2(z.normSquared()), 1.0))) / double(mapNbIter - 1);
      // double val= - std::log2(std::max(std::log2(z.normSquared()), 1.0));
      // if (val != 0.0) val= std::log2(std::max(std::log2(val), 1.0));

      // mapPos[x][y][2]= float(z.norm());
      // if (mapPos[x][y][2] != mapPos[x][y][2]) mapPos[x][y][2]= D.param[testVar8____].Get();
      Colormap::RatioToJetSmooth(float(val), mapCol[x][y][0], mapCol[x][y][1], mapCol[x][y][2]);

      mapPos[x][y][2]= 0.5f + 0.04f * std::min(std::max(float(val), 0.0f), 1.0f);
    }
  }

  // Smooth the positions
  for (int iter= 0; iter < std::max(mapNbX, mapNbY) / 128; iter++) {
    std::vector<std::vector<Math::Vec3f>> mapPosOld= mapPos;
#pragma omp parallel for
    for (int x= 0; x < mapNbX; x++) {
      for (int y= 0; y < mapNbY; y++) {
        int count= 0;
        mapPos[x][y][2]= 0.0;
        for (int xOff= std::max(x - 1, 0); xOff <= std::min(x + 1, mapNbX - 1); xOff++) {
          for (int yOff= std::max(y - 1, 0); yOff <= std::min(y + 1, mapNbY - 1); yOff++) {
            mapPos[x][y][2]+= mapPosOld[xOff][yOff][2];
            count++;
          }
        }
        mapPos[x][y][2]/= float(count);
      }
    }
  }

  // Compute normals
#pragma omp parallel for
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


void FractalElevMap::Animate() {
  if (!isActiveProject) return;
  if (!isInitialized) return;
  if (!isRefreshed) return;
}


void FractalElevMap::Draw() {
  if (!isActiveProject) return;
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
