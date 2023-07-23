#include "ProjectTemplate.hpp"


// Standard lib
#include <cstdio>
#include <vector>

// GLUT lib
#include <GL/freeglut.h>

// Project lib
#include "../Data.hpp"
#include "../math/Vectors.hpp"

extern Data D;


ProjectTemplate::ProjectTemplate() {
  isInitialized= false;
  isRefreshed= false;
}


void ProjectTemplate::Init() {
  isInitialized= true;
  isRefreshed= false;
}


void ProjectTemplate::Refresh() {
  if (!isInitialized) return;
  isRefreshed= true;
}


void ProjectTemplate::Animate() {
  if (!isInitialized) return;
  if (!isRefreshed) return;
}


void ProjectTemplate::Draw() {
  if (!isInitialized) return;
  if (!isRefreshed) return;
}
