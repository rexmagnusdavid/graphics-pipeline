#pragma once

#include "vector_3.h"

class Matrix3x3 {
public:
  std::array<Vector3, 3> rows;
  Matrix3x3() = default;
  Matrix3x3(Vector3 row_0, Vector3 row_1, Vector3 row_2);

  friend auto operator>>(std::istream &input, Matrix3x3 &matrix)
      -> std::istream &;
  friend auto operator<<(std::ostream &output, Matrix3x3 matrix)
      -> std::ostream &;

  auto operator[](int index) -> Vector3 &;
  auto operator*(Vector3 right) -> Vector3;
  auto operator*(Matrix3x3 right) -> Matrix3x3;

  auto GetColumn(int index) -> Vector3;
  void SetColumn(int index, Vector3 column);

  auto GetTranspose() -> Matrix3x3;
  auto GetInverse() -> Matrix3x3;

  static auto Identity() -> Matrix3x3;
  static auto RotateAboutX(float angle) -> Matrix3x3;
  static auto RotateAboutY(float angle) -> Matrix3x3;
  static auto RotateAboutZ(float angle) -> Matrix3x3;
};