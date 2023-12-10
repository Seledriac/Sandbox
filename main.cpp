// Standard lib
#include <cstdlib>
#include <ctime>
#include <limits>

// GLUT lib
#include "Libs/freeglut/include/GL/freeglut.h"

// Trackball lib
#include "Libs/tb/Camera.hpp"

// Project Data
#include "Data.hpp"

// Project Utilities
#include "Util/Colormap.hpp"

// Project Sandbox Classes
#include "Projects/AgentSwarmBoid/AgentSwarmBoid.hpp"
#include "Projects/CompuFluidDyna/CompuFluidDyna.hpp"
#include "Projects/FractalCurvDev/FractalCurvDev.hpp"
#include "Projects/FractalElevMap/FractalElevMap.hpp"
#include "Projects/MarkovProcGene/MarkovProcGene.hpp"
#include "Projects/MassSpringSyst/MassSpringSyst.hpp"
#include "Projects/PosiBasedDynam/PosiBasedDynam.hpp"
#include "Projects/SpaceTimeWorld/SpaceTimeWorld.hpp"
#include "Projects/StringArtOptim/StringArtOptim.hpp"
#include "Projects/TerrainErosion/TerrainErosion.hpp"


// Global variables used by the display
static int windowID;
static int winW, winH;
static int winPosW, winPosH;
static int currentProjectID;
static bool isMenuAttached;
static bool isMenuActive;
static bool isDarkMode;
static bool isSmoothDraw;

// Global constants used by the display
constexpr int winFPS= 60;
constexpr int paramLabelNbChar= 12;
constexpr int paramSpaceNbChar= 1;
constexpr int paramValSignNbChar= 1;
constexpr int paramValInteNbChar= 9;
constexpr int paramValSepaNbChar= 1;
constexpr int paramValFracNbChar= 9;
constexpr int paramValNbChar= paramValSignNbChar + paramValInteNbChar + paramValSepaNbChar + paramValFracNbChar;
constexpr int charHeight= 14;
constexpr int charWidth= 10;
constexpr int pixelMargin= 1;
constexpr int plotAreaW= 600;
constexpr int plotAreaH= 100;
constexpr int scatAreaW= 250;
constexpr int scatAreaH= 250;
constexpr int textBoxW= 9 * charWidth;
constexpr int textBoxH= charHeight;
Camera *cam;

// Global variables used by the projects
Data D;
AgentSwarmBoid myAgentSwarmBoid;
CompuFluidDyna myCompuFluidDyna;
FractalCurvDev myFractalCurvDev;
FractalElevMap myFractalElevMap;
MarkovProcGene myMarkovProcGene;
MassSpringSyst myMassSpringSyst;
PosiBasedDynam myPosiBasedDynam;
SpaceTimeWorld mySpaceTimeWorld;
StringArtOptim myStringArtOptim;
TerrainErosion myTerrainErosion;

enum ProjectID
{
  AaaaaaaaaaaaaaID,
  AgentSwarmBoidID,
  CompuFluidDynaID,
  FractalCurvDevID,
  FractalElevMapID,
  MarkovProcGeneID,
  MassSpringSystID,
  PosiBasedDynamID,
  SpaceTimeWorldID,
  StringArtOptimID,
  TerrainErosionID,
  ZzzzzzzzzzzzzzID,
};

void project_ForceHardInit() {
  D.plotLegend.clear();
  D.scatLegend.clear();
  D.plotData.clear();
  D.scatData.clear();

  if (currentProjectID != ProjectID::AgentSwarmBoidID && myAgentSwarmBoid.isActivProj) myAgentSwarmBoid= AgentSwarmBoid();
  if (currentProjectID != ProjectID::CompuFluidDynaID && myCompuFluidDyna.isActivProj) myCompuFluidDyna= CompuFluidDyna();
  if (currentProjectID != ProjectID::FractalCurvDevID && myFractalCurvDev.isActivProj) myFractalCurvDev= FractalCurvDev();
  if (currentProjectID != ProjectID::FractalElevMapID && myFractalElevMap.isActivProj) myFractalElevMap= FractalElevMap();
  if (currentProjectID != ProjectID::MarkovProcGeneID && myMarkovProcGene.isActivProj) myMarkovProcGene= MarkovProcGene();
  if (currentProjectID != ProjectID::MassSpringSystID && myMassSpringSyst.isActivProj) myMassSpringSyst= MassSpringSyst();
  if (currentProjectID != ProjectID::PosiBasedDynamID && myPosiBasedDynam.isActivProj) myPosiBasedDynam= PosiBasedDynam();
  if (currentProjectID != ProjectID::SpaceTimeWorldID && mySpaceTimeWorld.isActivProj) mySpaceTimeWorld= SpaceTimeWorld();
  if (currentProjectID != ProjectID::StringArtOptimID && myStringArtOptim.isActivProj) myStringArtOptim= StringArtOptim();
  if (currentProjectID != ProjectID::TerrainErosionID && myTerrainErosion.isActivProj) myTerrainErosion= TerrainErosion();

  if (currentProjectID == ProjectID::AgentSwarmBoidID) myAgentSwarmBoid.SetActiveProject();
  if (currentProjectID == ProjectID::CompuFluidDynaID) myCompuFluidDyna.SetActiveProject();
  if (currentProjectID == ProjectID::FractalCurvDevID) myFractalCurvDev.SetActiveProject();
  if (currentProjectID == ProjectID::FractalElevMapID) myFractalElevMap.SetActiveProject();
  if (currentProjectID == ProjectID::MarkovProcGeneID) myMarkovProcGene.SetActiveProject();
  if (currentProjectID == ProjectID::MassSpringSystID) myMassSpringSyst.SetActiveProject();
  if (currentProjectID == ProjectID::PosiBasedDynamID) myPosiBasedDynam.SetActiveProject();
  if (currentProjectID == ProjectID::SpaceTimeWorldID) mySpaceTimeWorld.SetActiveProject();
  if (currentProjectID == ProjectID::StringArtOptimID) myStringArtOptim.SetActiveProject();
  if (currentProjectID == ProjectID::TerrainErosionID) myTerrainErosion.SetActiveProject();
}


