#include "graphics_pipeline/scene.h"

#include <GL/glew.h>

#include <GL/freeglut.h>

Scene *scene;
constexpr int framebuffer_width = 800;
constexpr int framebuffer_height = 600;

Scene::Scene() {
  gui = new GUI();
  framebuffer = new Framebuffer(framebuffer_width, framebuffer_height);
  camera = new PlanarPinholeCamera(framebuffer_width, framebuffer_height, 1.0F);
  meshes = {};
}