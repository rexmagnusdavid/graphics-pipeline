#include "graphics_pipeline/triangle_mesh.h"

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