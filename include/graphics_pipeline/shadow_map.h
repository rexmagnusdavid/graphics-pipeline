#pragma once

#include <vector>

#include "matrix_3x3.h"
#include "vector_3.h"

class ShadowMap {
public:
  std::vector<float> depth_buffer;
  int width, height;
  Vector3 light_position;
  Matrix3x3 light_view_matrix;
  float light_fov;
  ShadowMap(int _width, int _height);

  auto GetPosition() -> Vector3;
  void SetPosition(Vector3 position, Vector3 look_at, Vector3 up_vector);

  auto GetDepthBuffer(int u_coordinate, int v_coordinate) -> float;
  void SetDepthBuffer(int u_coordinate, int v_coordinate, float depth);
  void ClearDepthBuffer();

  auto IsInShadow(Vector3 world_point, float epsilon) -> bool;

  auto Project(Vector3 world_point, Vector3 &light_space_point) -> int;
};