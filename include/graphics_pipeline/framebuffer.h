#pragma once

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <GL/glew.h>
#include <GL/glut.h>
#include <tiffio.h>

#include <vector>

#include "planar_pinhole_camera.h"
#include "vector_3.h"

class Framebuffer : public Fl_Gl_Window {
public:
  std::vector<unsigned int> pixels;
  int width, height;
  Framebuffer() = delete;
  Framebuffer(int u_coordinate, int v_coordinate, int _width, int _height);

  void draw() override;
  auto handle(int event) -> int override;

  static void HandleKeyboard();

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