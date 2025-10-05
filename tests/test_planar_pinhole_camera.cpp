#include <gtest/gtest.h>

#include <cmath>

#include "graphics_pipeline/planar_pinhole_camera.h"
#include "graphics_pipeline/vector_3.h"

// NOLINTBEGIN(readability-magic-numbers)

class PlanarPinholeCameraTest : public ::testing::Test {
protected:
  static constexpr float EPSILON = 1e-4F;

  static auto FloatEqual(float first_value, float second_value) -> bool {
    return std::abs(first_value - second_value) < EPSILON;
  }

  static auto VectorEqual(Vector3 first_vector, Vector3 second_vector) -> bool {
    return FloatEqual(first_vector[0], second_vector[0]) && FloatEqual(first_vector[1], second_vector[1]) &&
           FloatEqual(first_vector[2], second_vector[2]);
  }
};

TEST_F(PlanarPinholeCameraTest, DefaultConstructor) {
  PlanarPinholeCamera camera;
  // Just verify it compiles and doesn't crash
}

TEST_F(PlanarPinholeCameraTest, ParameterizedConstructor) {
  PlanarPinholeCamera camera(640, 480, M_PI / 3.0F);

  EXPECT_EQ(camera.width, 640);
  EXPECT_EQ(camera.height, 480);
  EXPECT_TRUE(FloatEqual(camera.horizontal_fov, M_PI / 3.0F));
}

TEST_F(PlanarPinholeCameraTest, GetSetHorizontalFov) {
  PlanarPinholeCamera camera(640, 480, M_PI / 3.0F);

  float field_of_view = camera.GetHorizontalFov();
  EXPECT_TRUE(FloatEqual(field_of_view, M_PI / 3.0F));

  camera.SetHorizontalFov(M_PI / 2.0F);
  EXPECT_TRUE(FloatEqual(camera.GetHorizontalFov(), M_PI / 2.0F));
}

TEST_F(PlanarPinholeCameraTest, Translate) {
  PlanarPinholeCamera camera(640, 480, M_PI / 3.0F);
  Vector3 initial_position = camera.position;

  Vector3 translation(1.0F, 2.0F, 3.0F);
  camera.Translate(translation);

  Vector3 expected_position(initial_position[0] + 1.0F, initial_position[1] + 2.0F, initial_position[2] + 3.0F);
  EXPECT_TRUE(VectorEqual(camera.position, expected_position));
}

TEST_F(PlanarPinholeCameraTest, Pose) {
  PlanarPinholeCamera camera(640, 480, M_PI / 3.0F);

  Vector3 new_position(5.0F, 5.0F, 5.0F);
  Vector3 look_at_point(0.0F, 0.0F, 0.0F);
  Vector3 up_vector(0.0F, 1.0F, 0.0F);

  camera.Pose(new_position, look_at_point, up_vector);

  EXPECT_TRUE(VectorEqual(camera.position, new_position));
  // View direction should point from position to look_at
  Vector3 view_direction = camera.GetViewDirection();
  EXPECT_GT(view_direction.GetMagnitude(), 0.0F);
}

TEST_F(PlanarPinholeCameraTest, Pan) {
  PlanarPinholeCamera camera(640, 480, M_PI / 3.0F);
  camera.Pose(Vector3(0.0F, 0.0F, 5.0F), Vector3(0.0F, 0.0F, 0.0F), Vector3(0.0F, 1.0F, 0.0F));

  Vector3 initial_forward = camera.forward;
  camera.Pan(M_PI / 4.0F);

  // Forward vector should have changed
  EXPECT_FALSE(VectorEqual(camera.forward, initial_forward));
}

TEST_F(PlanarPinholeCameraTest, Tilt) {
  PlanarPinholeCamera camera(640, 480, M_PI / 3.0F);
  camera.Pose(Vector3(0.0F, 0.0F, 5.0F), Vector3(0.0F, 0.0F, 0.0F), Vector3(0.0F, 1.0F, 0.0F));

  Vector3 initial_forward = camera.forward;
  camera.Tilt(M_PI / 4.0F);

  // Forward vector should have changed
  EXPECT_FALSE(VectorEqual(camera.forward, initial_forward));
}

TEST_F(PlanarPinholeCameraTest, Roll) {
  PlanarPinholeCamera camera(640, 480, M_PI / 3.0F);
  camera.Pose(Vector3(0.0F, 0.0F, 5.0F), Vector3(0.0F, 0.0F, 0.0F), Vector3(0.0F, 1.0F, 0.0F));

  Vector3 initial_up = camera.up;
  camera.Roll(M_PI / 4.0F);

  // Up vector should have changed
  EXPECT_FALSE(VectorEqual(camera.up, initial_up));
}

TEST_F(PlanarPinholeCameraTest, Zoom) {
  PlanarPinholeCamera camera(640, 480, M_PI / 3.0F);

  float initial_field_of_view = camera.GetHorizontalFov();
  camera.Zoom(2.0F);

  // Zooming should change the FOV
  EXPECT_FALSE(FloatEqual(camera.GetHorizontalFov(), initial_field_of_view));
}

