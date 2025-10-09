#include "graphics_pipeline/projector.h"

#include <cmath>

Projector::Projector(Vector3 _position, Vector3 _direction, float _fov)
    : position(_position), direction(_direction.GetNormal()), fov(_fov), texture_width(0), texture_height(0) {}

auto Projector::GetPosition() -> Vector3 { return position; }

void Projector::SetPosition(Vector3 new_position) { position = new_position; }

auto Projector::GetDirection() -> Vector3 { return direction; }

void Projector::SetDirection(Vector3 new_direction) {
  direction = new_direction.GetNormal();

  Vector3 up_vector(0.0F, 1.0F, 0.0F);
  if (fabsf(direction[0]) < fabsf(direction[1])) {
    up_vector = Vector3(1.0F, 0.0F, 0.0F);
  }

  Vector3 right_vector = (direction.Cross(up_vector)).GetNormal();
  up_vector = (right_vector.Cross(direction)).GetNormal();

  view_matrix.SetColumn(0, right_vector);
  view_matrix.SetColumn(1, up_vector);
  view_matrix.SetColumn(2, direction);
}

auto Projector::GetTexture() -> std::vector<unsigned int> { return texture; }

void Projector::SetTexture(const std::vector<unsigned int> &new_texture, int new_width, int new_height) {
  texture = new_texture;
  texture_width = new_width;
  texture_height = new_height;
}

auto Projector::ProjectPoint(Vector3 world_point, int &u_coordinate, int &v_coordinate) -> bool {
  Vector3 camera_space = view_matrix.GetInverse() * (world_point - position);

  if (camera_space[2] <= 0.0F) {
    return false;
  }

  float focal_length = (static_cast<float>(texture_width) / (float)2) / tanf(fov / (float)2);
  float u_float = (static_cast<float>(texture_width) / (float)2) + ((camera_space[0] / camera_space[2]) * focal_length);
  float v_float =
      (static_cast<float>(texture_height) / (float)2) - ((camera_space[1] / camera_space[2]) * focal_length);

  u_coordinate = static_cast<int>(u_float);
  v_coordinate = static_cast<int>(v_float);

  return (u_coordinate >= 0 && u_coordinate < texture_width && v_coordinate >= 0 && v_coordinate < texture_height);
}

auto Projector::ProjectColor(Vector3 world_point) -> Vector3 {
  int u_coordinate;
  int v_coordinate;
  if (!ProjectPoint(world_point, u_coordinate, v_coordinate) || texture.empty()) {
    return {0.0F, 0.0F, 0.0F};
  }

  long idx = (static_cast<long>(texture_height - 1 - v_coordinate) * texture_width) + u_coordinate;
  Vector3 color;
  color.SetColor(texture[idx]);
  return color;
}