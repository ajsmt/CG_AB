#include "../include/ColorController.h"
#include "imgui.h"
#include <algorithm>
#include <string>
#include <cmath>

void ColorController::Render()
{
    ColorPanelRender();
    ColorPaletteRender();

    if (RGB.Render())
    {
        RGBtoCMYK();
        RGBtoHSV();
    }
    if (CMYK.Render())
    {
        CMYKtoRGB();
        RGBtoHSV();
    }
    if (HSV.Render())
    {
        HSVtoRGB();
        RGBtoCMYK();
    }
}

void ColorController::RGBtoCMYK()
{
    float R_ = RGB.R / 255.0f;
    float G_ = RGB.G / 255.0f;
    float B_ = RGB.B / 255.0f;

    float max_val = std::max({R_, G_, B_});

    CMYK.K = 1.0f - max_val;

    constexpr float eps = 1e-6;

    if (std::abs(max_val) <= eps)
    {
        CMYK.C = 0.0f;
        CMYK.M = 0.0f;
        CMYK.Y = 0.0f;
        return;
    }

    CMYK.C = (1.0f - R_ - CMYK.K) / (1.0f - CMYK.K);
    CMYK.M = (1.0f - G_ - CMYK.K) / (1.0f - CMYK.K);
    CMYK.Y = (1.0f - B_ - CMYK.K) / (1.0f - CMYK.K);
}

void ColorController::RGBtoHSV()
{
    float R_ = RGB.R / 255.0f;
    float G_ = RGB.G / 255.0f;
    float B_ = RGB.B / 255.0f;

    float C_max = std::max({R_, G_, B_});
    float C_min = std::min({R_, G_, B_});

    float Delta = C_max - C_min;

    constexpr float eps = 1e-6f;

    float H_ = 0.0f;
    float S_ = 0.0f;
    float V_ = C_max;

    if (C_max > eps)
        S_ = Delta / C_max;

    if (Delta > eps)
    {
        if (std::abs(C_max - R_) <= eps)
            H_ = 60.0f * fmod(((G_ - B_) / Delta), 6.0f);
        else if (std::abs(C_max - G_) <= eps)
            H_ = 60.0f * (((B_ - R_) / Delta) + 2.0f);
        else if (std::abs(C_max - B_) <= eps)
            H_ = 60.0f * (((R_ - G_) / Delta) + 4.0f);

        if (H_ < 0.0f)
            H_ += 360.0f;
    }

    HSV.H = static_cast<int>(std::round(H_));
    HSV.S = S_;
    HSV.V = V_;

    if (HSV.H >= 360)
        HSV.H -= 360;
}

void ColorController::CMYKtoRGB()
{
    RGB.R = static_cast<int>(floor(255 * (1.0f - CMYK.C) * (1.0f - CMYK.K)));
    RGB.G = static_cast<int>(floor(255 * (1.0f - CMYK.M) * (1.0f - CMYK.K)));
    RGB.B = static_cast<int>(floor(255 * (1.0f - CMYK.Y) * (1.0f - CMYK.K)));
}

void ColorController::HSVtoRGB()
{
    float H_ = HSV.H;
    if (H_ >= 360.0f)
        H_ = 0.0f;

    float C = HSV.V * HSV.S;
    float X = C * (1 - fabs(fmod(H_ / 60.0f, 2.0f) - 1));
    float m = HSV.V - C;

    float R_, G_, B_;

    if (0 <= H_ && H_ < 60)
    {
        R_ = C;
        G_ = X;
        B_ = 0;
    }
    else if (60 <= H_ && H_ < 120)
    {
        R_ = X;
        G_ = C;
        B_ = 0;
    }
    else if (120 <= H_ && H_ < 180)
    {
        R_ = 0;
        G_ = C;
        B_ = X;
    }
    else if (180 <= H_ && H_ < 240)
    {
        R_ = 0;
        G_ = X;
        B_ = C;
    }
    else if (240 <= H_ && H_ < 300)
    {
        R_ = X;
        G_ = 0;
        B_ = C;
    }
    else if (300 <= H_ && H_ < 360)
    {
        R_ = C;
        G_ = 0;
        B_ = X;
    }

    RGB.R = static_cast<int>(floor((R_ + m) * 255));
    RGB.G = static_cast<int>(floor((G_ + m) * 255));
    RGB.B = static_cast<int>(floor((B_ + m) * 255));
}

void ColorController::ColorPanelRender()
{
    ImGui::SetNextWindowPos(ImVec2(320, 10), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(300, 410), ImGuiCond_Once);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Color panel", nullptr,
                 ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoTitleBar);
    ImGui::ColorButton("##preview",
                       ImVec4({RGB.R / 255.0f, RGB.G / 255.0f, RGB.B / 255.0f, 1.0f}),
                       ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop,
                       ImGui::GetContentRegionAvail());
    ImGui::End();
    ImGui::PopStyleVar();
}

void ColorController::ColorPaletteRender()
{
    ImGui::SetNextWindowPos(ImVec2(10, 430), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(610, 115), ImGuiCond_Once);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(18, 5));
    ImGui::Begin("Palette", nullptr,
                 ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoTitleBar);

    static ImVec4 paletteColors[20] = {
        {1.0f, 0.0f, 0.0f, 1.0f},
        {0.0f, 1.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 0.0f, 1.0f},
        {1.0f, 0.0f, 1.0f, 1.0f},
        {0.0f, 1.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f, 1.0f},
        {0.0f, 0.0f, 0.0f, 1.0f},
        {0.5f, 0.5f, 0.5f, 1.0f},
        {0.8f, 0.5f, 0.2f, 1.0f},
        {0.12f, 0.56f, 0.78f, 1.0f},
        {0.85f, 0.33f, 0.12f, 1.0f},
        {0.44f, 0.88f, 0.22f, 1.0f},
        {0.95f, 0.12f, 0.72f, 1.0f},
        {0.67f, 0.45f, 0.89f, 1.0f},
        {0.33f, 0.77f, 0.11f, 1.0f},
        {0.21f, 0.34f, 0.56f, 1.0f},
        {0.88f, 0.66f, 0.44f, 1.0f},
        {0.15f, 0.95f, 0.62f, 1.0f},
        {0.77f, 0.22f, 0.33f, 1.0f}};

    for (int i = 0; i < 20; ++i)
    {
        if (i > 0 && i % 10 != 0)
            ImGui::SameLine();

        if (ImGui::ColorButton(("##col" + std::to_string(i)).c_str(),
                               paletteColors[i],
                               ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop,
                               ImVec2(50, 50)))
        {
            RGB.R = static_cast<int>(paletteColors[i].x * 255.0f);
            RGB.G = static_cast<int>(paletteColors[i].y * 255.0f);
            RGB.B = static_cast<int>(paletteColors[i].z * 255.0f);
            RGBtoCMYK();
            RGBtoHSV();
        }
    }

    ImGui::End();
    ImGui::PopStyleVar();
}