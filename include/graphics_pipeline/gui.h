#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class GUI {
public:
  int width, height;
  GLFWwindow *window;
  GUI(int _width, int _height, const char *title);
  ~GUI();

  void Render();
};