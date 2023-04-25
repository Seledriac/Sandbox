
// Standard lib
#include <cstdio>
#include <ctime>

// GLUT lib
#include <GL/freeglut.h>

// Project Data
#include "Data.hpp"

// Project Utilities
#include "math/Vectors.hpp"
#include "tb/Camera.hpp"
#include "util/SrtColormap.hpp"

// Project Sandbox Classes
#include "AgentSwarm.hpp"
#include "FractalCurveDevelopment.hpp"
#include "ParticleSystem.hpp"
#include "SpaceTimeWorld.hpp"


// Global variables used for the display
unsigned int winFPS= 60;
int winW, winH;
int characterSize= 13;
Camera *cam;

// Global variables used by the scene
Data D;
AgentSwarm myAgentSwarm;
SpaceTimeWorld mySpaceTimeWorld;
ParticleSystem myParticleSystem;
FractalCurveDevelopment myFractalCurveDevelopment;

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


void draw_text(int const x, int const y, char *const text) {
  glPushMatrix();
  glTranslatef(float(x), float(y), 0.0f);
  glScalef(float(characterSize) / 152.38f, float(characterSize) / 152.38f, float(characterSize) / 152.38f);
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

  // Draw the reference frame
  if (D.showAxis) {
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
  }

  // Draw stuff in the scene
  myAgentSwarm.Draw();
  mySpaceTimeWorld.Draw();
  myFractalCurveDevelopment.Draw();
  myParticleSystem.Draw();

  // Set the camera transformation matrix for the HUD
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, double(winW), 0.0, double(winH), -1.0, 1.0);

  // Set the world transformation matrix for the HUD
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // Draw the parameter list
  for (unsigned int k= 0; k < D.param.size(); k++) {
    if (k == D.idxParamUI)
      glColor3f(0.8f, 0.4f, 0.4f);
    else
      glColor3f(0.8f, 0.8f, 0.8f);
    char str[50];
    sprintf(str, "%s = %4.6f", D.param[k].name.c_str(), D.param[k].val);
    draw_text(0, winH - (k + 1) * (characterSize + 2), str);
  }

  // Draw the 2D plot
  {
    int plotW= 400;
    int plotH= 100;
    int textW= 80;
    int textH= 12;
    for (unsigned int k0= 0; k0 < D.plotData.size(); k0++) {
      if (D.plotData[k0].second.empty()) continue;
      // Set the color
      float r, g, b;
      SrtColormap::RatioToJetBrightSmooth(float(k0) / float(D.plotData.size() - 1), r, g, b);
      glColor3f(r, g, b);
      // Find the min max range for vertical scaling
      double valMin= D.plotData[k0].second[0];
      double valMax= D.plotData[k0].second[0];
      for (unsigned int k1= 0; k1 < D.plotData[k0].second.size(); k1++) {
        if (valMin > D.plotData[k0].second[k1]) valMin= D.plotData[k0].second[k1];
        if (valMax < D.plotData[k0].second[k1]) valMax= D.plotData[k0].second[k1];
      }
      // Draw the text for legend and min max values
      char str[50];
      strcpy(str, D.plotData[k0].first.c_str());
      draw_text(winW - textW - plotW + k0 * textW, winH - textH, str);
      sprintf(str, "%+.2e", valMax);
      draw_text(winW - textW - plotW + k0 * textW, winH - 2 * textH, str);
      sprintf(str, "%+.2e", valMin);
      draw_text(winW - textW - plotW + k0 * textW, winH - plotH - 3 * textH, str);
      sprintf(str, "%+.2e", D.plotData[k0].second[D.plotData[k0].second.size() - 1]);
      draw_text(winW - textW, winH - textH - textH * k0, str);
      // Draw the polyline
      glBegin(GL_LINE_STRIP);
      for (unsigned int k1= 0; k1 < D.plotData[k0].second.size(); k1++) {
        double valScaled= (D.plotData[k0].second[k1] - valMin) / (valMax - valMin);
        glVertex3i(winW - plotW - textW + plotW * k1 / D.plotData[k0].second.size(), winH - plotH - 2 * textH + plotH * valScaled, 0);
      }
      glEnd();
    }
  }

  // Draw the frame time
  {
    glColor3f(0.8f, 0.8f, 0.8f);
    char str[50];
    sprintf(str, "%.3f s", elapsed_time());
    draw_text(0, 2, str);
  }

  // Commit the draw
  glutSwapBuffers();
}