void project_QueueSoftRefresh() {
  if (currentProjectID == ProjectID::AgentSwarmBoidID) myAgentSwarmBoid.isRefreshed= false;
  if (currentProjectID == ProjectID::CompuFluidDynaID) myCompuFluidDyna.isRefreshed= false;
  if (currentProjectID == ProjectID::FractalCurvDevID) myFractalCurvDev.isRefreshed= false;
  if (currentProjectID == ProjectID::FractalElevMapID) myFractalElevMap.isRefreshed= false;
  if (currentProjectID == ProjectID::MarkovProcGeneID) myMarkovProcGene.isRefreshed= false;
  if (currentProjectID == ProjectID::MassSpringSystID) myMassSpringSyst.isRefreshed= false;
  if (currentProjectID == ProjectID::PosiBasedDynamID) myPosiBasedDynam.isRefreshed= false;
  if (currentProjectID == ProjectID::SpaceTimeWorldID) mySpaceTimeWorld.isRefreshed= false;
  if (currentProjectID == ProjectID::StringArtOptimID) myStringArtOptim.isRefreshed= false;
  if (currentProjectID == ProjectID::TerrainErosionID) myTerrainErosion.isRefreshed= false;
}


void project_Refresh() {
  if (currentProjectID == ProjectID::AgentSwarmBoidID) myAgentSwarmBoid.Refresh();
  if (currentProjectID == ProjectID::CompuFluidDynaID) myCompuFluidDyna.Refresh();
  if (currentProjectID == ProjectID::FractalCurvDevID) myFractalCurvDev.Refresh();
  if (currentProjectID == ProjectID::FractalElevMapID) myFractalElevMap.Refresh();
  if (currentProjectID == ProjectID::MarkovProcGeneID) myMarkovProcGene.Refresh();
  if (currentProjectID == ProjectID::MassSpringSystID) myMassSpringSyst.Refresh();
  if (currentProjectID == ProjectID::PosiBasedDynamID) myPosiBasedDynam.Refresh();
  if (currentProjectID == ProjectID::SpaceTimeWorldID) mySpaceTimeWorld.Refresh();
  if (currentProjectID == ProjectID::StringArtOptimID) myStringArtOptim.Refresh();
  if (currentProjectID == ProjectID::TerrainErosionID) myTerrainErosion.Refresh();
}


void project_Animate() {
  if (currentProjectID == ProjectID::AgentSwarmBoidID) myAgentSwarmBoid.Animate();
  if (currentProjectID == ProjectID::CompuFluidDynaID) myCompuFluidDyna.Animate();
  if (currentProjectID == ProjectID::FractalCurvDevID) myFractalCurvDev.Animate();
  if (currentProjectID == ProjectID::FractalElevMapID) myFractalElevMap.Animate();
  if (currentProjectID == ProjectID::MarkovProcGeneID) myMarkovProcGene.Animate();
  if (currentProjectID == ProjectID::MassSpringSystID) myMassSpringSyst.Animate();
  if (currentProjectID == ProjectID::PosiBasedDynamID) myPosiBasedDynam.Animate();
  if (currentProjectID == ProjectID::SpaceTimeWorldID) mySpaceTimeWorld.Animate();
  if (currentProjectID == ProjectID::StringArtOptimID) myStringArtOptim.Animate();
  if (currentProjectID == ProjectID::TerrainErosionID) myTerrainErosion.Animate();
}


