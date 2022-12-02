#pragma once


class SrtColormap
{
  public:
  // Linear colormaps
  static void RatioToGrayscale(const double &iRatio, double &oR, double &oG, double &oB);
  static void RatioToPlasma(const double &iRatio, double &oR, double &oG, double &oB);
  static void RatioToViridis(const double &iRatio, double &oR, double &oG, double &oB);
  static void RatioToBlackBody(const double &iRatio, double &oR, double &oG, double &oB);

  // Divergent colormaps
  static void RatioToBlueToRed(const double &iRatio, double &oR, double &oG, double &oB);
  static void RatioToGreenToRed(const double &iRatio, double &oR, double &oG, double &oB);

  // Rainbow colormaps
  static void RatioToRainbow(const double &iRatio, double &oR, double &oG, double &oB);
  static void RatioToJet(const double &iRatio, double &oR, double &oG, double &oB);
  static void RatioToJetSmooth(const double &iRatio, double &oR, double &oG, double &oB);
  static void RatioToJetBright(const double &iRatio, double &oR, double &oG, double &oB);
  static void RatioToJetBrightSmooth(const double &iRatio, double &oR, double &oG, double &oB);

  // Circular colormaps
  static void RatioToRedGreenBlueRed(const double &iRatio, double &oR, double &oG, double &oB);
};
