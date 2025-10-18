#include "graphics_pipeline/texture.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <tiffio.h>

#include "graphics_pipeline/color.h"

Texture::Texture() : width(0), height(0), tiling_mode(REPEAT) {}

void Texture::LoadTiff(char *file_name) {
  TIFF *input = TIFFOpen(file_name, "r");
  if (input == nullptr) {
    std::cerr << file_name << " could not be opened" << '\n';
    return;
  }

  TIFFGetField(input, TIFFTAG_IMAGEWIDTH, &width);
  TIFFGetField(input, TIFFTAG_IMAGELENGTH, &height);

  pixels.clear();
  pixels.resize(static_cast<long>(width) * height);

  TIFFReadRGBAImage(input, width, height, pixels.data(), 0);

  TIFFClose(input);

  std::cerr << "INFO: loaded texture " << file_name << " (" << width << "x" << height << ")" << '\n';
}

auto Texture::WrapCoordinate(float coordinate, int dimension) -> int {
  if (tiling_mode == REPEAT) {
    float wrapped = coordinate - floorf(coordinate);
    int pixel = static_cast<int>(wrapped * static_cast<float>(dimension));
    pixel = std::max(pixel, 0);

    if (pixel >= dimension) {
      pixel = dimension - 1;
    }

    return pixel;
  }

  float tile_number = floorf(coordinate);
  float fraction = coordinate - tile_number;
  int tile = static_cast<int>(tile_number);

  constexpr int EVEN_TILE = 0;
  if (tile % 2 == EVEN_TILE) {
    int pixel = static_cast<int>(fraction * static_cast<float>(dimension));
    pixel = std::max(pixel, 0);

    if (pixel >= dimension) {
      pixel = dimension - 1;
    }

    return pixel;
  }

  int pixel = static_cast<int>((1.0F - fraction) * static_cast<float>(dimension));
  pixel = std::max(pixel, 0);

  if (pixel >= dimension) {
    pixel = dimension - 1;
  }

  return pixel;
}

auto Texture::Sample(float u_coordinate, float v_coordinate) -> unsigned int {
  if (pixels.empty() || width == 0 || height == 0) {
    return Color::WHITE;
  }

  int u_pixel = WrapCoordinate(u_coordinate, width);
  int v_pixel = WrapCoordinate(v_coordinate, height);

  long index = (static_cast<long>(height - 1 - v_pixel) * width) + u_pixel;

  if (index < 0 || index >= static_cast<long>(pixels.size())) {
    return Color::WHITE;
  }

  return pixels[index];
}