TEST_F(PlanarPinholeCameraTest, GetViewDirection) {
  PlanarPinholeCamera camera(640, 480, M_PI / 3.0F);
  camera.Pose(Vector3(0.0F, 0.0F, 5.0F), Vector3(0.0F, 0.0F, 0.0F), Vector3(0.0F, 1.0F, 0.0F));

  Vector3 view_direction = camera.GetViewDirection();

  // View direction should be normalized and equal to forward
  EXPECT_TRUE(FloatEqual(view_direction.GetMagnitude(), 1.0F));
  EXPECT_TRUE(VectorEqual(view_direction, camera.forward));

  // Should point toward negative Z (from (0,0,5) to (0,0,0))
  EXPECT_LT(view_direction[2], 0.0F);
  EXPECT_TRUE(FloatEqual(view_direction[0], 0.0F));
  EXPECT_TRUE(FloatEqual(view_direction[1], 0.0F));
  EXPECT_TRUE(FloatEqual(view_direction[2], -1.0F));
}

TEST_F(PlanarPinholeCameraTest, GetFocalLength) {
  PlanarPinholeCamera camera(640, 480, M_PI / 3.0F);

  float focal_length = camera.GetFocalLength();

  // Focal length should be positive
  EXPECT_GT(focal_length, 0.0F);

  // For a given width and FOV, focal length follows pinhole camera formula
  float expected_focal_length = ((float)camera.width / 2.0F) / std::tan(camera.horizontal_fov / 2.0F);
  EXPECT_TRUE(FloatEqual(focal_length, expected_focal_length));
}

TEST_F(PlanarPinholeCameraTest, ProjectUnproject) {
  PlanarPinholeCamera camera(640, 480, M_PI / 3.0F);
  camera.Pose(Vector3(0.0F, 0.0F, 5.0F), Vector3(0.0F, 0.0F, 0.0F), Vector3(0.0F, 1.0F, 0.0F));

  // Project a point in front of the camera
  Vector3 point(0.0F, 0.0F, 0.0F);
  Vector3 projected_point;
  int projection_result = camera.Project(point, projected_point);

  // Projection should succeed for point in front of camera
  EXPECT_GE(projection_result, 0);

  // Projected point should be within image bounds (approximately)
  EXPECT_GE(projected_point[0], 0.0F);
  EXPECT_LT(projected_point[0], camera.width);
  EXPECT_GE(projected_point[1], 0.0F);
  EXPECT_LT(projected_point[1], camera.height);
}

TEST_F(PlanarPinholeCameraTest, UnprojectCenterPixel) {
  PlanarPinholeCamera camera(640, 480, M_PI / 3.0F);
  camera.Pose(Vector3(0.0F, 0.0F, 5.0F), Vector3(0.0F, 0.0F, 0.0F), Vector3(0.0F, 1.0F, 0.0F));

  // Unproject the center pixel
  int u_coordinate = camera.width / 2;
  int v_coordinate = camera.height / 2;
  float inverse_depth = 0.2F; // depth = 5 units (1/0.2 = 5)

  Vector3 unprojected_point = camera.Unproject(u_coordinate, v_coordinate, inverse_depth);

  // The center pixel at depth 5 should unproject to approximately the look-at point (0,0,0)
  EXPECT_NEAR(unprojected_point[0], 0.0F, 0.01F);
  EXPECT_NEAR(unprojected_point[1], 0.0F, 0.01F);
  EXPECT_NEAR(unprojected_point[2], 0.0F, 0.01F);
}

TEST_F(PlanarPinholeCameraTest, ProjectBehindCamera) {
  PlanarPinholeCamera camera(640, 480, M_PI / 3.0F);
  camera.Pose(Vector3(0.0F, 0.0F, 5.0F), Vector3(0.0F, 0.0F, 0.0F), Vector3(0.0F, 1.0F, 0.0F));

  // Point behind the camera
  Vector3 point_behind_camera(0.0F, 0.0F, 10.0F);
  Vector3 projected_point;
  int projection_result = camera.Project(point_behind_camera, projected_point);

  // Projection should return 0 for point behind camera (camera_space_point[2] <= 0)
  EXPECT_EQ(projection_result, 0);
}

TEST_F(PlanarPinholeCameraTest, OrthogonalBasisVectors) {
  PlanarPinholeCamera camera(640, 480, M_PI / 3.0F);
  camera.Pose(Vector3(0.0F, 0.0F, 5.0F), Vector3(0.0F, 0.0F, 0.0F), Vector3(0.0F, 1.0F, 0.0F));

  // After the fix, right, up, and forward should all be orthonormal unit vectors
  float dot_product_right_up = camera.right.Dot(camera.up);
  float dot_product_right_forward = camera.right.Dot(camera.forward);
  float dot_product_up_forward = camera.up.Dot(camera.forward);

  EXPECT_TRUE(FloatEqual(dot_product_right_up, 0.0F));
  EXPECT_TRUE(FloatEqual(dot_product_right_forward, 0.0F));
  EXPECT_TRUE(FloatEqual(dot_product_up_forward, 0.0F));

  // All should be unit vectors
  EXPECT_TRUE(FloatEqual(camera.right.GetMagnitude(), 1.0F));
  EXPECT_TRUE(FloatEqual(camera.up.GetMagnitude(), 1.0F));
  EXPECT_TRUE(FloatEqual(camera.forward.GetMagnitude(), 1.0F));
}

// NOLINTEND(readability-magic-numbers)