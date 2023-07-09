#pragma once

#include <string>
#include <utility>
#include <vector>


class ParamUI
{
  public:
  std::string name;
  float val;

  ParamUI(std::string const iName, float const iVal) {
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
  GR_CursorWorldT_____,
  GR_MassReach________,
  GR_MassTimePersist__,
  GR_FactorCurv_______,
  GR_FactorDoppler____,
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
  TE_TerrainNbX_______,
  TE_TerrainNbY_______,
  TE_TerrainNbCuts____,
  TE_DropletNbK_______,
  TE_DropletRad_______,
  TE_SimuTimestep_____,
  TE_VelocityDecay____,
  TE_ErosionStrength__,
  TE_SmoothResist_____,
  S3__________________,
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
  bool autoRefresh= true;

  bool showAxis= false;

  bool displayMode1= true;
  bool displayMode2= true;
  bool displayMode3= true;
  bool displayMode4= true;
  bool displayMode5= true;
  bool displayMode6= true;
  bool displayMode7= true;
  bool displayMode8= true;
  bool displayMode9= true;
  bool displayMode0= true;

  unsigned int idxParamUI= 0;

  std::vector<ParamUI> param;

  std::vector<std::pair<std::string, std::vector<double>>> plotData;

  Data() {
    param.clear();
    param.push_back(ParamUI("GR_WorldNbT_________", 16));
    param.push_back(ParamUI("GR_WorldNbX_________", 32));
    param.push_back(ParamUI("GR_WorldNbY_________", 32));
    param.push_back(ParamUI("GR_WorldNbZ_________", 32));
    param.push_back(ParamUI("GR_ScreenNbH________", 64));
    param.push_back(ParamUI("GR_ScreenNbV________", 64));
    param.push_back(ParamUI("GR_ScreenNbS________", 64));
    param.push_back(ParamUI("GR_CursorWorldT_____", 8));
    param.push_back(ParamUI("GR_MassReach________", 8));
    param.push_back(ParamUI("GR_MassTimePersist__", 0.8));
    param.push_back(ParamUI("GR_FactorCurv_______", 1.0));
    param.push_back(ParamUI("GR_FactorDoppler____", 1.0));
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
    param.push_back(ParamUI("TE_TerrainNbX_______", 128));
    param.push_back(ParamUI("TE_TerrainNbY_______", 128));
    param.push_back(ParamUI("TE_TerrainNbCuts____", 256));
    param.push_back(ParamUI("TE_DropletNbK_______", 400));
    param.push_back(ParamUI("TE_DropletRad_______", 0.01));
    param.push_back(ParamUI("TE_SimuTimestep_____", 0.02));
    param.push_back(ParamUI("TE_VelocityDecay____", 0.5));
    param.push_back(ParamUI("TE_ErosionStrength__", 0.05));
    param.push_back(ParamUI("TE_SmoothResist_____", 0.99));
    param.push_back(ParamUI("S3__________________", 1.0));
    param.push_back(ParamUI("testVar0____________", 100.0));
    param.push_back(ParamUI("testVar1____________", 100.0));
    param.push_back(ParamUI("testVar2____________", 1.0));
    param.push_back(ParamUI("testVar3____________", 10.0));
    param.push_back(ParamUI("testVar4____________", 0.0));
    param.push_back(ParamUI("testVar5____________", 0.0));
    param.push_back(ParamUI("testVar6____________", 0.0));
    param.push_back(ParamUI("testVar7____________", 0.8));
    param.push_back(ParamUI("testVar8____________", 0.1));
    param.push_back(ParamUI("testVar9____________", 1.0));
  }
};
