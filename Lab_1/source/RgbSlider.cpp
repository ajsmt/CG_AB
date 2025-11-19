#include "../include/RgbSlider.h"
#include "imgui.h"

bool RgbSlider::Render()
{
    bool changed = false;

    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(300, 120), ImGuiCond_Once);

    ImGui::Begin("RGB", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    ImGui::PushItemWidth(200.0f);
    if (ImGui::SliderInt("##R", &R, 0, 255))
        changed = true;
    ImGui::PopItemWidth();
    ImGui::SameLine();
    ImGui::PushItemWidth(60.0f);
    if (ImGui::InputInt("R", &R, 0, 0))
        changed = true;
    ImGui::PopItemWidth();
    R = (R > 255) ? 255 : R;
    R = (R < 0) ? 0 : R;

    ImGui::PushItemWidth(200.0f);
    if (ImGui::SliderInt("##G", &G, 0, 255))
        changed = true;
    ImGui::PopItemWidth();
    ImGui::SameLine();
    ImGui::PushItemWidth(60.0f);
    if (ImGui::InputInt("G", &G, 0, 0))
        changed = true;
    ImGui::PopItemWidth();
    G = (G > 255) ? 255 : G;
    G = (G < 0) ? 0 : G;

    ImGui::PushItemWidth(200.0f);
    if (ImGui::SliderInt("##B", &B, 0, 255))
        changed = true;
    ImGui::PopItemWidth();
    ImGui::SameLine();
    ImGui::PushItemWidth(60.0f);
    if (ImGui::InputInt("B", &B, 0, 0))
        changed = true;
    ImGui::PopItemWidth();
    B = (B > 255) ? 255 : B;
    B = (B < 0) ? 0 : B;

    ImGui::End();

    return changed;
}