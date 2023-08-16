
#include "MarkovProcGene.hpp"


// Standard lib
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <vector>

// GLUT lib
#include <GL/freeglut.h>

// Project lib
#include "../Data.hpp"
#include "../Util/Field.hpp"
#include "../Util/Random.hpp"


extern Data D;

enum ParamType
{
  Scenario____,
  ResolutionX_,
  ResolutionY_,
  ResolutionZ_,
  NbSubsti____,
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
    D.param.push_back(ParamUI("Scenario____", 8));
    D.param.push_back(ParamUI("ResolutionX_", 11));
    D.param.push_back(ParamUI("ResolutionY_", 11));
    D.param.push_back(ParamUI("ResolutionZ_", 11));
    D.param.push_back(ParamUI("NbSubsti____", 1));
  }

  isActiveProject= true;
  isInitialized= false;
  isRefreshed= false;
  Initialize();
}


void MarkovProcGene::Initialize() {
  // Check if need to skip
  if (!isActiveProject) return;
  if (D.param[Scenario____].hasChanged()) isInitialized= false;
  if (D.param[ResolutionX_].hasChanged()) isInitialized= false;
  if (D.param[ResolutionY_].hasChanged()) isInitialized= false;
  if (D.param[ResolutionZ_].hasChanged()) isInitialized= false;
  if (isInitialized) return;
  isInitialized= true;

  // Get UI parameters
  nbX= std::max((int)std::round(D.param[ResolutionX_].Get()), 1);
  nbY= std::max((int)std::round(D.param[ResolutionY_].Get()), 1);
  nbZ= std::max((int)std::round(D.param[ResolutionZ_].Get()), 1);

  // Force refresh
  isRefreshed= false;
  Refresh();
}


