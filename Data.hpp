#pragma once

#include <string>
#include <vector>


class ParamUI
{
  public:
  std::string name;
  double val;

  ParamUI(std::string const iName, double const iVal) {
    name= iName;
    val= iVal;
  }
};

enum ParamType
{
  worldNbT____________,
  worldNbX____________,
  worldNbY____________,
  worldNbZ____________,
  screenNbH___________,
  screenNbV___________,
  screenNbS___________,
  cursorPosX__________,
  cursorPosY__________,
  cursorPosZ__________,
  gravStrength________,
  dragStrength________,
  dopplerShift________,
  nbParticles_________,
  timeStep____________,
  ratioDamping________,
  ratioConduction_____,
  forceGravity________,
  forceBoundary_______,
  forceCollision______,
  forceBuoyancy_______,
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

class Data
{
  public:
  bool playAnimation= false;

  bool showWorld= true;
  bool showScreen= true;
  bool showPhotonPath= false;
  bool showCursor= true;
  bool showGravity= false;

  int idxParamUI= 0;

  std::vector<ParamUI> param;

  Data() {
    param.clear();
    param.push_back(ParamUI("worldNbT____________", 1.0));
    param.push_back(ParamUI("worldNbX____________", 50.0));
    param.push_back(ParamUI("worldNbY____________", 50.0));
    param.push_back(ParamUI("worldNbZ____________", 50.0));
    param.push_back(ParamUI("screenNbH___________", 100.0));
    param.push_back(ParamUI("screenNbV___________", 100.0));
    param.push_back(ParamUI("screenNbS___________", 40.0));
    param.push_back(ParamUI("cursorPosX__________", 50.0));
    param.push_back(ParamUI("cursorPosY__________", 50.0));
    param.push_back(ParamUI("cursorPosZ__________", 50.0));
    param.push_back(ParamUI("gravStrength________", 1.0));
    param.push_back(ParamUI("dragStrength________", 1.0));
    param.push_back(ParamUI("dopplerShift________", 1.0));
    param.push_back(ParamUI("nbParticles_________", 20.0));
    param.push_back(ParamUI("timeStep____________", 0.1));
    param.push_back(ParamUI("ratioDamping________", 0.05));
    param.push_back(ParamUI("ratioConduction_____", 0.01));
    param.push_back(ParamUI("forceGravity________", 100.0));
    param.push_back(ParamUI("forceBoundary_______", 1000.0));
    param.push_back(ParamUI("forceCollision______", 1000.0));
    param.push_back(ParamUI("forceBuoyancy_______", 100.0));
    param.push_back(ParamUI("testVar0____________", 1.0));
    param.push_back(ParamUI("testVar1____________", 1.0));
    param.push_back(ParamUI("testVar2____________", 1.0));
    param.push_back(ParamUI("testVar3____________", 1.0));
    param.push_back(ParamUI("testVar4____________", 1.0));
    param.push_back(ParamUI("testVar5____________", 1.0));
    param.push_back(ParamUI("testVar6____________", 1.0));
    param.push_back(ParamUI("testVar7____________", 1.0));
    param.push_back(ParamUI("testVar8____________", 1.0));
    param.push_back(ParamUI("testVar9____________", 1.0));
  }
};
