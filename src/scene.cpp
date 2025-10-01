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
  int radius = framebuffer->width > framebuffer->height ? framebuffer->height / 4 : framebuffer->width / 4;

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

  framebuffer->DrawSegment(point1, point2, Color::BLACK);
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
  framebuffer->DrawSegment(n1, n2, Color::BLACK);
  framebuffer->DrawSegment(n1, n3, Color::BLACK);
  framebuffer->DrawSegment(n2, n3, Color::BLACK);
  u += 100;

  // Draw 'A'
  Vector3 a1(u, v, 0);
  Vector3 a2(u + 40, v - 100, 0);
  Vector3 a3(u + 80, v, 0);
  Vector3 a4(u + 20, v - 50, 0);
  Vector3 a5(u + 60, v - 50, 0);
  framebuffer->DrawSegment(a1, a2, Color::BLACK);
  framebuffer->DrawSegment(a2, a3, Color::BLACK);
  framebuffer->DrawSegment(a4, a5, Color::BLACK);
  u += 100;

  // Draw 'V'
  Vector3 v1(u, v - 100, 0);
  Vector3 v2(u + 40, v, 0);
  Vector3 v3(u + 80, v - 100, 0);
  framebuffer->DrawSegment(v1, v2, Color::BLACK);
  framebuffer->DrawSegment(v2, v3, Color::BLACK);

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
    framebuffer->DrawSegment(n1, n2, Color::BLACK);
    framebuffer->DrawSegment(n1, n3, Color::BLACK);
    framebuffer->DrawSegment(n2, n3, Color::BLACK);

    // Draw 'A'
    Vector3 a1(startX + 100, 200, 0);
    Vector3 a2(startX + 140, 100, 0);
    Vector3 a3(startX + 180, 200, 0);
    Vector3 a4(startX + 120, 150, 0);
    Vector3 a5(startX + 160, 150, 0);
    framebuffer->DrawSegment(a1, a2, Color::BLACK);
    framebuffer->DrawSegment(a2, a3, Color::BLACK);
    framebuffer->DrawSegment(a4, a5, Color::BLACK);

    // Draw 'V'
    Vector3 v1(startX + 200, 100, 0);
    Vector3 v2(startX + 240, 200, 0);
    Vector3 v3(startX + 280, 100, 0);
    framebuffer->DrawSegment(v1, v2, Color::BLACK);
    framebuffer->DrawSegment(v2, v3, Color::BLACK);

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

