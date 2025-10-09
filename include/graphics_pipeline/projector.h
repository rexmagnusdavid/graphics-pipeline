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

  auto GetPosition() -> Vector3;
  void SetPosition(Vector3 new_position);

  auto GetDirection() -> Vector3;
  void SetDirection(Vector3 new_direction);

  auto GetTexture() -> std::vector<unsigned int>;
  void SetTexture(const std::vector<unsigned int> &texture, int width, int height);

  auto ProjectPoint(Vector3 world_point, int &u_coordinate, int &v_coordinate) -> bool;
  auto ProjectColor(Vector3 world_point) -> Vector3;
};
