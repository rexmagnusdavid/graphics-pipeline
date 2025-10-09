#include "graphics_pipeline/shadow_map.h"

#include <algorithm>
#include <cmath>

#include "graphics_pipeline/matrix_3x3.h"

ShadowMap::ShadowMap(int _width, int _height) : width(_width), height(_height) {
  constexpr float DEFAULT_LIGHT_FOV = 1.5F;

  depth_buffer.resize(static_cast<long>(width) * height);
  light_fov = DEFAULT_LIGHT_FOV;
  ClearDepthBuffer();
}

auto ShadowMap::GetPosition() -> Vector3 { return light_position; }

void ShadowMap::SetPosition(Vector3 position, Vector3 look_at, Vector3 up_vector) {
  light_position = position;

  Vector3 forward = (look_at - position).GetNormal();
  Vector3 right = (forward.Cross(up_vector)).GetNormal();
  Vector3 new_up = (right.Cross(forward)).GetNormal();

  light_view_matrix.SetColumn(0, right);
  light_view_matrix.SetColumn(1, new_up);
  light_view_matrix.SetColumn(2, forward);
}

auto ShadowMap::GetDepthBuffer(int u_coordinate, int v_coordinate) -> float {
  if (u_coordinate < 0 || u_coordinate >= width || v_coordinate < 0 || v_coordinate >= height) {
    return 0.0F;
  }
  long idx = (static_cast<long>(height - 1 - v_coordinate) * width) + u_coordinate;
  return depth_buffer[idx];
}

void ShadowMap::SetDepthBuffer(int u_coordinate, int v_coordinate, float depth) {
  if (u_coordinate < 0 || u_coordinate >= width || v_coordinate < 0 || v_coordinate >= height) {
    return;
  }

  long idx = (static_cast<long>(height - 1 - v_coordinate) * width) + u_coordinate;
  depth_buffer[idx] = depth;
}

void ShadowMap::ClearDepthBuffer() { std::ranges::fill(depth_buffer.begin(), depth_buffer.end(), 0.0F); }

auto ShadowMap::IsInShadow(Vector3 world_point, float epsilon) -> bool {
  Vector3 light_space;
  if (Project(world_point, light_space) == 0) {
    return true;
  }

  float focal_length = (static_cast<float>(width) / (float)2) / tanf(light_fov / (float)2);
  float u_coordinate = (static_cast<float>(width) / (float)2) + (light_space[0] * focal_length);
  float v_coordinate = (static_cast<float>(height) / (float)2) - (light_space[1] * focal_length);

  if (u_coordinate < 0 || u_coordinate >= (float)width || v_coordinate < 0 || v_coordinate >= (float)height) {
    return true;
  }

  float stored_depth = GetDepthBuffer(static_cast<int>(u_coordinate), static_cast<int>(v_coordinate));
  return (light_space[2] - epsilon) < stored_depth;
}

auto ShadowMap::Project(Vector3 world_point, Vector3 &light_space_point) -> int {
  Vector3 camera_space = light_view_matrix.GetInverse() * (world_point - light_position);

  if (camera_space[2] <= 0.0F) {
    return 0;
  }

  light_space_point[0] = camera_space[0] / camera_space[2];
  light_space_point[1] = camera_space[1] / camera_space[2];
  light_space_point[2] = 1.0F / camera_space[2];

  return 1;
}