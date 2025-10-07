#include "graphics_pipeline/framebuffer.h"

#include <algorithm>
#include <cmath>
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

  glDrawPixels(width, height, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, pixels.data());

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

auto Framebuffer::GetZBuffer(int u_coordinate, int v_coordinate) -> float {
  if (u_coordinate < 0 || u_coordinate >= width || v_coordinate < 0 || v_coordinate >= height) {
    return 0.0F;
  }

  int index = ((height - 1 - v_coordinate) * width) + u_coordinate;
  return z_buffer[index];
}

void Framebuffer::SetZBuffer(int u_coordinate, int v_coordinate, float z_value) {
  if (u_coordinate < 0 || u_coordinate >= width || v_coordinate < 0 || v_coordinate >= height) {
    return;
  }

  int index = ((height - 1 - v_coordinate) * width) + u_coordinate;
  z_buffer[index] = z_value;
}

void Framebuffer::ClearZBuffer() { std::ranges::fill(z_buffer.begin(), z_buffer.end(), 0.0F); }

auto Framebuffer::IsFarther(int u_coordinate, int v_coordinate, float z_value) -> bool {
  if (u_coordinate < 0 || u_coordinate >= width || v_coordinate < 0 || v_coordinate >= height) {
    return true;
  }

  int index = ((height - 1 - v_coordinate) * width) + u_coordinate;
  return z_value <= z_buffer[index];
}

void Framebuffer::FillBackground(unsigned int color) {
  for (int i = 0; i < width * height; i++) {
    pixels[i] = color;
  }
}

void Framebuffer::DrawPoint(Vector3 point, int size, unsigned int color) {
  int u_coordinate = (int)point[0];
  int v_coordinate = (int)point[1];

  for (int i = v_coordinate - (size / 2); i <= v_coordinate + size / 2; i++) {
    for (int j = u_coordinate - (size / 2); j <= u_coordinate + size / 2; j++) {
      SetPixel(j, i, color);
    }
  }
}

void Framebuffer::DrawSegment(Vector3 start_point, Vector3 end_point, unsigned int color) {
  start_point[2] = 0.0F;
  end_point[2] = 0.0F;

  int length = (int)((end_point - start_point).GetMagnitude() + 2);
  for (int i = 0; i < length; i++) {
    Vector3 current_point = start_point + (end_point - start_point) * (float)i / (float)(length - 1);
    SetPixel((int)current_point[0], (int)current_point[1], color);
  }
}

void Framebuffer::DrawSegment(Vector3 start_point, Vector3 end_point, Vector3 start_color, Vector3 end_color) {
  start_point[2] = 0.0F;
  end_point[2] = 0.0F;

  int length = (int)((end_point - start_point).GetMagnitude() + 2);
  for (int i = 0; i < length; i++) {
    Vector3 current_point = start_point + (end_point - start_point) * (float)i / (float)(length - 1);
    Vector3 current_color = start_color + (end_color - start_color) * (float)i / (float)(length - 1);
    unsigned int color = current_color.GetColor();
    SetPixel((int)current_point[0], (int)current_point[1], color);
  }
}

void Framebuffer::DrawRectangle(int u_coordinate, int v_coordinate, int width, int height, unsigned int color) {
  Vector3 top_left((float)u_coordinate, (float)v_coordinate, 0.0F);
  Vector3 top_right((float)(u_coordinate + width - 1), (float)v_coordinate, 0.0F);
  Vector3 bottom_left((float)u_coordinate, (float)(v_coordinate + height - 1), 0.0F);
  Vector3 bottom_right((float)(u_coordinate + width - 1), (float)(v_coordinate + height - 1), 0.0F);

  DrawSegment(top_left, top_right, color);
  DrawSegment(top_right, bottom_right, color);
  DrawSegment(bottom_right, bottom_left, color);
  DrawSegment(bottom_left, top_left, color);
}

void Framebuffer::DrawRectangleFilled(int u_coordinate, int v_coordinate, int width, int height, unsigned int color) {
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      SetPixel(u_coordinate + j, v_coordinate + i, color);
    }
  }
}

void Framebuffer::DrawCircle(int u_center, int v_center, int radius, unsigned int color) {
  int u_coordiante = 0;
  int v_coordinate = radius;
  int decision = 1 - radius;

  auto draw_circle_points = [&](int center_u, int center_v, int delta_u, int delta_v) {
    SetPixel(center_u + delta_u, center_v + delta_v, color);
    SetPixel(center_u - delta_u, center_v + delta_v, color);
    SetPixel(center_u + delta_u, center_v - delta_v, color);
    SetPixel(center_u - delta_u, center_v - delta_v, color);
    SetPixel(center_u + delta_v, center_v + delta_u, color);
    SetPixel(center_u - delta_v, center_v + delta_u, color);
    SetPixel(center_u + delta_v, center_v - delta_u, color);
    SetPixel(center_u - delta_v, center_v - delta_u, color);
  };

  while (u_coordiante <= v_coordinate) {
    draw_circle_points(u_center, v_center, u_coordiante, v_coordinate);
    u_coordiante++;
    if (decision < 0) {
      decision += 2 * u_coordiante + 1;
    } else {
      v_coordinate--;
      decision += 2 * (u_coordiante - v_coordinate) + 1;
    }
  }
}

