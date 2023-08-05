
#include "MarkovProcGene.hpp"


// Standard lib
#include <cstdio>
#include <cstdlib>
#include <vector>

// GLUT lib
#include <GL/freeglut.h>

// Project lib
#include "../Data.hpp"
#include "../util/Field.hpp"


extern Data D;

enum ParamType
{
  Scenario____________,
  ResolutionX_________,
  ResolutionY_________,
  ResolutionZ_________,
  MaxSubstiCount______,
};


MarkovProcGene::MarkovProcGene() {
  D.param.clear();
  D.plotData.clear();
  isActiveProject= false;
  isInitialized= false;
  isRefreshed= false;
}


void MarkovProcGene::SetActiveProject() {
  if (!isActiveProject) {
    D.param.push_back(ParamUI("Scenario____________", 2));
    D.param.push_back(ParamUI("ResolutionX_________", 10));
    D.param.push_back(ParamUI("ResolutionY_________", 10));
    D.param.push_back(ParamUI("ResolutionZ_________", 10));
    D.param.push_back(ParamUI("MaxSubstiCount______", 1000));
  }

  isActiveProject= true;
  isInitialized= false;
  isRefreshed= false;
  Initialize();
}


void MarkovProcGene::Initialize() {
  // Check if need to skip
  if (!isActiveProject) return;
  if (D.param[Scenario____________].hasChanged()) isInitialized= false;
  if (D.param[ResolutionX_________].hasChanged()) isInitialized= false;
  if (D.param[ResolutionY_________].hasChanged()) isInitialized= false;
  if (D.param[ResolutionZ_________].hasChanged()) isInitialized= false;
  if (isInitialized) return;
  isInitialized= true;

  // Get UI parameters
  nbX= std::max((int)std::round(D.param[ResolutionX_________].Get()), 1);
  nbY= std::max((int)std::round(D.param[ResolutionY_________].Get()), 1);
  nbZ= std::max((int)std::round(D.param[ResolutionZ_________].Get()), 1);

  // Force refresh
  isRefreshed= false;
  Refresh();
}