void project_Draw() {
  if (currentProjectID == ProjectID::AgentSwarmBoidID) myAgentSwarmBoid.Draw();
  if (currentProjectID == ProjectID::CompuFluidDynaID) myCompuFluidDyna.Draw();
  if (currentProjectID == ProjectID::FractalCurvDevID) myFractalCurvDev.Draw();
  if (currentProjectID == ProjectID::FractalElevMapID) myFractalElevMap.Draw();
  if (currentProjectID == ProjectID::MarkovProcGeneID) myMarkovProcGene.Draw();
  if (currentProjectID == ProjectID::MassSpringSystID) myMassSpringSyst.Draw();
  if (currentProjectID == ProjectID::PosiBasedDynamID) myPosiBasedDynam.Draw();
  if (currentProjectID == ProjectID::SpaceTimeWorldID) mySpaceTimeWorld.Draw();
  if (currentProjectID == ProjectID::StringArtOptimID) myStringArtOptim.Draw();
  if (currentProjectID == ProjectID::TerrainErosionID) myTerrainErosion.Draw();
}


// Utility function to save persistent sandbox configuration on disk
void saveConfigSandbox() {
  FILE *file= nullptr;
  file= fopen("ConfigSandbox.txt", "w");
  if (file != nullptr) {
    fprintf(file, "winPosW winPosH %d %d\n", winPosW, winPosH);
    fprintf(file, "winW winH %d %d\n", winW, winH);
    fclose(file);
  }
}


// Utility function to load persistent sandbox configuration from disk
void loadConfigSandbox() {
  FILE *file= nullptr;
  file= fopen("ConfigSandbox.txt", "r");
  if (file != nullptr) {
    fscanf(file, "winPosW winPosH %d %d\n", &winPosW, &winPosH);
    fscanf(file, "winW winH %d %d\n", &winW, &winH);
    fclose(file);
  }
}


// Utility function to save persistent project configuration on disk
void saveConfigProject() {
  FILE *file= nullptr;
  file= fopen("ConfigProject.txt", "w");
  if (file != nullptr) {
    fprintf(file, "currentProjectID %d\n", currentProjectID);
    for (int idxParam= 0; idxParam < (int)D.UI.size(); idxParam++) {
      fprintf(file, "%s %lf\n", D.UI[idxParam].name.c_str(), D.UI[idxParam].GetD());
    }
    fclose(file);
  }
}


// Utility function to load persistent project configuration from disk
void loadConfigProject() {
  FILE *file= nullptr;
  file= fopen("ConfigProject.txt", "r");
  if (file != nullptr) {
    fscanf(file, "currentProjectID %d\n", &currentProjectID);
    for (int idxParam= 0; idxParam < (int)D.UI.size(); idxParam++) {
      double val= 0.0;
      char name[100];
      if (fscanf(file, "%s %lf\n", name, &val) == 2) {
        D.UI[idxParam].Set(val);
      }
    }
    fclose(file);
  }
}


// Returns the elapsed time since its last call
float elapsed_time() {
  static long last_time= -1;
  long current_time;
  float t= 0;

  current_time= glutGet(GLUT_ELAPSED_TIME);
  if (last_time == -1) {
    last_time= glutGet(GLUT_ELAPSED_TIME);
  }
  else {
    t+= (float)(current_time - last_time) / 1000.0f;
    last_time= current_time;
  }

  return t;
}


// Utility function to draw text
void draw_text(int const x, int const y, char *const text) {
  glPushMatrix();
  glTranslatef(float(x), float(y), 0.0f);
  const float baseHeight= 152.38f;
  const float baseWidth= 104.76f;
  glScalef(float(charWidth) / baseWidth, float(charHeight) / baseHeight, 1.0f);
  for (char *p= text; *p; p++)
    glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, *p);
  glPopMatrix();
}


