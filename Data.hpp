#include <string>
#include <vector>

enum ParamType
{
  worldNbT______,
  worldNbX______,
  worldNbY______,
  worldNbZ______,
  screenNbH_____,
  screenNbV_____,
  screenNbS_____,
  gravStrength__,
};


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


class Data
{
  public:
  bool playAnimation= false;

  int idxParamUI= 0;

  std::vector<ParamUI> param;
};
