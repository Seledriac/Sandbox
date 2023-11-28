#include "StringArtOptim.hpp"


// Standard lib
#include <cmath>
#include <cstdio>
#include <cstring>
#include <vector>

// GLUT lib
#include "../freeglut/include/GL/freeglut.h"

// Project lib
#include "../Data.hpp"
#include "../FileIO/FileInput.hpp"
#include "../Util/Colormap.hpp"
#include "../Util/Field.hpp"
#include "../Util/Random.hpp"
#include "../Util/Vector.hpp"


// Link to shared sandbox data
extern Data D;

// List of UI parameters for this project
enum ParamType
{
  ImageID_____,
  ImageSizeW__,
  ImageSizeH__,
  PegLayout___,
  PegNumber___,
  StepCount___,
  CoeffColor__,
  CoeffRGB____,
  CoeffCMY____,
  CoeffBias___,
  CoeffCommit_,
  Verbose_____,
};


// Constructor
StringArtOptim::StringArtOptim() {
  isActivProj= false;
  isAllocated= false;
  isRefreshed= false;
}


// Initialize Project UI parameters
void StringArtOptim::SetActiveProject() {
  if (!isActivProj) {
    D.UI.clear();
    D.UI.push_back(ParamUI("ImageID_____", 2));
    D.UI.push_back(ParamUI("ImageSizeW__", 512));
    D.UI.push_back(ParamUI("ImageSizeH__", 512));
    D.UI.push_back(ParamUI("PegLayout___", 1));
    D.UI.push_back(ParamUI("PegNumber___", 512));
    D.UI.push_back(ParamUI("StepCount___", 1));
    D.UI.push_back(ParamUI("CoeffColor__", 0.1));
    D.UI.push_back(ParamUI("CoeffRGB____", 1.0));
    D.UI.push_back(ParamUI("CoeffCMY____", 0.5));
    D.UI.push_back(ParamUI("CoeffBias___", 0.5));
    D.UI.push_back(ParamUI("CoeffCommit_", 0.0));
    D.UI.push_back(ParamUI("Verbose_____", -0.5));
  }

  if (D.UI.size() != Verbose_____ + 1) {
    printf("[ERROR] Invalid parameter count in UI\n");
  }

  D.boxMin= {0.0, 0.0, 0.0};
  D.boxMax= {1.0, 1.0, 1.0};

  isActivProj= true;
  isAllocated= false;
  isRefreshed= false;
}


// Check if parameter changes should trigger an allocation
void StringArtOptim::CheckAlloc() {
  if (D.UI[ImageID_____].hasChanged()) isAllocated= false;
  if (D.UI[ImageSizeW__].hasChanged()) isAllocated= false;
  if (D.UI[ImageSizeH__].hasChanged()) isAllocated= false;
  if (D.UI[PegLayout___].hasChanged()) isAllocated= false;
  if (D.UI[PegNumber___].hasChanged()) isAllocated= false;
}


// Check if parameter changes should trigger a refresh
void StringArtOptim::CheckRefresh() {
}


// Allocate the project data
void StringArtOptim::Allocate() {
  if (!isActivProj) return;
  CheckAlloc();
  if (isAllocated) return;
  isRefreshed= false;
  isAllocated= true;

  // Get UI parameters
  nW= std::max(D.UI[ImageSizeW__].GetI(), 1);
  nH= std::max(D.UI[ImageSizeH__].GetI(), 1);
}


