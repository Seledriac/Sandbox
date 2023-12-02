#include "StringArtOptim.hpp"


// Standard lib
#include <cmath>
#include <cstdio>
#include <vector>

// GLUT lib
#include "../freeglut/include/GL/freeglut.h"

// Project lib
#include "../Algo/Bresenham.hpp"
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
  ColorsAdd___,
  ColorsSub___,
  StepCount___,
  SingleLine__,
  BlendMode___,
  CoeffColor__,
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
    D.UI.push_back(ParamUI("ImageID_____", 3));
    D.UI.push_back(ParamUI("ImageSizeW__", 256));
    D.UI.push_back(ParamUI("ImageSizeH__", 256));
    D.UI.push_back(ParamUI("PegLayout___", 1));
    D.UI.push_back(ParamUI("PegNumber___", 256));
    D.UI.push_back(ParamUI("ColorsAdd___", 1));
    D.UI.push_back(ParamUI("ColorsSub___", 1));
    D.UI.push_back(ParamUI("StepCount___", 1));
    D.UI.push_back(ParamUI("SingleLine__", -0.5));
    D.UI.push_back(ParamUI("BlendMode___", -0.5));
    D.UI.push_back(ParamUI("CoeffColor__", 0.1));
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
  if (D.UI[ColorsAdd___].hasChanged()) isAllocated= false;
  if (D.UI[ColorsSub___].hasChanged()) isAllocated= false;
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
  if (D.UI[ImageID_____].GetI() == 3) FileInput::LoadImageBMPFile("Resources/SA_Butterfly.bmp", imageRGBA, false);
  if (D.UI[ImageID_____].GetI() == 4) FileInput::LoadImageBMPFile("Resources/GR_AlbertArt.bmp", imageRGBA, false);
  if (D.UI[ImageID_____].GetI() == 5) FileInput::LoadImageBMPFile("Resources/GR_DeepField.bmp", imageRGBA, false);
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

  // Initialize colors
  Colors.clear();
  if (D.UI[ColorsAdd___].GetI() >= 1) {
    Colors.push_back(Math::Vec3f(1.0f, 0.0f, 0.0f));
    Colors.push_back(Math::Vec3f(0.0f, 1.0f, 0.0f));
    Colors.push_back(Math::Vec3f(0.0f, 0.0f, 1.0f));
  }
  if (D.UI[ColorsAdd___].GetI() >= 2) {
    Colors.push_back(Math::Vec3f(0.0f, 1.0f, 1.0f));
    Colors.push_back(Math::Vec3f(1.0f, 0.0f, 1.0f));
    Colors.push_back(Math::Vec3f(1.0f, 1.0f, 0.0f));
  }
  if (D.UI[ColorsAdd___].GetI() >= 3) {
    Colors.push_back(Math::Vec3f(1.0f, 1.0f, 1.0f));
  }
  if (D.UI[ColorsSub___].GetI() >= 1) {
    Colors.push_back(Math::Vec3f(-1.0f, 0.0f, 0.0f));
    Colors.push_back(Math::Vec3f(0.0f, -1.0f, 0.0f));
    Colors.push_back(Math::Vec3f(0.0f, 0.0f, -1.0f));
  }
  if (D.UI[ColorsSub___].GetI() >= 2) {
    Colors.push_back(Math::Vec3f(0.0f, -1.0f, -1.0f));
    Colors.push_back(Math::Vec3f(-1.0f, 0.0f, -1.0f));
    Colors.push_back(Math::Vec3f(-1.0f, -1.0f, 0.0f));
  }
  if (D.UI[ColorsSub___].GetI() >= 3) {
    Colors.push_back(Math::Vec3f(-1.0f, -1.0f, -1.0f));
  }
  for (int idxCol= 0; idxCol < (int)Colors.size(); idxCol++)
    Colors[idxCol].normalize();

  // Initialize lines
  Lines.clear();
  Lines.resize(Colors.size(), std::vector<int>(1, 0));
}


