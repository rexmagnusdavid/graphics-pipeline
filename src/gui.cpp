#include "graphics_pipeline/gui.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "graphics_pipeline/scene.h"

extern Scene *scene;

GUI::GUI(int _width, int _height, const char *title) {
  width = _width;
  height = _height;

  window = glfwCreateWindow(width, height, title, nullptr, nullptr);
  glfwMakeContextCurrent(window);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &input_output = ImGui::GetIO();
  (void)input_output;
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 130");
}

GUI::~GUI() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
}

void GUI::Render() {
  ImGui::Begin("MainPanel");

  if (ImGui::Button("DBG")) {
    scene->DBG();
  }
  if (ImGui::Button("Draw Rectangle")) {
    scene->DrawRectangle();
  }
  if (ImGui::Button("Draw Circle")) {
    scene->DrawCircle();
  }
  if (ImGui::Button("Draw Line")) {
    scene->DrawLine();
  }
  if (ImGui::Button("Draw Name")) {
    scene->DrawName();
  }
  if (ImGui::Button("Animate Name")) {
    scene->AnimateName();
  }
  if (ImGui::Button("Save as .tiff")) {
    scene->SaveTiff();
  }

  ImGui::End();
}