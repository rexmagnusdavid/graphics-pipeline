#include <gtest/gtest.h>

#include <cmath>
#include <sstream>

#include "vector_3.h"

class Vector3Test : public ::testing::Test {
protected:
  static constexpr float EPSILON = 1e-5f;

  auto FloatEqual(float first_value, float second_value) -> bool {
    return std::abs(first_value - second_value) < EPSILON;
  }

  auto VectorEqual(Vector3 first_vector, Vector3 second_vector) -> bool {
    return FloatEqual(first_vector[0], second_vector[0]) && FloatEqual(first_vector[1], second_vector[1]) &&
           FloatEqual(first_vector[2], second_vector[2]);
  }
};

TEST_F(Vector3Test, DefaultConstructor) {
  Vector3 vector;
  EXPECT_EQ(vector.coordinates.size(), 3);
}

TEST_F(Vector3Test, ParameterizedConstructor) {
  Vector3 vector(1.0f, 2.0f, 3.0f);
  EXPECT_FLOAT_EQ(vector[0], 1.0f);
  EXPECT_FLOAT_EQ(vector[1], 2.0f);
  EXPECT_FLOAT_EQ(vector[2], 3.0f);
}

TEST_F(Vector3Test, IndexOperator) {
  Vector3 vector(1.0f, 2.0f, 3.0f);
  EXPECT_FLOAT_EQ(vector[0], 1.0f);
  EXPECT_FLOAT_EQ(vector[1], 2.0f);
  EXPECT_FLOAT_EQ(vector[2], 3.0f);

  vector[0] = 10.0f;
  EXPECT_FLOAT_EQ(vector[0], 10.0f);
}

TEST_F(Vector3Test, Addition) {
  Vector3 first_vector(1.0f, 2.0f, 3.0f);
  Vector3 second_vector(4.0f, 5.0f, 6.0f);
  Vector3 result = first_vector + second_vector;

  EXPECT_FLOAT_EQ(result[0], 5.0f);
  EXPECT_FLOAT_EQ(result[1], 7.0f);
  EXPECT_FLOAT_EQ(result[2], 9.0f);
}

TEST_F(Vector3Test, Subtraction) {
  Vector3 first_vector(4.0f, 5.0f, 6.0f);
  Vector3 second_vector(1.0f, 2.0f, 3.0f);
  Vector3 result = first_vector - second_vector;

  EXPECT_FLOAT_EQ(result[0], 3.0f);
  EXPECT_FLOAT_EQ(result[1], 3.0f);
  EXPECT_FLOAT_EQ(result[2], 3.0f);
}

TEST_F(Vector3Test, ScalarMultiplication) {
  Vector3 vector(1.0f, 2.0f, 3.0f);
  Vector3 result = vector * 2.0f;

  EXPECT_FLOAT_EQ(result[0], 2.0f);
  EXPECT_FLOAT_EQ(result[1], 4.0f);
  EXPECT_FLOAT_EQ(result[2], 6.0f);
}

TEST_F(Vector3Test, ScalarDivision) {
  Vector3 vector(2.0f, 4.0f, 6.0f);
  Vector3 result = vector / 2.0f;

  EXPECT_FLOAT_EQ(result[0], 1.0f);
  EXPECT_FLOAT_EQ(result[1], 2.0f);
  EXPECT_FLOAT_EQ(result[2], 3.0f);
}

TEST_F(Vector3Test, Magnitude) {
  Vector3 vector(3.0f, 4.0f, 0.0f);
  EXPECT_FLOAT_EQ(vector.GetMagnitude(), 5.0f);

  Vector3 unit_vector(1.0f, 1.0f, 1.0f);
  EXPECT_TRUE(FloatEqual(unit_vector.GetMagnitude(), std::sqrt(3.0f)));
}