// Display callback
void callback_display() {
  // Set and clear viewport
  glViewport(0, 0, winW, winH);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Set the camera transformation matrix for the scene
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0, double(winW) / double(winH), 0.1, 100.0);

  // Set the world transformation matrix for the scene
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  cam->setWindowSize(float(winW), float(winH));
  glMultMatrixf(cam->getViewMatrix());

  // Draw the reference frame and box
  if (D.showAxis) {
    // XZY basis lines
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 0.0f, 0.0f);
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 1.0f);
    glEnd();
    glLineWidth(1.0f);

    // XZY basis ends
    glPointSize(6.0f);
    glBegin(GL_POINTS);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(1.0f, 0.0f, 0.0f);
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(0.0f, 0.0f, 1.0f);
    glEnd();
    glPointSize(1.0f);

    // Bounding box
    glColor3f(0.5f, 0.5f, 0.5f);
    glPushMatrix();
    glTranslatef((float)D.boxMin[0], (float)D.boxMin[1], (float)D.boxMin[2]);
    glScalef((float)D.boxMax[0] - (float)D.boxMin[0], (float)D.boxMax[1] - (float)D.boxMin[1], (float)D.boxMax[2] - (float)D.boxMin[2]);
    glTranslatef(0.5f, 0.5f, 0.5f);
    glutWireCube(1.0);
    glPopMatrix();
  }

  // Draw stuff in the scene
  project_Draw();

  // Set the camera transformation matrix for the HUD
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, double(winW), 0.0, double(winH), -1.0, 1.0);

  // Set the world transformation matrix for the HUD
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // Draw the parameter list
  glLineWidth(2.0f);
  for (int k= 0; k < int(D.UI.size()); k++) {
    if (k == D.idxParamUI)
      glColor3f(0.8f, 0.4f, 0.4f);
    else {
      if (isDarkMode) glColor3f(0.8f, 0.8f, 0.8f);
      else glColor3f(0.2f, 0.2f, 0.2f);
    }
    char str[50];
    sprintf(str, "%s %+020.9f", D.UI[k].name.c_str(), D.UI[k].GetD());  // Format must match paramValNbChar settings
    draw_text(0, winH - (k + 1) * (charHeight + pixelMargin), str);
    if (k == D.idxParamUI) {
      sprintf(str, "_");
      draw_text((paramLabelNbChar + paramSpaceNbChar + D.idxCursorUI) * charWidth, winH - (k + 1) * (charHeight + pixelMargin), str);
      draw_text((paramLabelNbChar + paramSpaceNbChar + D.idxCursorUI) * charWidth, winH - 1 - k * (charHeight + pixelMargin), str);
    }
  }
  glLineWidth(1.0f);

  // Draw the 2D plot
  if (!D.plotData.empty()) {
    glLineWidth(2.0f);
    glPointSize(3.0f);
    for (int k0= 0; k0 < int(D.plotData.size()); k0++) {
      if (D.plotData[k0].empty()) continue;

      // Set the color
      float r, g, b;
      Colormap::RatioToRainbow(float(k0) / (float)std::max((int)D.plotData.size() - 1, 1), r, g, b);
      glColor3f(r, g, b);

      // Find the min max range for vertical scaling
      double valMin= std::numeric_limits<double>::max();
      double valMax= std::numeric_limits<double>::lowest();
      for (int k1= 0; k1 < int(D.plotData[k0].size()); k1++) {
        if (valMin > D.plotData[k0][k1]) valMin= D.plotData[k0][k1];
        if (valMax < D.plotData[k0][k1]) valMax= D.plotData[k0][k1];
      }

      // Draw the text for legend and min max values
      char str[50];
      if (k0 < (int)D.plotLegend.size())
        strcpy(str, D.plotLegend[k0].c_str());
      else
        strcpy(str, "<name>");
      draw_text(winW - plotAreaW - 3 * textBoxW, winH - textBoxH - textBoxH * k0 - textBoxH - pixelMargin, str);
      sprintf(str, "%+.2e", valMax);
      draw_text(winW - textBoxW - plotAreaW + k0 * textBoxW, winH - textBoxH - pixelMargin, str);
      sprintf(str, "%+.2e", valMin);
      draw_text(winW - textBoxW - plotAreaW + k0 * textBoxW, winH - plotAreaH - 2 * textBoxH - 2 * pixelMargin, str);
      sprintf(str, "%+.2e", D.plotData[k0][0]);
      draw_text(winW - plotAreaW - 2 * textBoxW, winH - textBoxH - textBoxH * k0 - textBoxH - pixelMargin, str);
      sprintf(str, "%+.2e", D.plotData[k0][D.plotData[k0].size() - 1]);
      draw_text(winW - textBoxW, winH - textBoxH - textBoxH * k0 - textBoxH - pixelMargin, str);

      // Draw the plot curves and markers
      if (int(D.plotData[k0].size()) >= 2) {
        for (int mode= 0; mode < 2; mode++) {
          if (mode == 0) glBegin(GL_LINE_STRIP);
          if (mode == 1) glBegin(GL_POINTS);
          for (int k1= 0; k1 < int(D.plotData[k0].size()); k1++) {
            double valScaled;
            if (valMax - valMin == 0.0) valScaled= 0.0;
            else if (!D.plotLogScale) valScaled= (D.plotData[k0][k1] - valMin) / (valMax - valMin);
            else if (D.plotData[k0][k1] <= 0.0) valScaled= 0.0;
            else if (valMin <= 0.0) valScaled= 1.0;
            else valScaled= (std::log10(D.plotData[k0][k1]) - std::log10(valMin)) / (std::log10(valMax) - std::log10(valMin));
            glVertex3i(winW - plotAreaW - textBoxW + plotAreaW * k1 / std::max((int)D.plotData[k0].size() - 1, 1), winH - plotAreaH - textBoxH - 2 * pixelMargin + plotAreaH * valScaled, 0);
          }
          glEnd();
        }
      }
    }
    glLineWidth(1.0f);
    glPointSize(1.0f);
  }

  // Draw the 2D scatter
  if (!D.scatData.empty()) {
    glLineWidth(2.0f);
    glBegin(GL_LINE_STRIP);
    glColor3f(0.7f, 0.7f, 0.7f);
    glVertex3i(textBoxW, scatAreaH + 3 * textBoxH, 0);
    glVertex3i(textBoxW, 3 * textBoxH, 0);
    glVertex3i(textBoxW + scatAreaW, 3 * textBoxH, 0);
    glEnd();

    // Find the min max range for scaling
    double valMinX= std::numeric_limits<double>::max();
    double valMinY= std::numeric_limits<double>::max();
    double valMaxX= std::numeric_limits<double>::lowest();
    double valMaxY= std::numeric_limits<double>::lowest();
    for (int k0= 0; k0 < int(D.scatData.size()); k0++) {
      for (int k1= 0; k1 < int(D.scatData[k0].size()); k1++) {
        if (valMinX > D.scatData[k0][k1][0]) valMinX= D.scatData[k0][k1][0];
        if (valMinY > D.scatData[k0][k1][1]) valMinY= D.scatData[k0][k1][1];
        if (valMaxX < D.scatData[k0][k1][0]) valMaxX= D.scatData[k0][k1][0];
        if (valMaxY < D.scatData[k0][k1][1]) valMaxY= D.scatData[k0][k1][1];
      }
    }

    // Draw min max values
    char str[50];
    sprintf(str, "%+.2e", valMinX);
    draw_text(textBoxW, 2 * textBoxH, str);
    sprintf(str, "%+.2e", valMaxX);
    draw_text(textBoxW + scatAreaW - textBoxW, 2 * textBoxH, str);
    sprintf(str, "%+.2e", valMinY);
    draw_text(0, 3 * textBoxH, str);
    sprintf(str, "%+.2e", valMaxY);
    draw_text(0, 3 * textBoxH + scatAreaH - textBoxH, str);

    glPointSize(3.0f);
    for (int k0= 0; k0 < int(D.scatData.size()); k0++) {
      if (D.scatData[k0].empty()) continue;

      // Set the color
      float r, g, b;
      Colormap::RatioToRainbow(float(k0) / (float)std::max((int)D.scatData.size() - 1, 1), r, g, b);
      glColor3f(r, g, b);

      // Draw the text for legend
      if (D.scatLegend.size() == D.scatData.size())
        strcpy(str, D.scatLegend[k0].c_str());
      else
        strcpy(str, "<name>");
      draw_text(0, scatAreaH - k0 * textBoxH, str);

      // Draw the polyline
      glBegin(GL_POINTS);
      for (int k1= 0; k1 < int(D.scatData[k0].size()); k1++) {
        const double relPosX= (D.scatData[k0][k1][0] - valMinX) / (valMaxX - valMinX);
        const double relPosY= (D.scatData[k0][k1][1] - valMinY) / (valMaxY - valMinY);
        glVertex3i(textBoxW + (int)std::round((double)scatAreaW * relPosX), 3 * textBoxH + (int)std::round((double)scatAreaH * relPosY), 0);
      }
      glEnd();
    }
    glPointSize(1.0f);
    glLineWidth(1.0f);
  }

  // Draw the frame time
  {
    glLineWidth(2.0f);
    glColor3f(0.8f, 0.8f, 0.8f);
    char str[50];

    sprintf(str, "%.3fs", elapsed_time());
    draw_text(0, 2, str);

    if (D.autoRefresh)
      glColor3f(1.0f, 0.6f, 0.6f);
    else
      glColor3f(0.8f, 0.8f, 0.8f);
    sprintf(str, "R");
    draw_text(0, 2 + charHeight, str);

    if (D.playAnimation)
      glColor3f(1.0f, 0.6f, 0.6f);
    else
      glColor3f(0.8f, 0.8f, 0.8f);
    sprintf(str, "P");
    draw_text(charWidth, 2 + charHeight, str);
    glLineWidth(1.0f);
  }

  // Commit the draw
  glutSwapBuffers();
}