// Animate the project
void StringArtOptim::Animate() {
  if (!isActivProj) return;
  CheckAlloc();
  if (!isAllocated) Allocate();
  CheckRefresh();
  if (!isRefreshed) Refresh();

  // Compute and add the new lines
  bool lineWasAdded= false;
  for (int idxStep= 0; idxStep < D.UI[StepCount___].GetI(); idxStep++) {
    if (StringArtOptim::AddLineStep()) lineWasAdded= true;
    else break;
  }

  if (lineWasAdded) {
    // Compute the total match error
    float Err= 0.0f;
    for (int w= 0; w < nW; w++) {
      for (int h= 0; h < nH; h++) {
        Err+= (ImCur[w][h] - ImRef[w][h]).normSquared();
      }
    }

    // Compute the total string length for each color
    std::vector<float> Len((int)Colors.size(), 0.0f);
    for (int idxCol= 0; idxCol < (int)Colors.size(); idxCol++) {
      for (int idxLine= 1; idxLine < (int)Lines[idxCol].size(); idxLine++) {
        Math::Vec2f pos0((Pegs[Lines[idxCol][idxLine - 1]][0] + 0.5f) / (float)(nW), (Pegs[Lines[idxCol][idxLine - 1]][1] + 0.5f) / (float)(nH));
        Math::Vec2f pos1((Pegs[Lines[idxCol][idxLine]][0] + 0.5f) / (float)(nW), (Pegs[Lines[idxCol][idxLine]][1] + 0.5f) / (float)(nH));
        Len[idxCol]+= (pos0 - pos1).norm();
      }
    }

    // Add to plot data
    D.plotLegend.resize(2);
    D.plotData.resize(2);
    D.plotData[1].resize(Pegs.size());
    D.plotLegend[0]= "MatchErr";
    D.plotLegend[1]= "PegCounts";
    D.plotData[0].push_back(Err);
    for (int idxPeg= 0; idxPeg < (int)Pegs.size(); idxPeg++) {
      D.plotData[1][idxPeg]= PegsCount[idxPeg];
    }

    // Add to scatter data
    D.scatLegend.resize(1);
    D.scatData.resize(1);
    D.scatData[0].resize(Colors.size());
    D.scatLegend[0]= "Lengths";
    for (int idxCol= 0; idxCol < (int)Colors.size(); idxCol++) {
      D.scatData[0][idxCol]= std::array<double, 2>({(double)idxCol, (double)Len[idxCol]});
    }
  }
}


