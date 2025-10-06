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
};