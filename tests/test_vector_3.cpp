#include <gtest/gtest.h>

#include <cmath>
#include <sstream>

#include "graphics_pipeline/vector_3.h"

// NOLINTBEGIN(readability-magic-numbers)

class Vector3Test : public ::testing::Test {
protected:
  static constexpr float EPSILON = 1e-5F;

  static auto FloatEqual(float first_value, float second_value) -> bool {
    return std::abs(first_value - second_value) < EPSILON;
  }

  static auto VectorEqual(Vector3 first_vector, Vector3 second_vector) -> bool {
    return FloatEqual(first_vector[0], second_vector[0]) && FloatEqual(first_vector[1], second_vector[1]) &&
           FloatEqual(first_vector[2], second_vector[2]);
  }
};

TEST_F(Vector3Test, DefaultConstructor) {
  Vector3 vector;
  EXPECT_EQ(vector.coordinates.size(), 3);
}

TEST_F(Vector3Test, ParameterizedConstructor) {
  Vector3 vector(1.0F, 2.0F, 3.0F);
  EXPECT_FLOAT_EQ(vector[0], 1.0F);
  EXPECT_FLOAT_EQ(vector[1], 2.0F);
  EXPECT_FLOAT_EQ(vector[2], 3.0F);
}

TEST_F(Vector3Test, IndexOperator) {
  Vector3 vector(1.0F, 2.0F, 3.0F);
  EXPECT_FLOAT_EQ(vector[0], 1.0F);
  EXPECT_FLOAT_EQ(vector[1], 2.0F);
  EXPECT_FLOAT_EQ(vector[2], 3.0F);

  vector[0] = 10.0F;
  EXPECT_FLOAT_EQ(vector[0], 10.0F);
}

TEST_F(Vector3Test, Addition) {
  Vector3 first_vector(1.0F, 2.0F, 3.0F);
  Vector3 second_vector(4.0F, 5.0F, 6.0F);
  Vector3 result = first_vector + second_vector;

  EXPECT_FLOAT_EQ(result[0], 5.0F);
  EXPECT_FLOAT_EQ(result[1], 7.0F);
  EXPECT_FLOAT_EQ(result[2], 9.0F);
}

TEST_F(Vector3Test, Subtraction) {
  Vector3 first_vector(4.0F, 5.0F, 6.0F);
  Vector3 second_vector(1.0F, 2.0F, 3.0F);
  Vector3 result = first_vector - second_vector;

  EXPECT_FLOAT_EQ(result[0], 3.0F);
  EXPECT_FLOAT_EQ(result[1], 3.0F);
  EXPECT_FLOAT_EQ(result[2], 3.0F);
}

TEST_F(Vector3Test, ScalarMultiplication) {
  Vector3 vector(1.0F, 2.0F, 3.0F);
  Vector3 result = vector * 2.0F;

  EXPECT_FLOAT_EQ(result[0], 2.0F);
  EXPECT_FLOAT_EQ(result[1], 4.0F);
  EXPECT_FLOAT_EQ(result[2], 6.0F);
}

TEST_F(Vector3Test, ScalarDivision) {
  Vector3 vector(2.0F, 4.0F, 6.0F);
  Vector3 result = vector / 2.0F;

  EXPECT_FLOAT_EQ(result[0], 1.0F);
  EXPECT_FLOAT_EQ(result[1], 2.0F);
  EXPECT_FLOAT_EQ(result[2], 3.0F);
}

TEST_F(Vector3Test, Magnitude) {
  Vector3 vector(3.0F, 4.0F, 0.0F);
  EXPECT_FLOAT_EQ(vector.GetMagnitude(), 5.0F);

  Vector3 unit_vector(1.0F, 1.0F, 1.0F);
  EXPECT_TRUE(FloatEqual(unit_vector.GetMagnitude(), std::sqrt(3.0F)));
}

TEST_F(Vector3Test, Normalize) {
  Vector3 vector(3.0F, 4.0F, 0.0F);
  Vector3 normalized = vector.GetNormal();

  EXPECT_FLOAT_EQ(normalized[0], 0.6F);
  EXPECT_FLOAT_EQ(normalized[1], 0.8F);
  EXPECT_FLOAT_EQ(normalized[2], 0.0F);
  EXPECT_TRUE(FloatEqual(normalized.GetMagnitude(), 1.0F));
}

