#include "menu.h"
#include "cheats.h" 
#include "imgui.h"

void menu::render_menu() {
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 5.0f;
    style.FrameRounding = 3.0f;

    ImGui::Begin("UE4 Cheat Menu by D7-Expert");

    ImGui::Text("Press 'INSERT' to hide/show this menu.");
    ImGui::Separator();

    ImGui::Checkbox("Show Actor List", &Cheats::bShowActorList);

    ImGui::Spacing();
    ImGui::Separator();

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    ImGui::End();
}
