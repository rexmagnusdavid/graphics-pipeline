#include "graphics_pipeline/scene.h"
#include "graphics_pipeline/color.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

Scene *scene;
constexpr int framebuffer_width = 640;
constexpr int framebuffer_height = 480;
constexpr int gui_width = 400;
constexpr int gui_height = 300;

Scene::Scene() {
  gui = new GUI(gui_width, gui_height, "GUI");
  framebuffer =
      new Framebuffer(framebuffer_width, framebuffer_height, "SW Framebuffer");
  camera = new PlanarPinholeCamera(framebuffer_width, framebuffer_height, 1.0F);
  meshes = {};

  // Set up input callbacks for framebuffer window
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

void Scene::DBG() {
  glfwMakeContextCurrent(framebuffer->window);

  int stepsN = 100;
  for (int si = 0; si < stepsN; si++) {
    framebuffer->FillBackground(Color::WHITE);
    int v = framebuffer->height / 2 + si;
    for (int u = 100; u < 200; u++) {
      framebuffer->SetPixel(u, v, Color::BLACK);
    }
    framebuffer->Render();
    glfwPollEvents();
  }

  std::cerr << std::endl;
  std::cerr << "INFO: pressed DBG button on GUI" << std::endl;
}

void Scene::DrawRectangle() {
  glfwMakeContextCurrent(framebuffer->window);

  framebuffer->FillBackground(Color::WHITE);

  int u1 = 150;
  int v1 = 150;
  int u2 = framebuffer->width - 150;
  int v2 = framebuffer->height - 150;

  for (int u = u1; u < u2; u++) {
    for (int v = v1; v < v2; v++) {
      framebuffer->SetPixel(u, v, Color::BLACK);
    }
  }

  framebuffer->Render();
}

void Scene::DrawCircle() {
  glfwMakeContextCurrent(framebuffer->window);

  framebuffer->FillBackground(Color::WHITE);

  int center_u = framebuffer->width / 2;
  int center_v = framebuffer->height / 2;
  int radius = framebuffer->width > framebuffer->height
                   ? framebuffer->height / 4
                   : framebuffer->width / 4;

  // Midpoint circle algorithm
  int x = radius;
  int y = 0;
  int decision = 1 - radius;

  auto drawCirclePoints = [&](int cx, int cy, int x, int y) {
    framebuffer->SetPixel(cx + x, cy + y, Color::BLACK);
    framebuffer->SetPixel(cx - x, cy + y, Color::BLACK);
    framebuffer->SetPixel(cx + x, cy - y, Color::BLACK);
    framebuffer->SetPixel(cx - x, cy - y, Color::BLACK);
    framebuffer->SetPixel(cx + y, cy + x, Color::BLACK);
    framebuffer->SetPixel(cx - y, cy + x, Color::BLACK);
    framebuffer->SetPixel(cx + y, cy - x, Color::BLACK);
    framebuffer->SetPixel(cx - y, cy - x, Color::BLACK);
  };

  while (x >= y) {
    drawCirclePoints(center_u, center_v, x, y);
    y++;
    if (decision <= 0) {
      decision += 2 * y + 1;
    } else {
      x--;
      decision += 2 * (y - x) + 1;
    }
  }

  framebuffer->Render();
}

void Scene::DrawLine() {
  glfwMakeContextCurrent(framebuffer->window);

  framebuffer->FillBackground(Color::WHITE);

  int u1 = 150;
  int v1 = 150;
  int u2 = framebuffer->width - 150;
  int v2 = framebuffer->height - 150;

  Vector3 point1(u1, v1, 0);
  Vector3 point2(u2, v2, 0);

  framebuffer->Draw2DSegment(point1, point2, Color::BLACK);
  framebuffer->Render();
}

void Scene::DrawName() {
  glfwMakeContextCurrent(framebuffer->window);

  framebuffer->FillBackground(Color::WHITE);

  int u = 150;
  int v = 150;

  // Draw 'N'
  Vector3 n1(u, v, 0);
  Vector3 n2(u, v - 100, 0);
  Vector3 n3(u + 60, v - 50, 0);
  framebuffer->Draw2DSegment(n1, n2, Color::BLACK);
  framebuffer->Draw2DSegment(n1, n3, Color::BLACK);
  framebuffer->Draw2DSegment(n2, n3, Color::BLACK);
  u += 100;

  // Draw 'A'
  Vector3 a1(u, v, 0);
  Vector3 a2(u + 40, v - 100, 0);
  Vector3 a3(u + 80, v, 0);
  Vector3 a4(u + 20, v - 50, 0);
  Vector3 a5(u + 60, v - 50, 0);
  framebuffer->Draw2DSegment(a1, a2, Color::BLACK);
  framebuffer->Draw2DSegment(a2, a3, Color::BLACK);
  framebuffer->Draw2DSegment(a4, a5, Color::BLACK);
  u += 100;

  // Draw 'V'
  Vector3 v1(u, v - 100, 0);
  Vector3 v2(u + 40, v, 0);
  Vector3 v3(u + 80, v - 100, 0);
  framebuffer->Draw2DSegment(v1, v2, Color::BLACK);
  framebuffer->Draw2DSegment(v2, v3, Color::BLACK);

  framebuffer->Render();
}

void Scene::AnimateName() {
  glfwMakeContextCurrent(framebuffer->window);

  int fps = 30;
  int seconds = 10;
  int frames = fps * seconds;

  int nameWidth = 280;
  double totalDistance = framebuffer->width + nameWidth;
  double speed = totalDistance / frames;

  for (int i = 0; i < frames; i++) {
    framebuffer->FillBackground(Color::WHITE);

    // Calculate position - start from right side, move left
    int uOffset = (int)(i * speed);
    int startX = framebuffer->width - uOffset;

    // Draw 'N'
    Vector3 n1(startX, 200, 0);
    Vector3 n2(startX, 100, 0);
    Vector3 n3(startX + 60, 150, 0);
    framebuffer->Draw2DSegment(n1, n2, Color::BLACK);
    framebuffer->Draw2DSegment(n1, n3, Color::BLACK);
    framebuffer->Draw2DSegment(n2, n3, Color::BLACK);

    // Draw 'A'
    Vector3 a1(startX + 100, 200, 0);
    Vector3 a2(startX + 140, 100, 0);
    Vector3 a3(startX + 180, 200, 0);
    Vector3 a4(startX + 120, 150, 0);
    Vector3 a5(startX + 160, 150, 0);
    framebuffer->Draw2DSegment(a1, a2, Color::BLACK);
    framebuffer->Draw2DSegment(a2, a3, Color::BLACK);
    framebuffer->Draw2DSegment(a4, a5, Color::BLACK);

    // Draw 'V'
    Vector3 v1(startX + 200, 100, 0);
    Vector3 v2(startX + 240, 200, 0);
    Vector3 v3(startX + 280, 100, 0);
    framebuffer->Draw2DSegment(v1, v2, Color::BLACK);
    framebuffer->Draw2DSegment(v2, v3, Color::BLACK);

    framebuffer->Render();
    glfwPollEvents();

    // Timing control
    double frameTime = 1.0 / fps;
    double targetTime = glfwGetTime() + frameTime;
    while (glfwGetTime() < targetTime) {
      // Busy wait for precise timing
    }
  }
}

void Scene::SaveTiff() {
  char filename[] = "framebuffer.tif";
  framebuffer->SaveTiff(filename);
  std::cout << "Saved framebuffer to " << filename << std::endl;
}
void Scene::Run() {
  // Main loop
  while (!glfwWindowShouldClose(framebuffer->window) &&
         !glfwWindowShouldClose(gui->window)) {

    // Render GUI
    glfwMakeContextCurrent(gui->window);
    glClear(GL_COLOR_BUFFER_BIT);
    gui->Render();
    glfwSwapBuffers(gui->window);

    // Render Framebuffer
    glfwMakeContextCurrent(framebuffer->window);
    glClear(GL_COLOR_BUFFER_BIT);
    framebuffer->Render();

    // Poll events for both windows
    glfwPollEvents();
  }
}

// Static callback functions that forward to instance methods
void Scene::KeyCallback(GLFWwindow *window, int key, int scancode, int action,
                        int mods) {
  Scene *scene = static_cast<Scene *>(glfwGetWindowUserPointer(window));
  if (scene) {
    scene->HandleKeyInput(key, action, mods);
  }
}

void Scene::MouseButtonCallback(GLFWwindow *window, int button, int action,
                                int mods) {
  Scene *scene = static_cast<Scene *>(glfwGetWindowUserPointer(window));
  if (scene) {
    scene->HandleMouseButton(button, action, mods);
  }
}

void Scene::CursorPositionCallback(GLFWwindow *window, double xpos,
                                   double ypos) {
  Scene *scene = static_cast<Scene *>(glfwGetWindowUserPointer(window));
  if (scene) {
    scene->HandleCursorPosition(xpos, ypos);
  }
}

void Scene::ScrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
  Scene *scene = static_cast<Scene *>(glfwGetWindowUserPointer(window));
  if (scene) {
    scene->HandleScroll(xoffset, yoffset);
  }
}