// Refresh the project
void StringArtOptim::Refresh() {
  if (!isActivProj) return;
  CheckAlloc();
  if (!isAllocated) Allocate();
  CheckRefresh();
  if (isRefreshed) return;
  isRefreshed= true;

  // Reset plot
  D.plotData.clear();
  D.plotLegend.clear();

  // Initialize images
  ImRef= Field::AllocField2D(nW, nH, Math::Vec3f(0.0f, 0.0f, 0.0f));
  ImCur= Field::AllocField2D(nW, nH, Math::Vec3f(0.0f, 0.0f, 0.0f));
  std::vector<std::vector<std::array<float, 4>>> imageRGBA;
  if (D.UI[ImageID_____].GetI() == 0) FileInput::LoadImageBMPFile("Resources/SA_Smile.bmp", imageRGBA, false);
  if (D.UI[ImageID_____].GetI() == 1) FileInput::LoadImageBMPFile("Resources/SA_MonaLisa.bmp", imageRGBA, false);
  if (D.UI[ImageID_____].GetI() == 2) FileInput::LoadImageBMPFile("Resources/SA_Pillars.bmp", imageRGBA, false);
  if (D.UI[ImageID_____].GetI() == 3) FileInput::LoadImageBMPFile("Resources/GR_AlbertArt.bmp", imageRGBA, false);
  if (D.UI[ImageID_____].GetI() == 4) FileInput::LoadImageBMPFile("Resources/GR_DeepField.bmp", imageRGBA, false);
  for (int w= 0; w < nW; w++) {
    for (int h= 0; h < nH; h++) {
      if (!imageRGBA.empty()) {
        const float posW= (float)(imageRGBA.size() - 1) * ((float)w + 0.5f) / (float)nW;
        const float posH= (float)(imageRGBA[0].size() - 1) * ((float)h + 0.5f) / (float)nH;
        const int idxPixelW= std::min(std::max((int)std::round(posW), 0), (int)imageRGBA.size() - 1);
        const int idxPixelH= std::min(std::max((int)std::round(posH), 0), (int)imageRGBA[0].size() - 1);
        const std::array<float, 4> colRGBA= imageRGBA[idxPixelW][idxPixelH];
        ImRef[w][h].set(colRGBA[0], colRGBA[1], colRGBA[2]);
      }
    }
  }

  // Initialize pegs
  Pegs.clear();
  for (int idxPeg= 0; idxPeg < D.UI[PegNumber___].GetI(); idxPeg++) {
    if (D.UI[PegLayout___].GetI() == 0) {
      Pegs.push_back(std::array<int, 2>{Random::Val(0, nW - 1), Random::Val(0, nH - 1)});
    }
    else {
      const float angle= 2.0f * M_PI * (float)idxPeg / (float)D.UI[PegNumber___].GetI();
      const int w= std::round((0.5 + 0.5 * std::cos(angle)) * (nW - 1));
      const int h= std::round((0.5 + 0.5 * std::sin(angle)) * (nH - 1));
      Pegs.push_back(std::array<int, 2>{std::min(std::max(w, 0), nW - 1), std::min(std::max(h, 0), nH - 1)});
    }
  }
  PegsCount= std::vector<int>(Pegs.size(), 0);

  // Initialize lines
  Lines.clear();
  Lines.resize(6);
  Lines[0].push_back(0);
  Lines[1].push_back(0);
  Lines[2].push_back(0);
  Lines[3].push_back(0);
  Lines[4].push_back(0);
  Lines[5].push_back(0);
}