// Timer program interruption callback
void callback_timer(int v) {
  // Compute animations
  if (D.playAnimation || D.stepAnimation) {
    project_Animate();
    glutPostRedisplay();
    D.stepAnimation= false;
  }

  glutTimerFunc(1000 / winFPS, callback_timer, v);
}


// Window reshape callback
void callback_reshape(int width, int height) {
  winW= width;
  winH= height;
  glutPostRedisplay();
}


// Keyboard interruption callback
void callback_keyboard(unsigned char key, int x, int y) {
  (void)x;  // Disable warning unused variable
  (void)y;  // Disable warning unused variable

  if (key == 27) {
    glutDestroyWindow(windowID);
    exit(EXIT_SUCCESS);
  }
  else if (key == ' ') D.playAnimation= !D.playAnimation;
  else if (key == '.') D.stepAnimation= !D.stepAnimation;
  else if (key == '\r') D.autoRefresh= !D.autoRefresh;
  else if (key == '\b') D.UI[D.idxParamUI].Set(0.0);
  else if (key == '\t') {
    if (!isMenuActive) {
      if (isMenuAttached) glutDetachMenu(GLUT_RIGHT_BUTTON);
      else glutAttachMenu(GLUT_RIGHT_BUTTON);
      isMenuAttached= !isMenuAttached;
    }
  }

  else if (key == '1') D.displayMode1= !D.displayMode1;
  else if (key == '2') D.displayMode2= !D.displayMode2;
  else if (key == '3') D.displayMode3= !D.displayMode3;
  else if (key == '4') D.displayMode4= !D.displayMode4;
  else if (key == '5') D.displayMode5= !D.displayMode5;
  else if (key == '6') D.displayMode6= !D.displayMode6;
  else if (key == '7') D.displayMode7= !D.displayMode7;
  else if (key == '8') D.displayMode8= !D.displayMode8;
  else if (key == '9') D.showAxis= !D.showAxis;
  else if (key == '0') {
    D.plotData.clear();
    D.scatData.clear();
  }
  else if (key == '-') D.plotLogScale= !D.plotLogScale;

  // TODO add mechanism to ass keypress to active project

  // Compute refresh
  if (D.autoRefresh)
    project_Refresh();

  glutPostRedisplay();
}


