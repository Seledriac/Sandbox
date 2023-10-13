#include "FractalCurvDev.hpp"


// Standard lib
#include <cmath>
#include <cstdio>
#include <vector>

// GLUT lib
#include "../freeglut/include/GL/freeglut.h"

// Project lib
#include "../Data.hpp"
#include "../Util/Colormap.hpp"
#include "../Util/Vector.hpp"


// Link to shared sandbox data
extern Data D;

// List of UI parameters for this project
enum ParamType
{
  MaxDepth____,
  testVar1____,
  testVar2____,
  testVar3____,
  WriteFile___,
};


#define KOCH_SNOWFLAKE
// #define DRAGON_CURVE


// Constructor
FractalCurvDev::FractalCurvDev() {
  isActivProj= false;
  isAllocated= false;
  isRefreshed= false;
}


// Initialize Project UI parameters
void FractalCurvDev::SetActiveProject() {
  if (!isActivProj) {
    D.UI.clear();
    D.UI.push_back(ParamUI("MaxDepth____", 5.0));
    D.UI.push_back(ParamUI("testVar1____", 0.2));
    D.UI.push_back(ParamUI("testVar2____", 1.5));
    D.UI.push_back(ParamUI("testVar3____", 1.0));
    D.UI.push_back(ParamUI("WriteFile___", 0.0));
  }

  D.boxMin= {0.0, 0.0, 0.0};
  D.boxMax= {1.0, 1.0, 1.0};

  isActivProj= true;
  isAllocated= false;
  isRefreshed= false;
}


// Check if parameter changes should trigger an allocation
void FractalCurvDev::CheckAlloc() {
}


// Check if parameter changes should trigger a refresh
void FractalCurvDev::CheckRefresh() {
  if (D.UI[MaxDepth____].hasChanged()) isRefreshed= false;
  if (D.UI[testVar1____].hasChanged()) isRefreshed= false;
  if (D.UI[testVar2____].hasChanged()) isRefreshed= false;
  if (D.UI[testVar3____].hasChanged()) isRefreshed= false;
}


// Allocate the project data
void FractalCurvDev::Allocate() {
  if (!isActivProj) return;
  CheckAlloc();
  if (isAllocated) return;
  isRefreshed= false;
  isAllocated= true;
}