void MarkovProcGene::Refresh() {
  if (isRefreshed) return;
  isRefreshed= true;

  // Reset progress counters
  idxSubsti= 0;
  activeSeq= 0;

  // Get the scenario ID
  int scenario= (int)std::round(D.param[Scenario____________].Get());

  // Initialize dictionnary and field values
  Dict.clear();
  Field= Field::AllocField3D(nbX, nbY, nbZ, 0);
  if (scenario == 0) {
    Dict.push_back(std::vector<std::array<std::vector<std::vector<std::vector<int>>>, 2>>());
    Dict[(int)Dict.size() - 1].push_back(std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(1, 1, 1, 0), Field::AllocField3D(1, 1, 1, 0)}));
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][0][0][0][0]= 0;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][0][0]= 1;
    Dict.push_back(std::vector<std::array<std::vector<std::vector<std::vector<int>>>, 2>>());
    Dict[(int)Dict.size() - 1].push_back(std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(1, 1, 1, 0), Field::AllocField3D(1, 1, 1, 0)}));
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][0][0][0][0]= 1;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][0][0]= 2;
    Dict[(int)Dict.size() - 1].push_back(std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(1, 1, 1, 0), Field::AllocField3D(1, 1, 1, 0)}));
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][0][0][0][0]= 1;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][0][0]= 3;
  }

  if (scenario == 1) {
    Dict.push_back(std::vector<std::array<std::vector<std::vector<std::vector<int>>>, 2>>());
    Dict[(int)Dict.size() - 1].push_back(std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(1, 1, 2, 0), Field::AllocField3D(1, 1, 2, 0)}));
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][0][0][0][0]= 0;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][0][0][0][1]= 1;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][0][0]= 1;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][0][1]= 1;
    Dict[(int)Dict.size() - 1].push_back(std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(1, 1, 2, 0), Field::AllocField3D(1, 1, 2, 0)}));
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][0][0][0][0]= 1;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][0][0][0][1]= 0;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][0][0]= 1;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][0][1]= 1;
    Dict[(int)Dict.size() - 1].push_back(std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(1, 2, 1, 0), Field::AllocField3D(1, 2, 1, 0)}));
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][0][0][0][0]= 0;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][0][0][1][0]= 1;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][0][0]= 1;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][1][0]= 1;
    Dict[(int)Dict.size() - 1].push_back(std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(1, 2, 1, 0), Field::AllocField3D(1, 2, 1, 0)}));
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][0][0][0][0]= 1;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][0][0][1][0]= 0;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][0][0]= 1;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][1][0]= 1;

    Field[nbX / 2][nbY / 2][nbZ / 2]= 1;
  }

  if (scenario == 2) {
    Dict.push_back(std::vector<std::array<std::vector<std::vector<std::vector<int>>>, 2>>());
    Dict[(int)Dict.size() - 1].push_back(std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(5, 5, 1, 2), Field::AllocField3D(5, 5, 1, 2)}));
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][0][2][2][0]= 2;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][2][2][0]= 3;
    Dict.push_back(std::vector<std::array<std::vector<std::vector<std::vector<int>>>, 2>>());
    Dict[(int)Dict.size() - 1].push_back(std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(1, 1, 2, 0), Field::AllocField3D(1, 1, 2, 0)}));
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][0][0][0][0]= 3;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][0][0]= 3;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][0][1]= 1;
    Dict.push_back(std::vector<std::array<std::vector<std::vector<std::vector<int>>>, 2>>());
    Dict[(int)Dict.size() - 1].push_back(std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(5, 5, 2, 0), Field::AllocField3D(5, 5, 2, 0)}));
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][0][2][2][0]= 1;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][2][2][0]= 2;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][2][2][1]= 1;
    Dict[(int)Dict.size() - 1].push_back(std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(5, 5, 2, 0), Field::AllocField3D(5, 5, 2, 0)}));
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][0][2][2][0]= 1;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][2][2][0]= 2;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][2][1][1]= 1;
    Dict[(int)Dict.size() - 1].push_back(std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(5, 5, 2, 0), Field::AllocField3D(5, 5, 2, 0)}));
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][0][2][2][0]= 1;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][2][2][0]= 2;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][1][2][1]= 1;
    Dict[(int)Dict.size() - 1].push_back(std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(5, 5, 2, 0), Field::AllocField3D(5, 5, 2, 0)}));
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][0][2][2][0]= 1;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][2][2][0]= 2;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][2][3][1]= 1;
    Dict[(int)Dict.size() - 1].push_back(std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(5, 5, 2, 0), Field::AllocField3D(5, 5, 2, 0)}));
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][0][2][2][0]= 1;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][2][2][0]= 2;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][3][2][1]= 1;
    Dict.push_back(std::vector<std::array<std::vector<std::vector<std::vector<int>>>, 2>>());
    Dict[(int)Dict.size() - 1].push_back(std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(3, 3, 1, 0), Field::AllocField3D(3, 3, 1, 0)}));
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][0][1][1][0]= 1;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][1][0][0]= 3;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][1][0]= 3;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][1][2][0]= 3;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][2][1][0]= 3;
    Dict[(int)Dict.size() - 1].push_back(std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(3, 3, 1, 0), Field::AllocField3D(3, 3, 1, 0)}));
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][0][1][1][0]= 1;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][1][0][0]= 4;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][1][0]= 4;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][1][2][0]= 4;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][2][1][0]= 4;
    Dict[(int)Dict.size() - 1].push_back(std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(3, 3, 1, 0), Field::AllocField3D(3, 3, 1, 0)}));
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][0][1][1][0]= 1;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][1][0][0]= 5;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][1][0]= 5;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][1][2][0]= 5;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][2][1][0]= 5;
    Dict[(int)Dict.size() - 1].push_back(std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(3, 3, 1, 0), Field::AllocField3D(3, 3, 1, 0)}));
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][0][1][1][0]= 1;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][1][0][0]= 6;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][1][0]= 6;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][1][2][0]= 6;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][2][1][0]= 6;

    for (int x= 0; x < nbX; x++)
      for (int y= 0; y < nbY; y++)
        Field[x][y][0]= 2;
  }

  if (scenario == 3) {
    Dict.push_back(std::vector<std::array<std::vector<std::vector<std::vector<int>>>, 2>>());
    Dict[(int)Dict.size() - 1].push_back(std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(1, 1, 3, 0), Field::AllocField3D(1, 1, 3, 0)}));
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][0][0][0][0]= 1;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][0][0]= 2;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][0][1]= 2;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][0][2]= 1;
    Dict[(int)Dict.size() - 1].push_back(std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(1, 1, 3, 0), Field::AllocField3D(1, 1, 3, 0)}));
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][0][0][0][2]= 1;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][0][0]= 1;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][0][1]= 2;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][0][2]= 2;
    Dict[(int)Dict.size() - 1].push_back(std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(1, 3, 1, 0), Field::AllocField3D(1, 3, 1, 0)}));
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][0][0][0][0]= 1;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][0][0]= 2;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][1][0]= 2;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][2][0]= 1;
    Dict[(int)Dict.size() - 1].push_back(std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(1, 3, 1, 0), Field::AllocField3D(1, 3, 1, 0)}));
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][0][0][2][0]= 1;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][0][0]= 1;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][1][0]= 2;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][2][0]= 2;

    Field[nbX / 2][nbY / 2][nbZ / 2]= 1;
  }

  if (scenario == 4) {
    Dict.push_back(std::vector<std::array<std::vector<std::vector<std::vector<int>>>, 2>>());
    Dict[(int)Dict.size() - 1].push_back(std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(1, 1, 3, 0), Field::AllocField3D(1, 1, 3, 0)}));
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][0][0][0][0]= 1;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][0][0]= 1;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][0][1]= 2;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][0][2]= 1;
    Dict[(int)Dict.size() - 1].push_back(std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(1, 1, 3, 0), Field::AllocField3D(1, 1, 3, 0)}));
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][0][0][0][2]= 1;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][0][0]= 1;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][0][1]= 2;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][0][2]= 1;
    Dict[(int)Dict.size() - 1].push_back(std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(1, 3, 1, 0), Field::AllocField3D(1, 3, 1, 0)}));
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][0][0][0][0]= 1;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][0][0]= 1;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][1][0]= 2;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][2][0]= 1;
    Dict[(int)Dict.size() - 1].push_back(std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(1, 3, 1, 0), Field::AllocField3D(1, 3, 1, 0)}));
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][0][0][2][0]= 1;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][0][0]= 1;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][1][0]= 2;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][2][0]= 1;

    Dict.push_back(std::vector<std::array<std::vector<std::vector<std::vector<int>>>, 2>>());
    Dict[(int)Dict.size() - 1].push_back(std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(1, 1, 1, 0), Field::AllocField3D(1, 1, 1, 0)}));
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][0][0][0][0]= 1;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][0][0]= 2;

    Field[nbX / 2][nbY / 2][nbZ / 2]= 1;
  }
}


