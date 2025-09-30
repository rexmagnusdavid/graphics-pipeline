#pragma once

#include <iostream>

class Vector3 {
public:
  std::array<float, 3> coordinates;
  Vector3() = default;
  Vector3(float coordinate_0, float coordinate_1, float coordinate_2);

  friend auto operator>>(std::istream &input, Vector3 &vector)
      -> std::istream &;
  friend auto operator<<(std::ostream &output, Vector3 vector)
      -> std::ostream &;

  auto operator[](int index) -> float &;
  auto operator+(Vector3 right) -> Vector3;
  auto operator-(Vector3 right) -> Vector3;
  auto operator*(float right) -> Vector3;
  auto operator/(float right) -> Vector3;

  auto GetMagnitude() -> float;
  auto GetNormal() -> Vector3;

  auto Cross(Vector3 with) -> Vector3;
  auto Dot(Vector3 with) -> float;
  auto RotateAboutAxis(Vector3 origin, Vector3 direction, float angle)
      -> Vector3;

  auto GetColor() -> unsigned int;
  void SetColor(unsigned int new_color);
};