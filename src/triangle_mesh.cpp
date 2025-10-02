#include "graphics_pipeline/triangle_mesh.h"
#include "graphics_pipeline/vector_3.h"

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

void TriangleMesh::LightDirection(Vector3 light_direction, float ambient_coefficient) {
  for (int vi = 0; vi < vertices.size(); vi++) {
    colors[vi] = Vector3(1.0F, 0.0F, 0.0F);
    colors[vi] = colors[vi].Light(normals[vi], light_direction, ambient_coefficient);
  }
}

void TriangleMesh::LightPoint(Vector3 light_point, float ambient_coefficient) {
  for (int vi = 0; vi < vertices.size(); vi++) {
    colors[vi] = Vector3(1.0F, 0.0F, 0.0F);
    Vector3 light_direction = (light_point - vertices[vi]).GetNormal();
    colors[vi] = colors[vi].Light(normals[vi], light_direction, ambient_coefficient);
  }
}