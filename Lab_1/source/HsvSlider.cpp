#include "../include/HsvSlider.h"
#include "imgui.h"

bool HsvSlider::Render()
{

    bool changed = false;

    ImGui::SetNextWindowPos(ImVec2(10, 300), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(300, 120), ImGuiCond_Once);

    ImGui::Begin("HSV", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    int H_ = this->H;
    int S_ = static_cast<int>(this->S * 100.0f + 0.5f);
    int V_ = static_cast<int>(this->V * 100.0f + 0.5f);

    ImGui::PushItemWidth(200.0f);
    if (ImGui::SliderInt("##H", &H_, 0, 359))
        changed = true;
    ImGui::PopItemWidth();
    ImGui::SameLine();
    ImGui::PushItemWidth(60.0f);
    if (ImGui::InputInt("H", &H_, 0, 0))
        changed = true;
    ImGui::PopItemWidth();

    ImGui::PushItemWidth(200.0f);
    if (ImGui::SliderInt("##S", &S_, 0, 100))
        changed = true;
    ImGui::PopItemWidth();
    ImGui::SameLine();
    ImGui::PushItemWidth(60.0f);
    if (ImGui::InputInt("S", &S_, 0, 0))
        changed = true;
    ImGui::PopItemWidth();

    ImGui::PushItemWidth(200.0f);
    if (ImGui::SliderInt("##V", &V_, 0, 100))
        changed = true;
    ImGui::PopItemWidth();
    ImGui::SameLine();
    ImGui::PushItemWidth(60.0f);
    if (ImGui::InputInt("V", &V_, 0, 0))
        changed = true;
    ImGui::PopItemWidth();

    ImGui::End();

    if (changed)
    {
        H_ = (H_ > 359) ? 359 : H_;
        H_ = (H_ < 0) ? 0 : H_;
        S_ = (S_ > 100) ? 100 : S_;
        S_ = (S_ < 0) ? 0 : S_;
        V_ = (V_ > 100) ? 100 : V_;
        V_ = (V_ < 0) ? 0 : V_;
        this->H = H_;
        this->S = S_ / 100.0f;
        this->V = V_ / 100.0f;
    }

    return changed;
}