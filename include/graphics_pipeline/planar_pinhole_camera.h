#pragma once

#include "vector_3.h"

class PlanarPinholeCamera {
public:
  Vector3 right, up, forward, position;
  int width, height;
  float horizontal_fov;
  PlanarPinholeCamera() = default;
  PlanarPinholeCamera(int _width, int _height, float _horizontal_fov);

  void LoadText(const char *file_name);
  void SaveText(const char *file_name);

  auto GetHorizontalFov() -> float;
  void SetHorizontalFov(float new_horizontal_fov);

  void Pan(float angle);
  void Tilt(float angle);
  void Roll(float angle);
  void Zoom(float factor);
  void Translate(Vector3 vector);
  auto Project(Vector3 point, Vector3 &projected_point) -> int;

  auto GetViewDirection() -> Vector3;
  auto GetFocalLength() -> float;
};