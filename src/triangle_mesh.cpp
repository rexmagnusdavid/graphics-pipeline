#include "graphics_pipeline/triangle_mesh.h"

#include "graphics_pipeline/vector_3.h"

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
  for (int i = 0; i < vertices.size(); i++) {
    colors[i] = Vector3(1.0F, 0.0F, 0.0F);
    colors[i] = colors[i].Light(normals[i], light_direction, ambient_coefficient);
  }
}

void TriangleMesh::LightPoint(Vector3 light_point, float ambient_coefficient) {
  for (int i = 0; i < vertices.size(); i++) {
    colors[i] = Vector3(1.0F, 0.0F, 0.0F);
    Vector3 light_direction = (light_point - vertices[i]).GetNormal();
    colors[i] = colors[i].Light(normals[i], light_direction, ambient_coefficient);
  }
}