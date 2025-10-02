#include <gtest/gtest.h>

#include <cmath>

#include "planar_pinhole_camera.h"
#include "vector_3.h"

class PlanarPinholeCameraTest : public ::testing::Test {
protected:
  static constexpr float EPSILON = 1e-4f;

  auto FloatEqual(float first_value, float second_value) -> bool {
    return std::abs(first_value - second_value) < EPSILON;
  }

  auto VectorEqual(Vector3 first_vector, Vector3 second_vector) -> bool {
    return FloatEqual(first_vector[0], second_vector[0]) && FloatEqual(first_vector[1], second_vector[1]) &&
           FloatEqual(first_vector[2], second_vector[2]);
  }
};

TEST_F(PlanarPinholeCameraTest, DefaultConstructor) {
  PlanarPinholeCamera camera;
  // Just verify it compiles and doesn't crash
}

TEST_F(PlanarPinholeCameraTest, ParameterizedConstructor) {
  PlanarPinholeCamera camera(640, 480, M_PI / 3.0f);

  EXPECT_EQ(camera.width, 640);
  EXPECT_EQ(camera.height, 480);
  EXPECT_TRUE(FloatEqual(camera.horizontal_fov, M_PI / 3.0f));
}

TEST_F(PlanarPinholeCameraTest, GetSetHorizontalFov) {
  PlanarPinholeCamera camera(640, 480, M_PI / 3.0f);

  float field_of_view = camera.GetHorizontalFov();
  EXPECT_TRUE(FloatEqual(field_of_view, M_PI / 3.0f));

  camera.SetHorizontalFov(M_PI / 2.0f);
  EXPECT_TRUE(FloatEqual(camera.GetHorizontalFov(), M_PI / 2.0f));
}

TEST_F(PlanarPinholeCameraTest, Translate) {
  PlanarPinholeCamera camera(640, 480, M_PI / 3.0f);
  Vector3 initial_position = camera.position;

  Vector3 translation(1.0f, 2.0f, 3.0f);
  camera.Translate(translation);

  Vector3 expected_position(initial_position[0] + 1.0f, initial_position[1] + 2.0f, initial_position[2] + 3.0f);
  EXPECT_TRUE(VectorEqual(camera.position, expected_position));
}

TEST_F(PlanarPinholeCameraTest, Pose) {
  PlanarPinholeCamera camera(640, 480, M_PI / 3.0f);

  Vector3 new_position(5.0f, 5.0f, 5.0f);
  Vector3 look_at_point(0.0f, 0.0f, 0.0f);
  Vector3 up_vector(0.0f, 1.0f, 0.0f);

  camera.Pose(new_position, look_at_point, up_vector);

  EXPECT_TRUE(VectorEqual(camera.position, new_position));
  // View direction should point from position to look_at
  Vector3 view_direction = camera.GetViewDirection();
  EXPECT_GT(view_direction.GetMagnitude(), 0.0f);
}

TEST_F(PlanarPinholeCameraTest, Pan) {
  PlanarPinholeCamera camera(640, 480, M_PI / 3.0f);
  camera.Pose(Vector3(0.0f, 0.0f, 5.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));

  Vector3 initial_forward = camera.forward;
  camera.Pan(M_PI / 4.0f);

  // Forward vector should have changed
  EXPECT_FALSE(VectorEqual(camera.forward, initial_forward));
}

TEST_F(PlanarPinholeCameraTest, Tilt) {
  PlanarPinholeCamera camera(640, 480, M_PI / 3.0f);
  camera.Pose(Vector3(0.0f, 0.0f, 5.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));

  Vector3 initial_forward = camera.forward;
  camera.Tilt(M_PI / 4.0f);

  // Forward vector should have changed
  EXPECT_FALSE(VectorEqual(camera.forward, initial_forward));
}

TEST_F(PlanarPinholeCameraTest, Roll) {
  PlanarPinholeCamera camera(640, 480, M_PI / 3.0f);
  camera.Pose(Vector3(0.0f, 0.0f, 5.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));

  Vector3 initial_up = camera.up;
  camera.Roll(M_PI / 4.0f);

  // Up vector should have changed
  EXPECT_FALSE(VectorEqual(camera.up, initial_up));
}

TEST_F(PlanarPinholeCameraTest, Zoom) {
  PlanarPinholeCamera camera(640, 480, M_PI / 3.0f);

  float initial_field_of_view = camera.GetHorizontalFov();
  camera.Zoom(2.0f);

  // Zooming should change the FOV
  EXPECT_FALSE(FloatEqual(camera.GetHorizontalFov(), initial_field_of_view));
}