// Refresh the project
void FractalCurvDev::Refresh() {
  if (!isActivProj) return;
  CheckAlloc();
  if (!isAllocated) Allocate();
  CheckRefresh();
  if (isRefreshed) return;
  isRefreshed= true;

  int maxDepth= std::max(D.UI[MaxDepth____].GetI(), 2);
  Faces.clear();
  Nodes.clear();

  // Initialize the curve at depth 0
#ifdef KOCH_SNOWFLAKE
  Nodes.resize(1);
  Nodes[0].push_back(Math::Vec3f(-std::sqrt(3.0f) / 2.0f, -1.0f, 0.0f));
  Nodes[0].push_back(Math::Vec3f(+std::sqrt(3.0f) / 2.0f, +0.0f, 0.0f));
  Nodes[0].push_back(Math::Vec3f(-std::sqrt(3.0f) / 2.0f, +1.0f, 0.0f));
  Nodes[0].push_back(Math::Vec3f(-std::sqrt(3.0f) / 2.0f, -1.0f, 0.0f));
#endif
#ifdef DRAGON_CURVE
  Nodes.resize(1);
  Nodes[0].push_back(Math::Vec3f(+0.0f, -1.0f, 0.0f));
  Nodes[0].push_back(Math::Vec3f(+1.0f, +0.0f, 0.0f));
  Nodes[0].push_back(Math::Vec3f(+0.0f, +1.0f, 0.0f));
  Nodes[0].push_back(Math::Vec3f(-1.0f, +0.0f, 0.0f));
  Nodes[0].push_back(Math::Vec3f(+0.0f, -1.0f, 0.0f));
#endif

  // Iteratively build the next level in the fractal recursion depth
  for (int idxDepth= 1; idxDepth < maxDepth; idxDepth++) {
    Nodes.push_back(std::vector<Math::Vec3f>());
    for (int idxNode= 0; idxNode < int(Nodes[idxDepth - 1].size()) - 1; idxNode++) {
      Math::Vec3f posA= Nodes[idxDepth - 1][idxNode];
      Math::Vec3f posB= Nodes[idxDepth - 1][idxNode + 1];
      Math::Vec3f ZOffset(0.0f, 0.0f, -D.UI[testVar1____].GetF() / std::pow(D.UI[testVar2____].GetF(), double(idxDepth)));

#ifdef KOCH_SNOWFLAKE
      Math::Vec3f posN0= ZOffset + posA;
      Math::Vec3f posN1= ZOffset + posA + (posB - posA) * 1.0 / 3.0;
      Math::Vec3f posN2= ZOffset + (posA + posB) / 2.0 + (std::sqrt(3.0) / 6.0) * (posB - posA).norm() * (posB - posA).cross(Math::Vec3f(0.0, 0.0, 1.0)).normalized();
      Math::Vec3f posN3= ZOffset + posA + (posB - posA) * 2.0 / 3.0;
      Math::Vec3f posN4= ZOffset + posB;

      if (idxNode == 0)
        Nodes[idxDepth].push_back(posN0);
      Nodes[idxDepth].push_back(posN1);
      Nodes[idxDepth].push_back(posN2);
      Nodes[idxDepth].push_back(posN3);
      Nodes[idxDepth].push_back(posN4);

      Math::Vec3f posM= (posA + posB) / 2.0f;
      Faces.push_back({posM, posA, posN0});
      Faces.push_back({posM, posN0, posN1});
      Faces.push_back({posM, posN1, posN2});
      Faces.push_back({posM, posN2, posN3});
      Faces.push_back({posM, posN3, posN4});
      Faces.push_back({posM, posN4, posB});
#endif
#ifdef DRAGON_CURVE
      Math::Vec3f dir= (posB - posA).cross(Math::Vec3f(0.0f, 0.0f, 1.0f)).normalized();
      if (idxNode % 2 == 0)
        dir= Math::Vec3f() - dir;
      Math::Vec3f posN0= ZOffset + posA;
      Math::Vec3f posN1= ZOffset + 0.5f * (posA + posB) + 0.5f * D.UI[testVar3____].Get() * (posB - posA).norm() * dir;
      Math::Vec3f posN2= ZOffset + posB;

      if (idxNode == 0)
        Nodes[idxDepth].push_back(posN0);
      Nodes[idxDepth].push_back(posN1);
      Nodes[idxDepth].push_back(posN2);

      Math::Vec3f posM= (posA + posB) / 2.0f;
      Faces.push_back({posM, posA, posN0});
      Faces.push_back({posM, posN0, posN1});
      Faces.push_back({posM, posN1, posN2});
      Faces.push_back({posM, posN2, posB});
#endif
    }
  }

  // Save OBJ file of developed surface
  if (D.UI[WriteFile___].GetB()) {
    std::string iFullpath= "Outputs/test.obj";
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
  }
}


// Animate the project
void FractalCurvDev::Animate() {
  if (!isActivProj) return;
  CheckAlloc();
  if (!isAllocated) Allocate();
  CheckRefresh();
  if (!isRefreshed) Refresh();
}


// Draw the project
void FractalCurvDev::Draw() {
  if (!isActivProj) return;
  if (!isAllocated) return;
  if (!isRefreshed) return;

  // Draw vertices
  if (D.displayMode1) {
    glPointSize(2.0f);
    glBegin(GL_POINTS);
    for (int idxDepth= 0; idxDepth < int(Nodes.size()); idxDepth++) {
      float r, g, b;
      Colormap::RatioToJetBrightSmooth(float(idxDepth) / float(Nodes.size() - 1), r, g, b);
      glColor3f(r, g, b);
      for (int idxNode= 0; idxNode < int(Nodes[idxDepth].size()); idxNode++) {
        glVertex3fv(Nodes[idxDepth][idxNode].array());
      }
    }
    glEnd();
    glPointSize(1.0f);
  }

  // Draw wireframe
  if (D.displayMode2) {
    glLineWidth(2.0f);
    for (int idxDepth= 0; idxDepth < int(Nodes.size()); idxDepth++) {
      float r, g, b;
      Colormap::RatioToJetBrightSmooth(float(idxDepth) / float(Nodes.size() - 1), r, g, b);
      glBegin(GL_LINE_STRIP);
      glColor3f(r, g, b);
      for (int idxNode= 0; idxNode < int(Nodes[idxDepth].size()); idxNode++) {
        glVertex3fv(Nodes[idxDepth][idxNode].array());
      }
      glEnd();
    }
    glLineWidth(1.0f);
  }

  // Draw faces
  if (D.displayMode3) {
    glEnable(GL_LIGHTING);
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_TRIANGLES);
    for (auto face : Faces) {
      Math::Vec3f normal= (face[1] - face[0]).cross(face[2] - face[0]).normalized();
      glNormal3fv(normal.array());
      glVertex3fv(face[0].array());
      glVertex3fv(face[1].array());
      glVertex3fv(face[2].array());
    }
    glEnd();
    glDisable(GL_LIGHTING);
  }
}