// Keyboard special interruption callback
void callback_keyboard_special(int key, int x, int y) {
  (void)x;  // Disable warning unused variable
  (void)y;  // Disable warning unused variable

  if (D.UI.empty()) return;

  if (glutGetModifiers() & GLUT_ACTIVE_SHIFT) {
    if (key == GLUT_KEY_UP) D.idxParamUI= (D.idxParamUI - 5 + int(D.UI.size())) % int(D.UI.size());
    if (key == GLUT_KEY_DOWN) D.idxParamUI= (D.idxParamUI + 5) % int(D.UI.size());
  }
  else {
    if (key == GLUT_KEY_UP) D.idxParamUI= (D.idxParamUI - 1 + int(D.UI.size())) % int(D.UI.size());
    if (key == GLUT_KEY_DOWN) D.idxParamUI= (D.idxParamUI + 1) % int(D.UI.size());
  }

  if (glutGetModifiers() & GLUT_ACTIVE_SHIFT) {
    if (key == GLUT_KEY_LEFT) D.UI[D.idxParamUI].Set(D.UI[D.idxParamUI].GetD() / 2.0);
    if (key == GLUT_KEY_RIGHT) D.UI[D.idxParamUI].Set(D.UI[D.idxParamUI].GetD() * 2.0);
  }
  else if (glutGetModifiers() & GLUT_ACTIVE_CTRL) {
    if (key == GLUT_KEY_LEFT) D.UI[D.idxParamUI].Set(D.UI[D.idxParamUI].GetD() / (1.0 + 1.0 / 16.0));
    if (key == GLUT_KEY_RIGHT) D.UI[D.idxParamUI].Set(D.UI[D.idxParamUI].GetD() * (1.0 + 1.0 / 16.0));
  }
  else if (glutGetModifiers() & GLUT_ACTIVE_ALT) {
    if (key == GLUT_KEY_LEFT) D.UI[D.idxParamUI].Set(D.UI[D.idxParamUI].GetD() - 1.0 / 16.0);
    if (key == GLUT_KEY_RIGHT) D.UI[D.idxParamUI].Set(D.UI[D.idxParamUI].GetD() + 1.0 / 16.0);
  }
  else {
    if (key == GLUT_KEY_LEFT) D.UI[D.idxParamUI].Set(D.UI[D.idxParamUI].GetD() - 1.0);
    if (key == GLUT_KEY_RIGHT) D.UI[D.idxParamUI].Set(D.UI[D.idxParamUI].GetD() + 1.0);
  }

  // Compute refresh
  if (D.autoRefresh)
    project_Refresh();

  glutPostRedisplay();
}


