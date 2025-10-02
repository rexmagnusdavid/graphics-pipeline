#include <gtest/gtest.h>

#include <cmath>
#include <sstream>

#include "graphics_pipeline/matrix_3x3.h"
#include "graphics_pipeline/vector_3.h"

// NOLINTBEGIN(readability-magic-numbers)

class Matrix3x3Test : public ::testing::Test {
protected:
  static constexpr float EPSILON = 1e-5F;

  static auto FloatEqual(float first_value, float second_value) -> bool {
    return std::abs(first_value - second_value) < EPSILON;
  }

  static auto VectorEqual(Vector3 first_vector, Vector3 second_vector) -> bool {
    return FloatEqual(first_vector[0], second_vector[0]) && FloatEqual(first_vector[1], second_vector[1]) &&
           FloatEqual(first_vector[2], second_vector[2]);
  }

  static auto MatrixEqual(Matrix3x3 first_matrix, Matrix3x3 second_matrix) -> bool {
    return VectorEqual(first_matrix[0], second_matrix[0]) && VectorEqual(first_matrix[1], second_matrix[1]) &&
           VectorEqual(first_matrix[2], second_matrix[2]);
  }
};

TEST_F(Matrix3x3Test, DefaultConstructor) {
  Matrix3x3 matrix;
  EXPECT_EQ(matrix.rows.size(), 3);
}

TEST_F(Matrix3x3Test, ParameterizedConstructor) {
  Vector3 row_zero(1.0F, 2.0F, 3.0F);
  Vector3 row_one(4.0F, 5.0F, 6.0F);
  Vector3 row_two(7.0F, 8.0F, 9.0F);

  Matrix3x3 matrix(row_zero, row_one, row_two);

  EXPECT_TRUE(VectorEqual(matrix[0], row_zero));
  EXPECT_TRUE(VectorEqual(matrix[1], row_one));
  EXPECT_TRUE(VectorEqual(matrix[2], row_two));
}

TEST_F(Matrix3x3Test, IndexOperator) {
  Vector3 row_zero(1.0F, 2.0F, 3.0F);
  Vector3 row_one(4.0F, 5.0F, 6.0F);
  Vector3 row_two(7.0F, 8.0F, 9.0F);

  Matrix3x3 matrix(row_zero, row_one, row_two);

  EXPECT_FLOAT_EQ(matrix[0][0], 1.0F);
  EXPECT_FLOAT_EQ(matrix[1][1], 5.0F);
  EXPECT_FLOAT_EQ(matrix[2][2], 9.0F);
}

TEST_F(Matrix3x3Test, IdentityMatrix) {
  Matrix3x3 identity = Matrix3x3::Identity();

  EXPECT_FLOAT_EQ(identity[0][0], 1.0F);
  EXPECT_FLOAT_EQ(identity[0][1], 0.0F);
  EXPECT_FLOAT_EQ(identity[0][2], 0.0F);

  EXPECT_FLOAT_EQ(identity[1][0], 0.0F);
  EXPECT_FLOAT_EQ(identity[1][1], 1.0F);
  EXPECT_FLOAT_EQ(identity[1][2], 0.0F);

  EXPECT_FLOAT_EQ(identity[2][0], 0.0F);
  EXPECT_FLOAT_EQ(identity[2][1], 0.0F);
  EXPECT_FLOAT_EQ(identity[2][2], 1.0F);
}

TEST_F(Matrix3x3Test, GetColumn) {
  Vector3 row_zero(1.0F, 2.0F, 3.0F);
  Vector3 row_one(4.0F, 5.0F, 6.0F);
  Vector3 row_two(7.0F, 8.0F, 9.0F);

  Matrix3x3 matrix(row_zero, row_one, row_two);

  Vector3 column_zero = matrix.GetColumn(0);
  EXPECT_FLOAT_EQ(column_zero[0], 1.0F);
  EXPECT_FLOAT_EQ(column_zero[1], 4.0F);
  EXPECT_FLOAT_EQ(column_zero[2], 7.0F);

  Vector3 column_one = matrix.GetColumn(1);
  EXPECT_FLOAT_EQ(column_one[0], 2.0F);
  EXPECT_FLOAT_EQ(column_one[1], 5.0F);
  EXPECT_FLOAT_EQ(column_one[2], 8.0F);
}

TEST_F(Matrix3x3Test, SetColumn) {
  Matrix3x3 matrix = Matrix3x3::Identity();
  Vector3 new_column(1.0F, 2.0F, 3.0F);

  matrix.SetColumn(0, new_column);

  EXPECT_FLOAT_EQ(matrix[0][0], 1.0F);
  EXPECT_FLOAT_EQ(matrix[1][0], 2.0F);
  EXPECT_FLOAT_EQ(matrix[2][0], 3.0F);
}

TEST_F(Matrix3x3Test, MatrixVectorMultiplication) {
  Matrix3x3 matrix = Matrix3x3::Identity();
  Vector3 vector(1.0F, 2.0F, 3.0F);

  Vector3 result = matrix * vector;

  EXPECT_TRUE(VectorEqual(result, vector));
}

TEST_F(Matrix3x3Test, MatrixVectorMultiplicationNonIdentity) {
  Vector3 row_zero(1.0F, 0.0F, 0.0F);
  Vector3 row_one(0.0F, 2.0F, 0.0F);
  Vector3 row_two(0.0F, 0.0F, 3.0F);

  Matrix3x3 matrix(row_zero, row_one, row_two);
  Vector3 vector(1.0F, 1.0F, 1.0F);

  Vector3 result = matrix * vector;

  EXPECT_FLOAT_EQ(result[0], 1.0F);
  EXPECT_FLOAT_EQ(result[1], 2.0F);
  EXPECT_FLOAT_EQ(result[2], 3.0F);
}