void Framebuffer::DrawCircleFilled(int u_center, int v_center, int radius, unsigned int color) {
  for (int i = -radius; i <= radius; i++) {
    for (int j = -radius; j <= radius; j++) {
      if (j * j + i * i <= radius * radius) {
        SetPixel(u_center + j, v_center + i, color);
      }
    }
  }
}

void Framebuffer::DrawTriangle(Vector3 point_0, Vector3 point_1, Vector3 point_2, unsigned int color) {
  DrawSegment(point_0, point_1, color);
  DrawSegment(point_1, point_2, color);
  DrawSegment(point_2, point_0, color);
}

void Framebuffer::DrawTriangleFilled(Vector3 point_0, Vector3 point_1, Vector3 point_2, unsigned int color) {
  if (point_0[1] > point_1[1]) {
    std::swap(point_0, point_1);
  }
  if (point_0[1] > point_2[1]) {
    std::swap(point_0, point_2);
  }
  if (point_1[1] > point_2[1]) {
    std::swap(point_1, point_2);
  }

  auto fill_flat_bottom_triangle = [&](Vector3 vector_0, Vector3 vector_1, Vector3 vector_2) {
    float inverse_slope_1 = (vector_1[0] - vector_0[0]) / (vector_1[1] - vector_0[1]);
    float inverse_slope_2 = (vector_2[0] - vector_0[0]) / (vector_2[1] - vector_0[1]);

    float u_1 = vector_0[0];
    float u_2 = vector_0[0];

    for (int i = (int)vector_0[1]; i <= (int)vector_1[1]; i++) {
      Vector3 start_point(u_1, (float)i, 0.0F);
      Vector3 end_point(u_2, (float)i, 0.0F);
      DrawSegment(start_point, end_point, color);
      u_1 += inverse_slope_1;
      u_2 += inverse_slope_2;
    }
  };

  auto fill_flat_top_triangle = [&](Vector3 vector_0, Vector3 vector_1, Vector3 vector_2) {
    float inverse_slope_1 = (vector_2[0] - vector_0[0]) / (vector_2[1] - vector_0[1]);
    float inverse_slope_2 = (vector_2[0] - vector_1[0]) / (vector_2[1] - vector_1[1]);

    float u_1 = vector_2[0];
    float u_2 = vector_2[0];

    for (int i = (int)vector_2[1]; i > (int)vector_0[1]; i--) {
      Vector3 start_point(u_1, (float)i, 0.0F);
      Vector3 end_point(u_2, (float)i, 0.0F);
      DrawSegment(start_point, end_point, color);
      u_1 -= inverse_slope_1;
      u_2 -= inverse_slope_2;
    }
  };

  if (point_1[1] == point_2[1]) {
    fill_flat_bottom_triangle(point_0, point_1, point_2);
  } else if (point_0[1] == point_1[1]) {
    fill_flat_top_triangle(point_0, point_1, point_2);
  } else {
    Vector3 point_3;
    point_3[1] = point_1[1];
    point_3[0] = point_0[0] + ((point_1[1] - point_0[1]) / (point_2[1] - point_0[1])) * (point_2[0] - point_0[0]);
    point_3[2] = 0.0F;

    fill_flat_bottom_triangle(point_0, point_1, point_3);
    fill_flat_top_triangle(point_1, point_3, point_2);
  }
}