void MarkovProcGene::Refresh() {
  if (isRefreshed) return;
  isRefreshed= true;

  // Reset progress counters
  activeSet= 0;
  activeRul= 0;
  D.plotData.clear();


  // Initialize dictionnary and field values
  Dict.clear();
  Field= Field::AllocField3D(nbX, nbY, nbZ, 0);
  std::array<std::vector<std::vector<std::vector<int>>>, 2> tmpRule;
  int scenario= 0;

  // Random noise
  if (scenario++ == (int)std::round(D.param[Scenario____].Get())) {
    Dict.push_back(std::vector<std::array<std::vector<std::vector<std::vector<int>>>, 2>>());
    Dict[(int)Dict.size() - 1].push_back(std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(1, 1, 1, 0), Field::AllocField3D(1, 1, 1, 0)}));
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][0][0][0][0]= 0;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][0][0]= 1;
  }

  // Random noise with multiple sets
  if (scenario++ == (int)std::round(D.param[Scenario____].Get())) {
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

    Dict.push_back(std::vector<std::array<std::vector<std::vector<std::vector<int>>>, 2>>());
    Dict[(int)Dict.size() - 1].push_back(std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(1, 1, 1, 0), Field::AllocField3D(1, 1, 1, 0)}));
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][0][0][0][0]= 2;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][0][0]= 1;
  }

  // Spread
  if (scenario++ == (int)std::round(D.param[Scenario____].Get())) {
    Dict.push_back(std::vector<std::array<std::vector<std::vector<std::vector<int>>>, 2>>());
    tmpRule= std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(1, 1, 2, 0), Field::AllocField3D(1, 1, 2, 0)});
    tmpRule[0][0][0][0]= 0;
    tmpRule[0][0][0][1]= 1;
    tmpRule[1][0][0][0]= 1;
    tmpRule[1][0][0][1]= 1;
    Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(+1, +2, +3, tmpRule));
    Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(+1, +2, -3, tmpRule));
    Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(+2, +3, +1, tmpRule));
    Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(+2, -3, +1, tmpRule));
    Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(+3, +1, +2, tmpRule));
    Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(-3, +1, +2, tmpRule));

    Field[nbX / 2][nbY / 2][nbZ / 2]= 1;
  }

  // Tron infinite wall
  if (scenario++ == (int)std::round(D.param[Scenario____].Get())) {
    Dict.push_back(std::vector<std::array<std::vector<std::vector<std::vector<int>>>, 2>>());
    tmpRule= std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(1, 1, 3, 0), Field::AllocField3D(1, 1, 3, 0)});
    tmpRule[0][0][0][0]= 1;
    tmpRule[1][0][0][0]= 2;
    tmpRule[1][0][0][1]= 2;
    tmpRule[1][0][0][2]= 1;
    Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(+1, +2, +3, tmpRule));
    Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(+1, +2, -3, tmpRule));
    Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(+2, +3, +1, tmpRule));
    Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(+2, -3, +1, tmpRule));
    Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(+3, +1, +2, tmpRule));
    Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(-3, +1, +2, tmpRule));

    Field[nbX / 2][nbY / 2][nbZ / 2]= 1;
  }

  // Snake
  if (scenario++ == (int)std::round(D.param[Scenario____].Get())) {
    Dict.push_back(std::vector<std::array<std::vector<std::vector<std::vector<int>>>, 2>>());
    Dict[(int)Dict.size() - 1].push_back(std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(1, 1, 5, 0), Field::AllocField3D(1, 1, 5, 0)}));
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][0][0][0][2]= 3;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][0][0]= 4;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][0][1]= 2;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][0][2]= 2;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][0][3]= 2;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][0][4]= 1;

    Dict.push_back(std::vector<std::array<std::vector<std::vector<std::vector<int>>>, 2>>());
    tmpRule= std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(1, 1, 3, 0), Field::AllocField3D(1, 1, 3, 0)});
    tmpRule[0][0][0][0]= 1;
    tmpRule[1][0][0][0]= 2;
    tmpRule[1][0][0][1]= 2;
    tmpRule[1][0][0][2]= 1;
    Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(+1, +2, +3, tmpRule));
    Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(+1, +2, -3, tmpRule));
    Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(+2, +3, +1, tmpRule));
    Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(+2, -3, +1, tmpRule));
    Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(+3, +1, +2, tmpRule));
    Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(-3, +1, +2, tmpRule));

    Dict.push_back(std::vector<std::array<std::vector<std::vector<std::vector<int>>>, 2>>());
    tmpRule= std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(1, 1, 3, 0), Field::AllocField3D(1, 1, 3, 0)});
    tmpRule[0][0][0][0]= 4;
    tmpRule[0][0][0][1]= 2;
    tmpRule[0][0][0][2]= 2;
    tmpRule[1][0][0][2]= 4;
    Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(+1, +2, +3, tmpRule));
    Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(+1, +2, -3, tmpRule));
    Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(+2, +3, +1, tmpRule));
    Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(+2, -3, +1, tmpRule));
    Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(+3, +1, +2, tmpRule));
    Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(-3, +1, +2, tmpRule));

    Field[nbX / 2][nbY / 2][nbZ / 2]= 3;
  }

  // Spanning tree
  if (scenario++ == (int)std::round(D.param[Scenario____].Get())) {
    Dict.push_back(std::vector<std::array<std::vector<std::vector<std::vector<int>>>, 2>>());
    tmpRule= std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(1, 1, 3, 0), Field::AllocField3D(1, 1, 3, 0)});
    tmpRule[0][0][0][0]= 3;
    tmpRule[1][0][0][0]= 3;
    tmpRule[1][0][0][1]= 8;
    tmpRule[1][0][0][2]= 3;
    Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(+1, +2, +3, tmpRule));
    Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(+1, +2, -3, tmpRule));
    Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(+2, +3, +1, tmpRule));
    Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(+2, -3, +1, tmpRule));
    Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(+3, +1, +2, tmpRule));
    Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(-3, +1, +2, tmpRule));

    Dict.push_back(std::vector<std::array<std::vector<std::vector<std::vector<int>>>, 2>>());
    Dict[(int)Dict.size() - 1].push_back(std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(1, 1, 1, 0), Field::AllocField3D(1, 1, 1, 0)}));
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][0][0][0][0]= 3;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][0][0]= 8;

    Field[nbX / 2][nbY / 2][nbZ / 2]= 3;
  }

  // Flower garden
  if (scenario++ == (int)std::round(D.param[Scenario____].Get())) {
    Dict.push_back(std::vector<std::array<std::vector<std::vector<std::vector<int>>>, 2>>());
    Dict[(int)Dict.size() - 1].push_back(std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(5, 5, 1, 0), Field::AllocField3D(5, 5, 1, 0)}));
    FillRuleBox(Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1], 0, 0, 0, 4, 4, 0, 2, true, true);
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][0][2][2][0]= 2;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][2][2][0]= 1;

    Dict.push_back(std::vector<std::array<std::vector<std::vector<std::vector<int>>>, 2>>());
    Dict[(int)Dict.size() - 1].push_back(std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(1, 1, 2, 0), Field::AllocField3D(1, 1, 2, 0)}));
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][0][0][0][0]= 1;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][0][0]= 1;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][0][1]= 6;

    Dict.push_back(std::vector<std::array<std::vector<std::vector<std::vector<int>>>, 2>>());
    Dict[(int)Dict.size() - 1].push_back(std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(5, 5, 2, 0), Field::AllocField3D(5, 5, 2, 0)}));
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][0][2][2][0]= 6;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][2][2][0]= 2;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][2][2][1]= 6;

    tmpRule= std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(5, 5, 2, 0), Field::AllocField3D(5, 5, 2, 0)});
    tmpRule[0][2][2][0]= 6;
    tmpRule[1][2][2][0]= 2;
    tmpRule[1][2][1][1]= 6;
    Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(+1, +2, +3, tmpRule));
    Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(+1, -2, +3, tmpRule));
    Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(+2, +1, +3, tmpRule));
    Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(-2, +1, +3, tmpRule));

    Dict.push_back(std::vector<std::array<std::vector<std::vector<std::vector<int>>>, 2>>());
    tmpRule= std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(3, 3, 1, 0), Field::AllocField3D(3, 3, 1, 0)});
    tmpRule[0][1][1][0]= 6;
    tmpRule[1][1][1][0]= 6;
    tmpRule[1][1][0][0]= 7;
    tmpRule[1][0][1][0]= 7;
    tmpRule[1][1][2][0]= 7;
    tmpRule[1][2][1][0]= 7;
    Dict[(int)Dict.size() - 1].push_back(BuildColorSwap(7, 3, tmpRule));
    Dict[(int)Dict.size() - 1].push_back(BuildColorSwap(7, 4, tmpRule));
    Dict[(int)Dict.size() - 1].push_back(BuildColorSwap(7, 5, tmpRule));

    for (int x= 0; x < nbX; x++)
      for (int y= 0; y < nbY; y++)
        Field[x][y][0]= 2;
  }

  // Galton board
  if (scenario++ == (int)std::round(D.param[Scenario____].Get())) {
    Dict.push_back(std::vector<std::array<std::vector<std::vector<std::vector<int>>>, 2>>());
    Dict[(int)Dict.size() - 1].push_back(std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(1, 1, 2, 0), Field::AllocField3D(1, 1, 2, 0)}));
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][0][0][0][1]= 3;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][0][1]= 3;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][0][0]= 6;
    tmpRule= std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(1, 2, 2, 0), Field::AllocField3D(1, 2, 2, 0)});
    tmpRule[0][0][0][1]= 4;
    tmpRule[0][0][1][0]= 4;
    tmpRule[0][0][1][1]= 6;
    tmpRule[1][0][0][0]= 6;
    tmpRule[1][0][0][1]= 4;
    tmpRule[1][0][1][0]= 4;
    Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(+1, +2, +3, tmpRule));
    Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(+1, -2, +3, tmpRule));
    Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(+2, +1, +3, tmpRule));
    Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(-2, +1, +3, tmpRule));

    Dict[(int)Dict.size() - 1].push_back(std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(1, 1, 2, 0), Field::AllocField3D(1, 1, 2, 0)}));
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][0][0][0][1]= 6;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][0][0]= 6;
    Dict[(int)Dict.size() - 1].push_back(std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(1, 1, 3, 0), Field::AllocField3D(1, 1, 3, 0)}));
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][0][0][0][2]= 6;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][0][0]= 6;
    Dict[(int)Dict.size() - 1].push_back(std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(1, 1, 4, 0), Field::AllocField3D(1, 1, 4, 0)}));
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][0][0][0][3]= 6;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][0][0]= 6;

    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          if ((x + y + z) % 2 == 0 && z > 3 * nbZ / 5 && z < nbZ - 2)
            Field[x][y][z]= 4;
          if (std::abs(x - (nbX - 1) / 2) <= 1 && std::abs(y - (nbY - 1) / 2) <= 1 && z == nbZ - 1)
            Field[x][y][z]= 3;
        }
      }
    }
  }

  // Skyscraper
  if (scenario++ == (int)std::round(D.param[Scenario____].Get())) {
    // Fill the floor
    Dict.push_back(std::vector<std::array<std::vector<std::vector<std::vector<int>>>, 2>>());
    tmpRule= std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(6, 6, 1, 0), Field::AllocField3D(6, 6, 1, 0)});
    FillRuleBox(tmpRule, 0, 0, 0, 5, 5, 0, 1, true, false);
    tmpRule[0][0][0][0]= 2;
    tmpRule[0][0][5][0]= 2;
    tmpRule[0][5][0][0]= 2;
    tmpRule[0][5][5][0]= 2;
    FillRuleBox(tmpRule, 0, 0, 0, 5, 5, 0, 3, false, true);
    tmpRule[1][0][0][0]= 2;
    tmpRule[1][0][5][0]= 2;
    tmpRule[1][5][0][0]= 2;
    tmpRule[1][5][5][0]= 2;
    Dict[(int)Dict.size() - 1].push_back(BuildColorSwap(3, 3, tmpRule));
    Dict[(int)Dict.size() - 1].push_back(BuildColorSwap(3, 4, tmpRule));

    // Spread the floor grid
    Dict.push_back(std::vector<std::array<std::vector<std::vector<std::vector<int>>>, 2>>());
    tmpRule= std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(1, 6, 1, 0), Field::AllocField3D(1, 6, 1, 0)});
    FillRuleBox(tmpRule, 0, 0, 0, 0, 5, 0, 1, true, true);
    tmpRule[0][0][0][0]= 2;
    tmpRule[1][0][0][0]= 2;
    tmpRule[1][0][5][0]= 2;
    Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(+1, +2, +3, tmpRule));
    Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(+1, -2, +3, tmpRule));
    Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(+2, +1, +3, tmpRule));
    Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(-2, +1, +3, tmpRule));

    // Seed the floor grid
    Dict.push_back(std::vector<std::array<std::vector<std::vector<std::vector<int>>>, 2>>());
    Dict[(int)Dict.size() - 1].push_back(std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(1, 1, 1, 0), Field::AllocField3D(1, 1, 1, 0)}));
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][0][0][0][0]= 1;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][0][0]= 2;


    // Dict[(int)Dict.size() - 1].push_back(tmpRule);
    // Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(+2, +1, +3, BuildColorSwap(3, 4, tmpRule)));
    // tmpRule= std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(1, 3, 1, 0), Field::AllocField3D(1, 3, 1, 0)});
    // tmpRule[0][0][0][0]= 3;
    // tmpRule[1][0][0][0]= 3;
    // tmpRule[1][0][1][0]= 3;
    // Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(+1, +2, +3, tmpRule));
    // Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(+1, -2, +3, tmpRule));
    // Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(+2, +1, +3, BuildColorSwap(3, 4, tmpRule)));
    // Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(-2, +1, +3, BuildColorSwap(3, 4, tmpRule)));

    // Dict.push_back(std::vector<std::array<std::vector<std::vector<std::vector<int>>>, 2>>());
    // tmpRule= std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(3, 2, 1, 0), Field::AllocField3D(3, 2, 1, 0)});
    // FillRuleBox(tmpRule, 0, 0, 0, 2, 1, 0, 7, true, true);
    // tmpRule[0][1][0][0]= 8;
    // tmpRule[1][1][0][0]= 7;
    // Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(+1, +2, +3, tmpRule));
    // Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(+1, -2, +3, tmpRule));
    // Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(+2, +1, +3, tmpRule));
    // Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(-2, +1, +3, tmpRule));

    // Dict.push_back(std::vector<std::array<std::vector<std::vector<std::vector<int>>>, 2>>());
    // tmpRule= std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(2, 2, 1, 0), Field::AllocField3D(2, 2, 1, 0)});
    // FillRuleBox(tmpRule, 0, 0, 0, 1, 1, 0, 8, true, true);
    // tmpRule[0][0][0][0]= 7;
    // tmpRule[0][0][0][0]= 6;
    // Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(+1, +2, +3, tmpRule));
    // Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(+1, -2, +3, tmpRule));
    // Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(+2, +1, +3, tmpRule));
    // Dict[(int)Dict.size() - 1].push_back(BuildSymmetric(-2, +1, +3, tmpRule));

    for (int x= 0; x < nbX; x++)
      for (int y= 0; y < nbY; y++)
        Field[x][y][0]= 1;
  }
}


