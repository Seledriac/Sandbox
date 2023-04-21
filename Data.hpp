#pragma once

#include <string>
#include <utility>
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
  GR_ScreenNbT________,
  GR_ScreenNbH________,
  GR_ScreenNbV________,
  GR_ScreenNbS________,
  GR_CursorWorldT_____,
  GR_CursorScreenT____,
  GR_CurvaSmoothIter__,
  GR_CurvaTimePersist_,
  GR_CurvFactor_______,
  GR_DopplerShift_____,
  S0__________________,
  PD_Contrain2D_______,
  PD_NbParticles______,
  PD_TimeStep_________,
  PD_NbSubStep________,
  PD_VelocityDecay____,
  PD_FactorConduction_,
  PD_ForceGravity_____,
  PD_ForceBuoyancy____,
  PD_HeatInput________,
  PD_HeatOutput_______,
  S1__________________,
  AS_NbAgents_________,
  AS_SizeAgent________,
  AS_TimeStep_________,
  AS_CoeffSeparation__,
  AS_CoeffAlignment___,
  AS_CoeffCohesion____,
  AS_CoeffHunger______,
  AS_CoeffFear________,
  S2__________________,
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

  bool showAxis= false;

  bool showWorld= true;
  bool showScreen= true;
  bool showPhotonPath= false;
  bool showCursor= false;
  bool showFlow= false;

  bool showNodes= true;
  bool showEdges= true;
  bool showFaces= true;

  unsigned int idxParamUI= 0;

  std::vector<ParamUI> param;

  std::vector<std::pair<std::string, std::vector<double>>> plotData;

  Data() {
    param.clear();
    param.push_back(ParamUI("GR_WorldNbT_________", 10));
    param.push_back(ParamUI("GR_WorldNbX_________", 50));
    param.push_back(ParamUI("GR_WorldNbY_________", 50));
    param.push_back(ParamUI("GR_WorldNbZ_________", 50));
    param.push_back(ParamUI("GR_ScreenNbT________", 10));
    param.push_back(ParamUI("GR_ScreenNbH________", 50));
    param.push_back(ParamUI("GR_ScreenNbV________", 50));
    param.push_back(ParamUI("GR_ScreenNbS________", 50));
    param.push_back(ParamUI("GR_CursorWorldT_____", 0));
    param.push_back(ParamUI("GR_CursorScreenT____", 0));
    param.push_back(ParamUI("GR_CurvaSmoothIter__", 4));
    param.push_back(ParamUI("GR_CurvaTimePersist_", 0.8));
    param.push_back(ParamUI("GR_CurvFactor_______", 1.0));
    param.push_back(ParamUI("GR_DopplerShift_____", 1.0));
    param.push_back(ParamUI("S0__________________", 1.0));
    param.push_back(ParamUI("PD_Contrain2D_______", 1));
    param.push_back(ParamUI("PD_NbParticles______", 1000));
    param.push_back(ParamUI("PD_TimeStep_________", 0.05));
    param.push_back(ParamUI("PD_NbSubStep________", 8));
    param.push_back(ParamUI("PD_VelocityDecay____", 0.1));
    param.push_back(ParamUI("PD_FactorConduction_", 2.0));
    param.push_back(ParamUI("PD_ForceGravity_____", -1.0));
    param.push_back(ParamUI("PD_ForceBuoyancy____", 4.0));
    param.push_back(ParamUI("PD_HeatInput________", 4.0));
    param.push_back(ParamUI("PD_HeatOutput_______", 0.1));
    param.push_back(ParamUI("S1__________________", 1.0));
    param.push_back(ParamUI("AS_NbAgents_________", 100));
    param.push_back(ParamUI("AS_SizeAgent________", 0.05));
    param.push_back(ParamUI("AS_TimeStep_________", 0.05));
    param.push_back(ParamUI("AS_CoeffSeparation__", 0.75));
    param.push_back(ParamUI("AS_CoeffAlignment___", 0.015));
    param.push_back(ParamUI("AS_CoeffCohesion____", 0.45));
    param.push_back(ParamUI("AS_CoeffHunger______", 0.04));
    param.push_back(ParamUI("AS_CoeffFear________", 0.06));
    param.push_back(ParamUI("S2__________________", 1.0));
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
