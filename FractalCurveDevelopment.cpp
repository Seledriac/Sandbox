#include "FractalCurveDevelopment.hpp"


// Standard lib
#include <cmath>
#include <cstdio>
#include <vector>

// GLUT lib
#include <GL/freeglut.h>

// Project lib
#include "Data.hpp"
#include "SrtColormap.hpp"
#include "math/Util.hpp"
#include "math/Vectors.hpp"


extern Data D;


FractalCurveDevelopment::FractalCurveDevelopment() {
  isInitialized= false;
}


void FractalCurveDevelopment::Init() {
  isInitialized= true;

  Nodes.clear();

  int maxDepth= int(std::round(D.param[testVar0____________].val));
  if (maxDepth < 1) return;

  Nodes.push_back(std::vector<Math::Vec3>());
  Nodes[0].push_back(Math::Vec3(0.0, 0.0, 0.0));
  Nodes[0].push_back(Math::Vec3(0.0, 1.0, 0.0));

  for (int idxDepth= 1; idxDepth < maxDepth; idxDepth++) {
    Nodes.push_back(std::vector<Math::Vec3>());
    for (int idxNode= 0; idxNode < int(Nodes[idxDepth - 1].size()) - 1; idxNode++) {
      Math::Vec3 posA= Nodes[idxDepth - 1][idxNode];
      Math::Vec3 posB= Nodes[idxDepth - 1][idxNode + 1];

      Math::Vec3 posM= (posA + posB) / 2.0 + 0.4 * (posB - posA).norm() * (posB - posA).cross(Math::Vec3(0.0, 0.0, 1.0)).normalized();

      Nodes[idxDepth].push_back(Math::Vec3(0.0, 0.0, 0.1) + posA);
      Nodes[idxDepth].push_back(Math::Vec3(0.0, 0.0, 0.1) + posA + (posB - posA) * 0.25);
      // Nodes[idxDepth].push_back(Math::Vec3(0.0, 0.0, 0.1) + posA + (posB - posA) * 0.50 + Math::Vec3(0.3, 0.0, 0.0));
      Nodes[idxDepth].push_back(Math::Vec3(0.0, 0.0, 0.1) + posM);
      Nodes[idxDepth].push_back(Math::Vec3(0.0, 0.0, 0.1) + posA + (posB - posA) * 0.75);

      if (idxNode == int(Nodes[idxDepth - 1].size()) - 2) {
        Nodes[idxDepth].push_back(Math::Vec3(0.0, 0.0, 0.1) + posB);
      }
    }
  }
}


void FractalCurveDevelopment::Draw() {
  if (!isInitialized) return;

  if (D.showNodes) {
    glPointSize(3.0f);
    glBegin(GL_POINTS);
    for (int idxDepth= 0; idxDepth < int(Nodes.size()); idxDepth++) {
      double r, g, b;
      SrtColormap::RatioToJetBrightSmooth(double(idxDepth) / double(Nodes.size() - 1), r, g, b);
      glColor3d(r, g, b);
      for (int idxNode= 0; idxNode < int(Nodes[idxDepth].size()); idxNode++) {
        glVertex3dv(Nodes[idxDepth][idxNode].array());
      }
    }
    glEnd();
    glPointSize(1.0f);
  }

  if (D.showEdges) {
    glLineWidth(3.0f);
    for (int idxDepth= 0; idxDepth < int(Nodes.size()); idxDepth++) {
      double r, g, b;
      SrtColormap::RatioToJetBrightSmooth(double(idxDepth) / double(Nodes.size() - 1), r, g, b);
      glBegin(GL_LINE_STRIP);
      glColor3d(r, g, b);
      for (int idxNode= 0; idxNode < int(Nodes[idxDepth].size()); idxNode++) {
        glVertex3dv(Nodes[idxDepth][idxNode].array());
      }
      glEnd();
    }
    glLineWidth(1.0f);
  }

  if (D.showFaces) {
  }
}
