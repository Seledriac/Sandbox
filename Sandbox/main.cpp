
// Standard lib
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// GLUT lib
#include <GL/freeglut.h>

// Project lib
#include "tb/Camera.hpp"
#include "draw.hpp"
#include "flock.hpp"


// Global variables used for the display
Camera *cam;


// Global variables used by the scene
Flock myFlock(500, 0.1);

// Returns the elapsed time since its last call
float elapsed_time() {
  static long last_time= -1;
  long current_time;
  float t= 0;

  current_time= glutGet(GLUT_ELAPSED_TIME);
  if(last_time == -1) {
    last_time= glutGet(GLUT_ELAPSED_TIME);
  }
  else {
    t += (float)(current_time-last_time) / 1000.0f;
    last_time= current_time;
  }
  
  return t;
}


// Display callback
void callback_display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glMultMatrixf(cam->getViewMatrix());
    
  myFlock.animate(elapsed_time());
  myFlock.draw();
  
  draw_base(Vector(0, 0, 0), Vector(1, 0, 0), Vector(0, 1, 0), Vector(0, 0, 1));
  draw_box(Vector(-0.5, -0.5, -0.5), Vector(0.5, 0.5, 0.5), 0.0, 0.0, 0.0);

  glutSwapBuffers();
}


// Window reshape callback
void callback_reshape(int width, int height) {
  cam->setWindowSize(float(width), float(height));

  glViewport(0, 0, width, height);
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0, double(width)/double(height), 0.1, 100.0);
  
  glutPostRedisplay();
}


// Keyboard interruption callback
void callback_keyboard(unsigned char key, int x, int y) {
  (void) x; // Disable warning unused variable
  (void) y; // Disable warning unused variable
  
  if (key == 27) {
    exit(EXIT_SUCCESS);
  }
  else if (key == 'f') {
    myFlock.food.x= float(rand())/float(RAND_MAX) - 0.5f;
    myFlock.food.z= float(rand())/float(RAND_MAX) - 0.5f;
  }
  else if (key == 'p') {
    myFlock.predator.z= float(rand())/float(RAND_MAX) - 0.5f;
    myFlock.predator.x= float(rand())/float(RAND_MAX) - 0.5f;
  }

  glutPostRedisplay();
}


// Mouse click interruption callback
void callback_mouse_click(int button, int state, int x, int y) {
  cam->setCurrentMousePos(float(x), float(y));
  
  if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON) cam->beginRotate();
  else if (state == GLUT_UP && button == GLUT_LEFT_BUTTON) cam->endRotate();
  else if (state == GLUT_DOWN && button == GLUT_RIGHT_BUTTON) cam->beginZoom();
  else if (state == GLUT_UP && button == GLUT_RIGHT_BUTTON) cam->endZoom();
  else if (state == GLUT_DOWN && button == GLUT_MIDDLE_BUTTON) cam->beginPan();
  else if (state == GLUT_UP && button == GLUT_MIDDLE_BUTTON) cam->endPan();
  else if (state == GLUT_UP && button == 3) cam->zoom(20.0f);
  else if (state == GLUT_UP && button == 4) cam->zoom(-20.0f);

  glutPostRedisplay();
}


// Mouse motion interruption callback
void callback_mouse_motion(int x, int y) {
  cam->setCurrentMousePos(float(x), float(y));
  glutPostRedisplay();
}


// Idle program interruption callback
GLvoid callback_idle() {
  glutPostRedisplay();
}


// OpenGL initialization
void init_GL() {

  // Set background color
  glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
    
  // Define light properties
  float global_ambient []= {1.0f, 1.0f, 1.0f, 1.0f};
  float light0_ambient []= {0.2f, 0.2f, 0.2f, 1.0f};
  float light0_diffuse []= {0.8f, 0.8f, 0.8f, 1.0f};
  float light0_specular[]= {1.0f, 1.0f, 1.0f, 1.0f};
  glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
  
  // Position lights
  GLfloat position0[4]= {4.0f, 4.0f, 4.0f, 1.0f};
  glLightfv (GL_LIGHT0, GL_POSITION, position0);
  
  // Enable lights
  glEnable(GL_LIGHT0);
  
  // Define material shader properties
  GLfloat mat_ambiant     [4]= {0.1f, 0.1f, 0.1f, 1.0f};
  GLfloat mat_diffuse     [4]= {0.8f, 0.8f, 0.8f, 1.0f};
  GLfloat mat_specular     [4]= {1.0f, 1.0f, 1.0f, 1.0f};
  GLfloat mat_shininess   [1]= {128.0f};
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
}


// Scene initialization
void init_scene() {

  // Initialize pseudo random number generator
  srand(time(0));
  
  // Initialize camera and arcball
  cam= new Camera;
  cam->setEye(2.0f, 1.0f, 1.0f);
  cam->setCenter(0.0f, 0.0f, 0.0f);
}


// Main function
int main(int argc, char *argv[]) {
  
  // Window creation
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(800, 600);
  glutInitWindowPosition(500, 200);
  glutCreateWindow("Display");

  // World initialization
  init_GL();
  init_scene();

  // Callbacks affectation
  glutDisplayFunc(&callback_display);
  glutReshapeFunc(&callback_reshape);
  glutKeyboardFunc(&callback_keyboard);
  glutMouseFunc(&callback_mouse_click);
  glutMotionFunc(&callback_mouse_motion);
  glutIdleFunc(&callback_idle);

  // Start refresh loop
  glutMainLoop();

  return 0;
}