void Scene::DrawRotationGraph() {
  glfwMakeContextCurrent(framebuffer->window);

  std::cout << "Drawing rotation graph..." << std::endl;

  framebuffer->FillBackground(Color::WHITE);

  // Define the point to rotate
  Vector3 point(1.0F, 0.5F, 0.25F);

  // Define arbitrary axis
  Vector3 axis_origin(0.0F, 0.0F, 0.0F);
  Vector3 axis_direction(1.0F, 1.0F, 1.0F);

  // Rotate 360 times with 2 degree increments (180 steps for 360 degrees)
  constexpr int num_steps = 180;
  constexpr float angle_increment = 2.0F * 3.14F / 180.0F; // 2 degrees in radians

  std::vector<Vector3> rotated_points;
  rotated_points.reserve(num_steps);

  // Calculate all rotated points
  for (int i = 0; i < num_steps; i++) {
    float angle = i * angle_increment;
    Vector3 rotated = point.RotateAboutAxis(axis_origin, axis_direction, angle);
    rotated_points.push_back(rotated);
  }

  // Find min and max values for scaling
  float min_x = rotated_points[0].coordinates[0];
  float max_x = rotated_points[0].coordinates[0];
  float min_y = rotated_points[0].coordinates[1];
  float max_y = rotated_points[0].coordinates[1];
  float min_z = rotated_points[0].coordinates[2];
  float max_z = rotated_points[0].coordinates[2];

  for (const auto &point_data : rotated_points) {
    min_x = std::min(min_x, point_data.coordinates[0]);
    max_x = std::max(max_x, point_data.coordinates[0]);
    min_y = std::min(min_y, point_data.coordinates[1]);
    max_y = std::max(max_y, point_data.coordinates[1]);
    min_z = std::min(min_z, point_data.coordinates[2]);
    max_z = std::max(max_z, point_data.coordinates[2]);
  }

  // Graph layout parameters
  const int margin = 50;
  const int graph_width = framebuffer->width - 2 * margin;
  const int graph_height = framebuffer->height - 2 * margin;

  // Find overall min/max for y-axis scaling
  float data_min = std::min({min_x, min_y, min_z});
  float data_max = std::max({max_x, max_y, max_z});
  float data_range = data_max - data_min;

  // Draw axes
  Vector3 origin(margin, framebuffer->height - margin, 0);
  Vector3 x_axis_end(margin + graph_width, framebuffer->height - margin, 0);
  Vector3 y_axis_end(margin, framebuffer->height - margin - graph_height, 0);

  framebuffer->DrawSegment(origin, x_axis_end, Color::BLACK);
  framebuffer->DrawSegment(origin, y_axis_end, Color::BLACK);

  // Draw grid lines and labels
  for (int i = 0; i <= 4; i++) {
    int y_pos = framebuffer->height - margin - (i * graph_height / 4);
    Vector3 grid_start(margin, y_pos, 0);
    Vector3 grid_end(margin + graph_width, y_pos, 0);
    framebuffer->DrawSegment(grid_start, grid_end, 0xE0E0E0FF); // Light gray
  }

  // Plot the three curves
  auto plot_curve = [&](int coord_index, unsigned int color) {
    for (int i = 0; i < num_steps - 1; i++) {
      // Map step index to x coordinate
      int x1 = margin + (i * graph_width / num_steps);
      int x2 = margin + ((i + 1) * graph_width / num_steps);

      // Map coordinate value to y coordinate
      float val1 = (rotated_points[i].coordinates[coord_index] - data_min) / data_range;
      float val2 = (rotated_points[i + 1].coordinates[coord_index] - data_min) / data_range;

      int y1 = framebuffer->height - margin - (int)(val1 * graph_height);
      int y2 = framebuffer->height - margin - (int)(val2 * graph_height);

      Vector3 p1(x1, y1, 0);
      Vector3 p2(x2, y2, 0);

      framebuffer->DrawSegment(p1, p2, color);
    }
  };

  // Plot X coordinate in red
  plot_curve(0, Color::RED);

  // Plot Y coordinate in green
  plot_curve(1, Color::GREEN);

  // Plot Z coordinate in blue
  plot_curve(2, Color::BLUE);

  // Draw legend
  int legend_x = margin + 20;
  int legend_y = margin + 20;
  int legend_spacing = 20;

  Vector3 legend_x_start(legend_x, legend_y, 0);
  Vector3 legend_x_end(legend_x + 30, legend_y, 0);
  framebuffer->DrawSegment(legend_x_start, legend_x_end, Color::RED);

  Vector3 legend_y_start(legend_x, legend_y + legend_spacing, 0);
  Vector3 legend_y_end(legend_x + 30, legend_y + legend_spacing, 0);
  framebuffer->DrawSegment(legend_y_start, legend_y_end, Color::GREEN);

  Vector3 legend_z_start(legend_x, legend_y + 2 * legend_spacing, 0);
  Vector3 legend_z_end(legend_x + 30, legend_y + 2 * legend_spacing, 0);
  framebuffer->DrawSegment(legend_z_start, legend_z_end, Color::BLUE);

  framebuffer->Render();

  std::cout << "Graph complete. Point: (" << point[0] << ", " << point[1] << ", " << point[2] << ")" << std::endl;
  std::cout << "Axis origin: (" << axis_origin[0] << ", " << axis_origin[1] << ", " << axis_origin[2] << ")"
            << std::endl;
  std::cout << "Axis direction: (" << axis_direction[0] << ", " << axis_direction[1] << ", " << axis_direction[2] << ")"
            << std::endl;
  std::cout << "Red = X coordinate, Green = Y coordinate, Blue = Z coordinate" << std::endl;
}

void Scene::SaveTiff() {
  char filename[] = "framebuffer.tif";
  framebuffer->SaveTiff(filename);
  std::cout << "Saved framebuffer to " << filename << std::endl;
}

void Scene::Run() {
  // Main loop
  while (!glfwWindowShouldClose(framebuffer->window) && !glfwWindowShouldClose(gui->window)) {

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
void Scene::KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
  Scene *scene = static_cast<Scene *>(glfwGetWindowUserPointer(window));
  if (scene) {
    scene->HandleKeyInput(key, action, mods);
  }
}

void Scene::MouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
  Scene *scene = static_cast<Scene *>(glfwGetWindowUserPointer(window));
  if (scene) {
    scene->HandleMouseButton(button, action, mods);
  }
}

void Scene::CursorPositionCallback(GLFWwindow *window, double xpos, double ypos) {
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
    case GLFW_KEY_5:
      DrawRotationGraph();
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
      std::cout << "Left click at (" << xpos << ", " << ypos << ")" << std::endl;
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
      std::cout << "Right click at (" << xpos << ", " << ypos << ")" << std::endl;
    } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
      std::cout << "Middle click at (" << xpos << ", " << ypos << ")" << std::endl;
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