// Instance methods for handling input
void Scene::HandleKeyInput(int key, int action, int mods) {
  if (action == GLFW_PRESS || action == GLFW_REPEAT) {
    const float move_speed = 0.5f;
    const float rotate_speed = 0.1f;

    switch (key) {
    // Camera translation
    case GLFW_KEY_W:
      camera->Translate(camera->forward * move_speed);
      std::cout << "Camera moved forward" << std::endl;
      break;
    case GLFW_KEY_S:
      camera->Translate(camera->forward * -move_speed);
      std::cout << "Camera moved backward" << std::endl;
      break;
    case GLFW_KEY_A:
      camera->Translate(camera->right * -move_speed);
      std::cout << "Camera moved left" << std::endl;
      break;
    case GLFW_KEY_D:
      camera->Translate(camera->right * move_speed);
      std::cout << "Camera moved right" << std::endl;
      break;
    case GLFW_KEY_SPACE:
      camera->Translate(camera->up * move_speed);
      std::cout << "Camera moved up" << std::endl;
      break;
    case GLFW_KEY_LEFT_SHIFT:
    case GLFW_KEY_RIGHT_SHIFT:
      camera->Translate(camera->up * -move_speed);
      std::cout << "Camera moved down" << std::endl;
      break;

    // Camera rotation
    case GLFW_KEY_LEFT:
      camera->Pan(rotate_speed);
      std::cout << "Camera panned left" << std::endl;
      break;
    case GLFW_KEY_RIGHT:
      camera->Pan(-rotate_speed);
      std::cout << "Camera panned right" << std::endl;
      break;
    case GLFW_KEY_UP:
      camera->Tilt(rotate_speed);
      std::cout << "Camera tilted up" << std::endl;
      break;
    case GLFW_KEY_DOWN:
      camera->Tilt(-rotate_speed);
      std::cout << "Camera tilted down" << std::endl;
      break;
    case GLFW_KEY_Q:
      camera->Roll(rotate_speed);
      std::cout << "Camera rolled left" << std::endl;
      break;
    case GLFW_KEY_E:
      camera->Roll(-rotate_speed);
      std::cout << "Camera rolled right" << std::endl;
      break;

    // Camera zoom
    case GLFW_KEY_Z:
      camera->Zoom(1.1f);
      std::cout << "Camera zoomed in" << std::endl;
      break;
    case GLFW_KEY_X:
      camera->Zoom(0.9f);
      std::cout << "Camera zoomed out" << std::endl;
      break;

    // Quick draw commands
    case GLFW_KEY_1:
      DrawRectangle();
      break;
    case GLFW_KEY_2:
      DrawCircle();
      break;
    case GLFW_KEY_3:
      DrawLine();
      break;
    case GLFW_KEY_4:
      DrawName();
      break;

    // Save screenshot
    case GLFW_KEY_P:
      SaveTiff();
      break;

    // Exit
    case GLFW_KEY_ESCAPE:
      glfwSetWindowShouldClose(framebuffer->window, GLFW_TRUE);
      std::cout << "Closing application" << std::endl;
      break;
    }
  }
}

void Scene::HandleMouseButton(int button, int action, int mods) {
  if (action == GLFW_PRESS) {
    double xpos, ypos;
    glfwGetCursorPos(framebuffer->window, &xpos, &ypos);

    if (button == GLFW_MOUSE_BUTTON_LEFT) {
      std::cout << "Left click at (" << xpos << ", " << ypos << ")"
                << std::endl;
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
      std::cout << "Right click at (" << xpos << ", " << ypos << ")"
                << std::endl;
    } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
      std::cout << "Middle click at (" << xpos << ", " << ypos << ")"
                << std::endl;
    }
  }
}

void Scene::HandleCursorPosition(double xpos, double ypos) {
  // This gets called very frequently, so we typically don't log it
  // You can use this for drag operations or hover effects
}

void Scene::HandleScroll(double xoffset, double yoffset) {
  // Use scroll for zooming
  if (yoffset > 0) {
    camera->Zoom(1.1F);
    std::cout << "Scroll zoom in" << '\n';
  } else if (yoffset < 0) {
    camera->Zoom(0.9F);
    std::cout << "Scroll zoom out" << '\n';
  }
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