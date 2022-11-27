
// Standard lib
#include <cstdio>
#include <ctime>

// GLUT lib
#include <GL/freeglut.h>

// Project lib
#include "Data.hpp"
#include "SpaceTimeWorld.hpp"
#include "flock.hpp"
#include "math/Vec.hpp"
#include "tb/Camera.hpp"


// Global variables used for the display
unsigned int winFPS= 60;
int winW, winH;
Camera *cam;

// Global variables used by the scene
Data D;
// Flock myFlock(100, 0.05);
SpaceTimeWorld mySpaceTimeWorld;

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


void draw_text(int const x, int const y, int const characterSize, char *const text) {
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
  glLineWidth(6.0f);
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
  glPointSize(15.0f);
  glBegin(GL_POINTS);
  glColor3f(1.0, 0.0, 0.0);
  glVertex3f(1.0f, 0.0f, 0.0f);
  glColor3f(0.0, 1.0, 0.0);
  glVertex3f(0.0f, 1.0f, 0.0f);
  glColor3f(0.0, 0.0, 1.0);
  glVertex3f(0.0f, 0.0f, 1.0f);
  glEnd();
  glPointSize(1.0f);


  // Draw stuff in the scene
  // myFlock.draw();
  mySpaceTimeWorld.draw();

  // Set the camera transformation matrix for the HUD
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, double(winW), 0.0, double(winH), -1.0, 1.0);

  // Set the world transformation matrix for the HUD
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // Draw the HUD
  glLineWidth(2.0f);
  glColor3f(0.8f, 0.8f, 0.8f);
  char str[50];
  int characterSize= 15;
  for (unsigned int k= 0; k < D.param.size(); k++) {
    sprintf(str, "%s = %4.6f", D.param[k].name.c_str(), D.param[k].val);
    draw_text(0, winH - (k + 1) * (characterSize+2), characterSize, str);
  }
  glColor3f(0.8f, 0.3f, 0.3f);
  sprintf(str, "______________");
  draw_text(0, winH - (D.idxParamUI + 1) * (characterSize+2)-3, characterSize, str);
  sprintf(str, "______________");
  draw_text(0, winH - (D.idxParamUI) * (characterSize+2), characterSize, str);

  sprintf(str, "%.3f s", elapsed_time());
  draw_text(0, 2, characterSize, str);
  glLineWidth(1.0f);

  // Commit the draw
  glutSwapBuffers();
}


// Timer program interruption callback
void callback_timer(int v) {
  if (D.playAnimation) {
    // Compute animations
    // myFlock.animate(0.02f);
    // mySpaceTimeWorld.animate(0.02f);

    // Refresh display and set timer for next frame
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

  if (key == 27) {
    exit(EXIT_SUCCESS);
  }
  else if (key == ' ') {
    D.playAnimation= !D.playAnimation;
  }
  else if (key == '1') {
    D.showWorld= !D.showWorld;
  }
  else if (key == '2') {
    D.showScreen= !D.showScreen;
  }
  else if (key == '3') {
    D.showPhotonPath= !D.showPhotonPath;
  }
  else if (key == '4') {
    D.showGravity= !D.showGravity;
  }

  else if (key == 'r') {
    mySpaceTimeWorld= SpaceTimeWorld();
  }

  glutPostRedisplay();
}


// Keyboard special interruption callback
void callback_keyboard_special(int key, int x, int y) {
  (void)x;  // Disable warning unused variable
  (void)y;  // Disable warning unused variable

  if (key == GLUT_KEY_UP)
    D.idxParamUI= (int(D.param.size()) + D.idxParamUI - 1) % int(D.param.size());
  else if (key == GLUT_KEY_DOWN)
    D.idxParamUI= (D.idxParamUI + 1) % int(D.param.size());

  else if (key == GLUT_KEY_LEFT && (glutGetModifiers() & GLUT_ACTIVE_SHIFT))
    D.param[D.idxParamUI].val/= 10.0;
  else if (key == GLUT_KEY_LEFT && (glutGetModifiers() & GLUT_ACTIVE_CTRL))
    D.param[D.idxParamUI].val/= 2.0;
  else if (key == GLUT_KEY_LEFT && (glutGetModifiers() & GLUT_ACTIVE_ALT))
    D.param[D.idxParamUI].val-= 10.0;
  else if (key == GLUT_KEY_LEFT)
    D.param[D.idxParamUI].val-= 1.0;

  else if (key == GLUT_KEY_RIGHT && (glutGetModifiers() & GLUT_ACTIVE_SHIFT))
    D.param[D.idxParamUI].val*= 10.0;
  else if (key == GLUT_KEY_RIGHT && (glutGetModifiers() & GLUT_ACTIVE_CTRL))
    D.param[D.idxParamUI].val*= 2.0;
  else if (key == GLUT_KEY_RIGHT && (glutGetModifiers() & GLUT_ACTIVE_ALT))
    D.param[D.idxParamUI].val+= 10.0;
  else if (key == GLUT_KEY_RIGHT)
    D.param[D.idxParamUI].val+= 1.0;

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


// // Idle program interruption callback
// void callback_idle() {
//   glutPostRedisplay();
// }


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
  // glEnable(GL_POINT_SMOOTH);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_NORMALIZE);
  glEnable(GL_DEPTH_TEST);
  // glEnable(GL_LINE_SMOOTH);
  glDepthFunc(GL_LEQUAL);
  glShadeModel(GL_SMOOTH);
}


// Scene initialization
void init_scene() {
  // Initialize pseudo random number generator
  srand(time(0));

  // Initialize camera and arcball
  cam= new Camera;
  cam->setEye(2.0f, 2.5f, 1.5f);
  cam->setCenter(0.5f, 0.5f, 0.5f);
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
  // glutIdleFunc(&callback_idle);
  glutTimerFunc(100, callback_timer, 0);

  // Start refresh loop
  glutMainLoop();

  return 0;
}
