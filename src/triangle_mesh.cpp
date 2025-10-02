#include "graphics_pipeline/triangle_mesh.h"

#include <cmath>
#include <fstream>

void TriangleMesh::LoadBinary(char *file_name) {
  std::ifstream file(file_name, std::ios::binary);
  if (!file.is_open()) {
    return;
  }

  size_t num_vertices;
  file.read(reinterpret_cast<char *>(&num_vertices), sizeof(size_t));
  vertices.resize(num_vertices);
  for (size_t i = 0; i < num_vertices; ++i) {
    file.read(reinterpret_cast<char *>(&vertices[i][0]), sizeof(float));
    file.read(reinterpret_cast<char *>(&vertices[i][1]), sizeof(float));
    file.read(reinterpret_cast<char *>(&vertices[i][2]), sizeof(float));
  }

  size_t num_colors;
  file.read(reinterpret_cast<char *>(&num_colors), sizeof(size_t));
  colors.resize(num_colors);
  for (size_t i = 0; i < num_colors; ++i) {
    file.read(reinterpret_cast<char *>(&colors[i][0]), sizeof(float));
    file.read(reinterpret_cast<char *>(&colors[i][1]), sizeof(float));
    file.read(reinterpret_cast<char *>(&colors[i][2]), sizeof(float));
  }

  size_t num_triangles;
  file.read(reinterpret_cast<char *>(&num_triangles), sizeof(size_t));
  triangles.resize(num_triangles);
  for (size_t i = 0; i < num_triangles; ++i) {
    file.read(reinterpret_cast<char *>(&triangles[i]), sizeof(unsigned int));
  }

  file.close();
}

void TriangleMesh::SaveBinary(char *file_name) {
  std::ofstream file(file_name, std::ios::binary);
  if (!file.is_open()) {
    return;
  }

  size_t num_vertices = vertices.size();
  file.write(reinterpret_cast<const char *>(&num_vertices), sizeof(size_t));
  for (auto &vertex : vertices) {
    file.write(reinterpret_cast<const char *>(&vertex[0]), sizeof(float));
    file.write(reinterpret_cast<const char *>(&vertex[1]), sizeof(float));
    file.write(reinterpret_cast<const char *>(&vertex[2]), sizeof(float));
  }

  size_t num_colors = colors.size();
  file.write(reinterpret_cast<const char *>(&num_colors), sizeof(size_t));
  for (auto &color : colors) {
    file.write(reinterpret_cast<const char *>(&color[0]), sizeof(float));
    file.write(reinterpret_cast<const char *>(&color[1]), sizeof(float));
    file.write(reinterpret_cast<const char *>(&color[2]), sizeof(float));
  }

  size_t num_triangles = triangles.size();
  file.write(reinterpret_cast<const char *>(&num_triangles), sizeof(size_t));
  for (const auto &triangle : triangles) {
    file.write(reinterpret_cast<const char *>(&triangle), sizeof(unsigned int));
  }

  file.close();
}

auto TriangleMesh::GetPosition() -> Vector3 {
  Vector3 ret;

  for (auto &vertex : vertices) {
    ret = ret + vertex;
  }

  ret = ret / (float)vertices.size();

  return ret;
}

void TriangleMesh::SetPosition(Vector3 new_position) { Translate(GetPosition() - new_position); }

void TriangleMesh::Scale(float factor) {
  Vector3 position = GetPosition();
  for (auto &vertex : vertices) {
    vertex = position + (vertex - position) * factor;
  }
}

void TriangleMesh::Translate(Vector3 vector) {
  for (auto &vertex : vertices) {
    vertex = vertex + vector;
  }
}

void TriangleMesh::RotateAboutAxis(Vector3 origin, Vector3 direction, float angle) {
  for (auto &vertex : vertices) {
    vertex = vertex.RotateAboutAxis(origin, direction, angle);
  }
}

