#include "graphics_pipeline/framebuffer.h"

#include <iostream>
#include <tiffio.h>

Framebuffer::Framebuffer(int _width, int _height, const char *title) {
  width = _width;
  height = _height;
  pixels.resize(static_cast<long>(width) * height);

  glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
  window = glfwCreateWindow(width, height, title, nullptr, nullptr);
  glfwMakeContextCurrent(window);

  glewInit();
}

Framebuffer::~Framebuffer() { glfwDestroyWindow(window); }

void Framebuffer::Render() {
  glDrawPixels(width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

  glfwSwapBuffers(window);
}

void Framebuffer::LoadTiff(char *file_name) {
  TIFF *input = TIFFOpen(file_name, "r");
  if (input == nullptr) {
    std::cerr << file_name << " could not be opened" << '\n';
    return;
  }

  int _width;
  int _height;
  TIFFGetField(input, TIFFTAG_IMAGEWIDTH, &_width);
  TIFFGetField(input, TIFFTAG_IMAGELENGTH, &_height);

  width = _width;
  height = _height;
  pixels.clear();
  pixels.resize(static_cast<long>(width) * height);
  glfwSetWindowSize(window, width, height);

  if (TIFFReadRGBAImage(input, width, height, pixels.data(), 0) == 0) {
    std::cerr << "failed to load " << file_name << '\n';
  }
  TIFFClose(input);
}

void Framebuffer::SaveTiff(char *file_name) {
  TIFF *output = TIFFOpen(file_name, "w");
  if (output == nullptr) {
    std::cerr << file_name << " could not be opened" << '\n';
    return;
  }

  constexpr int SAMPLES_PER_PIXEL = 4;
  constexpr int BITS_PER_CHANNEL = 8;

  TIFFSetField(output, TIFFTAG_IMAGEWIDTH, width);
  TIFFSetField(output, TIFFTAG_IMAGELENGTH, height);
  TIFFSetField(output, TIFFTAG_SAMPLESPERPIXEL, SAMPLES_PER_PIXEL);
  TIFFSetField(output, TIFFTAG_BITSPERSAMPLE, BITS_PER_CHANNEL);
  TIFFSetField(output, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
  TIFFSetField(output, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
  TIFFSetField(output, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
  for (int row = 0; row < (unsigned int)height; row++) {
    TIFFWriteScanline(
        output, &pixels[(static_cast<long>(height - row - 1)) * width], row);
  }

  TIFFClose(output);
}

auto Framebuffer::GetPixel(int u_coordinate, int v_coordinate) -> unsigned int {
  return pixels[((height - 1 - v_coordinate) * width) + u_coordinate];
}

void Framebuffer::SetPixel(int u_coordinate, int v_coordinate,
                           unsigned int color) {
  if (u_coordinate < 0 || u_coordinate > width - 1 || v_coordinate < 0 ||
      v_coordinate > height - 1) {
    return;
  }

  pixels[((height - 1 - v_coordinate) * width) + u_coordinate] = color;
}

void Framebuffer::FillBackground(unsigned int color) {
  for (int uv = 0; uv < width * height; uv++) {
    pixels[uv] = color;
  }
}

void Framebuffer::FillCheckboard(int size, unsigned int color_0,
                                 unsigned int color_1) {
  for (int v_coordinate = 0; v_coordinate < height; v_coordinate++) {
    for (int u_coordinate = 0; u_coordinate < width; u_coordinate++) {
      int color_u = u_coordinate / size;
      int color_v = v_coordinate / size;
      if (((color_u + color_v) % 2) != 0) {
        SetPixel(u_coordinate, v_coordinate, color_0);
        continue;
      }

      SetPixel(u_coordinate, v_coordinate, color_1);
    }
  }
}

void Framebuffer::Draw2DPoint(Vector3 point, int size, unsigned int color) {
  int u_point = (int)point[0];
  int v_point = (int)point[1];

  for (int v_coordinate = v_point - (size / 2);
       v_coordinate < v_point + size / 2; v_coordinate++) {
    for (int u_coordinate = u_point - (size / 2);
         u_coordinate < u_point + size / 2; u_coordinate++) {
      SetPixel(u_coordinate, v_coordinate, color);
    }
  }
}

void Framebuffer::Draw2DSegment(Vector3 point_0, Vector3 point_1,
                                unsigned int color_0) {
  point_0[2] = 0.0F;
  point_1[2] = 0.0F;

  int pixn = (int)((point_1 - point_0).GetMagnitude() + 2);
  for (int si = 0; si < pixn; si++) {
    Vector3 currP =
        point_0 + (point_1 - point_0) * (float)si / (float)(pixn - 1);
    SetPixel((int)currP[0], (int)currP[1], color_0);
  }
}

void Framebuffer::Draw2DSegment(Vector3 point_0, Vector3 point_1,
                                Vector3 color_0, Vector3 color_1) {
  point_0[2] = 0.0F;
  point_1[2] = 0.0F;

  int pixn = (int)((point_1 - point_0).GetMagnitude() + 2);
  for (int si = 0; si < pixn; si++) {
    Vector3 currP =
        point_0 + (point_1 - point_0) * (float)si / (float)(pixn - 1);
    Vector3 currC =
        color_0 + (color_1 - color_0) * (float)si / (float)(pixn - 1);
    unsigned int color = currC.GetColor();
    SetPixel((int)currP[0], (int)currP[1], color);
  }
}

void Framebuffer::Draw3DPoint(PlanarPinholeCamera *camera, Vector3 point,
                              int size, unsigned int color) {
  Vector3 projected_point;
  if (camera->Project(point, projected_point) == 0) {
    return;
  }

  Draw2DPoint(projected_point, size, color);
}

void Framebuffer::Draw3DSegment(PlanarPinholeCamera *camera, Vector3 point_0,
                                Vector3 point_1, unsigned int color_0) {
  Vector3 projected_point_0;
  if (camera->Project(point_0, projected_point_0) == 0) {
    return;
  }

  Vector3 projected_point_1;
  if (camera->Project(point_1, projected_point_1) == 0) {
    return;
  }

  Draw2DSegment(projected_point_0, projected_point_1, color_0);
}

void Framebuffer::Draw3DSegment(PlanarPinholeCamera *camera, Vector3 point_0,
                                Vector3 point_1, Vector3 color_0,
                                Vector3 color_1) {
  Vector3 projected_point_0;
  if (camera->Project(point_0, projected_point_0) == 0) {
    return;
  }

  Vector3 projected_point_1;
  if (camera->Project(point_1, projected_point_1) == 0) {
    return;
  }

  Draw2DSegment(projected_point_0, projected_point_1, color_0, color_1);
}