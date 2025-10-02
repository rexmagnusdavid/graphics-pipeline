#pragma once

#include <vector>

#include "vector_3.h"

class TriangleMesh {
public:
  std::vector<Vector3> colors;
  std::vector<Vector3> vertices;
  std::vector<Vector3> normals;
  std::vector<unsigned int> triangles;
  TriangleMesh() = default;

  void LoadBinary(char *file_name);
  void SaveBinary(char *file_name);

  auto GetPosition() -> Vector3;
  void SetPosition(Vector3 new_position);

  void Scale(float factor);
  void Translate(Vector3 vector);
  void RotateAboutAxis(Vector3 origin, Vector3 direction, float angle);

  void LightDirection(Vector3 light_direction, float ambient_coefficient);
  void LightPoint(Vector3 light_point, float ambient_coefficient);

  static auto AxisAlignedBox(Vector3 min_corner, Vector3 max_corner, unsigned int color) -> TriangleMesh;
  static auto Sphere(Vector3 position, float radius, int subdivisions, unsigned int color) -> TriangleMesh;
  static auto Cylinder(Vector3 position, float radius, float height, int subdivisions, unsigned int color)
      -> TriangleMesh;
};