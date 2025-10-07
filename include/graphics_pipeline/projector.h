#pragma once

#include <vector>

#include "matrix_3x3.h"
#include "vector_3.h"

class Projector {
public:
  Vector3 position;
  Vector3 direction;
  Matrix3x3 view_matrix;
  float fov;
  int texture_width, texture_height;
  std::vector<unsigned int> texture;
  Projector(Vector3 _position, Vector3 _direction, float _fov);

  void SetTexture(const std::vector<unsigned int> &texture, int width, int height);
  void Update(Vector3 position, Vector3 direction);
  auto ProjectPoint(Vector3 world_point, int &u_coordinate, int &v_coordinate) -> bool;
  auto GetProjectedColor(Vector3 world_point) -> Vector3;
};
