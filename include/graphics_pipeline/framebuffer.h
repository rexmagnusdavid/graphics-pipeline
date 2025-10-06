#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>

#include "vector_3.h"

class Framebuffer {
public:
  std::vector<unsigned int> pixels;
  int width, height;
  GLFWwindow *window;
  Framebuffer(int _width, int _height, const char *title);
  ~Framebuffer();

  void Render();
  void Resize(int _width, int _height);

  void LoadTiff(char *file_name);
  void SaveTiff(char *file_name);

  auto GetPixel(int u_coordinate, int v_coordinate) -> unsigned int;
  void SetPixel(int u_coordinate, int v_coordinate, unsigned int color);

  void FillBackground(unsigned int color);
  void DrawPoint(Vector3 point, int size, unsigned int color);
  void DrawSegment(Vector3 start_point, Vector3 end_point, unsigned int color);
  void DrawSegment(Vector3 start_point, Vector3 end_point, Vector3 start_color, Vector3 end_color);
  void DrawRectangle(int u_coordinate, int v_coordinate, int width, int height, unsigned int color);
  void DrawRectangleFilled(int u_coordinate, int v_coordinate, int width, int height, unsigned int color);
  void DrawCircle(int u_center, int v_center, int radius, unsigned int color);
  void DrawCircleFilled(int u_center, int v_center, int radius, unsigned int color);
  void DrawTriangle(Vector3 point_0, Vector3 point_1, Vector3 point_2, unsigned int color);
  void DrawTriangleFilled(Vector3 point_0, Vector3 point_1, Vector3 point_2, unsigned int color);
};