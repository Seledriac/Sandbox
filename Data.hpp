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
  GR_WorldNbT_________,
  GR_WorldNbX_________,
  GR_WorldNbY_________,
  GR_WorldNbZ_________,
  GR_ScreenNbH________,
  GR_ScreenNbV________,
  GR_ScreenNbS________,
  GR_CursorPosX_______,
  GR_CursorPosY_______,
  GR_CursorPosZ_______,
  GR_GravStrength_____,
  GR_DragStrength_____,
  GR_DopplerShift_____,
  PD_Contrain2D_______,
  PD_NbParticles______,
  PD_TimeStep_________,
  PD_NbSubStep________,
  PD_FactorConduction_,
  PD_ForceGravity_____,
  PD_ForceBuoyancy____,
  PD_HeatInput________,
  PD_HeatOutput_______,
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
    param.push_back(ParamUI("GR_WorldNbT_________", 1));
    param.push_back(ParamUI("GR_WorldNbX_________", 50));
    param.push_back(ParamUI("GR_WorldNbY_________", 50));
    param.push_back(ParamUI("GR_WorldNbZ_________", 50));
    param.push_back(ParamUI("GR_ScreenNbH________", 100));
    param.push_back(ParamUI("GR_ScreenNbV________", 100));
    param.push_back(ParamUI("GR_ScreenNbS________", 40));
    param.push_back(ParamUI("GR_CursorPosX_______", 50));
    param.push_back(ParamUI("GR_CursorPosY_______", 50));
    param.push_back(ParamUI("GR_CursorPosZ_______", 50));
    param.push_back(ParamUI("GR_GravStrength_____", 1.0));
    param.push_back(ParamUI("GR_DragStrength_____", 1.0));
    param.push_back(ParamUI("GR_DopplerShift_____", 1.0));
    param.push_back(ParamUI("PD_Contrain2D_______", 1));
    param.push_back(ParamUI("PD_NbParticles______", 1000));
    param.push_back(ParamUI("PD_TimeStep_________", 0.05));
    param.push_back(ParamUI("PD_NbSubStep________", 4));
    param.push_back(ParamUI("PD_FactorConduction_", 2.0));
    param.push_back(ParamUI("PD_ForceGravity_____", -1.0));
    param.push_back(ParamUI("PD_ForceBuoyancy____", 8.0));
    param.push_back(ParamUI("PD_HeatInput________", 1.0));
    param.push_back(ParamUI("PD_HeatOutput_______", 0.3));
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
