#include "graphics_pipeline/scene.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "graphics_pipeline/color.h"

Scene *scene;
constexpr int framebuffer_width = 640;
constexpr int framebuffer_height = 480;
constexpr int gui_width = 400;
constexpr int gui_height = 300;

Scene::Scene() {
  gui = new GUI(gui_width, gui_height, "GUI");
  framebuffer = new Framebuffer(framebuffer_width, framebuffer_height, "SW Framebuffer");
  camera = new PlanarPinholeCamera(framebuffer_width, framebuffer_height, 1.0F);
  mesh = {};

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
  delete mesh;
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