void MarkovProcGene::Animate() {
  if (!isActiveProject) return;
  if (!isInitialized) return;
  if (!isRefreshed) return;

  // Iterate over the desired number of substitutions
  if (Dict.empty()) return;
  for (int idxIter= 0; idxIter < (int)std::round(D.param[NbSubsti____].Get()); idxIter++) {
    activeRul= -1;
    activeSet= -1;
    int activeMatchCount= 0;

    for (int idxSet= 0; idxSet < (int)Dict.size(); idxSet++) {
      int matchCount= 0;
      for (int idxRule= 0; idxRule < (int)Dict[idxSet].size(); idxRule++) {
        int nbXRule= (int)Dict[idxSet][idxRule][0].size();
        int nbYRule= (int)Dict[idxSet][idxRule][0][0].size();
        int nbZRule= (int)Dict[idxSet][idxRule][0][0][0].size();
        for (int xF= 0; xF <= nbX - nbXRule; xF++) {
          for (int yF= 0; yF <= nbY - nbYRule; yF++) {
            for (int zF= 0; zF <= nbZ - nbZRule; zF++) {
              bool isMatch= true;
              for (int xR= 0; xR < nbXRule && isMatch; xR++)
                for (int yR= 0; yR < nbYRule && isMatch; yR++)
                  for (int zR= 0; zR < nbZRule && isMatch; zR++)
                    if (Dict[idxSet][idxRule][0][xR][yR][zR] >= 0)
                      if (Field[xF + xR][yF + yR][zF + zR] != Dict[idxSet][idxRule][0][xR][yR][zR])
                        isMatch= false;
              if (isMatch)
                matchCount++;
            }
          }
        }
      }
      if (activeSet < 0 && matchCount > 0) {
        activeSet= idxSet;
        activeMatchCount= matchCount;
      }
      D.plotData.resize(Dict.size());
      D.plotData[idxSet].first= "Set" + std::to_string(idxSet);
      D.plotData[idxSet].second.push_back((double)matchCount);
    }

    // // Skip to next iteration with next ruleset if no match is found
    // activeRul= -1;
    // if (matchCount == 0) {
    //   activeSet++;
    //   continue;
    // }
    if (activeSet < 0) continue;

    // Choose a random match to substitute according to its rule
    int matchChosen= Random::Val(0, activeMatchCount);
    bool substitutionDone= false;
    for (int idxRule= 0; idxRule < (int)Dict[activeSet].size() && !substitutionDone; idxRule++) {
      int nbXRule= (int)Dict[activeSet][idxRule][0].size();
      int nbYRule= (int)Dict[activeSet][idxRule][0][0].size();
      int nbZRule= (int)Dict[activeSet][idxRule][0][0][0].size();
      for (int xF= 0; xF <= nbX - nbXRule && !substitutionDone; xF++) {
        for (int yF= 0; yF <= nbY - nbYRule && !substitutionDone; yF++) {
          for (int zF= 0; zF <= nbZ - nbZRule && !substitutionDone; zF++) {
            bool isMatch= true;
            for (int xR= 0; xR < nbXRule && isMatch; xR++)
              for (int yR= 0; yR < nbYRule && isMatch; yR++)
                for (int zR= 0; zR < nbZRule && isMatch; zR++)
                  if (Dict[activeSet][idxRule][0][xR][yR][zR] >= 0)
                    if (Field[xF + xR][yF + yR][zF + zR] != Dict[activeSet][idxRule][0][xR][yR][zR])
                      isMatch= false;
            if (isMatch)
              activeMatchCount--;
            if (activeMatchCount == matchChosen) {
              for (int xR= 0; xR < nbXRule && isMatch; xR++)
                for (int yR= 0; yR < nbYRule && isMatch; yR++)
                  for (int zR= 0; zR < nbZRule && isMatch; zR++)
                    if (Dict[activeSet][idxRule][1][xR][yR][zR] >= 0)
                      Field[xF + xR][yF + yR][zF + zR]= Dict[activeSet][idxRule][1][xR][yR][zR];
              substitutionDone= true;
              activeRul= idxRule;
            }
          }
        }
      }
    }
  }
}


