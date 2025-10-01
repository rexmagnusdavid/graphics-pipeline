#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>

#include "planar_pinhole_camera.h"
#include "vector_3.h"

class Framebuffer {
public:
  std::vector<unsigned int> pixels;
  int width, height;
  GLFWwindow *window;
  Framebuffer(int _width, int _height, const char *title);
  ~Framebuffer();

  void Render();

  void LoadTiff(char *file_name);
  void SaveTiff(char *file_name);

  auto GetPixel(int u_coordinate, int v_coordinate) -> unsigned int;
  void SetPixel(int u_coordinate, int v_coordinate, unsigned int color);

  void FillBackground(unsigned int color);
  void FillCheckboard(int size, unsigned int color_0, unsigned int color_1);

  void Draw2DPoint(Vector3 point, int size, unsigned int color);
  void Draw2DSegment(Vector3 point_0, Vector3 point_1, unsigned int color);
  void Draw2DSegment(Vector3 point_0, Vector3 point_1, Vector3 color_0,
                     Vector3 color_1);

  void Draw3DPoint(PlanarPinholeCamera *camera, Vector3 point, int size,
                   unsigned int color);
  void Draw3DSegment(PlanarPinholeCamera *camera, Vector3 point_0,
                     Vector3 point_1, unsigned int color);
  void Draw3DSegment(PlanarPinholeCamera *camera, Vector3 point_0,
                     Vector3 point_1, Vector3 color_0, Vector3 color_1);
};