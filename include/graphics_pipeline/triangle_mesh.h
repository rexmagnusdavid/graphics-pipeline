#pragma once

#include <vector>

#include "framebuffer.h"
#include "planar_pinhole_camera.h"
#include "vector_3.h"

class TriangleMesh {
public:
  std::vector<Vector3> colors;
  std::vector<Vector3> vertices;
  std::vector<unsigned int> triangles;
  TriangleMesh() = default;

  void LoadBinary(char *file_name);
  void SaveBinary(char *file_name);

  auto GetPosition() -> Vector3;
  void SetPosition(Vector3 new_position);

  void Scale(float factor);
  void Translate(Vector3 vector);
  void RotateAboutAxis(Vector3 origin, Vector3 direction, float angle);

  void DrawPoints(PlanarPinholeCamera *camera, Framebuffer *framebuffer,
                  unsigned int color, int size);
  void DrawWireFrame(PlanarPinholeCamera *camera, Framebuffer *framebuffer,
                     unsigned int color);
};