void util_SetColorVoxel(const int iVal, const float iShading) {
  if (iVal < 0) glColor3f(0.0f, 0.0f, 0.0f);
  if (iVal == 1) glColor3f(iShading * 0.7f, iShading * 0.3f, iShading * 0.3f);
  if (iVal == 2) glColor3f(iShading * 0.3f, iShading * 0.7f, iShading * 0.3f);
  if (iVal == 3) glColor3f(iShading * 0.3f, iShading * 0.3f, iShading * 0.7f);
  if (iVal == 4) glColor3f(iShading * 0.2f, iShading * 0.5f, iShading * 0.5f);
  if (iVal == 5) glColor3f(iShading * 0.5f, iShading * 0.2f, iShading * 0.5f);
  if (iVal == 6) glColor3f(iShading * 0.5f, iShading * 0.5f, iShading * 0.2f);
  if (iVal == 7) glColor3f(iShading * 0.2f, iShading * 0.2f, iShading * 0.2f);
  if (iVal == 8) glColor3f(iShading * 0.5f, iShading * 0.5f, iShading * 0.5f);
  if (iVal == 9) glColor3f(iShading * 0.8f, iShading * 0.8f, iShading * 0.8f);
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

  // Get sizes
  int maxDim= std::max(std::max(nbX, nbY), nbZ);
  float voxSize= 1.0 / (float)maxDim;

  // Compute the shading directions
  std::vector<std::array<int, 3>> ShaDir;
  for (int x= -1; x <= 1; x++)
    for (int y= -1; y <= 1; y++)
      for (int z= -1; z <= 1; z++)
        if (x != 0 || y != 0 || z != 0)
          ShaDir.push_back(std::array<int, 3>({x, y, z}));

  // Compute the voxel shading map
  std::vector<std::vector<std::vector<float>>> FieldVisi= Field::AllocField3D(nbX, nbY, nbZ, 0.0f);
  for (int x= 0; x < nbX; x++) {
    for (int y= 0; y < nbY; y++) {
      for (int z= 0; z < nbZ; z++) {
        int countVisi= 0;
        for (int idxDir= 0; idxDir < (int)ShaDir.size(); idxDir++) {
          int xOff= x;
          int yOff= y;
          int zOff= z;
          bool isOccluded= false;
          while (!isOccluded) {
            xOff+= ShaDir[idxDir][0];
            yOff+= ShaDir[idxDir][1];
            zOff+= ShaDir[idxDir][2];
            if (xOff < 0 || xOff >= nbX) break;
            if (yOff < 0 || yOff >= nbY) break;
            if (zOff < 0 || zOff >= nbZ) break;
            if (Field[xOff][yOff][zOff] > 0) isOccluded= true;
          }
          if (!isOccluded)
            countVisi++;
        }
        FieldVisi[x][y][z]= 0.5f + 0.5f * (float)countVisi / (float)ShaDir.size();
      }
    }
  }

  // Smooth the voxel shading map
  std::vector<std::vector<std::vector<float>>> FieldVisiOld= FieldVisi;
  for (int x= 0; x < nbX; x++) {
    for (int y= 0; y < nbY; y++) {
      for (int z= 0; z < nbZ; z++) {
        FieldVisi[x][y][z]= 0.0f;
        int count= 0;
        for (int xOff= std::max(x - 1, 0); xOff <= std::min(x + 1, nbX - 1); xOff++) {
          for (int yOff= std::max(y - 1, 0); yOff <= std::min(y + 1, nbY - 1); yOff++) {
            for (int zOff= std::max(z - 1, 0); zOff <= std::min(z + 1, nbZ - 1); zOff++) {
              if (Field[xOff][yOff][zOff] > 0) {
                FieldVisi[x][y][z]+= FieldVisiOld[xOff][yOff][zOff];
                count++;
              }
            }
          }
        }
        FieldVisi[x][y][z]/= (float)count;
      }
    }
  }

  // Draw the voxels
  if (D.displayMode1) {
    glEnable(GL_LIGHTING);
    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          if (Field[x][y][z] == 0) continue;
          util_SetColorVoxel(Field[x][y][z], FieldVisi[x][y][z]);
          util_DrawBoxPosSiz(0.5f - 0.5f * (float)nbX / (float)maxDim + (float)x * voxSize,
                             0.5f - 0.5f * (float)nbY / (float)maxDim + (float)y * voxSize,
                             0.5f - 0.5f * (float)nbZ / (float)maxDim + (float)z * voxSize,
                             voxSize, voxSize, voxSize, true);
        }
      }
    }
    glDisable(GL_LIGHTING);
  }

  // Draw the dictionnary
  if (D.displayMode2) {
    glLineWidth(3.0);
    int offsetZ= 0;
    int currentRul= 0;
    for (int idxSet= 0; idxSet < (int)Dict.size(); idxSet++) {
      for (int idxRule= 0; idxRule < (int)Dict[idxSet].size(); idxRule++) {
        int nbXRule= (int)Dict[idxSet][idxRule][0].size();
        int nbYRule= (int)Dict[idxSet][idxRule][0][0].size();
        int nbZRule= (int)Dict[idxSet][idxRule][0][0][0].size();
        float begXI= 0.5f;
        float begYI= 1.0f + voxSize;
        float begZI= offsetZ * voxSize;
        float begXO= 0.5f;
        float begYO= 1.0f + voxSize + (nbYRule + 1) * voxSize;
        float begZO= 0.0f + offsetZ * voxSize;
        if (idxSet == activeSet && idxRule == activeRul)
          glColor3f(0.8f, 0.8f, 0.8f);
        else
          glColor3f(0.3f, 0.3f, 0.3f);
        util_DrawBoxPosSiz(begXI, begYI, begZI, nbXRule * voxSize, nbYRule * voxSize, nbZRule * voxSize, false);
        util_DrawBoxPosSiz(begXO, begYO, begZO, nbXRule * voxSize, nbYRule * voxSize, nbZRule * voxSize, false);
        glEnable(GL_LIGHTING);
        for (int xR= 0; xR < nbXRule; xR++) {
          for (int yR= 0; yR < nbYRule; yR++) {
            for (int zR= 0; zR < nbZRule; zR++) {
              if (Dict[idxSet][idxRule][0][xR][yR][zR] != 0) {
                util_SetColorVoxel(Dict[idxSet][idxRule][0][xR][yR][zR], 0.8f);
                util_DrawBoxPosSiz(begXI + xR * voxSize, begYI + yR * voxSize, begZI + zR * voxSize, voxSize, voxSize, voxSize, true);
              }
              if (Dict[idxSet][idxRule][1][xR][yR][zR] != 0) {
                util_SetColorVoxel(Dict[idxSet][idxRule][1][xR][yR][zR], 0.8f);
                util_DrawBoxPosSiz(begXO + xR * voxSize, begYO + yR * voxSize, begZO + zR * voxSize, voxSize, voxSize, voxSize, true);
              }
            }
          }
        }
        glDisable(GL_LIGHTING);
        offsetZ+= nbZRule + 1;
        currentRul++;
      }
      offsetZ+= 1;
    }
    glLineWidth(1.0);
  }

  // Draw the box
  if (D.displayMode3) {
    glLineWidth(3.0);
    glColor3f(0.5f, 0.5f, 0.5f);
    util_DrawBoxPosSiz(0.5f - 0.5f * (float)nbX / (float)maxDim,
                       0.5f - 0.5f * (float)nbY / (float)maxDim,
                       0.5f - 0.5f * (float)nbZ / (float)maxDim,
                       voxSize * nbX, voxSize * nbY, voxSize * nbZ, false);
    glLineWidth(1.0);
  }
}


