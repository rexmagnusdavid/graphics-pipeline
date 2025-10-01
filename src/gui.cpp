#include "graphics_pipeline/gui.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "graphics_pipeline/scene.h"

extern Scene *scene;

GUI::GUI(int _width, int _height, const char *title) {
  width = _width;
  height = _height;

  glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
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
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::SetNextWindowSize(ImVec2((float)width, (float)height));
  ImGuiWindowFlags window_flags =
      ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings |
      ImGuiWindowFlags_NoBringToFrontOnFocus;
  ImGui::Begin("MainPanel", nullptr, window_flags);
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

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}