void Framebuffer::DrawTriangleFilled(Vector3 point_0, Vector3 point_1, Vector3 point_2, Vector3 color_0,
                                     Vector3 color_1, Vector3 color_2, float depth_0, float depth_1, float depth_2) {
  if (point_0[1] > point_1[1]) {
    std::swap(point_0, point_1);
    std::swap(color_0, color_1);
    std::swap(depth_0, depth_1);
  }
  if (point_0[1] > point_2[1]) {
    std::swap(point_0, point_2);
    std::swap(color_0, color_2);
    std::swap(depth_0, depth_2);
  }
  if (point_1[1] > point_2[1]) {
    std::swap(point_1, point_2);
    std::swap(color_1, color_2);
    std::swap(depth_1, depth_2);
  }

  constexpr float EPSILON = 0.001F;

  auto fill_scanline = [&](int scanline_y, float start_x, float end_x, Vector3 start_color, Vector3 end_color,
                           float start_depth, float end_depth) {
    if (start_x > end_x) {
      std::swap(start_x, end_x);
      std::swap(start_color, end_color);
      std::swap(start_depth, end_depth);
    }

    int begin_x = static_cast<int>(start_x);
    int finish_x = static_cast<int>(end_x);

    for (int pixel_x = begin_x; pixel_x <= finish_x; pixel_x++) {
      if (pixel_x < 0 || pixel_x >= width || scanline_y < 0 || scanline_y >= height) {
        continue;
      }

      float interpolation_t =
          (finish_x == begin_x) ? 0.0F : static_cast<float>(pixel_x - begin_x) / static_cast<float>(finish_x - begin_x);
      float current_depth = start_depth + ((end_depth - start_depth) * interpolation_t);

      if (!IsFarther(pixel_x, scanline_y, current_depth)) {
        Vector3 current_color = start_color + (end_color - start_color) * interpolation_t;
        SetPixel(pixel_x, scanline_y, current_color.GetColor());
        SetZBuffer(pixel_x, scanline_y, current_depth);
      }
    }
  };

  auto interpolate_edge = [](Vector3 start_point, Vector3 end_point, Vector3 start_color, Vector3 end_color,
                             float start_depth, float end_depth, int scanline_y) -> std::tuple<float, Vector3, float> {
    constexpr float MIN_HEIGHT_DIFF = 0.001F;
    if (fabsf(end_point[1] - start_point[1]) < MIN_HEIGHT_DIFF) {
      return {start_point[0], start_color, start_depth};
    }
    float interpolation_t =
        static_cast<float>(scanline_y - static_cast<int>(start_point[1])) / (end_point[1] - start_point[1]);
    float interpolated_x = start_point[0] + ((end_point[0] - start_point[0]) * interpolation_t);
    Vector3 interpolated_color = start_color + (end_color - start_color) * interpolation_t;
    float interpolated_depth = start_depth + ((end_depth - start_depth) * interpolation_t);
    return {interpolated_x, interpolated_color, interpolated_depth};
  };

  if (fabsf(point_1[1] - point_2[1]) < EPSILON) {
    for (int scanline_y = static_cast<int>(point_0[1]); scanline_y <= static_cast<int>(point_1[1]); scanline_y++) {
      auto [x_left, color_left, depth_left] =
          interpolate_edge(point_0, point_1, color_0, color_1, depth_0, depth_1, scanline_y);
      auto [x_right, color_right, depth_right] =
          interpolate_edge(point_0, point_2, color_0, color_2, depth_0, depth_2, scanline_y);
      fill_scanline(scanline_y, x_left, x_right, color_left, color_right, depth_left, depth_right);
    }
    return;
  }

  if (fabsf(point_0[1] - point_1[1]) < EPSILON) {
    for (int scanline_y = static_cast<int>(point_0[1]); scanline_y <= static_cast<int>(point_2[1]); scanline_y++) {
      auto [x_left, color_left, depth_left] =
          interpolate_edge(point_0, point_2, color_0, color_2, depth_0, depth_2, scanline_y);
      auto [x_right, color_right, depth_right] =
          interpolate_edge(point_1, point_2, color_1, color_2, depth_1, depth_2, scanline_y);
      fill_scanline(scanline_y, x_left, x_right, color_left, color_right, depth_left, depth_right);
    }
    return;
  }

  float middle_t = (point_1[1] - point_0[1]) / (point_2[1] - point_0[1]);
  Vector3 middle_point;
  middle_point[0] = point_0[0] + (point_2[0] - point_0[0]) * middle_t;
  middle_point[1] = point_1[1];
  middle_point[2] = 0.0F;
  Vector3 middle_color = color_0 + (color_2 - color_0) * middle_t;
  float middle_depth = depth_0 + ((depth_2 - depth_0) * middle_t);

  for (int scanline_y = static_cast<int>(point_0[1]); scanline_y <= static_cast<int>(point_1[1]); scanline_y++) {
    auto [x_left, color_left, depth_left] =
        interpolate_edge(point_0, point_1, color_0, color_1, depth_0, depth_1, scanline_y);
    auto [x_right, color_right, depth_right] =
        interpolate_edge(point_0, middle_point, color_0, middle_color, depth_0, middle_depth, scanline_y);
    fill_scanline(scanline_y, x_left, x_right, color_left, color_right, depth_left, depth_right);
  }

  constexpr int SCANLINE_OFFSET = 1;
  for (int scanline_y = static_cast<int>(point_1[1]) + SCANLINE_OFFSET; scanline_y <= static_cast<int>(point_2[1]);
       scanline_y++) {
    auto [x_left, color_left, depth_left] =
        interpolate_edge(point_1, point_2, color_1, color_2, depth_1, depth_2, scanline_y);
    auto [x_right, color_right, depth_right] =
        interpolate_edge(middle_point, point_2, middle_color, color_2, middle_depth, depth_2, scanline_y);
    fill_scanline(scanline_y, x_left, x_right, color_left, color_right, depth_left, depth_right);
  }
}