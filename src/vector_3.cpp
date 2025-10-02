#include "graphics_pipeline/vector_3.h"

#include <cmath>

#include "graphics_pipeline/color.h"
#include "graphics_pipeline/matrix_3x3.h"

Vector3::Vector3(float coordinate_0, float coordinate_1, float coordinate_2) {
  coordinates[0] = coordinate_0;
  coordinates[1] = coordinate_1;
  coordinates[2] = coordinate_2;
}

auto operator>>(std::istream &input, Vector3 &vector) -> std::istream & {
  return input >> vector[0] >> vector[1] >> vector[2];
}

auto operator<<(std::ostream &output, Vector3 vector) -> std::ostream & {
  return output << vector[0] << " " << vector[1] << " " << vector[2];
}

auto Vector3::operator[](int index) -> float & { return coordinates.at(index); }

auto Vector3::operator+(Vector3 right) -> Vector3 {
  Vector3 ret;

  ret[0] = coordinates[0] + right[0];
  ret[1] = coordinates[1] + right[1];
  ret[2] = coordinates[2] + right[2];

  return ret;
}

auto Vector3::operator-(Vector3 right) -> Vector3 {
  Vector3 ret;

  ret[0] = coordinates[0] - right[0];
  ret[1] = coordinates[1] - right[1];
  ret[2] = coordinates[2] - right[2];

  return ret;
}

auto Vector3::operator*(float right) -> Vector3 {
  Vector3 ret;

  ret[0] = coordinates[0] * right;
  ret[1] = coordinates[1] * right;
  ret[2] = coordinates[2] * right;

  return ret;
}

auto Vector3::operator/(float right) -> Vector3 {
  Vector3 ret;

  ret[0] = coordinates[0] / right;
  ret[1] = coordinates[1] / right;
  ret[2] = coordinates[2] / right;

  return ret;
}

auto Vector3::GetMagnitude() -> float { return sqrtf((*this).Dot((*this))); }

auto Vector3::GetNormal() -> Vector3 { return (*this) / (*this).GetMagnitude(); }

auto Vector3::Cross(Vector3 with) -> Vector3 {
  Vector3 ret;

  ret[0] = coordinates[1] * with[2] - coordinates[2] * with[1];
  ret[1] = coordinates[2] * with[0] - coordinates[0] * with[2];
  ret[2] = coordinates[0] * with[1] - coordinates[1] * with[0];

  return ret;
}

auto Vector3::Dot(Vector3 with) -> float {
  float ret = NAN;

  ret = coordinates[0] * with[0] + coordinates[1] * with[1] + coordinates[2] * with[2];

  return ret;
}

auto Vector3::RotateAboutAxis(Vector3 origin, Vector3 direction, float angle) -> Vector3 {
  Vector3 ret;

  Matrix3x3 local_coordinate_system;

  Vector3 local_y_axis = direction.GetNormal();
  local_coordinate_system[1] = local_y_axis;
  Vector3 auxiliary = Vector3(0.0F, 1.0F, 0.0F);
  if (fabsf(local_y_axis[0]) < fabsf(local_y_axis[1])) {
    auxiliary = Vector3(1.0F, 0.0F, 0.0F);
  }

  Vector3 local_x_axis = (auxiliary.Cross(local_y_axis)).GetNormal();
  local_coordinate_system[0] = local_x_axis;
  Vector3 local_z_axis = local_x_axis.Cross(local_y_axis);
  local_coordinate_system[2] = local_z_axis;

  Vector3 point_in_local_space = local_coordinate_system * ((*this) - origin);

  Matrix3x3 y_axis_rotation_matrix = Matrix3x3::RotationAboutY(angle);
  Vector3 rotated_point_in_local_space = y_axis_rotation_matrix * point_in_local_space;

  ret = local_coordinate_system.GetInverse() * rotated_point_in_local_space + origin;

  return ret;
}

auto Vector3::GetColor() -> unsigned int {
  unsigned int ret;
  std::array<unsigned char, 3> rgb;

  for (int channel_index = 0; channel_index < 3; ++channel_index) {
    float component_value = (*this)[channel_index];
    component_value = std::clamp(component_value, 0.0F, 1.0F);
    rgb[channel_index] = static_cast<unsigned char>(std::lround(component_value * Color::MIN_ALPHA_CHANNEL));
  }

  const auto alpha_channel = Color::BLACK;
  const auto red_channel = static_cast<std::uint32_t>(rgb[2]) << 16;
  const auto green_channel = static_cast<std::uint32_t>(rgb[1]) << 8;
  const auto blue_channel = static_cast<std::uint32_t>(rgb[0]);

  ret = alpha_channel | red_channel | green_channel | blue_channel;

  return ret;
}

void Vector3::SetColor(unsigned int color) {
  (*this)[0] = (float)((unsigned char *)&color)[0] / Color::MAX_ALPHA_CHANNEL;
  (*this)[1] = (float)((unsigned char *)&color)[1] / Color::MAX_ALPHA_CHANNEL;
  (*this)[2] = (float)((unsigned char *)&color)[2] / Color::MAX_ALPHA_CHANNEL;
}

auto Vector3::Light(Vector3 normal, Vector3 light_direction, float ambient_coefficient) -> Vector3 {
  Vector3 &S = *this;
  float kd = normal.Dot(light_direction);
  kd = (kd < 0.0F) ? 0.0F : kd;
  Vector3 ret = S * (ambient_coefficient + ((1.0F - ambient_coefficient) * kd));
  return ret;
}

auto Vector3::Reflect(Vector3 light_direction) -> Vector3 {
  Vector3 &n = *this;
  Vector3 ln = n * (n.Dot(light_direction));
  Vector3 lt = light_direction - ln;
  Vector3 ret = ln - lt;
  return ret;
}
