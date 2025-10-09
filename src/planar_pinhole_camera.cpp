#include "graphics_pipeline/planar_pinhole_camera.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>

#include "graphics_pipeline/matrix_3x3.h"
#include "graphics_pipeline/vector_3.h"

PlanarPinholeCamera::PlanarPinholeCamera(int width, int height, float horizontal_fov) : width(width), height(height) {
  position = Vector3(0.0F, 0.0F, 0.0F);
  right = Vector3(1.0F, 0.0F, 0.0F);
  up = Vector3(0.0F, 1.0F, 0.0F);
  forward = Vector3(0.0F, 0.0F, -1.0F);
  SetHorizontalFov(horizontal_fov);
}

void PlanarPinholeCamera::LoadText(const char *file_name) {
  std::ifstream ifs(file_name);
  if (!ifs.is_open()) {
    return;
  }

  ifs >> position;
  ifs >> right;
  ifs >> up;
  ifs >> forward;
  ifs >> horizontal_fov;

  ifs.close();
}

void PlanarPinholeCamera::SaveText(const char *file_name) {
  std::ofstream ofs(file_name);
  if (!ofs.is_open()) {
    return;
  }

  ofs << position << '\n';
  ofs << right << '\n';
  ofs << up << '\n';
  ofs << forward << '\n';
  ofs << horizontal_fov << '\n';

  ofs.close();
}

auto PlanarPinholeCamera::GetHorizontalFov() -> float { return horizontal_fov; }

void PlanarPinholeCamera::SetHorizontalFov(float new_horizontal_fov) {
  constexpr float MIN_FOV = 0.01F;
  constexpr float MAX_FOV = 3.13F;

  horizontal_fov = std::clamp(new_horizontal_fov, MIN_FOV, MAX_FOV);
}

void PlanarPinholeCamera::Pan(float angle) {
  right = right.RotateAboutAxis(Vector3(0, 0, 0), up, angle);
  forward = forward.RotateAboutAxis(Vector3(0, 0, 0), up, angle);
}

void PlanarPinholeCamera::Tilt(float angle) {
  up = up.RotateAboutAxis(Vector3(0, 0, 0), right, angle);
  forward = forward.RotateAboutAxis(Vector3(0, 0, 0), right, angle);
}

void PlanarPinholeCamera::Roll(float angle) {
  right = right.RotateAboutAxis(Vector3(0, 0, 0), forward, angle);
  up = up.RotateAboutAxis(Vector3(0, 0, 0), forward, angle);
}

void PlanarPinholeCamera::Zoom(float factor) {
  float fov = GetHorizontalFov();
  SetHorizontalFov(fov / factor);
}

void PlanarPinholeCamera::Translate(Vector3 translation_vector) { position = position + translation_vector; }

void PlanarPinholeCamera::Pose(Vector3 new_position, Vector3 look_at_point, Vector3 up_vector) {
  Vector3 new_forward = (look_at_point - new_position).GetNormal();
  Vector3 new_right = (new_forward.Cross(up_vector)).GetNormal();
  Vector3 new_up = (new_right.Cross(new_forward)).GetNormal();

  right = new_right;
  up = new_up;
  forward = new_forward;
  position = new_position;
}

auto PlanarPinholeCamera::Project(Vector3 point, Vector3 &projected_point) -> int {
  Matrix3x3 camera_matrix;
  camera_matrix.SetColumn(0, right);
  camera_matrix.SetColumn(1, up);
  camera_matrix.SetColumn(2, forward);

  Vector3 camera_space_point = camera_matrix.GetInverse() * (point - position);

  if (camera_space_point[2] <= 0.0F) {
    return 0;
  }

  projected_point[0] = camera_space_point[0] / camera_space_point[2];
  projected_point[1] = camera_space_point[1] / camera_space_point[2];
  projected_point[2] = 1.0F / camera_space_point[2];

  return 1;
}

auto PlanarPinholeCamera::Unproject(int u_coordinate, int v_coordinate, float inverse_depth) -> Vector3 {
  Vector3 ret;

  constexpr float HALF_DIMENSION = 0.5F;
  float x_camera = (static_cast<float>(u_coordinate) + HALF_DIMENSION - (static_cast<float>(width) * HALF_DIMENSION));
  float y_camera = (static_cast<float>(v_coordinate) + HALF_DIMENSION - (static_cast<float>(height) * HALF_DIMENSION));
  float z_camera = GetFocalLength();

  float depth = 1.0F / inverse_depth;
  ret = position + (right * x_camera + up * y_camera + forward * z_camera) * (depth / GetFocalLength());

  return ret;
}

auto PlanarPinholeCamera::GetViewDirection() -> Vector3 { return forward; }

auto PlanarPinholeCamera::GetFocalLength() -> float {
  constexpr float HALF_WIDTH_DIVISOR = 2.0F;
  constexpr float HALF_FOV_DIVISOR = 2.0F;
  float ret = (static_cast<float>(width) / HALF_WIDTH_DIVISOR) / tanf(horizontal_fov / HALF_FOV_DIVISOR);

  return ret;
}

auto PlanarPinholeCamera::InterpolateLinear(PlanarPinholeCamera *start_camera, PlanarPinholeCamera *end_camera,
                                            float time) -> PlanarPinholeCamera {
  PlanarPinholeCamera ret;

  ret.position = start_camera->position + (end_camera->position - start_camera->position) * time;
  ret.right = (start_camera->right + (end_camera->right - start_camera->right) * time).GetNormal();
  ret.up = (start_camera->up + (end_camera->up - start_camera->up) * time).GetNormal();
  ret.forward = (start_camera->forward + (end_camera->forward - start_camera->forward) * time).GetNormal();

  ret.width = start_camera->width;
  ret.height = start_camera->height;
  ret.horizontal_fov =
      start_camera->horizontal_fov + (end_camera->horizontal_fov - start_camera->horizontal_fov) * time;

  return ret;
}

auto PlanarPinholeCamera::InterpolateNonLinear(PlanarPinholeCamera *start_camera, PlanarPinholeCamera *end_camera,
                                               float time) -> PlanarPinholeCamera {
  PlanarPinholeCamera ret;
  constexpr float SMOOTHSTEP_QUADRATIC_COEFFICIENT = 3.0F;
  constexpr float SMOOTHSTEP_CUBIC_COEFFICIENT = 2.0F;
  float smooth_time = time * time * (SMOOTHSTEP_QUADRATIC_COEFFICIENT - SMOOTHSTEP_CUBIC_COEFFICIENT * time);

  ret.position = start_camera->position + (end_camera->position - start_camera->position) * smooth_time;
  ret.right = (start_camera->right + (end_camera->right - start_camera->right) * smooth_time).GetNormal();
  ret.up = (start_camera->up + (end_camera->up - start_camera->up) * smooth_time).GetNormal();
  ret.forward = (start_camera->forward + (end_camera->forward - start_camera->forward) * smooth_time).GetNormal();

  ret.width = start_camera->width;
  ret.height = start_camera->height;
  ret.horizontal_fov =
      start_camera->horizontal_fov + (end_camera->horizontal_fov - start_camera->horizontal_fov) * smooth_time;

  return ret;
}