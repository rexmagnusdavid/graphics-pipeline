#include "graphics_pipeline/matrix_3x3.h"

#include <cmath>

Matrix3x3::Matrix3x3(Vector3 row_0, Vector3 row_1, Vector3 row_2) {
  rows[0] = row_0;
  rows[1] = row_1;
  rows[2] = row_2;
}

auto operator>>(std::istream &input, Matrix3x3 &matrix) -> std::istream & {
  return input >> matrix[0] >> matrix[1] >> matrix[2];
}

auto operator<<(std::ostream &output, Matrix3x3 matrix) -> std::ostream & {
  return output << matrix[0] << "\n" << matrix[1] << "\n" << matrix[2];
}

auto Matrix3x3::operator[](int index) -> Vector3 & { return rows.at(index); }

auto Matrix3x3::operator*(Vector3 right) -> Vector3 {
  Vector3 ret;

  ret[0] = rows[0].Dot(right);
  ret[1] = rows[1].Dot(right);
  ret[2] = rows[2].Dot(right);

  return ret;
}

auto Matrix3x3::operator*(Matrix3x3 right) -> Matrix3x3 {
  Matrix3x3 ret;

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      ret[i][j] = rows[i].Dot(right.GetColumn(j));
    }
  }

  return ret;
}

auto Matrix3x3::GetColumn(int index) -> Vector3 {
  Vector3 ret;

  ret[0] = rows[0][index];
  ret[1] = rows[1][index];
  ret[2] = rows[2][index];

  return ret;
}

void Matrix3x3::SetColumn(int index, Vector3 column) {
  Matrix3x3 ret;

  rows[0][index] = column[0];
  rows[1][index] = column[1];
  rows[2][index] = column[2];
}

auto Matrix3x3::GetTranspose() -> Matrix3x3 {
  Matrix3x3 ret;

  ret[0] = (*this).GetColumn(0);
  ret[1] = (*this).GetColumn(1);
  ret[2] = (*this).GetColumn(2);

  return ret;
}

auto Matrix3x3::GetInverse() -> Matrix3x3 {
  Matrix3x3 ret;

  Vector3 column_0 = (*this).GetColumn(0);
  Vector3 column_1 = (*this).GetColumn(1);
  Vector3 column_2 = (*this).GetColumn(2);
  float inv_determinant = 1.0F / column_0.Dot(column_1.Cross(column_2));

  ret[0] = column_1.Cross(column_2) * inv_determinant;
  ret[1] = column_2.Cross(column_0) * inv_determinant;
  ret[2] = column_0.Cross(column_1) * inv_determinant;

  return ret;
}

auto Matrix3x3::Identity() -> Matrix3x3 {
  Matrix3x3 ret;

  ret[0] = Vector3(1.0F, 0.0F, 0.0F);
  ret[1] = Vector3(0.0F, 1.0F, 0.0F);
  ret[2] = Vector3(0.0F, 0.0F, 1.0F);

  return ret;
}

auto Matrix3x3::RotateAboutX(float angle) -> Matrix3x3 {
  Matrix3x3 ret;

  ret[0] = Vector3(1.0F, 0.0F, 0.0F);
  ret[1] = Vector3(0.0F, cosf(angle), -sinf(angle));
  ret[2] = Vector3(0.0F, sinf(angle), cosf(angle));

  return ret;
}

auto Matrix3x3::RotateAboutY(float angle) -> Matrix3x3 {
  Matrix3x3 ret;

  ret[0] = Vector3(cosf(angle), 0.0F, sinf(angle));
  ret[1] = Vector3(0.0F, 1.0F, 0.0F);
  ret[2] = Vector3(-sinf(angle), 0.0F, cosf(angle));

  return ret;
}

auto Matrix3x3::RotateAboutZ(float angle) -> Matrix3x3 {
  Matrix3x3 ret;

  ret[0] = Vector3(cosf(angle), -sinf(angle), 0.0F);
  ret[1] = Vector3(sinf(angle), cosf(angle), 0.0F);
  ret[2] = Vector3(0.0F, 0.0F, 1.0F);

  return ret;
}