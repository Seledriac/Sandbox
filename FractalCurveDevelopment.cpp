#include "FractalCurveDevelopment.hpp"


// Standard lib
#include <cmath>
#include <cstdio>
#include <fstream>
#include <vector>

// GLUT lib
#include <GL/freeglut.h>

// Project lib
#include "Data.hpp"
#include "SrtColormap.hpp"
#include "math/Util.hpp"
#include "math/Vectors.hpp"


extern Data D;

#define KOCH_SNOWFLAKE
// #define DRAGON_CURVE

FractalCurveDevelopment::FractalCurveDevelopment() {
  isInitialized= false;
  Nodes.clear();
}


void FractalCurveDevelopment::Init() {
  int maxDepth= int(std::round(D.param[testVar0____________].val));
  if (maxDepth < 2) return;

  Faces.clear();
  Nodes.clear();

  // Initialize the curve at depth 0
#ifdef KOCH_SNOWFLAKE
  Nodes.resize(1);
  Nodes[0].push_back(Math::Vec3(-std::sqrt(3.0) / 2.0, -1.0, 0.0));
  Nodes[0].push_back(Math::Vec3(std::sqrt(3.0) / 2.0, 0.0, 0.0));
  Nodes[0].push_back(Math::Vec3(-std::sqrt(3.0) / 2.0, 1.0, 0.0));
  Nodes[0].push_back(Math::Vec3(-std::sqrt(3.0) / 2.0, -1.0, 0.0));
#endif
#ifdef DRAGON_CURVE
  Nodes.resize(1);
  Nodes[0].push_back(Math::Vec3(0.0, -1.0, 0.0));
  Nodes[0].push_back(Math::Vec3(1.0, 0.0, 0.0));
  Nodes[0].push_back(Math::Vec3(0.0, 1.0, 0.0));
  Nodes[0].push_back(Math::Vec3(-1.0, 0.0, 0.0));
  Nodes[0].push_back(Math::Vec3(0.0, -1.0, 0.0));
#endif

  // Iteratively build the next level in the fractal recursion depth
  for (int idxDepth= 1; idxDepth < maxDepth; idxDepth++) {
    Nodes.push_back(std::vector<Math::Vec3>());
    for (int idxNode= 0; idxNode < int(Nodes[idxDepth - 1].size()) - 1; idxNode++) {
      Math::Vec3 posA= Nodes[idxDepth - 1][idxNode];
      Math::Vec3 posB= Nodes[idxDepth - 1][idxNode + 1];
      Math::Vec3 ZOffset(0.0, 0.0, -D.param[testVar1____________].val / std::pow(D.param[testVar2____________].val, double(idxDepth)));

#ifdef KOCH_SNOWFLAKE
      Math::Vec3 posN0= ZOffset + posA;
      Math::Vec3 posN1= ZOffset + posA + (posB - posA) * 1.0 / 3.0;
      Math::Vec3 posN2= ZOffset + (posA + posB) / 2.0 + (std::sqrt(3.0) / 6.0) * (posB - posA).norm() * (posB - posA).cross(Math::Vec3(0.0, 0.0, 1.0)).normalized();
      Math::Vec3 posN3= ZOffset + posA + (posB - posA) * 2.0 / 3.0;
      Math::Vec3 posN4= ZOffset + posB;

      if (idxNode == 0)
        Nodes[idxDepth].push_back(posN0);
      Nodes[idxDepth].push_back(posN1);
      Nodes[idxDepth].push_back(posN2);
      Nodes[idxDepth].push_back(posN3);
      Nodes[idxDepth].push_back(posN4);

      Math::Vec3 posM= (posA + posB) / 2.0;
      Faces.push_back({posM, posA, posN0});
      Faces.push_back({posM, posN0, posN1});
      Faces.push_back({posM, posN1, posN2});
      Faces.push_back({posM, posN2, posN3});
      Faces.push_back({posM, posN3, posN4});
      Faces.push_back({posM, posN4, posB});
#endif
#ifdef DRAGON_CURVE
      Math::Vec3 dir= (posB - posA).cross(Math::Vec3(0.0, 0.0, 1.0)).normalized();
      if (idxNode % 2 == 0)
        dir= Math::Vec3()-dir;
      Math::Vec3 posN0= ZOffset + posA;
      Math::Vec3 posN1= ZOffset + 0.5 * (posA + posB) + 0.5*D.param[testVar3____________].val * (posB - posA).norm() * dir;
      Math::Vec3 posN2= ZOffset + posB;

      if (idxNode == 0)
        Nodes[idxDepth].push_back(posN0);
      Nodes[idxDepth].push_back(posN1);
      Nodes[idxDepth].push_back(posN2);

      Math::Vec3 posM= (posA + posB) / 2.0;
      Faces.push_back({posM, posA, posN0});
      Faces.push_back({posM, posN0, posN1});
      Faces.push_back({posM, posN1, posN2});
      Faces.push_back({posM, posN2, posB});
#endif
    }
  }

  // Save OBJ file of developed surface
  std::string iFullpath= "D:/3DModelsUnsorted/test.obj";
  printf("Saving OBJ mesh file [%s]\n", iFullpath.c_str());
  FILE* outputFile= nullptr;
  outputFile= fopen(iFullpath.c_str(), "w");
  if (outputFile == nullptr) {
    printf("[ERROR] Unable to create the file\n\n");
    throw 0;
  }
  for (unsigned int k= 0; k < Faces.size(); k++) {
    fprintf(outputFile, "v %lf %lf %lf\n", Faces[k][0][0], Faces[k][0][1], Faces[k][0][2]);
    fprintf(outputFile, "v %lf %lf %lf\n", Faces[k][1][0], Faces[k][1][1], Faces[k][1][2]);
    fprintf(outputFile, "v %lf %lf %lf\n", Faces[k][2][0], Faces[k][2][1], Faces[k][2][2]);
  }
  for (unsigned int k= 0; k < Faces.size(); k++) {
    fprintf(outputFile, "f %d %d %d\n", k * 3 + 1, k * 3 + 2, k * 3 + 3);
  }
  fclose(outputFile);
  printf("File saved: %zd vertices, %zd triangles\n", Faces.size() * 3, Faces.size());

  isInitialized= true;
}


void FractalCurveDevelopment::Draw() {
  if (!isInitialized) return;

  if (D.showNodes) {
    glPointSize(2.0f);
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
    glLineWidth(2.0f);
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
    glEnable(GL_LIGHTING);
    glColor3f(0.8f, 0.8f, 0.8f);
    glBegin(GL_TRIANGLES);
    for (auto face : Faces) {
      Math::Vec3 normal= (face[1] - face[0]).cross(face[2] - face[0]).normalized();
      glNormal3dv(normal.array());
      glVertex3dv(face[0].array());
      glVertex3dv(face[1].array());
      glVertex3dv(face[2].array());
    }
    glEnd();
    glDisable(GL_LIGHTING);
  }
}