// Timer program interruption callback
void callback_timer(int v) {
  // Compute animations
  if (D.playAnimation) {
    myAgentSwarm.Animate();
    myParticleSystem.Animate();

    glutPostRedisplay();
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

  if (key == 27)
    exit(EXIT_SUCCESS);
  else if (key == ' ')
    D.playAnimation= !D.playAnimation;
  else if (key == 13)
    D.autoRefresh= !D.autoRefresh;
  else if (key == '1')
    D.showWorld= !D.showWorld;
  else if (key == '2')
    D.showScreen= !D.showScreen;
  else if (key == '3')
    D.showPhotonPath= !D.showPhotonPath;
  else if (key == '4')
    D.showCursor= !D.showCursor;
  else if (key == '5')
    D.showFlow= !D.showFlow;

  else if (key == 'a')
    D.showAxis= !D.showAxis;
  else if (key == 'q')
    D.plotData.clear();

  else if (key == 'R')
    mySpaceTimeWorld= SpaceTimeWorld();
  else if (key == 'r')
    mySpaceTimeWorld.Init();
  else if (key == 'F')
    myFractalCurveDevelopment= FractalCurveDevelopment();
  else if (key == 'f')
    myFractalCurveDevelopment.Init();
  else if (key == 'P')
    myParticleSystem= ParticleSystem();
  else if (key == 'p')
    myParticleSystem.Init();
  else if (key == 'S')
    myAgentSwarm= AgentSwarm();
  else if (key == 's')
    myAgentSwarm.Init();

  // Compute refresh
  if (D.autoRefresh) {
    mySpaceTimeWorld.Refresh();
  }

  glutPostRedisplay();
}


// Keyboard special interruption callback
void callback_keyboard_special(int key, int x, int y) {
  (void)x;  // Disable warning unused variable
  (void)y;  // Disable warning unused variable

  if (key == GLUT_KEY_UP && (glutGetModifiers() & GLUT_ACTIVE_SHIFT))
    D.idxParamUI= (int(D.param.size()) + D.idxParamUI - 5) % int(D.param.size());
  else if (key == GLUT_KEY_UP)
    D.idxParamUI= (int(D.param.size()) + D.idxParamUI - 1) % int(D.param.size());
  else if (key == GLUT_KEY_DOWN && (glutGetModifiers() & GLUT_ACTIVE_SHIFT))
    D.idxParamUI= (D.idxParamUI + 5) % int(D.param.size());
  else if (key == GLUT_KEY_DOWN)
    D.idxParamUI= (D.idxParamUI + 1) % int(D.param.size());

  else if (key == GLUT_KEY_LEFT && (glutGetModifiers() & GLUT_ACTIVE_SHIFT))
    D.param[D.idxParamUI].val/= 10.0;
  else if (key == GLUT_KEY_LEFT && (glutGetModifiers() & GLUT_ACTIVE_CTRL))
    D.param[D.idxParamUI].val/= 2.0;
  else if (key == GLUT_KEY_LEFT && (glutGetModifiers() & GLUT_ACTIVE_ALT))
    D.param[D.idxParamUI].val-= 0.01;
  else if (key == GLUT_KEY_LEFT)
    D.param[D.idxParamUI].val-= 1.0;

  else if (key == GLUT_KEY_RIGHT && (glutGetModifiers() & GLUT_ACTIVE_SHIFT))
    D.param[D.idxParamUI].val*= 10.0;
  else if (key == GLUT_KEY_RIGHT && (glutGetModifiers() & GLUT_ACTIVE_CTRL))
    D.param[D.idxParamUI].val*= 2.0;
  else if (key == GLUT_KEY_RIGHT && (glutGetModifiers() & GLUT_ACTIVE_ALT))
    D.param[D.idxParamUI].val+= 0.01;
  else if (key == GLUT_KEY_RIGHT)
    D.param[D.idxParamUI].val+= 1.0;

  // Compute refresh
  if (D.autoRefresh && key != GLUT_KEY_UP && key != GLUT_KEY_DOWN) {
    mySpaceTimeWorld.Refresh();
  }

  glutPostRedisplay();
}


// Mouse click interruption callback
void callback_mouse_click(int button, int state, int x, int y) {
  cam->setCurrentMousePos(float(x), float(y));

  if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON)
    cam->beginRotate();
  else if (state == GLUT_UP && button == GLUT_LEFT_BUTTON)
    cam->endRotate();
  else if (state == GLUT_DOWN && button == GLUT_RIGHT_BUTTON)
    cam->beginZoom();
  else if (state == GLUT_UP && button == GLUT_RIGHT_BUTTON)
    cam->endZoom();
  else if (state == GLUT_DOWN && button == GLUT_MIDDLE_BUTTON)
    cam->beginPan();
  else if (state == GLUT_UP && button == GLUT_MIDDLE_BUTTON)
    cam->endPan();
  else if (state == GLUT_UP && button == 3)
    cam->zoom(20.0f);
  else if (state == GLUT_UP && button == 4)
    cam->zoom(-20.0f);

  glutPostRedisplay();
}


