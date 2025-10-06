#include "graphics_pipeline/scene.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "graphics_pipeline/color.h"
#include "graphics_pipeline/triangle_mesh.h"
#include "graphics_pipeline/vector_3.h"

Scene *scene;
constexpr int framebuffer_width = 640;
constexpr int framebuffer_height = 480;
constexpr int gui_width = 400;
constexpr int gui_height = 300;

Scene::Scene() {
  gui = new GUI(gui_width, gui_height, "GUI");
  framebuffer = new Framebuffer(framebuffer_width, framebuffer_height, "SW Framebuffer");
  camera = new PlanarPinholeCamera(framebuffer_width, framebuffer_height, 1.0F);

  glfwSetWindowUserPointer(framebuffer->window, this);
  glfwSetKeyCallback(framebuffer->window, KeyCallback);
  glfwSetMouseButtonCallback(framebuffer->window, MouseButtonCallback);
  glfwSetCursorPosCallback(framebuffer->window, CursorPositionCallback);
  glfwSetScrollCallback(framebuffer->window, ScrollCallback);
}

Scene::~Scene() {
  delete camera;
  delete framebuffer;
  delete gui;
}

void Scene::Run() {
  while ((glfwWindowShouldClose(framebuffer->window) == 0) && (glfwWindowShouldClose(gui->window) == 0)) {
    glfwMakeContextCurrent(gui->window);
    glClear(GL_COLOR_BUFFER_BIT);
    gui->Render();
    glfwSwapBuffers(gui->window);

    glfwMakeContextCurrent(framebuffer->window);
    glClear(GL_COLOR_BUFFER_BIT);
    framebuffer->Render();

    glfwPollEvents();
  }
}

void Scene::Draw3DPoint(Vector3 point, int size, unsigned int color) {
  Vector3 projected_point;

  if (camera->Project(point, projected_point) == 0) {
    return;
  }

  framebuffer->DrawPoint(projected_point, size, color);
}

void Scene::Draw3DSegment(Vector3 start_point, Vector3 end_point, unsigned int color) {
  Vector3 projected_start_point;
  Vector3 projected_end_point;

  if (camera->Project(start_point, projected_start_point) == 0) {
    return;
  }

  if (camera->Project(end_point, projected_end_point) == 0) {
    return;
  }

  framebuffer->DrawSegment(projected_start_point, projected_end_point, color);
}

void Scene::Draw3DSegment(Vector3 start_point, Vector3 end_point, Vector3 start_color, Vector3 end_color) {
  Vector3 projected_start_point;
  Vector3 projected_end_point;

  if (camera->Project(start_point, projected_start_point) == 0) {
    return;
  }

  if (camera->Project(end_point, projected_end_point) == 0) {
    return;
  }

  framebuffer->DrawSegment(projected_start_point, projected_end_point, start_color, end_color);
}

void Scene::DrawMeshPoints(TriangleMesh *mesh, int size, unsigned int color) {
  for (auto vertex : mesh->vertices) {
    Draw3DPoint(vertex, size, color);
  }
}

void Scene::DrawMeshWireframe(TriangleMesh *mesh, unsigned int color) {
  for (int tri = 0; tri < mesh->triangles.size(); tri++) {
    std::array<Vector3, 3> vertices;
    vertices[0] = mesh->vertices[mesh->triangles[(tri * 3) + 0]];
    vertices[1] = mesh->vertices[mesh->triangles[(tri * 3) + 1]];
    vertices[2] = mesh->vertices[mesh->triangles[(tri * 3) + 2]];

    std::array<Vector3, 3> colors;
    if (!mesh->colors.empty()) {
      colors[0] = mesh->colors[mesh->triangles[(tri * 3) + 0]];
      colors[1] = mesh->colors[mesh->triangles[(tri * 3) + 1]];
      colors[2] = mesh->colors[mesh->triangles[(tri * 3) + 2]];
    }

    for (int ei = 0; ei < 3; ei++) {
      if (!mesh->colors.empty()) {
        Draw3DSegment(vertices[ei], vertices[(ei + 1) % 3], colors[ei], colors[(ei + 1) % 3]);
        continue;
      }

      Draw3DSegment(vertices[ei], vertices[(ei + 1) % 3], color);
    }
  }
}

void Scene::DrawMeshNormals(TriangleMesh *mesh, int size) {
  if (mesh->normals.empty()) {
    return;
  }

  for (int i = 0; i < mesh->vertices.size(); i++) {
    Draw3DSegment(mesh->vertices[i], mesh->vertices[i] + (mesh->normals[i].GetNormal() * (float)size), mesh->colors[i],
                  Vector3(1.0F, 0.0F, 0.0F));
  }
}

void Scene::KeyCallback(GLFWwindow *window, int key, int scan_code, int action, int mods) {
  auto *scene = static_cast<Scene *>(glfwGetWindowUserPointer(window));
  if (scene != nullptr) {
    scene->HandleKeyInput(key, action, mods);
  }
}

void Scene::MouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
  auto *scene = static_cast<Scene *>(glfwGetWindowUserPointer(window));
  if (scene != nullptr) {
    scene->HandleMouseButton(button, action, mods);
  }
}

void Scene::CursorPositionCallback(GLFWwindow *window, double u_coordinate, double v_coordinate) {
  auto *scene = static_cast<Scene *>(glfwGetWindowUserPointer(window));
  if (scene != nullptr) {
    scene->HandleCursorPosition(u_coordinate, v_coordinate);
  }
}

void Scene::ScrollCallback(GLFWwindow *window, double u_offset, double v_offset) {
  auto *scene = static_cast<Scene *>(glfwGetWindowUserPointer(window));
  if (scene != nullptr) {
    scene->HandleScroll(u_offset, v_offset);
  }
}

void Scene::HandleKeyInput(int key, int action, int mods) {}

void Scene::HandleMouseButton(int button, int action, int mods) {
  if (action == GLFW_PRESS) {
    double u_coordinate;
    double v_coordinate;
    glfwGetCursorPos(framebuffer->window, &u_coordinate, &v_coordinate);

    if (button == GLFW_MOUSE_BUTTON_LEFT) {
      std::cout << "Left click at (" << u_coordinate << ", " << v_coordinate << ")" << '\n';
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
      std::cout << "Right click at (" << u_coordinate << ", " << v_coordinate << ")" << '\n';
    } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
      std::cout << "Middle click at (" << u_coordinate << ", " << v_coordinate << ")" << '\n';
    }
  }
}

void Scene::HandleCursorPosition(double u_coordinate, double v_coordinate) {}

void Scene::HandleScroll(double u_offset, double v_offset) {}

// Assignment-related functions here.
void Scene::DBG() {
  glfwMakeContextCurrent(framebuffer->window);

  constexpr int stepsN = 100;
  for (int si = 0; si < stepsN; si++) {
    framebuffer->FillBackground(Color::WHITE);
    int v_coordinate = (framebuffer->height / 2) + si;
    for (int u_coordinate = stepsN; u_coordinate < stepsN * 2; u_coordinate++) {
      framebuffer->SetPixel(u_coordinate, v_coordinate, Color::BLACK);
    }
    framebuffer->Render();
    glfwPollEvents();
  }

  std::cerr << '\n';
  std::cerr << "INFO: pressed DBG button on GUI" << '\n';
}

auto main(int argc, char *argv[]) -> int {
  if (glfwInit() == 0) {
    std::cerr << "Failed to initialize GLFW" << '\n';
    return -1;
  }

  scene = new Scene();
  scene->Run();

  delete scene;
  glfwTerminate();

  return 0;
}