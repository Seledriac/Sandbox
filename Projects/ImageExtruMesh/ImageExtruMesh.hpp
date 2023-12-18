#pragma once


class ImageExtruMesh
{
  public:
  bool isActivProj;
  bool isAllocated;
  bool isRefreshed;

  ImageExtruMesh();

  void SetActiveProject();
  bool CheckAlloc();
  bool CheckRefresh();
  void Allocate();
  void KeyPress(const unsigned char key);
  void Refresh();
  void Animate();
  void Draw();
};