void MarkovProcGene::FillRuleBox(
    std::array<std::vector<std::vector<std::vector<int>>>, 2>& ioRule,
    const int iMinX, const int iMinY, const int iMinZ,
    const int iMaxX, const int iMaxY, const int iMaxZ,
    const int iVal, const bool iFillI, const bool iFillO) {
  int nbXRule= (int)ioRule[0].size();
  int nbYRule= (int)ioRule[0][0].size();
  int nbZRule= (int)ioRule[0][0][0].size();
  for (int x= std::max(iMinX, 0); x <= std::min(iMaxX, nbXRule - 1); x++) {
    for (int y= std::max(iMinY, 0); y <= std::min(iMaxY, nbYRule - 1); y++) {
      for (int z= std::max(iMinZ, 0); z <= std::min(iMaxZ, nbZRule - 1); z++) {
        if (iFillI) ioRule[0][x][y][z]= iVal;
        if (iFillO) ioRule[1][x][y][z]= iVal;
      }
    }
  }
}


std::array<std::vector<std::vector<std::vector<int>>>, 2> MarkovProcGene::BuildSymmetric(
    const int iDim1, const int iDim2, const int iDim3,
    const std::array<std::vector<std::vector<std::vector<int>>>, 2>& iRule) {
  if (iRule[0].empty()) throw;
  if (iRule[0][0].empty()) throw;
  if (iRule[0][0][0].empty()) throw;
  int nbXS= (int)iRule[0].size();
  int nbYS= (int)iRule[0][0].size();
  int nbZS= (int)iRule[0][0][0].size();
  if (iDim1 == 0 || std::abs(iDim1) > 3) throw;
  if (iDim2 == 0 || std::abs(iDim2) > 3) throw;
  if (iDim3 == 0 || std::abs(iDim3) > 3) throw;
  if (std::abs(iDim1) == std::abs(iDim2)) throw;
  if (std::abs(iDim1) == std::abs(iDim3)) throw;
  if (std::abs(iDim2) == std::abs(iDim3)) throw;

  int nbXD= (std::abs(iDim1) == 1) ? (nbXS) : ((std::abs(iDim1) == 2) ? (nbYS) : (nbZS));
  int nbYD= (std::abs(iDim2) == 1) ? (nbXS) : ((std::abs(iDim2) == 2) ? (nbYS) : (nbZS));
  int nbZD= (std::abs(iDim3) == 1) ? (nbXS) : ((std::abs(iDim3) == 2) ? (nbYS) : (nbZS));
  std::array<std::vector<std::vector<std::vector<int>>>, 2> oRule({Field::AllocField3D(nbXD, nbYD, nbZD, 0), Field::AllocField3D(nbXD, nbYD, nbZD, 0)});
  for (int xS= 0; xS < nbXS; xS++) {
    for (int yS= 0; yS < nbYS; yS++) {
      for (int zS= 0; zS < nbZS; zS++) {
        int xD= 0, yD= 0, zD= 0;
        if (iDim1 < 0) xD= (std::abs(iDim1) == 1) ? (nbXS - 1 - xS) : ((std::abs(iDim1) == 2) ? (nbYS - 1 - yS) : (nbZS - 1 - zS));
        if (iDim1 > 0) xD= (std::abs(iDim1) == 1) ? (xS) : ((std::abs(iDim1) == 2) ? (yS) : (zS));
        if (iDim2 < 0) yD= (std::abs(iDim2) == 1) ? (nbXS - 1 - xS) : ((std::abs(iDim2) == 2) ? (nbYS - 1 - yS) : (nbZS - 1 - zS));
        if (iDim2 > 0) yD= (std::abs(iDim2) == 1) ? (xS) : ((std::abs(iDim2) == 2) ? (yS) : (zS));
        if (iDim3 < 0) zD= (std::abs(iDim3) == 1) ? (nbXS - 1 - xS) : ((std::abs(iDim3) == 2) ? (nbYS - 1 - yS) : (nbZS - 1 - zS));
        if (iDim3 > 0) zD= (std::abs(iDim3) == 1) ? (xS) : ((std::abs(iDim3) == 2) ? (yS) : (zS));
        oRule[0][xD][yD][zD]= iRule[0][xS][yS][zS];
        oRule[1][xD][yD][zD]= iRule[1][xS][yS][zS];
      }
    }
  }

  return oRule;
}