TEST_F(PlanarPinholeCameraTest, GetViewDirection) {
  PlanarPinholeCamera camera(640, 480, M_PI / 3.0f);
  camera.Pose(Vector3(0.0f, 0.0f, 5.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));

  Vector3 view_direction = camera.GetViewDirection();

  // View direction should be normalized
  EXPECT_TRUE(FloatEqual(view_direction.GetMagnitude(), 1.0f));

  // Should point roughly toward negative Z
  EXPECT_LT(view_direction[2], 0.0f);
}

TEST_F(PlanarPinholeCameraTest, GetFocalLength) {
  PlanarPinholeCamera camera(640, 480, M_PI / 3.0f);

  float focal_length = camera.GetFocalLength();

  // Focal length should be positive
  EXPECT_GT(focal_length, 0.0f);

  // For a given width and FOV, focal length should be deterministic
  float expected_focal_length = (camera.width / 2.0f) / std::tan(camera.horizontal_fov / 2.0f);
  EXPECT_TRUE(FloatEqual(focal_length, expected_focal_length));
}

TEST_F(PlanarPinholeCameraTest, ProjectUnproject) {
  PlanarPinholeCamera camera(640, 480, M_PI / 3.0f);
  camera.Pose(Vector3(0.0f, 0.0f, 5.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));

  // Project a point in front of the camera
  Vector3 point(0.0f, 0.0f, 0.0f);
  Vector3 projected_point;
  int projection_result = camera.Project(point, projected_point);

  // Projection should succeed for point in front of camera
  EXPECT_GE(projection_result, 0);

  // Projected point should be within image bounds (approximately)
  EXPECT_GE(projected_point[0], 0.0f);
  EXPECT_LT(projected_point[0], camera.width);
  EXPECT_GE(projected_point[1], 0.0f);
  EXPECT_LT(projected_point[1], camera.height);
}

TEST_F(PlanarPinholeCameraTest, UnprojectCenterPixel) {
  PlanarPinholeCamera camera(640, 480, M_PI / 3.0f);
  camera.Pose(Vector3(0.0f, 0.0f, 5.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));

  // Unproject the center pixel
  int u_coordinate = camera.width / 2;
  int v_coordinate = camera.height / 2;
  float inverse_depth = 0.2f; // 5 units away

  Vector3 unprojected_point = camera.Unproject(u_coordinate, v_coordinate, inverse_depth);

  // Unprojected point should be roughly at origin (where camera is looking)
  EXPECT_TRUE(FloatEqual(unprojected_point[0], 0.0f));
  EXPECT_TRUE(FloatEqual(unprojected_point[1], 0.0f));
}

TEST_F(PlanarPinholeCameraTest, ProjectBehindCamera) {
  PlanarPinholeCamera camera(640, 480, M_PI / 3.0f);
  camera.Pose(Vector3(0.0f, 0.0f, 5.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));

  // Point behind the camera
  Vector3 point_behind_camera(0.0f, 0.0f, 10.0f);
  Vector3 projected_point;
  int projection_result = camera.Project(point_behind_camera, projected_point);

  // Projection should fail (or return special value) for point behind camera
  EXPECT_LT(projection_result, 0);
}

TEST_F(PlanarPinholeCameraTest, OrthogonalBasisVectors) {
  PlanarPinholeCamera camera(640, 480, M_PI / 3.0f);
  camera.Pose(Vector3(0.0f, 0.0f, 5.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));

  // Right, up, and forward should be orthogonal
  float dot_product_right_up = camera.right.Dot(camera.up);
  float dot_product_right_forward = camera.right.Dot(camera.forward);
  float dot_product_up_forward = camera.up.Dot(camera.forward);

  EXPECT_TRUE(FloatEqual(dot_product_right_up, 0.0f));
  EXPECT_TRUE(FloatEqual(dot_product_right_forward, 0.0f));
  EXPECT_TRUE(FloatEqual(dot_product_up_forward, 0.0f));

  // All should be unit vectors
  EXPECT_TRUE(FloatEqual(camera.right.GetMagnitude(), 1.0f));
  EXPECT_TRUE(FloatEqual(camera.up.GetMagnitude(), 1.0f));
  EXPECT_TRUE(FloatEqual(camera.forward.GetMagnitude(), 1.0f));
}