TEST_F(Vector3Test, Normalize) {
  Vector3 vector(3.0f, 4.0f, 0.0f);
  Vector3 normalized = vector.GetNormal();

  EXPECT_FLOAT_EQ(normalized[0], 0.6f);
  EXPECT_FLOAT_EQ(normalized[1], 0.8f);
  EXPECT_FLOAT_EQ(normalized[2], 0.0f);
  EXPECT_TRUE(FloatEqual(normalized.GetMagnitude(), 1.0f));
}

TEST_F(Vector3Test, DotProduct) {
  Vector3 first_vector(1.0f, 2.0f, 3.0f);
  Vector3 second_vector(4.0f, 5.0f, 6.0f);

  float dot_product = first_vector.Dot(second_vector);
  EXPECT_FLOAT_EQ(dot_product, 32.0f); // 1*4 + 2*5 + 3*6 = 4 + 10 + 18 = 32
}

TEST_F(Vector3Test, CrossProduct) {
  Vector3 first_vector(1.0f, 0.0f, 0.0f);
  Vector3 second_vector(0.0f, 1.0f, 0.0f);
  Vector3 result = first_vector.Cross(second_vector);

  EXPECT_FLOAT_EQ(result[0], 0.0f);
  EXPECT_FLOAT_EQ(result[1], 0.0f);
  EXPECT_FLOAT_EQ(result[2], 1.0f);
}

TEST_F(Vector3Test, RotateAboutAxis) {
  Vector3 point(1.0f, 0.0f, 0.0f);
  Vector3 origin(0.0f, 0.0f, 0.0f);
  Vector3 axis(0.0f, 0.0f, 1.0f);
  float angle = M_PI / 2.0f; // 90 degrees

  Vector3 rotated = point.RotateAboutAxis(origin, axis, angle);

  EXPECT_TRUE(FloatEqual(rotated[0], 0.0f));
  EXPECT_TRUE(FloatEqual(rotated[1], 1.0f));
  EXPECT_TRUE(FloatEqual(rotated[2], 0.0f));
}

TEST_F(Vector3Test, ColorGetSet) {
  Vector3 vector;
  unsigned int color = 0xFF00FF00; // Green in ARGB
  vector.SetColor(color);

  unsigned int retrieved_color = vector.GetColor();
  EXPECT_EQ(retrieved_color, color);
}

TEST_F(Vector3Test, StreamOutput) {
  Vector3 vector(1.0f, 2.0f, 3.0f);
  std::ostringstream output_stream;
  output_stream << vector;

  std::string output = output_stream.str();
  EXPECT_FALSE(output.empty());
}

TEST_F(Vector3Test, StreamInput) {
  std::istringstream input_stream("1.0 2.0 3.0");
  Vector3 vector;
  input_stream >> vector;

  EXPECT_FLOAT_EQ(vector[0], 1.0f);
  EXPECT_FLOAT_EQ(vector[1], 2.0f);
  EXPECT_FLOAT_EQ(vector[2], 3.0f);
}

TEST_F(Vector3Test, LightingCalculation) {
  Vector3 color(1.0f, 1.0f, 1.0f);
  Vector3 normal(0.0f, 0.0f, 1.0f);
  Vector3 light_direction(0.0f, 0.0f, -1.0f);
  float ambient_coefficient = 0.1f;

  Vector3 lit_color = color.Light(normal, light_direction, ambient_coefficient);

  // Should have some lighting effect
  EXPECT_GE(lit_color[0], 0.0f);
  EXPECT_LE(lit_color[0], 1.0f);
}

TEST_F(Vector3Test, Reflection) {
  Vector3 normal(0.0f, 1.0f, 0.0f);
  Vector3 light_direction(1.0f, -1.0f, 0.0f);

  Vector3 reflected = normal.Reflect(light_direction);

  // Reflected vector should have y component flipped
  EXPECT_TRUE(FloatEqual(reflected[0], 1.0f));
  EXPECT_TRUE(FloatEqual(reflected[1], 1.0f));
  EXPECT_TRUE(FloatEqual(reflected[2], 0.0f));
}