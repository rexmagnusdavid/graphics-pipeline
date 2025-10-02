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
  glfwMakeContextCurrent(window);

  glDrawPixels(width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

  glfwSwapBuffers(window);
}

void Framebuffer::Resize(int _width, int _height) {
  width = _width;
  height = _height;
  pixels.clear();
  pixels.resize(static_cast<long>(width) * height);
  glfwSetWindowSize(window, width, height);
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
  Resize(_width, _height);
  TIFFReadRGBAImage(input, width, height, pixels.data(), 0);

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
    TIFFWriteScanline(output, &pixels[(static_cast<long>(height - row - 1)) * width], row);
  }

  TIFFClose(output);
}

auto Framebuffer::GetPixel(int u_coordinate, int v_coordinate) -> unsigned int {
  return pixels[((height - 1 - v_coordinate) * width) + u_coordinate];
}

void Framebuffer::SetPixel(int u_coordinate, int v_coordinate, unsigned int color) {
  if (u_coordinate < 0 || u_coordinate > width - 1 || v_coordinate < 0 || v_coordinate > height - 1) {
    return;
  }

  pixels[((height - 1 - v_coordinate) * width) + u_coordinate] = color;
}

void Framebuffer::FillBackground(unsigned int color) {
  for (int i = 0; i < width * height; i++) {
    pixels[i] = color;
  }
}

void Framebuffer::DrawSegment(Vector3 start_point, Vector3 end_point, unsigned int color) {
  start_point[2] = 0.0F;
  end_point[2] = 0.0F;

  int number_of_pixels = (int)((end_point - start_point).GetMagnitude() + 2);
  for (int i = 0; i < number_of_pixels; i++) {
    Vector3 current_point = start_point + (end_point - start_point) * (float)i / (float)(number_of_pixels - 1);
    SetPixel((int)current_point[0], (int)current_point[1], color);
  }
}

void Framebuffer::DrawSegment(Vector3 start_point, Vector3 end_point, Vector3 start_color, Vector3 end_color) {
  start_point[2] = 0.0F;
  end_point[2] = 0.0F;

  int number_of_pixels = (int)((end_point - start_point).GetMagnitude() + 2);
  for (int i = 0; i < number_of_pixels; i++) {
    Vector3 current_point = start_point + (end_point - start_point) * (float)i / (float)(number_of_pixels - 1);
    Vector3 current_color = start_color + (end_color - start_color) * (float)i / (float)(number_of_pixels - 1);
    unsigned int color = current_color.GetColor();
    SetPixel((int)current_point[0], (int)current_point[1], color);
  }
}