#include "../include/CmykSlider.h"
#include "imgui.h"

bool CmykSlider::Render()
{
    bool changed = false;

    ImGui::SetNextWindowPos(ImVec2(10, 140), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(300, 150), ImGuiCond_Once);

    ImGui::Begin("CMYK", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    int C_ = static_cast<int>(this->C * 100.0f + 0.5f);
    int M_ = static_cast<int>(this->M * 100.0f + 0.5f);
    int Y_ = static_cast<int>(this->Y * 100.0f + 0.5f);
    int K_ = static_cast<int>(this->K * 100.0f + 0.5f);

    ImGui::PushItemWidth(200.0f);
    if (ImGui::SliderInt("##C", &C_, 0, 100))
        changed = true;
    ImGui::PopItemWidth();
    ImGui::SameLine();
    ImGui::PushItemWidth(60.0f);
    if (ImGui::InputInt("C", &C_, 0, 0))
        changed = true;
    ImGui::PopItemWidth();

    ImGui::PushItemWidth(200.0f);
    if (ImGui::SliderInt("##M", &M_, 0, 100))
        changed = true;
    ImGui::PopItemWidth();
    ImGui::SameLine();
    ImGui::PushItemWidth(60.0f);
    if (ImGui::InputInt("M", &M_, 0, 0))
        changed = true;
    ImGui::PopItemWidth();

    ImGui::PushItemWidth(200.0f);
    if (ImGui::SliderInt("##Y", &Y_, 0, 100))
        changed = true;
    ImGui::PopItemWidth();
    ImGui::SameLine();
    ImGui::PushItemWidth(60.0f);
    if (ImGui::InputInt("Y", &Y_, 0, 0))
        changed = true;
    ImGui::PopItemWidth();

    ImGui::PushItemWidth(200.0f);
    if (ImGui::SliderInt("##K", &K_, 0, 100))
        changed = true;
    ImGui::PopItemWidth();
    ImGui::SameLine();
    ImGui::PushItemWidth(60.0f);
    if (ImGui::InputInt("K", &K_, 0, 0))
        changed = true;
    ImGui::PopItemWidth();

    if (changed)
    {
        C_ = (C_ > 100) ? 100 : C_;
        C_ = (C_ < 0) ? 0 : C_;
        M_ = (M_ > 100) ? 100 : M_;
        M_ = (M_ < 0) ? 0 : M_;
        Y_ = (Y_ > 100) ? 100 : Y_;
        Y_ = (Y_ < 0) ? 0 : Y_;
        K_ = (K_ > 100) ? 100 : K_;
        K_ = (K_ < 0) ? 0 : K_;
        this->C = C_ / 100.0f;
        this->M = M_ / 100.0f;
        this->Y = Y_ / 100.0f;
        this->K = K_ / 100.0f;
    }

    ImGui::End();

    return changed;
}