// Mouse click interruption callback
void callback_mouse_click(int button, int state, int x, int y) {
  cam->setCurrentMousePos(float(x), float(y));

  if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON) cam->beginRotate();
  if (state == GLUT_UP && button == GLUT_LEFT_BUTTON) cam->endRotate();
  if (state == GLUT_DOWN && button == GLUT_RIGHT_BUTTON) cam->beginZoom();
  if (state == GLUT_UP && button == GLUT_RIGHT_BUTTON) cam->endZoom();
  if (state == GLUT_DOWN && button == GLUT_MIDDLE_BUTTON) cam->beginPan();
  if (state == GLUT_UP && button == GLUT_MIDDLE_BUTTON) cam->endPan();

  if (state == GLUT_UP && (button == 3 || button == 4)) {
    if (!D.UI.empty()) {
      if (x < (paramLabelNbChar + paramSpaceNbChar + paramValNbChar) * charWidth) {
        if ((y - 3) > pixelMargin && (y - 3) < int(D.UI.size()) * (charHeight + pixelMargin)) {
          if (button == 3) {  // Mouse wheel up
            if (D.idxCursorUI < paramValSignNbChar) D.UI[D.idxParamUI].Set(-D.UI[D.idxParamUI].GetD());
            if (D.idxCursorUI >= paramValSignNbChar && D.idxCursorUI < paramValSignNbChar + paramValInteNbChar)
              D.UI[D.idxParamUI].Set(D.UI[D.idxParamUI].GetD() + std::pow(10.0, double(paramValInteNbChar - D.idxCursorUI)));
            if (D.idxCursorUI >= paramValSignNbChar + paramValInteNbChar + paramValSepaNbChar && D.idxCursorUI < paramValNbChar)
              D.UI[D.idxParamUI].Set(D.UI[D.idxParamUI].GetD() + std::pow(10.0, double(paramValInteNbChar + paramValSepaNbChar - D.idxCursorUI)));
          }
          if (button == 4) {  // Mouse wheel down
            if (D.idxCursorUI < paramValSignNbChar) D.UI[D.idxParamUI].Set(-D.UI[D.idxParamUI].GetD());
            if (D.idxCursorUI >= paramValSignNbChar && D.idxCursorUI < paramValSignNbChar + paramValInteNbChar)
              D.UI[D.idxParamUI].Set(D.UI[D.idxParamUI].GetD() - std::pow(10.0, double(paramValInteNbChar - D.idxCursorUI)));
            if (D.idxCursorUI >= paramValSignNbChar + paramValInteNbChar + paramValSepaNbChar && D.idxCursorUI < paramValNbChar)
              D.UI[D.idxParamUI].Set(D.UI[D.idxParamUI].GetD() - std::pow(10.0, double(paramValInteNbChar + paramValSepaNbChar - D.idxCursorUI)));
          }

          // Compute refresh
          if (D.autoRefresh)
            project_Refresh();
        }
      }
    }
  }

  glutPostRedisplay();
}


// Mouse active motion interruption callback
void callback_mouse_motion(int x, int y) {
  cam->setCurrentMousePos(float(x), float(y));

  glutPostRedisplay();
}


// Mouse passive motion interruption callback
void callback_passive_mouse_motion(int x, int y) {
  const int prevParamIdx= D.idxParamUI;
  const int prevCursorIdx= D.idxCursorUI;
  if (x < (paramLabelNbChar + paramSpaceNbChar + paramValNbChar) * charWidth) {
    if ((y - 3) > pixelMargin && (y - 3) < int(D.UI.size()) * (charHeight + pixelMargin)) {
      D.idxParamUI= (y - 3) / (charHeight + pixelMargin);
      D.idxCursorUI= std::max((x - (paramLabelNbChar + paramSpaceNbChar) * charWidth) / charWidth, 0);
    }
  }

  if (D.idxParamUI != prevParamIdx || D.idxCursorUI != prevCursorIdx)
    glutPostRedisplay();
}


// Menu activation callback
void callback_menu_status(int status, int x, int y) {
  (void)x;  // Disable warning unused variable
  (void)y;  // Disable warning unused variable
  if (status == GLUT_MENU_IN_USE) isMenuActive= true;
  if (status == GLUT_MENU_NOT_IN_USE) isMenuActive= false;
}


// Menu selection callback
void callback_menu(int num) {
  // Reset or activate the selected project
  if (num > ProjectID::AaaaaaaaaaaaaaID && num < ProjectID::ZzzzzzzzzzzzzzID) {
    currentProjectID= num;
    project_ForceHardInit();
  }
  // Toggle dark mode display
  if (num == -1) {
    isDarkMode= !isDarkMode;
    if (isDarkMode) glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    else glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  }
  // Toggle smooth drawing option
  if (num == -2) {
    isSmoothDraw= !isSmoothDraw;
    if (isSmoothDraw) {
      glEnable(GL_POINT_SMOOTH);
      glEnable(GL_LINE_SMOOTH);
    }
    else {
      glDisable(GL_POINT_SMOOTH);
      glDisable(GL_LINE_SMOOTH);
    }
  }
  // Save sandbox settings
  if (num == -3) {
    winPosW= glutGet((GLenum)GLUT_WINDOW_X);
    winPosH= glutGet((GLenum)GLUT_WINDOW_Y);
    saveConfigSandbox();
  }
  // Save project parameters
  if (num == -4) {
    saveConfigProject();
  }
  if (num == -5) {
    project_QueueSoftRefresh();
  }

  // Compute refresh
  if (D.autoRefresh)
    project_Refresh();

  glutPostRedisplay();
}


