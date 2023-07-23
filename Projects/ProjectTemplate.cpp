#include "ProjectTemplate.hpp"


// Standard lib
#include <cstdio>
#include <vector>

// GLUT lib
#include <GL/freeglut.h>

// Project lib
#include "../Data.hpp"
#include "../math/Vectors.hpp"


extern Data D;

enum ParamType
{
  testVar0____________,
  testVar1____________,
  testVar2____________,
  testVar3____________,
  testVar4____________,
  testVar5____________,
  testVar6____________,
  testVar7____________,
  testVar8____________,
  testVar9____________,
};

ProjectTemplate::ProjectTemplate() {
  isInitialized= false;
  isRefreshed= false;
}


void ProjectTemplate::Init() {
  isInitialized= true;
  isRefreshed= false;

  D.param.clear();
  D.param.push_back(ParamUI("testVar0____________", 1.0));
  D.param.push_back(ParamUI("testVar1____________", 1.0));
  D.param.push_back(ParamUI("testVar2____________", 1.0));
  D.param.push_back(ParamUI("testVar3____________", 1.0));
  D.param.push_back(ParamUI("testVar4____________", 1.0));
  D.param.push_back(ParamUI("testVar5____________", 1.0));
  D.param.push_back(ParamUI("testVar6____________", 1.0));
  D.param.push_back(ParamUI("testVar7____________", 1.0));
  D.param.push_back(ParamUI("testVar8____________", 1.0));
  D.param.push_back(ParamUI("testVar9____________", 1.0));
}


void ProjectTemplate::Refresh() {
  if (!isInitialized) return;
  isRefreshed= true;

  nbX= int(std::round(D.param[testVar0____________].val));
}


void ProjectTemplate::Animate() {
  if (!isInitialized) return;
  if (!isRefreshed) return;
}


void ProjectTemplate::Draw() {
  if (!isInitialized) return;
  if (!isRefreshed) return;
}
