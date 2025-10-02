#include <gtest/gtest.h>

#include <cmath>

#include "graphics_pipeline/triangle_mesh.h"
#include "graphics_pipeline/vector_3.h"

// NOLINTBEGIN(readability-magic-numbers)

class TriangleMeshTest : public ::testing::Test {
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

TEST_F(TriangleMeshTest, DefaultConstructor) {
  TriangleMesh mesh;

  EXPECT_TRUE(mesh.vertices.empty());
  EXPECT_TRUE(mesh.normals.empty());
  EXPECT_TRUE(mesh.colors.empty());
  EXPECT_TRUE(mesh.triangles.empty());
}

TEST_F(TriangleMeshTest, AxisAlignedBox) {
  Vector3 min_corner(0.0F, 0.0F, 0.0F);
  Vector3 max_corner(1.0F, 1.0F, 1.0F);
  unsigned int color = 0xFFFFFFFF;

  TriangleMesh box = TriangleMesh::AxisAlignedBox(min_corner, max_corner, color);

  // A box should have 8 vertices
  EXPECT_EQ(box.vertices.size(), 8);

  // Should have 12 triangles (2 per face * 6 faces)
  EXPECT_EQ(box.triangles.size(), 36); // 12 triangles * 3 indices

  // Should have normals
  EXPECT_FALSE(box.normals.empty());
}

TEST_F(TriangleMeshTest, Sphere) {
  Vector3 position(0.0F, 0.0F, 0.0F);
  float radius = 1.0F;
  int subdivisions = 2;
  unsigned int color = 0xFF0000FF;

  TriangleMesh sphere = TriangleMesh::Sphere(position, radius, subdivisions, color);

  // Sphere should have vertices
  EXPECT_GT(sphere.vertices.size(), 0);

  // Should have triangles
  EXPECT_GT(sphere.triangles.size(), 0);

  // All vertices should be approximately 'radius' distance from center
  for (size_t index = 0; index < sphere.vertices.size(); ++index) {
    Vector3 vertex = sphere.vertices[index];
    Vector3 difference_vector = vertex - position;
    float distance = difference_vector.GetMagnitude();
    EXPECT_TRUE(FloatEqual(distance, radius));
  }
}

TEST_F(TriangleMeshTest, Cylinder) {
  Vector3 position(0.0F, 0.0F, 0.0F);
  float radius = 1.0F;
  float height = 2.0F;
  int subdivisions = 8;
  unsigned int color = 0xFF00FF00;

  TriangleMesh cylinder = TriangleMesh::Cylinder(position, radius, height, subdivisions, color);

  // Cylinder should have vertices
  EXPECT_GT(cylinder.vertices.size(), 0);

  // Should have triangles
  EXPECT_GT(cylinder.triangles.size(), 0);

  // Should have normals
  EXPECT_FALSE(cylinder.normals.empty());
}

TEST_F(TriangleMeshTest, GetSetPosition) {
  TriangleMesh box = TriangleMesh::AxisAlignedBox(Vector3(0.0F, 0.0F, 0.0F), Vector3(1.0F, 1.0F, 1.0F), 0xFFFFFFFF);

  Vector3 initial_position = box.GetPosition();

  Vector3 new_position(5.0F, 5.0F, 5.0F);
  box.SetPosition(new_position);

  Vector3 current_position = box.GetPosition();
  EXPECT_TRUE(VectorEqual(current_position, new_position));
}

TEST_F(TriangleMeshTest, Translate) {
  TriangleMesh box = TriangleMesh::AxisAlignedBox(Vector3(0.0F, 0.0F, 0.0F), Vector3(1.0F, 1.0F, 1.0F), 0xFFFFFFFF);

  Vector3 initial_vertex = box.vertices[0];
  Vector3 translation(1.0F, 2.0F, 3.0F);

  box.Translate(translation);

  Vector3 translated_vertex = box.vertices[0];
  Vector3 expected_vertex = initial_vertex + translation;

  EXPECT_TRUE(VectorEqual(translated_vertex, expected_vertex));
}

TEST_F(TriangleMeshTest, Scale) {
  TriangleMesh box = TriangleMesh::AxisAlignedBox(Vector3(-1.0F, -1.0F, -1.0F), Vector3(1.0F, 1.0F, 1.0F), 0xFFFFFFFF);

  Vector3 center_position = box.GetPosition();
  Vector3 initial_vertex = box.vertices[0];
  float initial_distance = (initial_vertex - center_position).GetMagnitude();

  float scale_factor = 2.0F;
  box.Scale(scale_factor);

  Vector3 scaled_vertex = box.vertices[0];
  float scaled_distance = (scaled_vertex - center_position).GetMagnitude();

  EXPECT_TRUE(FloatEqual(scaled_distance, initial_distance * scale_factor));
}

TEST_F(TriangleMeshTest, RotateAboutAxis) {
  TriangleMesh box = TriangleMesh::AxisAlignedBox(Vector3(0.0F, 0.0F, 0.0F), Vector3(1.0F, 1.0F, 1.0F), 0xFFFFFFFF);

  Vector3 origin(0.5F, 0.5F, 0.5F);
  Vector3 axis(0.0F, 0.0F, 1.0F);
  float angle = M_PI / 2.0F;

  Vector3 initial_vertex = box.vertices[0];
  box.RotateAboutAxis(origin, axis, angle);

  // Vertex should have moved after rotation
  EXPECT_FALSE(VectorEqual(box.vertices[0], initial_vertex));
}

TEST_F(TriangleMeshTest, LightDirection) {
  TriangleMesh sphere = TriangleMesh::Sphere(Vector3(0.0F, 0.0F, 0.0F), 1.0F, 2, 0xFFFFFFFF);

  Vector3 light_direction(0.0F, 0.0F, -1.0F);
  float ambient_coefficient = 0.2F;

  // Store initial colors
  std::vector<Vector3> initial_colors = sphere.colors;

  sphere.LightDirection(light_direction, ambient_coefficient);

  // Colors should be modified by lighting
  bool colors_changed = false;
  for (size_t index = 0; index < sphere.colors.size(); ++index) {
    if (!VectorEqual(sphere.colors[index], initial_colors[index])) {
      colors_changed = true;
      break;
    }
  }
  EXPECT_TRUE(colors_changed);
}

TEST_F(TriangleMeshTest, LightPoint) {
  TriangleMesh sphere = TriangleMesh::Sphere(Vector3(0.0F, 0.0F, 0.0F), 1.0F, 2, 0xFFFFFFFF);

  Vector3 light_point(5.0F, 5.0F, 5.0F);
  float ambient_coefficient = 0.2F;

  // Store initial colors
  std::vector<Vector3> initial_colors = sphere.colors;

  sphere.LightPoint(light_point, ambient_coefficient);

  // Colors should be modified by lighting
  bool colors_changed = false;
  for (size_t index = 0; index < sphere.colors.size(); ++index) {
    if (!VectorEqual(sphere.colors[index], initial_colors[index])) {
      colors_changed = true;
      break;
    }
  }
  EXPECT_TRUE(colors_changed);
}

TEST_F(TriangleMeshTest, TriangleIndicesValid) {
  TriangleMesh box = TriangleMesh::AxisAlignedBox(Vector3(0.0F, 0.0F, 0.0F), Vector3(1.0F, 1.0F, 1.0F), 0xFFFFFFFF);

  // All triangle indices should be valid (within vertex array bounds)
  for (unsigned int triangle_index : box.triangles) {
    EXPECT_LT(triangle_index, box.vertices.size());
  }
}

TEST_F(TriangleMeshTest, NormalsExistForVertices) {
  TriangleMesh sphere = TriangleMesh::Sphere(Vector3(0.0F, 0.0F, 0.0F), 1.0F, 2, 0xFFFFFFFF);

  // Should have same number of normals as vertices
  EXPECT_EQ(sphere.normals.size(), sphere.vertices.size());
}

TEST_F(TriangleMeshTest, ColorsExistForVertices) {
  TriangleMesh sphere = TriangleMesh::Sphere(Vector3(0.0F, 0.0F, 0.0F), 1.0F, 2, 0xFFFFFFFF);

  // Should have same number of colors as vertices
  EXPECT_EQ(sphere.colors.size(), sphere.vertices.size());
}

TEST_F(TriangleMeshTest, SphereNormalsPointOutward) {
  TriangleMesh sphere = TriangleMesh::Sphere(Vector3(0.0F, 0.0F, 0.0F), 1.0F, 2, 0xFFFFFFFF);

  // For a sphere at origin, normals should point in same direction as vertices
  for (size_t index = 0; index < sphere.vertices.size(); ++index) {
    Vector3 vertex = sphere.vertices[index];
    Vector3 normal = sphere.normals[index];

    // Normalize vertex (which should point radially outward from center)
    Vector3 radial_direction = vertex.GetNormal();

    // Normal should be roughly parallel to radial direction
    float dot_product = normal.Dot(radial_direction);
    EXPECT_GT(dot_product, 0.5F); // Should point generally outward
  }
}

// NOLINTEND(readability-magic-numbers)