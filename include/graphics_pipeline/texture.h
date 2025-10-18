#pragma once

#include <cstdint>
#include <vector>

class Texture {
public:
  enum TilingMode : std::uint8_t { REPEAT, MIRROR };

  std::vector<unsigned int> pixels;
  int width, height;
  TilingMode tiling_mode;
  Texture();

  void LoadTiff(char *file_name);

  auto WrapCoordinate(float coordinate, int dimension) -> int;

  auto Sample(float u_coordinate, float v_coordinate) -> unsigned int;
};