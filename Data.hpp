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
  gravStrength________,
  dragStrength________,
  dopplerStrength_____,
};

class Data
{
  public:
  bool playAnimation= false;

  bool showWorld= true;
  bool showScreen= true;
  bool showPhotonPath= false;
  bool showGravity= false;

  int idxParamUI= 0;

  std::vector<ParamUI> param;

  Data() {
    param.clear();
    param.push_back(ParamUI("worldNbT____________", 1));
    param.push_back(ParamUI("worldNbX____________", 50));
    param.push_back(ParamUI("worldNbY____________", 50));
    param.push_back(ParamUI("worldNbZ____________", 50));
    param.push_back(ParamUI("screenNbH___________", 100));
    param.push_back(ParamUI("screenNbV___________", 100));
    param.push_back(ParamUI("screenNbS___________", 40));
    param.push_back(ParamUI("gravStrength________", 1.0));
    param.push_back(ParamUI("dragStrength________", 1.0));
    param.push_back(ParamUI("dopplerStrength_____", 1.0));
  }
};