// Mouse motion interruption callback
void callback_mouse_motion(int x, int y) {
  cam->setCurrentMousePos(float(x), float(y));

  glutPostRedisplay();
}


// Mouse motion interruption callback
void callback_passive_mouse_motion(int x, int y) {
  (void)x;  // Disable warning unused variable
  (void)y;  // Disable warning unused variable
  unsigned int prevParamIdx= D.idxParamUI;
  // if (x < 180) {
  //   int targetParam= (y - 3) / (characterSize + 2);
  //   if (targetParam >= 0 && targetParam < int(D.param.size()))
  //     D.idxParamUI= targetParam;
  // }

  if (D.idxParamUI != prevParamIdx)
    glutPostRedisplay();
}


// OpenGL initialization
void init_GL() {
  // Set background color
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

  // Define light properties
  float global_ambient[]= {1.0f, 1.0f, 1.0f, 1.0f};
  float light0_ambient[]= {0.2f, 0.2f, 0.2f, 1.0f};
  float light0_diffuse[]= {0.8f, 0.8f, 0.8f, 1.0f};
  float light0_specular[]= {1.0f, 1.0f, 1.0f, 1.0f};
  glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

  // Position lights
  GLfloat position0[4]= {4.0f, 4.0f, 4.0f, 1.0f};
  glLightfv(GL_LIGHT0, GL_POSITION, position0);

  // Enable lights
  glEnable(GL_LIGHT0);

  // Define material shader properties
  GLfloat mat_ambiant[4]= {0.1f, 0.1f, 0.1f, 1.0f};
  GLfloat mat_diffuse[4]= {0.8f, 0.8f, 0.8f, 1.0f};
  GLfloat mat_specular[4]= {1.0f, 1.0f, 1.0f, 1.0f};
  GLfloat mat_shininess[1]= {128.0f};
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
  // glEnable(GL_POINT_SMOOTH);
  // glEnable(GL_LINE_SMOOTH);
}


// Scene initialization
void init_scene() {
  // Initialize pseudo random number generator
  srand(time(0));

  // Initialize camera and arcball
  cam= new Camera;
  cam->setEye(1.5f, 2.0f, 1.0f);
  cam->setCenter(0.0f, 0.0f, 0.0f);
  // cam->setEye(2.0f, 2.5f, 1.5f);
  // cam->setCenter(0.5f, 0.5f, 0.5f);
}


// Main function
int main(int argc, char *argv[]) {
  // Window creation
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(1200, 800);
  glutInitWindowPosition(200, 100);
  glutCreateWindow("Display");

  // World initialization
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
  glutTimerFunc(100, callback_timer, 0);

  // Start refresh loop
  glutMainLoop();

  return 0;
}