auto TriangleMesh::AxisAlignedBox(Vector3 corner_0, Vector3 corner_1, unsigned int color) -> TriangleMesh {
  TriangleMesh ret;

  constexpr int NUM_BOX_VERTICES = 8;
  constexpr std::array<unsigned int, 36> BOX_TRIANGLE_INDICES = {
      0, 1, 2, 0, 2, 3, // Front face
      1, 5, 6, 1, 6, 2, // Right face
      5, 4, 7, 5, 7, 6, // Back face
      4, 0, 3, 4, 3, 7, // Left face
      3, 2, 6, 3, 6, 7, // Top face
      4, 5, 1, 4, 1, 0  // Bottom face
  };

  ret.vertices = {Vector3(corner_0[0], corner_0[1], corner_0[2]), Vector3(corner_1[0], corner_0[1], corner_0[2]),
                  Vector3(corner_1[0], corner_1[1], corner_0[2]), Vector3(corner_0[0], corner_1[1], corner_0[2]),
                  Vector3(corner_0[0], corner_0[1], corner_1[2]), Vector3(corner_1[0], corner_0[1], corner_1[2]),
                  Vector3(corner_1[0], corner_1[1], corner_1[2]), Vector3(corner_0[0], corner_1[1], corner_1[2])};
  Vector3 vertex_color;
  vertex_color.SetColor(color);
  ret.colors.resize(NUM_BOX_VERTICES, vertex_color);
  ret.triangles.assign(std::begin(BOX_TRIANGLE_INDICES), std::end(BOX_TRIANGLE_INDICES));

  return ret;
}

auto TriangleMesh::Sphere(Vector3 position, float radius, int subdivisions, unsigned int color) -> TriangleMesh {
  TriangleMesh ret;

  Vector3 vertex_color;
  vertex_color.SetColor(color);

  for (int lat = 0; lat <= subdivisions; ++lat) {
    float theta = (float)lat * std::numbers::pi_v<float> / (float)subdivisions;
    float sin_theta = sinf(theta);
    float cos_theta = cosf(theta);

    for (int lon = 0; lon <= subdivisions; ++lon) {
      float phi = (float)lon * (float)2 * std::numbers::pi_v<float> / (float)subdivisions;
      float sin_phi = sinf(phi);
      float cos_phi = cosf(phi);

      float x_coordinate = radius * sin_theta * cos_phi;
      float y_coordinate = radius * cos_theta;
      float z_coordinate = radius * sin_theta * sin_phi;

      ret.vertices.push_back(position + Vector3(x_coordinate, y_coordinate, z_coordinate));
      ret.colors.push_back(vertex_color);
    }
  }

  for (int lat = 0; lat < subdivisions; ++lat) {
    for (int lon = 0; lon < subdivisions; ++lon) {
      int first = (lat * (subdivisions + 1)) + lon;
      int second = first + subdivisions + 1;

      ret.triangles.push_back(first);
      ret.triangles.push_back(second);
      ret.triangles.push_back(first + 1);

      ret.triangles.push_back(second);
      ret.triangles.push_back(second + 1);
      ret.triangles.push_back(first + 1);
    }
  }

  return ret;
}

auto TriangleMesh::Cylinder(Vector3 position, float radius, float height, int subdivisions, unsigned int color)
    -> TriangleMesh {
  TriangleMesh ret;

  Vector3 vertex_color;
  vertex_color.SetColor(color);

  float half_height = height / (float)2;

  ret.vertices.push_back(position + Vector3(0.0F, half_height, 0.0F));
  ret.colors.push_back(vertex_color);
  ret.vertices.push_back(position + Vector3(0.0F, -half_height, 0.0F));
  ret.colors.push_back(vertex_color);

  for (int i = 0; i <= subdivisions; ++i) {
    float angle = (float)i * (float)2 * std::numbers::pi_v<float> / (float)subdivisions;
    float x_coordinate = radius * cosf(angle);
    float z_coordinate = radius * sinf(angle);

    ret.vertices.push_back(position + Vector3(x_coordinate, half_height, z_coordinate));
    ret.colors.push_back(vertex_color);

    ret.vertices.push_back(position + Vector3(x_coordinate, -half_height, z_coordinate));
    ret.colors.push_back(vertex_color);
  }

  for (int i = 0; i < subdivisions; ++i) {
    ret.triangles.push_back(0);
    ret.triangles.push_back(2 + (i * 2));
    ret.triangles.push_back(2 + ((i + 1) * 2));
  }

  for (int i = 0; i < subdivisions; ++i) {
    ret.triangles.push_back(1);
    ret.triangles.push_back(3 + ((i + 1) * 2));
    ret.triangles.push_back(3 + (i * 2));
  }

  for (int i = 0; i < subdivisions; ++i) {
    int top_current = 2 + (i * 2);
    int bottom_current = 3 + (i * 2);
    int top_next = 2 + ((i + 1) * 2);
    int bottom_next = 3 + ((i + 1) * 2);

    ret.triangles.push_back(top_current);
    ret.triangles.push_back(bottom_current);
    ret.triangles.push_back(top_next);

    ret.triangles.push_back(bottom_current);
    ret.triangles.push_back(bottom_next);
    ret.triangles.push_back(top_next);
  }

  return ret;
}