#pragma once

// #include <iostream>
#include <string>
#include <utility>
#include <vector>

class ParamUI
{
  private:
  double val;
  double valOld;
  bool flag;
  std::string name;

  public:
  ParamUI(std::string const iName, double const iVal) {
    name= iName;
    val= iVal;
    flag= true;
    valOld= iVal - 1.0;
  }
  void Set(double const iVal) {
    flag= true;
    valOld= val;
    val= iVal;
  }
  double Get() {
    return val;
  }
  std::string GetName() {
    return name;
  }
  bool hasChanged() {
    if (flag) {
      flag= false;
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

  bool showAxis= true;

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
};