// Menu initialization
void init_menu() {
  // Create menu tree starting from leaves
  const int menuDisplay= glutCreateMenu(callback_menu);
  glutAddMenuEntry("Dark mode", -1);
  glutAddMenuEntry("Smooth draw", -2);
  const int menuProject= glutCreateMenu(callback_menu);
  glutAddMenuEntry("AgentSwarmBoid", ProjectID::AgentSwarmBoidID);
  glutAddMenuEntry("CompuFluidDyna", ProjectID::CompuFluidDynaID);
  glutAddMenuEntry("FractalCurvDev", ProjectID::FractalCurvDevID);
  glutAddMenuEntry("FractalElevMap", ProjectID::FractalElevMapID);
  glutAddMenuEntry("MarkovProcGene", ProjectID::MarkovProcGeneID);
  glutAddMenuEntry("MassSpringSyst", ProjectID::MassSpringSystID);
  glutAddMenuEntry("PosiBasedDynam", ProjectID::PosiBasedDynamID);
  glutAddMenuEntry("SpaceTimeWorld", ProjectID::SpaceTimeWorldID);
  glutAddMenuEntry("StringArtOptim", ProjectID::StringArtOptimID);
  glutAddMenuEntry("TerrainErosion", ProjectID::TerrainErosionID);
  const int menuSave= glutCreateMenu(callback_menu);
  glutAddMenuEntry("Save settings", -3);
  glutAddMenuEntry("Save parameters", -4);
  glutAddMenuEntry("Queue refresh", -5);
  glutCreateMenu(callback_menu);
  glutAddSubMenu("Display", menuDisplay);
  glutAddSubMenu("Project", menuProject);
  glutAddSubMenu("Save", menuSave);

  // Add menu status callback and set flags
  glutMenuStatusFunc(&callback_menu_status);
  glutAttachMenu(GLUT_RIGHT_BUTTON);
  isMenuAttached= false;
  isMenuActive= true;
}


// OpenGL initialization
void init_GL() {
  // Set background color
  isDarkMode= true;
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

  // Define light properties
  constexpr float global_ambient[]= {1.0f, 1.0f, 1.0f, 1.0f};
  constexpr float light0_ambient[]= {0.2f, 0.2f, 0.2f, 1.0f};
  constexpr float light0_diffuse[]= {0.8f, 0.8f, 0.8f, 1.0f};
  constexpr float light0_specular[]= {1.0f, 1.0f, 1.0f, 1.0f};
  glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

  // Position lights
  constexpr GLfloat position0[4]= {4.0f, 4.0f, 4.0f, 1.0f};
  glLightfv(GL_LIGHT0, GL_POSITION, position0);

  // Enable lights
  glEnable(GL_LIGHT0);

  // Define material shader properties
  constexpr GLfloat mat_ambiant[4]= {0.1f, 0.1f, 0.1f, 1.0f};
  constexpr GLfloat mat_diffuse[4]= {0.8f, 0.8f, 0.8f, 1.0f};
  constexpr GLfloat mat_specular[4]= {0.3f, 0.3f, 0.3f, 0.3f};
  // constexpr GLfloat mat_specular[4]= {1.0f, 1.0f, 1.0f, 1.0f};
  constexpr GLfloat mat_shininess[1]= {64.0f};
  // GLfloat mat_shininess[1]= {128.0f};
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambiant);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);

  // Misc
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_NORMALIZE);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glShadeModel(GL_SMOOTH);
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_COLOR_MATERIAL);
  isSmoothDraw= true;
  glEnable(GL_POINT_SMOOTH);
  glEnable(GL_LINE_SMOOTH);
}


// Scene initialization
void init_scene() {
  // Initialize pseudo random number generator
  srand(time(0));

  // Initialize camera and arcball positions
  cam= new Camera;
  cam->setEye(2.5f, 0.5f, 0.5f);
  cam->setCenter(0.5f, 0.5f, 0.5f);
}


// Main function
int main(int argc, char *argv[]) {
  // Load window settings or use default values
  winW= 1400;
  winH= 900;
  winPosW= -1;
  winPosH= -1;
  loadConfigSandbox();

  // Window creation
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(winW, winH);
  glutInitWindowPosition(winPosW, winPosH);
  windowID= glutCreateWindow("Sandbox");

  // World initialization
  init_menu();
  init_GL();
  init_scene();

  // Callbacks affectation
  glutDisplayFunc(&callback_display);
  glutReshapeFunc(&callback_reshape);
  glutKeyboardFunc(&callback_keyboard);
  glutSpecialFunc(&callback_keyboard_special);
  glutMouseFunc(&callback_mouse_click);
  glutMotionFunc(&callback_mouse_motion);
  glutPassiveMotionFunc(&callback_passive_mouse_motion);
  glutTimerFunc(100, &callback_timer, 0);

  // Compute refresh
  currentProjectID= 0;
  loadConfigProject();
  project_ForceHardInit();
  loadConfigProject();
  project_Refresh();

  // Start refresh loop
  glutMainLoop();

  return 0;
}
