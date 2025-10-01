#include "graphics_pipeline/gui.h"

#include <imgui.h>

GUI::GUI() = default;

void GUI::Render() {
  ImGui::Begin("MainPanel");
  ImGui::Button("DBG");
  ImGui::End();
}