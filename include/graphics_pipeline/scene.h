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
  TriangleMesh *mesh;
  Scene();
  ~Scene();

  void Run();

  // Assignment-related functions here.
  void DBG();

private:
  static void KeyCallback(GLFWwindow *window, int key, int scan_code, int action, int mods);
  static void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
  static void CursorPositionCallback(GLFWwindow *window, double u_coordinate, double v_coordinate);
  static void ScrollCallback(GLFWwindow *window, double u_offset, double v_offset);

  void HandleKeyInput(int key, int action, int mods);
  void HandleMouseButton(int button, int action, int mods);
  void HandleCursorPosition(double u_coordinate, double v_coordinate);
  void HandleScroll(double u_offset, double v_offset);
};