// Draw the project
void StringArtOptim::Draw() {
  if (!isActivProj) return;
  if (!isAllocated) return;
  if (!isRefreshed) return;

  // Draw the reference and generated images
  if (D.displayMode1 || D.displayMode2) {
    glPushMatrix();
    glTranslatef(0.5f, 0.0f, 0.0f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    for (int w= 0; w < nW; w++) {
      for (int h= 0; h < nH; h++) {
        if (D.displayMode1 && !D.displayMode2) {
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
    for (int idxCol= 0; idxCol < (int)Colors.size(); idxCol++) {
      avgPegCount+= (float)Lines[idxCol].size();
    }
    avgPegCount/= (float)Pegs.size();
    glPointSize(10.0f);
    glBegin(GL_POINTS);
    for (int idxPeg= 0; idxPeg < (int)Pegs.size(); idxPeg++) {
      float r= 0.0f, g= 0.0f, b= 0.0f;
      Colormap::RatioToJetBrightSmooth(0.5f * (float)PegsCount[idxPeg] / avgPegCount, r, g, b);
      glColor3f(r, g, b);
      glVertex3f(0.5f + 0.001f, (Pegs[idxPeg][0] + 0.5f) / float(nW), (Pegs[idxPeg][1] + 0.5f) / float(nH));
    }
    glEnd();
    glPointSize(1.0f);
  }

  // Draw the string
  if (D.displayMode4) {
    for (int idxCol= 0; idxCol < (int)Colors.size(); idxCol++) {
      if (Lines[idxCol].size() >= 2) {
        glLineWidth(2.0f);
        glBegin(GL_LINE_STRIP);
        glColor3fv((0.5f * Colors[idxCol] + Math::Vec3f(0.5f, 0.5f, 0.5f)).array());
        for (int idxLine= 0; idxLine < (int)Lines[idxCol].size(); idxLine++) {
          Math::Vec3f pos(0.5f + 0.05f * float(idxLine) / float(Lines[idxCol].size()),
                          (Pegs[Lines[idxCol][idxLine]][0] + 0.5f) / (float)(nW),
                          (Pegs[Lines[idxCol][idxLine]][1] + 0.5f) / (float)(nH));
          glVertex3fv(pos.array());
        }
        glEnd();
        glLineWidth(1.0f);
      }
    }
  }
}


bool StringArtOptim::AddLineStep() {
  // Intialize the update arrays
  std::vector<int> bestPeg((int)Colors.size(), -1);
  std::vector<float> bestErr((int)Colors.size(), 0.0f);

// Find the best next peg for each color with greedy search
#pragma omp parallel for
  for (int idxCol= 0; idxCol < (int)Colors.size(); idxCol++) {
    const int idxPeg0= Lines[idxCol].back();
    for (int idxPeg1= 0; idxPeg1 < (int)Pegs.size(); idxPeg1++) {
      std::vector<std::array<int, 2>> path= Bresenham::Line2D(Pegs[idxPeg0][0], Pegs[idxPeg0][1], Pegs[idxPeg1][0], Pegs[idxPeg1][1]);
      float chgErr= 0.0f;
      for (int idxPos= 0; idxPos < (int)path.size(); idxPos++) {
        const int w= path[idxPos][0];
        const int h= path[idxPos][1];
        Math::Vec3f newVal;
        if (D.UI[BlendMode___].GetB())
          newVal= (1.0f - D.UI[CoeffColor__].GetF()) * ImCur[w][h] + D.UI[CoeffColor__].GetF() * Colors[idxCol];
        else
          newVal= ImCur[w][h] + D.UI[CoeffColor__].GetF() * Colors[idxCol];
        for (int k= 0; k < 3; k++) newVal[k]= std::min(std::max(newVal[k], 0.0f), 1.0f);
        const Math::Vec3f curErr= ImRef[w][h] - ImCur[w][h];
        const Math::Vec3f newErr= ImRef[w][h] - newVal;
        chgErr+= newErr.normSquared() - curErr.normSquared();
      }
      if (bestPeg[idxCol] < 0 || bestErr[idxCol] > chgErr) {
        bestErr[idxCol]= chgErr;
        bestPeg[idxCol]= idxPeg1;
      }
    }
  }

  // Optionally keep only the best color
  if (D.UI[SingleLine__].GetB()) {
    int idxBestCol= 0;
    for (int idxCol= 0; idxCol < (int)Colors.size(); idxCol++) {
      if (bestErr[idxBestCol] > bestErr[idxCol]) {
        idxBestCol= idxCol;
      }
    }
    for (int idxCol= 0; idxCol < (int)Colors.size(); idxCol++) {
      if (idxCol != idxBestCol) {
        bestPeg[idxCol]= -1;
      }
    }
  }

  // Update the lines
  bool lineWasAdded= false;
  for (int idxCol= 0; idxCol < (int)Colors.size(); idxCol++) {
    if (bestPeg[idxCol] >= 0 && bestErr[idxCol] < 0.0f) {
      Lines[idxCol].push_back(bestPeg[idxCol]);
      PegsCount[bestPeg[idxCol]]++;
      lineWasAdded= true;
      // Update the image
      if ((int)Lines[idxCol].size() > 1) {
        const int w0= Pegs[Lines[idxCol][Lines[idxCol].size() - 2]][0];
        const int h0= Pegs[Lines[idxCol][Lines[idxCol].size() - 2]][1];
        const int w1= Pegs[Lines[idxCol][Lines[idxCol].size() - 1]][0];
        const int h1= Pegs[Lines[idxCol][Lines[idxCol].size() - 1]][1];
        std::vector<std::array<int, 2>> path= Bresenham::Line2D(w0, h0, w1, h1);
        for (int idxPos= 0; idxPos < (int)path.size(); idxPos++) {
          const int w= path[idxPos][0];
          const int h= path[idxPos][1];
          if (D.UI[BlendMode___].GetB())
            ImCur[w][h]= (1.0f - D.UI[CoeffColor__].GetF()) * ImCur[w][h] + D.UI[CoeffColor__].GetF() * Colors[idxCol];
          else
            ImCur[w][h]= ImCur[w][h] + D.UI[CoeffColor__].GetF() * Colors[idxCol];
          for (int k= 0; k < 3; k++) ImCur[w][h][k]= std::min(std::max(ImCur[w][h][k], 0.0f), 1.0f);
        }
      }
    }
  }

  return lineWasAdded;
}
