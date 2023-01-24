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


std::vector<Math::Vec3> KochNewNodes(Math::Vec3 iPosA, Math::Vec3 iPosB) {
  std::vector<Math::Vec3> newNodes(3);
  newNodes[0]= iPosA + (iPosB - iPosA) * 1.0 / 3.0;
  newNodes[1]= (iPosA + iPosB) / 2.0 + (std::sqrt(3.0) / 6.0) * (iPosB - iPosA).norm() * (iPosB - iPosA).cross(Math::Vec3(0.0, 0.0, 1.0)).normalized();
  newNodes[2]= iPosA + (iPosB - iPosA) * 2.0 / 3.0;
  return newNodes;
}


FractalCurveDevelopment::FractalCurveDevelopment() {
  isInitialized= false;
  Nodes.clear();
}


void FractalCurveDevelopment::Init() {
  int maxDepth= int(std::round(D.param[testVar0____________].val));
  if (maxDepth < 1) return;

  isInitialized= true;

  Nodes.resize(1, std::vector<Math::Vec3>(4));
  Nodes[0][0]= Math::Vec3(0.0, -1.0, 0.0);
  Nodes[0][1]= Math::Vec3(std::sqrt(3.0), 0.0, 0.0);
  Nodes[0][2]= Math::Vec3(0.0, 1.0, 0.0);
  Nodes[0][3]= Math::Vec3(0.0, -1.0, 0.0);

  Faces.clear();

  for (int idxDepth= 1; idxDepth < maxDepth; idxDepth++) {
    Nodes.push_back(std::vector<Math::Vec3>());
    for (int idxNode= 0; idxNode < int(Nodes[idxDepth - 1].size()) - 1; idxNode++) {
      Math::Vec3 posA= Nodes[idxDepth - 1][idxNode];
      Math::Vec3 posB= Nodes[idxDepth - 1][idxNode + 1];

      std::vector<Math::Vec3> newNodes= KochNewNodes(posA, posB);
      // Math::Vec3 ZOffset(0.0, 0.0, 0.2);
      Math::Vec3 ZOffset(0.0, 0.0, 0.6 / std::pow(1.5, double(idxDepth)));

      Nodes[idxDepth].push_back(posA + ZOffset);
      for (auto newNode : newNodes)
        Nodes[idxDepth].push_back(newNode + ZOffset);
      if (idxNode == int(Nodes[idxDepth - 1].size()) - 2)
        Nodes[idxDepth].push_back(posB + ZOffset);

      Math::Vec3 posM= (posA + posB) / 2.0;
      Faces.push_back({posM, posA, posA + ZOffset});
      Faces.push_back({posM, posA + ZOffset, newNodes[0] + ZOffset});
      Faces.push_back({posM, newNodes[0] + ZOffset, newNodes[1] + ZOffset});
      Faces.push_back({posM, newNodes[1] + ZOffset, newNodes[2] + ZOffset});
      Faces.push_back({posM, newNodes[2] + ZOffset, posB + ZOffset});
      Faces.push_back({posM, posB + ZOffset, posB});
    }
  }


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
      glNormal3dv((Math::Vec3() - normal).array());
      glVertex3dv(face[0].array());
      glVertex3dv(face[1].array());
      glVertex3dv(face[2].array());
    }
    glEnd();
    glDisable(GL_LIGHTING);
  }
}