TEST_F(Vector3Test, DotProduct) {
  Vector3 first_vector(1.0F, 2.0F, 3.0F);
  Vector3 second_vector(4.0F, 5.0F, 6.0F);

  float dot_product = first_vector.Dot(second_vector);
  EXPECT_FLOAT_EQ(dot_product, 32.0F); // 1*4 + 2*5 + 3*6 = 4 + 10 + 18 = 32
}

TEST_F(Vector3Test, CrossProduct) {
  Vector3 first_vector(1.0F, 0.0F, 0.0F);
  Vector3 second_vector(0.0F, 1.0F, 0.0F);
  Vector3 result = first_vector.Cross(second_vector);

  EXPECT_FLOAT_EQ(result[0], 0.0F);
  EXPECT_FLOAT_EQ(result[1], 0.0F);
  EXPECT_FLOAT_EQ(result[2], 1.0F);
}

TEST_F(Vector3Test, RotateAboutAxis) {
  Vector3 point(1.0F, 0.0F, 0.0F);
  Vector3 origin(0.0F, 0.0F, 0.0F);
  Vector3 axis(0.0F, 0.0F, 1.0F);
  float angle = M_PI / 2.0F; // 90 degrees

  Vector3 rotated = point.RotateAboutAxis(origin, axis, angle);

  EXPECT_TRUE(FloatEqual(rotated[0], 0.0F));
  EXPECT_TRUE(FloatEqual(rotated[1], 1.0F));
  EXPECT_TRUE(FloatEqual(rotated[2], 0.0F));
}

TEST_F(Vector3Test, ColorGetSet) {
  Vector3 vector(0.5F, 0.75F, 0.25F);

  unsigned int color = vector.GetColor();

  Vector3 vector2;
  vector2.SetColor(color);

  // After round-trip, values should be approximately equal
  // Note: Due to quantization to 8-bit, some precision is lost
  EXPECT_NEAR(vector[0], vector2[0], 0.01F);
  EXPECT_NEAR(vector[1], vector2[1], 0.01F);
  EXPECT_NEAR(vector[2], vector2[2], 0.01F);
}

TEST_F(Vector3Test, StreamOutput) {
  Vector3 vector(1.0F, 2.0F, 3.0F);
  std::ostringstream output_stream;
  output_stream << vector;

  std::string output = output_stream.str();
  EXPECT_FALSE(output.empty());
}

TEST_F(Vector3Test, StreamInput) {
  std::istringstream input_stream("1.0 2.0 3.0");
  Vector3 vector;
  input_stream >> vector;

  EXPECT_FLOAT_EQ(vector[0], 1.0F);
  EXPECT_FLOAT_EQ(vector[1], 2.0F);
  EXPECT_FLOAT_EQ(vector[2], 3.0F);
}

TEST_F(Vector3Test, LightingCalculation) {
  Vector3 color(1.0F, 1.0F, 1.0F);
  Vector3 normal(0.0F, 0.0F, 1.0F);
  Vector3 light_direction(0.0F, 0.0F, -1.0F);
  float ambient_coefficient = 0.1F;

  Vector3 lit_color = color.Light(normal, light_direction, ambient_coefficient);

  // Should have some lighting effect
  EXPECT_GE(lit_color[0], 0.0F);
  EXPECT_LE(lit_color[0], 1.0F);
}

TEST_F(Vector3Test, Reflection) {
  Vector3 surface_normal(0.0F, 1.0F, 0.0F);
  Vector3 light_direction(1.0F, -1.0F, 0.0F);

  Vector3 reflected = surface_normal.Reflect(light_direction);

  // Based on the actual implementation:
  // normal_component = surface_normal * (surface_normal.Dot(light_direction))
  // normal_component = (0,1,0) * ((0,1,0)·(1,-1,0)) = (0,1,0) * (-1) = (0,-1,0)
  // tangent_component = light_direction - normal_component = (1,-1,0) - (0,-1,0) = (1,0,0)
  // reflected = normal_component - tangent_component = (0,-1,0) - (1,0,0) = (-1,-1,0)

  EXPECT_TRUE(FloatEqual(reflected[0], -1.0F));
  EXPECT_TRUE(FloatEqual(reflected[1], -1.0F));
  EXPECT_TRUE(FloatEqual(reflected[2], 0.0F));
}

// NOLINTEND(readability-magic-numbers)