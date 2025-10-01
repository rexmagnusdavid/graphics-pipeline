#pragma once

#include "framebuffer.h"
#include "gui.h"
#include "planar_pinhole_camera.h"
#include "triangle_mesh.h"

class Scene {
public:
  GUI *gui;
  Framebuffer *framebuffer;
  PlanarPinholeCamera *camera;
  std::vector<TriangleMesh> meshes;
  Scene();
  ~Scene();

  void Run();

  void DBG();
  void DrawRectangle();
  void DrawCircle();
  void DrawLine();
  void DrawName();
  void AnimateName();
  void SaveTiff();

private:
  static void KeyCallback(GLFWwindow *window, int key, int scancode, int action,
                          int mods);
  static void MouseButtonCallback(GLFWwindow *window, int button, int action,
                                  int mods);
  static void CursorPositionCallback(GLFWwindow *window, double xpos,
                                     double ypos);
  static void ScrollCallback(GLFWwindow *window, double xoffset,
                             double yoffset);

  void HandleKeyInput(int key, int action, int mods);
  void HandleMouseButton(int button, int action, int mods);
  void HandleCursorPosition(double xpos, double ypos);
  void HandleScroll(double xoffset, double yoffset);
};