std::array<std::vector<std::vector<std::vector<int>>>, 2> MarkovProcGene::BuildColorSwap(
    const int iOldColor, const int iNewColor,
    const std::array<std::vector<std::vector<std::vector<int>>>, 2>& iRule) {
  if (iRule[0].empty()) throw;
  if (iRule[0][0].empty()) throw;
  if (iRule[0][0][0].empty()) throw;
  int nbXS= (int)iRule[0].size();
  int nbYS= (int)iRule[0][0].size();
  int nbZS= (int)iRule[0][0][0].size();

  std::array<std::vector<std::vector<std::vector<int>>>, 2> oRule({Field::AllocField3D(nbXS, nbYS, nbZS, 0), Field::AllocField3D(nbXS, nbYS, nbZS, 0)});
  for (int xS= 0; xS < nbXS; xS++) {
    for (int yS= 0; yS < nbYS; yS++) {
      for (int zS= 0; zS < nbZS; zS++) {
        oRule[0][xS][yS][zS]= iRule[0][xS][yS][zS];
        oRule[1][xS][yS][zS]= iRule[1][xS][yS][zS];
        if (oRule[0][xS][yS][zS] == iOldColor) oRule[0][xS][yS][zS]= iNewColor;
        if (oRule[1][xS][yS][zS] == iOldColor) oRule[1][xS][yS][zS]= iNewColor;
      }
    }
  }

  return oRule;
}