TEST_F(Matrix3x3Test, MatrixMatrixMultiplication) {
  Matrix3x3 first_matrix = Matrix3x3::Identity();
  Matrix3x3 second_matrix = Matrix3x3::Identity();

  Matrix3x3 result = first_matrix * second_matrix;

  EXPECT_TRUE(MatrixEqual(result, Matrix3x3::Identity()));
}

TEST_F(Matrix3x3Test, Transpose) {
  Vector3 row_zero(1.0F, 2.0F, 3.0F);
  Vector3 row_one(4.0F, 5.0F, 6.0F);
  Vector3 row_two(7.0F, 8.0F, 9.0F);

  Matrix3x3 matrix(row_zero, row_one, row_two);
  Matrix3x3 transpose = matrix.GetTranspose();

  EXPECT_FLOAT_EQ(transpose[0][0], 1.0F);
  EXPECT_FLOAT_EQ(transpose[0][1], 4.0F);
  EXPECT_FLOAT_EQ(transpose[0][2], 7.0F);

  EXPECT_FLOAT_EQ(transpose[1][0], 2.0F);
  EXPECT_FLOAT_EQ(transpose[1][1], 5.0F);
  EXPECT_FLOAT_EQ(transpose[1][2], 8.0F);
}

TEST_F(Matrix3x3Test, InverseOfIdentity) {
  Matrix3x3 identity = Matrix3x3::Identity();
  Matrix3x3 inverse = identity.GetInverse();

  EXPECT_TRUE(MatrixEqual(inverse, identity));
}

TEST_F(Matrix3x3Test, RotationAboutX) {
  float angle = M_PI / 2.0F; // 90 degrees
  Matrix3x3 rotation = Matrix3x3::RotationAboutX(angle);

  Vector3 point(0.0F, 1.0F, 0.0F);
  Vector3 rotated = rotation * point;

  EXPECT_TRUE(FloatEqual(rotated[0], 0.0F));
  EXPECT_TRUE(FloatEqual(rotated[1], 0.0F));
  EXPECT_TRUE(FloatEqual(rotated[2], 1.0F));
}

TEST_F(Matrix3x3Test, RotationAboutY) {
  float angle = M_PI / 2.0F; // 90 degrees
  Matrix3x3 rotation = Matrix3x3::RotationAboutY(angle);

  Vector3 point(1.0F, 0.0F, 0.0F);
  Vector3 rotated = rotation * point;

  EXPECT_TRUE(FloatEqual(rotated[0], 0.0F));
  EXPECT_TRUE(FloatEqual(rotated[1], 0.0F));
  EXPECT_TRUE(FloatEqual(rotated[2], -1.0F));
}

TEST_F(Matrix3x3Test, RotationAboutZ) {
  float angle = M_PI / 2.0F; // 90 degrees
  Matrix3x3 rotation = Matrix3x3::RotationAboutZ(angle);

  Vector3 point(1.0F, 0.0F, 0.0F);
  Vector3 rotated = rotation * point;

  EXPECT_TRUE(FloatEqual(rotated[0], 0.0F));
  EXPECT_TRUE(FloatEqual(rotated[1], 1.0F));
  EXPECT_TRUE(FloatEqual(rotated[2], 0.0F));
}

TEST_F(Matrix3x3Test, StreamOutput) {
  Matrix3x3 matrix = Matrix3x3::Identity();
  std::ostringstream output_stream;
  output_stream << matrix;

  std::string output = output_stream.str();
  EXPECT_FALSE(output.empty());
}

TEST_F(Matrix3x3Test, StreamInput) {
  std::istringstream input_stream("1 0 0 0 1 0 0 0 1");
  Matrix3x3 matrix;
  input_stream >> matrix;

  EXPECT_TRUE(MatrixEqual(matrix, Matrix3x3::Identity()));
}

TEST_F(Matrix3x3Test, RotationPreservesLength) {
  float angle = M_PI / 4.0F;
  Matrix3x3 rotation = Matrix3x3::RotationAboutZ(angle);

  Vector3 vector(1.0F, 2.0F, 3.0F);
  Vector3 rotated = rotation * vector;

  float original_length = vector.GetMagnitude();
  float rotated_length = rotated.GetMagnitude();

  EXPECT_TRUE(FloatEqual(original_length, rotated_length));
}

TEST_F(Matrix3x3Test, InverseTimesOriginalIsIdentity) {
  Vector3 row_zero(1.0F, 2.0F, 3.0F);
  Vector3 row_one(0.0F, 1.0F, 4.0F);
  Vector3 row_two(5.0F, 6.0F, 0.0F);

  Matrix3x3 matrix(row_zero, row_one, row_two);
  Matrix3x3 inverse = matrix.GetInverse();
  Matrix3x3 product = matrix * inverse;

  // Should be close to identity (allowing for floating point error)
  EXPECT_TRUE(FloatEqual(product[0][0], 1.0F));
  EXPECT_TRUE(FloatEqual(product[1][1], 1.0F));
  EXPECT_TRUE(FloatEqual(product[2][2], 1.0F));
  EXPECT_TRUE(FloatEqual(product[0][1], 0.0F));
  EXPECT_TRUE(FloatEqual(product[0][2], 0.0F));
}

// NOLINTEND(readability-magic-numbers)