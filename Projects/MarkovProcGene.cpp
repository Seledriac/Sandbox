
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
  SeedPosX____________,
  SeedPosY____________,
  SeedPosZ____________,
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
    D.param.push_back(ParamUI("SeedPosX____________", 0.5));
    D.param.push_back(ParamUI("SeedPosY____________", 0.5));
    D.param.push_back(ParamUI("SeedPosZ____________", 0.5));
    D.param.push_back(ParamUI("MaxSubstiCount______", 5));
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
  if (D.param[SeedPosX____________].hasChanged()) isInitialized= false;
  if (D.param[SeedPosY____________].hasChanged()) isInitialized= false;
  if (D.param[SeedPosZ____________].hasChanged()) isInitialized= false;
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
    Dict[0].push_back(std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(1, 1, 1, 0), Field::AllocField3D(1, 1, 1, 0)}));
    Dict[0][0][0][0][0][0]= 0;
    Dict[0][0][1][0][0][0]= 1;

    Dict.push_back(std::vector<std::array<std::vector<std::vector<std::vector<int>>>, 2>>());
    Dict[1].push_back(std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(1, 1, 1, 0), Field::AllocField3D(1, 1, 1, 0)}));
    Dict[1][0][0][0][0][0]= 1;
    Dict[1][0][1][0][0][0]= 2;

    Dict[1].push_back(std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(1, 1, 1, 0), Field::AllocField3D(1, 1, 1, 0)}));
    Dict[1][1][0][0][0][0]= 1;
    Dict[1][1][1][0][0][0]= 3;
  }
  if (scenario == 1) {
    Dict.push_back(std::vector<std::array<std::vector<std::vector<std::vector<int>>>, 2>>());
    Dict[0].push_back(std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(1, 1, 2, 0), Field::AllocField3D(1, 1, 2, 0)}));
    Dict[0][0][0][0][0][0]= 0;
    Dict[0][0][0][0][0][1]= 1;
    Dict[0][0][1][0][0][0]= 1;
    Dict[0][0][1][0][0][1]= 1;
    Dict[0].push_back(std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(1, 1, 2, 0), Field::AllocField3D(1, 1, 2, 0)}));
    Dict[0][1][0][0][0][0]= 1;
    Dict[0][1][0][0][0][1]= 0;
    Dict[0][1][1][0][0][0]= 1;
    Dict[0][1][1][0][0][1]= 1;
    Dict[0].push_back(std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(1, 2, 1, 0), Field::AllocField3D(1, 2, 1, 0)}));
    Dict[0][2][0][0][0][0]= 0;
    Dict[0][2][0][0][1][0]= 1;
    Dict[0][2][1][0][0][0]= 1;
    Dict[0][2][1][0][1][0]= 1;
    Dict[0].push_back(std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(1, 2, 1, 0), Field::AllocField3D(1, 2, 1, 0)}));
    Dict[0][3][0][0][0][0]= 1;
    Dict[0][3][0][0][1][0]= 0;
    Dict[0][3][1][0][0][0]= 1;
    Dict[0][3][1][0][1][0]= 1;

    Dict.push_back(std::vector<std::array<std::vector<std::vector<std::vector<int>>>, 2>>());
    Dict[1].push_back(std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(1, 1, 1, 0), Field::AllocField3D(1, 1, 1, 0)}));
    Dict[1][0][0][0][0][0]= 1;
    Dict[1][0][1][0][0][0]= 2;

    int idxSeedX= std::min(std::max((int)std::round(D.param[SeedPosX____________].Get() * nbX), 0), nbX - 1);
    int idxSeedY= std::min(std::max((int)std::round(D.param[SeedPosY____________].Get() * nbY), 0), nbY - 1);
    int idxSeedZ= std::min(std::max((int)std::round(D.param[SeedPosZ____________].Get() * nbZ), 0), nbZ - 1);
    Field[idxSeedX][idxSeedY][idxSeedZ]= 1;
  }
  if (scenario == 2) {
    Dict.push_back(std::vector<std::array<std::vector<std::vector<std::vector<int>>>, 2>>());
    Dict[(int)Dict.size() - 1].push_back(std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(5, 5, 1, 2), Field::AllocField3D(5, 5, 1, 2)}));
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][0][2][2][0]= 2;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][2][2][0]= 1;

    Dict.push_back(std::vector<std::array<std::vector<std::vector<std::vector<int>>>, 2>>());
    Dict[(int)Dict.size() - 1].push_back(std::array<std::vector<std::vector<std::vector<int>>>, 2>({Field::AllocField3D(1, 1, nbZ, 0), Field::AllocField3D(1, 1, nbZ, 0)}));
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][0][0][0][0]= 1;
    Dict[(int)Dict.size() - 1][(int)Dict[Dict.size() - 1].size() - 1][1][0][0][0]= 2;
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

    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          if (z == 0) Field[x][y][z]= 2;
        }
      }
    }
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
    std::vector<std::vector<std::vector<int>>> RuleI= Dict[activeSeq][idxRule][0];
    int nbXRule= (int)RuleI.size();
    int nbYRule= (int)RuleI[0].size();
    int nbZRule= (int)RuleI[0][0].size();
    for (int xF= 0; xF <= nbX - nbXRule; xF++) {
      for (int yF= 0; yF <= nbY - nbYRule; yF++) {
        for (int zF= 0; zF <= nbZ - nbZRule; zF++) {
          bool isMatch= true;
          for (int xR= 0; xR < nbXRule && isMatch; xR++)
            for (int yR= 0; yR < nbYRule && isMatch; yR++)
              for (int zR= 0; zR < nbZRule && isMatch; zR++)
                // if (RuleI[xR][yR][zR] >= 0)
                if (Field[xF + xR][yF + yR][zF + zR] != RuleI[xR][yR][zR])
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
    std::vector<std::vector<std::vector<int>>> RuleI= Dict[activeSeq][idxRule][0];
    std::vector<std::vector<std::vector<int>>> RuleO= Dict[activeSeq][idxRule][1];
    int nbXRule= (int)RuleI.size();
    int nbYRule= (int)RuleI[0].size();
    int nbZRule= (int)RuleI[0][0].size();
    for (int xF= 0; xF <= nbX - nbXRule; xF++) {
      for (int yF= 0; yF <= nbY - nbYRule; yF++) {
        for (int zF= 0; zF <= nbZ - nbZRule; zF++) {
          bool isMatch= true;
          for (int xR= 0; xR < nbXRule && isMatch; xR++)
            for (int yR= 0; yR < nbYRule && isMatch; yR++)
              for (int zR= 0; zR < nbZRule && isMatch; zR++)
                // if (RuleI[xR][yR][zR] >= 0)
                if (Field[xF + xR][yF + yR][zF + zR] != RuleI[xR][yR][zR])
                  isMatch= false;
          if (isMatch)
            matchCount--;
          if (matchCount == matchChosen)
            for (int xR= 0; xR < nbXRule && isMatch; xR++)
              for (int yR= 0; yR < nbYRule && isMatch; yR++)
                for (int zR= 0; zR < nbZRule && isMatch; zR++)
                  // if (RuleI[xR][yR][zR] >= 0)
                  Field[xF + xR][yF + yR][zF + zR]= RuleO[xR][yR][zR];
        }
      }
    }
  }
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
    glPushMatrix();
    glTranslatef(0.5f + 0.5f * voxSize - 0.5f * (float)nbX / (float)maxDim,
                 0.5f + 0.5f * voxSize - 0.5f * (float)nbY / (float)maxDim,
                 0.5f + 0.5f * voxSize - 0.5f * (float)nbZ / (float)maxDim);
    glScalef(voxSize, voxSize, voxSize);
    for (int x= 0; x < nbX; x++) {
      for (int y= 0; y < nbY; y++) {
        for (int z= 0; z < nbZ; z++) {
          if (Field[x][y][z] == 0) continue;
          // if (Field[x][y][z] == 0) glColor3f(0.1f, 0.1f, 0.1f);
          if (Field[x][y][z] == 1) glColor3f(0.5f, 0.2f, 0.2f);
          if (Field[x][y][z] == 2) glColor3f(0.2f, 0.5f, 0.2f);
          if (Field[x][y][z] == 3) glColor3f(0.2f, 0.2f, 0.5f);
          if (Field[x][y][z] == 4) glColor3f(0.2f, 0.4f, 0.4f);
          if (Field[x][y][z] == 5) glColor3f(0.4f, 0.2f, 0.4f);
          if (Field[x][y][z] == 6) glColor3f(0.4f, 0.4f, 0.2f);
          glPushMatrix();
          glTranslatef((float)x, (float)y, (float)z);
          glutSolidCube(1.0);
          glPopMatrix();
        }
      }
    }
    glPopMatrix();
    glDisable(GL_LIGHTING);
  }
}
