#include "draw.hpp"

// GLUT lib
#include <GL/freeglut.h>

// Draws the space origin
void draw_base(Vector o, Vector x, Vector y, Vector z) {
  glLineWidth(2);
  glBegin(GL_LINES);
  glColor3f(1.0, 0.0, 0.0);
  glVertex3f(o.x, o.y, o.z);
  glVertex3f(o.x + x.x, o.y + x.y, o.z + x.z);
  glColor3f(0.0, 1.0, 0.0);
  glVertex3f(o.x, o.y, o.z);
  glVertex3f(o.x + y.x, o.y + y.y, o.z + y.z);
  glColor3f(0.0, 0.0, 1.0);
  glVertex3f(o.x, o.y, o.z);
  glVertex3f(o.x + z.x, o.y + z.y, o.z + z.z);
  glEnd();
  glLineWidth(1);
}

// Draws a boid
void draw_boid(Vector front, Vector left, Vector up, Vector o, float size) {
  Vector p1, p2, p3, p4, p5, p6, p7;

  p1 = o + size * (0.5 * front + 0.0 * left + 0.0 * up);
  p2 = o + size * (0.0 * front - 0.07 * left + 0.1 * up);
  p3 = o + size * (0.0 * front + 0.07 * left + 0.1 * up);
  p4 = o + size * (0.0 * front + 0.0 * left - 0.1 * up);
  p5 = o + size * (-0.4 * front + 0.0 * left + 0.0 * up);
  p6 = o + size * (-0.6 * front + 0.0 * left - 0.2 * up);
  p7 = o + size * (-0.6 * front + 0.0 * left + 0.2 * up);

  glBegin(GL_TRIANGLES);
  glColor3f(1.0, 0.0, 1.0);
  glVertex3f(p1.x, p1.y, p1.z);
  glColor3f(1.0, 0.0, 0.5);
  glVertex3f(p2.x, p2.y, p2.z);
  glColor3f(1.0, 0.0, 0.5);
  glVertex3f(p3.x, p3.y, p3.z);

  glColor3f(0.0, 0.0, 1.0);
  glVertex3f(p1.x, p1.y, p1.z);
  glColor3f(0.0, 0.0, 0.5);
  glVertex3f(p4.x, p4.y, p4.z);
  glColor3f(0.0, 0.0, 0.5);
  glVertex3f(p2.x, p2.y, p2.z);

  glColor3f(0.0, 0.0, 1.0);
  glVertex3f(p1.x, p1.y, p1.z);
  glColor3f(0.0, 0.0, 0.5);
  glVertex3f(p3.x, p3.y, p3.z);
  glColor3f(0.0, 0.0, 0.5);
  glVertex3f(p4.x, p4.y, p4.z);

  glColor3f(0.0, 0.0, 0.2);
  glVertex3f(p2.x, p2.y, p2.z);
  glColor3f(0.0, 0.0, 0.2);
  glVertex3f(p4.x, p4.y, p4.z);
  glColor3f(0.0, 0.0, 0.2);
  glVertex3f(p5.x, p5.y, p5.z);

  glColor3f(0.0, 0.0, 0.2);
  glVertex3f(p3.x, p3.y, p3.z);
  glColor3f(0.0, 0.0, 0.2);
  glVertex3f(p2.x, p2.y, p2.z);
  glColor3f(0.0, 0.0, 0.2);
  glVertex3f(p5.x, p5.y, p5.z);

  glColor3f(0.0, 0.0, 0.2);
  glVertex3f(p3.x, p3.y, p3.z);
  glColor3f(0.0, 0.0, 0.2);
  glVertex3f(p5.x, p5.y, p5.z);
  glColor3f(0.0, 0.0, 0.2);
  glVertex3f(p4.x, p4.y, p4.z);

  glColor3f(0.0, 0.0, 0.2);
  glVertex3f(p5.x, p5.y, p5.z);
  glColor3f(0.0, 1.0, 1.0);
  glVertex3f(p6.x, p6.y, p6.z);
  glColor3f(0.0, 1.0, 1.0);
  glVertex3f(p7.x, p7.y, p7.z);
  glEnd();
}

// Draws a line
void draw_line(Vector p1, Vector p2) {
  glBegin(GL_LINES);
  glColor3f(0.0, 0.0, 1.0);
  glVertex3f(p1.x, p1.y, p1.z);
  glNormal3f(0, 1, 0);
  glColor3f(1.0, 0.0, 0.0);
  glVertex3f(p2.x, p2.y, p2.z);
  glNormal3f(0, 1, 0);
  glEnd();
}

// Draws a point
void draw_point(Vector p1, float r, float g, float b) {
  glColor3f(r, g, b);
  glPointSize(10.0f);
  glBegin(GL_POINTS);
  glVertex3f(p1.x, p1.y, p1.z);
  glEnd();
  glPointSize(1);
}

// Draws a box
void draw_box(Vector p1, Vector p2, float r, float g, float b) {
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glColor3f(r, g, b);
  glBegin(GL_QUADS);
  glVertex3f(p1.x, p1.y, p1.z);
  glVertex3f(p2.x, p1.y, p1.z);
  glVertex3f(p2.x, p2.y, p1.z);
  glVertex3f(p1.x, p2.y, p1.z);

  glVertex3f(p1.x, p1.y, p2.z);
  glVertex3f(p2.x, p1.y, p2.z);
  glVertex3f(p2.x, p2.y, p2.z);
  glVertex3f(p1.x, p2.y, p2.z);

  glVertex3f(p1.x, p1.y, p1.z);
  glVertex3f(p1.x, p2.y, p1.z);
  glVertex3f(p1.x, p2.y, p2.z);
  glVertex3f(p1.x, p1.y, p2.z);

  glVertex3f(p2.x, p1.y, p1.z);
  glVertex3f(p2.x, p2.y, p1.z);
  glVertex3f(p2.x, p2.y, p2.z);
  glVertex3f(p2.x, p1.y, p2.z);

  glVertex3f(p1.x, p1.y, p1.z);
  glVertex3f(p2.x, p1.y, p1.z);
  glVertex3f(p2.x, p1.y, p2.z);
  glVertex3f(p1.x, p1.y, p2.z);

  glVertex3f(p1.x, p2.y, p1.z);
  glVertex3f(p2.x, p2.y, p1.z);
  glVertex3f(p2.x, p2.y, p2.z);
  glVertex3f(p1.x, p2.y, p2.z);
  glEnd();
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
