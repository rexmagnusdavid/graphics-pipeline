#include "graphics_pipeline/planar_pinhole_camera.h"

#include <algorithm>
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

  Vector3 newa;
  Vector3 newb;
  Vector3 newc;

  Vector3 newvd = (look_at_point - new_position).GetNormal();
  newa = (newvd.Cross(up_vector)).GetNormal();
  newb = (newvd.Cross(newa)).GetNormal();
  float focal_length = GetFocalLength();

  newc = newvd * focal_length - newa * ((float)width / (float)2) - newb * ((float)height / (float)2);

  right = newa;
  up = newb;
  forward = newc;
  position = new_position;
}

auto PlanarPinholeCamera::Project(Vector3 point, Vector3 &projected_point) -> int {
  Matrix3x3 matrix;
  matrix.SetColumn(0, right);
  matrix.SetColumn(1, up);
  matrix.SetColumn(2, forward);

  Vector3 q_var = matrix.GetInverse() * (point - position);

  if (q_var[2] <= 0.0F) {
    return 0;
  }

  projected_point[0] = q_var[0] / q_var[2];
  projected_point[1] = q_var[1] / q_var[2];
  projected_point[2] = 1.0F / q_var[2];

  return 1;
}

auto PlanarPinholeCamera::Unproject(int u_coordinate, int v_coordinate, float inverse_depth) -> Vector3 {
  Vector3 ret = position + (right * (0.5F + (float)u_coordinate) + up * (0.5F + (float)v_coordinate) + forward) *
                               (1.0F / inverse_depth);
  return ret;
}

auto PlanarPinholeCamera::GetViewDirection() -> Vector3 {
  Vector3 ret = (right.Cross(up)).GetNormal();

  return ret;
}

auto PlanarPinholeCamera::GetFocalLength() -> float {
  float ret;
  Vector3 view_direction = GetViewDirection();
  ret = view_direction.Dot(forward);

  return ret;
}