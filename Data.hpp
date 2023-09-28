#pragma once

#include <array>
#include <string>
#include <vector>

class ParamUI
{
  private:
  double val;
  bool changeFlag;

  public:
  std::string name;
  ParamUI(std::string const iName, double const iVal) {
    name= iName;
    val= iVal;
    changeFlag= true;
  }

  void Set(double const iVal) {
    changeFlag= true;
    val= iVal;
  }

  bool GetB() { return val > 0.0; }
  int GetI() { return (int)((val < 0.0) ? (val - 0.5) : (val + 0.5)); }
  float GetF() { return (float)val; }
  double GetD() { return val; }

  bool hasChanged() {
    if (changeFlag) {
      changeFlag= false;
      return true;
    }
    return false;
  }
};


class Data
{
  public:
  bool playAnimation= false;
  bool stepAnimation= false;
  bool autoRefresh= true;

  bool displayMode1= true;
  bool displayMode2= true;
  bool displayMode3= true;
  bool displayMode4= true;
  bool displayMode5= true;
  bool displayMode6= true;
  bool displayMode7= true;
  bool displayMode8= true;
  bool showAxis= true;

  std::array<double, 3> boxMin= {0.0, 0.0, 0.0};
  std::array<double, 3> boxMax= {1.0, 1.0, 1.0};

  int idxParamUI= 0;
  int idxCursorUI= 0;
  std::vector<ParamUI> UI;

  bool plotLogScale= true;
  std::vector<std::string> plotLegend;
  std::vector<std::vector<double>> plotData;

  std::vector<std::string> scatLegend;
  std::vector<std::vector<std::array<double, 2>>> scatData;
};
