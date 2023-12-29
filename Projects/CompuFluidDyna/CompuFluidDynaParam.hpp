#pragma once

// Sandbox lib
#include "../../Data.hpp"

// Link to shared sandbox data
extern Data D;

// List of UI parameters for this project
enum ParamType
{
  Scenario____,
  InputFile___,
  ResolutionX_,
  ResolutionY_,
  ResolutionZ_,
  VoxelSize___,
  TimeStep____,
  SolvMaxIter_,
  SolvType____,
  SolvSOR_____,
  SolvTolRhs__,
  SolvTolRel__,
  SolvTolAbs__,
  OptimPDDTol_,
  KEDTol______,
  FlushTol____,
  StaIterWin__,
  OptiIterWin_,
  SafeZoneRad_,
  FracErosion_,
  CoeffFluTime,
  CoeffGravi__,
  CoeffAdvec__,
  CoeffDiffuS_,
  CoeffDiffuV_,
  CoeffVorti__,
  CoeffProj___,
  BCVelX______,
  BCVelY______,
  BCVelZ______,
  BCPres______,
  BCSmok______,
  BCSmokTime__,
  ObjectPosX__,
  ObjectPosY__,
  ObjectPosZ__,
  ObjectSize0_,
  ObjectSize1_,
  ScaleFactor_,
  ColorFactor_,
  ColorThresh_,
  ColorMode___,
  SliceDim____,
  SlicePlotX__,
  SlicePlotY__,
  SlicePlotZ__,
  VerboseSolv_,
  VerboseTime_,
  Verbose_____,
};
