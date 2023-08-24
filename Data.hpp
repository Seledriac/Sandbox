#pragma once

#include <string>
#include <utility>
#include <vector>
#include <array>

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

  double Get() {
    return val;
  }

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

  int idxParamUI= 0;
  int idxCursorUI= 0;

  std::vector<ParamUI> param;

  std::vector<std::pair<std::string, std::vector<double>>> plotData;
  std::vector<std::pair<std::string, std::vector<std::array<double, 2>>>> scatData;
};