void MarkovProcGene::Animate() {
  if (!isActiveProject) return;
  if (!isInitialized) return;
  if (!isRefreshed) return;

  if (idxSubsti >= (int)std::round(D.param[MaxSubstiCount______].Get())) return;
  if (activeSeq >= (int)Dict.size()) return;

  int matchCount= 0;
  for (int idxRule= 0; idxRule < (int)Dict[activeSeq].size(); idxRule++) {
    int nbXRule= (int)Dict[activeSeq][idxRule][0].size();
    int nbYRule= (int)Dict[activeSeq][idxRule][0][0].size();
    int nbZRule= (int)Dict[activeSeq][idxRule][0][0][0].size();
    for (int xF= 0; xF <= nbX - nbXRule; xF++) {
      for (int yF= 0; yF <= nbY - nbYRule; yF++) {
        for (int zF= 0; zF <= nbZ - nbZRule; zF++) {
          bool isMatch= true;
          for (int xR= 0; xR < nbXRule && isMatch; xR++)
            for (int yR= 0; yR < nbYRule && isMatch; yR++)
              for (int zR= 0; zR < nbZRule && isMatch; zR++)
                if (Dict[activeSeq][idxRule][0][xR][yR][zR] >= 0)
                  if (Field[xF + xR][yF + yR][zF + zR] != Dict[activeSeq][idxRule][0][xR][yR][zR])
                    isMatch= false;
          if (isMatch)
            matchCount++;
        }
      }
    }
  }
  if (matchCount == 0) {
    activeSeq++;
    return;
  }
  idxSubsti++;

  int matchChosen= rand() % matchCount;
  for (int idxRule= 0; idxRule < (int)Dict[activeSeq].size(); idxRule++) {
    int nbXRule= (int)Dict[activeSeq][idxRule][0].size();
    int nbYRule= (int)Dict[activeSeq][idxRule][0][0].size();
    int nbZRule= (int)Dict[activeSeq][idxRule][0][0][0].size();
    for (int xF= 0; xF <= nbX - nbXRule; xF++) {
      for (int yF= 0; yF <= nbY - nbYRule; yF++) {
        for (int zF= 0; zF <= nbZ - nbZRule; zF++) {
          bool isMatch= true;
          for (int xR= 0; xR < nbXRule && isMatch; xR++)
            for (int yR= 0; yR < nbYRule && isMatch; yR++)
              for (int zR= 0; zR < nbZRule && isMatch; zR++)
                if (Dict[activeSeq][idxRule][0][xR][yR][zR] >= 0)
                  if (Field[xF + xR][yF + yR][zF + zR] != Dict[activeSeq][idxRule][0][xR][yR][zR])
                    isMatch= false;
          if (isMatch)
            matchCount--;
          if (matchCount == matchChosen)
            for (int xR= 0; xR < nbXRule && isMatch; xR++)
              for (int yR= 0; yR < nbYRule && isMatch; yR++)
                for (int zR= 0; zR < nbZRule && isMatch; zR++)
                  if (Dict[activeSeq][idxRule][0][xR][yR][zR] >= 0)
                    Field[xF + xR][yF + yR][zF + zR]= Dict[activeSeq][idxRule][1][xR][yR][zR];
        }
      }
    }
  }
}