// Animate the project
void StringArtOptim::Animate() {
  if (!isActivProj) return;
  CheckAlloc();
  if (!isAllocated) Allocate();
  CheckRefresh();
  if (!isRefreshed) Refresh();

  for (int idxStep= 0; idxStep < D.UI[StepCount___].GetI(); idxStep++)
    StringArtOptim::AddLineStep();

  // Compute the total match error
  float Err= 0.0f;
  for (int w= 0; w < nW; w++)
    for (int h= 0; h < nH; h++)
      Err+= (ImCur[w][h] - ImRef[w][h]).norm();

  // Compute the total string length for each color
  float Len[6]= {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
  for (int idxCol= 0; idxCol < 6; idxCol++) {
    for (int idxLine= 1; idxLine < (int)Lines[idxCol].size(); idxLine++) {
      Math::Vec2f pos0((Pegs[Lines[idxCol][idxLine - 1]][0] + 0.5f) / (float)(nW), (Pegs[Lines[idxCol][idxLine - 1]][1] + 0.5f) / (float)(nH));
      Math::Vec2f pos1((Pegs[Lines[idxCol][idxLine]][0] + 0.5f) / (float)(nW), (Pegs[Lines[idxCol][idxLine]][1] + 0.5f) / (float)(nH));
      Len[idxCol]+= (pos0 - pos1).norm();
    }
  }

  // Add to plot data
  D.plotData.resize(7);
  D.plotLegend.resize(7);
  D.plotLegend[0]= "Match Err";
  D.plotLegend[1]= "Length R";
  D.plotLegend[2]= "Length G";
  D.plotLegend[3]= "Length B";
  D.plotLegend[4]= "Length C";
  D.plotLegend[5]= "Length M";
  D.plotLegend[6]= "Length Y";
  D.plotData[0].push_back(Err);
  D.plotData[1].push_back(Len[0]);
  D.plotData[2].push_back(Len[1]);
  D.plotData[3].push_back(Len[2]);
  D.plotData[4].push_back(Len[3]);
  D.plotData[5].push_back(Len[4]);
  D.plotData[6].push_back(Len[5]);

  D.scatLegend.resize(1);
  D.scatLegend[0]= "Counts";
  D.scatData.clear();
  D.scatData.resize(1);
  for (int idxCol= 0; idxCol < 6; idxCol++)
    D.scatData[0].push_back(std::array<double, 2>({(double)idxCol, (double)Lines[idxCol].size()}));
}


// Draw the project
void StringArtOptim::Draw() {
  if (!isActivProj) return;
  if (!isAllocated) return;
  if (!isRefreshed) return;

  // Draw the reference and generated images
  if (D.displayMode1 || D.displayMode2) {
    glPushMatrix();
    glTranslatef(1.0f, 0.0f, 0.0f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    for (int w= 0; w < nW; w++) {
      for (int h= 0; h < nH; h++) {
        if (D.displayMode1) {
          glColor3fv(ImRef[w][h].array());
          glRectf(float(w) / float(nW), float(h) / float(nH), float(w + 1) / float(nW), float(h + 1) / float(nH));
        }
        if (D.displayMode2) {
          glColor3fv(ImCur[w][h].array());
          glRectf(float(w) / float(nW), float(h) / float(nH), float(w + 1) / float(nW), float(h + 1) / float(nH));
        }
      }
    }
    glPopMatrix();
  }

  // Draw the pegs
  if (D.displayMode3) {
    float avgPegCount= 0.0f;
    for (int idxCol= 0; idxCol < 6; idxCol++)
      avgPegCount+= (float)Lines[idxCol].size();
    avgPegCount/= (float)Pegs.size();
    glPointSize(10.0f);
    glBegin(GL_POINTS);
    for (int idxPeg= 0; idxPeg < (int)Pegs.size(); idxPeg++) {
      float r= 0.0f, g= 0.0f, b= 0.0f;
      Colormap::RatioToJetBrightSmooth(0.5f * (float)PegsCount[idxPeg] / avgPegCount, r, g, b);
      glColor3f(r, g, b);
      glVertex3f(1.01f, (Pegs[idxPeg][0] + 0.5f) / float(nW), (Pegs[idxPeg][1] + 0.5f) / float(nH));
    }
    glEnd();
    glPointSize(1.0f);
  }

  // Draw the string
  if (D.displayMode4) {
    for (int idxCol= 0; idxCol < 6; idxCol++) {
      if (Lines[idxCol].size() >= 2) {
        glLineWidth(2.0f);
        glBegin(GL_LINE_STRIP);
        if (idxCol == 0) glColor3f(1.0f, 0.0f, 0.0f);
        if (idxCol == 1) glColor3f(0.0f, 1.0f, 0.0f);
        if (idxCol == 2) glColor3f(0.0f, 0.0f, 1.0f);
        if (idxCol == 3) glColor3f(0.0f, 0.5f, 0.5f);
        if (idxCol == 4) glColor3f(0.5f, 0.0f, 0.5f);
        if (idxCol == 5) glColor3f(0.5f, 0.5f, 0.0f);
        for (int idxLine= 0; idxLine < (int)Lines[idxCol].size(); idxLine++) {
          Math::Vec3f pos(1.02f, (Pegs[Lines[idxCol][idxLine]][0] + 0.5f) / (float)(nW), (Pegs[Lines[idxCol][idxLine]][1] + 0.5f) / (float)(nH));
          glVertex3fv(pos.array());
        }
        glEnd();
        glLineWidth(1.0f);
      }
    }
  }
}


void StringArtOptim::AddLineStep() {
  // Intialize the update array
  int bestPeg[6]= {-1, -1, -1, -1, -1, -1};
  float bestErr[6]= {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

// Find the best next peg for each color
#pragma omp parallel for
  for (int idxCol= 0; idxCol < 6; idxCol++) {
    const int idxPeg0= Lines[idxCol][Lines[idxCol].size() - 1];
    for (int idxPeg1= 0; idxPeg1 < (int)Pegs.size(); idxPeg1++) {
      std::vector<std::array<int, 2>> path= Bresenham2D(Pegs[idxPeg0][0], Pegs[idxPeg0][1], Pegs[idxPeg1][0], Pegs[idxPeg1][1]);
      float chgErr= 0.0f;
      for (int idxPos= 0; idxPos < (int)path.size(); idxPos++) {
        const int w= path[idxPos][0];
        const int h= path[idxPos][1];
        const float colChg= ((idxCol < 3) ? (D.UI[CoeffRGB____].GetF()) : (-D.UI[CoeffCMY____].GetF())) * D.UI[CoeffColor__].GetF();
        const float newVal= std::min(std::max(ImCur[w][h][idxCol % 3] + colChg, 0.0f), 1.0f);
        const float curErr= ImRef[w][h][idxCol % 3] - ImCur[w][h][idxCol % 3];
        const float newErr= ImRef[w][h][idxCol % 3] - newVal;
        chgErr+= ((newErr > 0) ? (D.UI[CoeffBias___].GetF()) : (1.0f - D.UI[CoeffBias___].GetF())) * newErr * newErr;
        chgErr-= ((curErr > 0) ? (D.UI[CoeffBias___].GetF()) : (1.0f - D.UI[CoeffBias___].GetF())) * curErr * curErr;
      }
      if (bestPeg[idxCol] < 0 || bestErr[idxCol] > chgErr) {
        bestErr[idxCol]= chgErr;
        bestPeg[idxCol]= idxPeg1;
      }
    }
  }


  // Update the image
  for (int idxCol= 0; idxCol < 6; idxCol++) {
    if (bestPeg[idxCol] >= 0 && bestErr[idxCol] < D.UI[CoeffCommit_].GetF()) {
      Lines[idxCol].push_back(bestPeg[idxCol]);
      PegsCount[bestPeg[idxCol]]++;
      if ((int)Lines[idxCol].size() > 1) {
        const int w0= Pegs[Lines[idxCol][Lines[idxCol].size() - 2]][0];
        const int h0= Pegs[Lines[idxCol][Lines[idxCol].size() - 2]][1];
        const int w1= Pegs[Lines[idxCol][Lines[idxCol].size() - 1]][0];
        const int h1= Pegs[Lines[idxCol][Lines[idxCol].size() - 1]][1];
        std::vector<std::array<int, 2>> path= Bresenham2D(w0, h0, w1, h1);
        for (int idxPos= 0; idxPos < (int)path.size(); idxPos++) {
          const int w= path[idxPos][0];
          const int h= path[idxPos][1];
          const float colChg= ((idxCol < 3) ? (D.UI[CoeffRGB____].GetF()) : (-D.UI[CoeffCMY____].GetF())) * D.UI[CoeffColor__].GetF();
          ImCur[w][h][idxCol % 3]= std::min(std::max(ImCur[w][h][idxCol % 3] + colChg, 0.0f), 1.0f);
        }
      }
    }
  }
}


std::vector<std::array<int, 2>> StringArtOptim::Bresenham2D(int x0, int y0, int x1, int y1) {
  std::vector<std::array<int, 2>> listVoxels;
  listVoxels.push_back({x0, y0});

  int dx= std::abs(x1 - x0);
  int dy= std::abs(y1 - y0);
  int xs= (x1 > x0) ? 1 : -1;
  int ys= (y1 > y0) ? 1 : -1;

  // Driving axis is X-axis
  if (dx >= dy) {
    int p1= 2 * dy - dx;
    while (x0 != x1) {
      x0+= xs;
      if (p1 >= 0) {
        y0+= ys;
        p1-= 2 * dx;
      }
      p1+= 2 * dy;
      listVoxels.push_back({x0, y0});
    }
  }

  // Driving axis is Y-axis
  else {
    int p1= 2 * dx - dy;
    while (y0 != y1) {
      y0+= ys;
      if (p1 >= 0) {
        x0+= xs;
        p1-= 2 * dy;
      }
      p1+= 2 * dx;
      listVoxels.push_back({x0, y0});
    }
  }

  return listVoxels;
}
