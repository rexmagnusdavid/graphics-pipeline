#pragma once

#include "framebuffer.h"
#include "planar_pinhole_camera.h"
#include "triangle_mesh.h"

class Scene {
public:
  Framebuffer *framebuffer;
  PlanarPinholeCamera *camera;
  std::vector<TriangleMesh> meshes;
  Scene();

  void Render();
};

extern Scene *scene;