void util_SetColorVoxel(const int val) {
  if (val == 1) glColor3f(0.5f, 0.2f, 0.2f);
  if (val == 2) glColor3f(0.2f, 0.5f, 0.2f);
  if (val == 3) glColor3f(0.2f, 0.2f, 0.5f);
  if (val == 4) glColor3f(0.1f, 0.4f, 0.4f);
  if (val == 5) glColor3f(0.4f, 0.1f, 0.4f);
  if (val == 6) glColor3f(0.4f, 0.4f, 0.1f);
}

void util_DrawBoxPosPos(const float begX, const float begY, const float begZ,
                        const float endX, const float endY, const float endZ, bool const isSolid) {
  glPushMatrix();
  glTranslatef(begX, begY, begZ);
  glScalef(endX - begX, endY - begY, endZ - begZ);
  glTranslatef(0.5f, 0.5f, 0.5f);
  if (isSolid) glutSolidCube(1.0);
  else glutWireCube(1.0);
  glPopMatrix();
}

void util_DrawBoxPosSiz(const float begX, const float begY, const float begZ,
                        const float sizX, const float sizY, const float sizZ, bool const isSolid) {
  glPushMatrix();
  glTranslatef(begX, begY, begZ);
  glScalef(sizX, sizY, sizZ);
  glTranslatef(0.5f, 0.5f, 0.5f);
  if (isSolid) glutSolidCube(1.0);
  else glutWireCube(1.0);
  glPopMatrix();
}

void MarkovProcGene::Draw() {
  if (!isActiveProject) return;
  if (!isInitialized) return;
  if (!isRefreshed) return;

  int maxDim= std::max(std::max(nbX, nbY), nbZ);
  float voxSize= 1.0 / (float)maxDim;

  // Draw the voxels
  if (D.displayMode1) {
    glEnable(GL_LIGHTING);
    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          if (Field[x][y][z] == 0) continue;
          util_SetColorVoxel(Field[x][y][z]);
          util_DrawBoxPosSiz(0.5f - 0.5f * (float)nbX / (float)maxDim + (float)(x + 0) * voxSize,
                             0.5f - 0.5f * (float)nbY / (float)maxDim + (float)(y + 0) * voxSize,
                             0.5f - 0.5f * (float)nbZ / (float)maxDim + (float)(z + 0) * voxSize,
                             voxSize, voxSize, voxSize, true);
        }
      }
    }
    glDisable(GL_LIGHTING);
  }

  // Draw the dictionnary
  if (D.displayMode2) {
    glLineWidth(3.0);
    glEnable(GL_LIGHTING);
    int offsetZ= 0;
    for (int idxSequ= 0; idxSequ < (int)Dict.size(); idxSequ++) {
      for (int idxRule= 0; idxRule < (int)Dict[idxSequ].size(); idxRule++) {
        int nbXRule= (int)Dict[idxSequ][idxRule][0].size();
        int nbYRule= (int)Dict[idxSequ][idxRule][0][0].size();
        int nbZRule= (int)Dict[idxSequ][idxRule][0][0][0].size();
        float begXI= 0.0f;
        float begYI= 1.0f + voxSize;
        float begZI= offsetZ * voxSize;
        float begXO= 0.0f;
        float begYO= 1.0f + voxSize + (nbYRule + 1) * voxSize;
        float begZO= 0.0f + offsetZ * voxSize;
        glColor3f(0.3f, 0.3f, 0.3f);
        util_DrawBoxPosSiz(begXI, begYI, begZI, nbXRule * voxSize, nbYRule * voxSize, nbZRule * voxSize, false);
        util_DrawBoxPosSiz(begXO, begYO, begZO, nbXRule * voxSize, nbYRule * voxSize, nbZRule * voxSize, false);
        for (int xR= 0; xR < nbXRule; xR++) {
          for (int yR= 0; yR < nbYRule; yR++) {
            for (int zR= 0; zR < nbZRule; zR++) {
              if (Dict[idxSequ][idxRule][0][xR][yR][zR] != 0) {
                util_SetColorVoxel(Dict[idxSequ][idxRule][0][xR][yR][zR]);
                util_DrawBoxPosSiz(begXI + xR * voxSize, begYI + yR * voxSize, begZI + zR * voxSize, voxSize, voxSize, voxSize, true);
              }
              if (Dict[idxSequ][idxRule][1][xR][yR][zR] != 0) {
                util_SetColorVoxel(Dict[idxSequ][idxRule][1][xR][yR][zR]);
                util_DrawBoxPosSiz(begXO + xR * voxSize, begYO + yR * voxSize, begZO + zR * voxSize, voxSize, voxSize, voxSize, true);
              }
            }
          }
        }
        offsetZ+= nbZRule + 1;
      }
      offsetZ+= 1;
    }
    glDisable(GL_LIGHTING);
    glLineWidth(1.0);
  }
}
