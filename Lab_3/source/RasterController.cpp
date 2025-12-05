#include "../include/RasterController.h"
#include <cmath>
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <string>

static const float PANEL_WIDTH = 350.0f;
static const int WINDOW_WIDTH = 1280;
static const int WINDOW_HEIGHT = 720;

RasterController::RasterController() {
    scale = 25;
    p1 = {0, 0};
    p2 = {8, 5};
    radius = 10;
    currentAlgo = ALGO_BRESENHAM_LINE;
    
    currentExecutionTime = 0.0;
    averageExecutionTime = 0.0;
    totalTimeAccumulator = 0.0;
    frameCount = 0;

    lastP1 = p1;
    lastP2 = p2;
    lastRadius = radius;
    lastAlgo = currentAlgo;

    scrolling = ImVec2(0.0f, 0.0f);
}

void RasterController::Plot(int x, int y) {
    pixels.push_back({x, y});
}

void RasterController::AlgoStepByStep() {
    if (p1.x == p2.x && p1.y == p2.y) { Plot(p1.x, p1.y); return; }
    int dx = p2.x - p1.x;
    int dy = p2.y - p1.y;
    int steps = std::max(std::abs(dx), std::abs(dy));
    float x = (float)p1.x;
    float y = (float)p1.y;
    float xInc = (float)dx / steps;
    float yInc = (float)dy / steps;
    for (int i = 0; i <= steps; i++) {
        Plot((int)round(x), (int)round(y));
        x += xInc;
        y += yInc;
    }
}

void RasterController::AlgoDDA() {
    int dx = p2.x - p1.x;
    int dy = p2.y - p1.y;
    int length = std::max(std::abs(dx), std::abs(dy));
    float x = (float)p1.x;
    float y = (float)p1.y;
    float dX = (float)dx / length;
    float dY = (float)dy / length;
    for(int i = 0; i <= length; i++) {
        Plot((int)round(x), (int)round(y));
        x += dX;
        y += dY;
    }
}

void RasterController::AlgoBresenhamLine() {
    int x0 = p1.x;
    int y0 = p1.y;
    int x1 = p2.x;
    int y1 = p2.y;
    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;
    while (true) {
        Plot(x0, y0);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 < dx) { err += dx; y0 += sy; }
    }
}

void RasterController::AlgoBresenhamCircle() {
    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius; 
    auto plot8 = [&](int xc, int yc, int x, int y) {
        Plot(xc + x, yc + y); Plot(xc - x, yc + y); 
        Plot(xc + x, yc - y); Plot(xc - x, yc - y); 
        Plot(xc + y, yc + x); Plot(xc - y, yc + x); 
        Plot(xc + y, yc - x); Plot(xc - y, yc - x); 
    };
    plot8(p1.x, p1.y, x, y);
    while (y >= x) {
        x++;
        if (d > 0) { y--; d = d + 4 * (x - y) + 10; } 
        else { d = d + 4 * x + 6; }
        plot8(p1.x, p1.y, x, y);
    }
}

void RasterController::Calculate() {
    bool changed = false;
    if (p1.x != lastP1.x || p1.y != lastP1.y ||
        p2.x != lastP2.x || p2.y != lastP2.y ||
        radius != lastRadius || currentAlgo != lastAlgo) 
    {
        changed = true;
        lastP1 = p1; lastP2 = p2;
        lastRadius = radius; lastAlgo = currentAlgo;
        
        totalTimeAccumulator = 0.0;
        frameCount = 0;
    }

    pixels.clear();
    pixels.reserve(2000); 

    auto start = std::chrono::high_resolution_clock::now();
    int iterations = 1000;
    for(int i=0; i<iterations; i++) {
        if (i > 0) pixels.clear(); 
        switch (currentAlgo) {
            case ALGO_STEP_BY_STEP: AlgoStepByStep(); break;
            case ALGO_DDA:          AlgoDDA(); break;
            case ALGO_BRESENHAM_LINE: AlgoBresenhamLine(); break;
            case ALGO_BRESENHAM_CIRCLE: AlgoBresenhamCircle(); break;
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    std::chrono::duration<double, std::micro> elapsed = end - start;
    
    currentExecutionTime = elapsed.count() / (double)iterations;

    totalTimeAccumulator += currentExecutionTime;
    frameCount++;
    averageExecutionTime = totalTimeAccumulator / (double)frameCount;
}

void RasterController::Render() {
    Calculate();

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove | 
                                    ImGuiWindowFlags_NoResize | 
                                    ImGuiWindowFlags_NoCollapse;

    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(PANEL_WIDTH, (float)WINDOW_HEIGHT), ImGuiCond_Always);
    
    ImGui::Begin("Настройки", nullptr, window_flags);
    
    ImGui::TextDisabled("Алгоритмы");
    ImGui::Separator();
    if (ImGui::RadioButton("Пошаговый", currentAlgo == ALGO_STEP_BY_STEP)) currentAlgo = ALGO_STEP_BY_STEP;
    if (ImGui::RadioButton("ЦДА (DDA)", currentAlgo == ALGO_DDA)) currentAlgo = ALGO_DDA;
    if (ImGui::RadioButton("Брезенхем (Отрезок)", currentAlgo == ALGO_BRESENHAM_LINE)) currentAlgo = ALGO_BRESENHAM_LINE;
    if (ImGui::RadioButton("Брезенхем (Окружность)", currentAlgo == ALGO_BRESENHAM_CIRCLE)) currentAlgo = ALGO_BRESENHAM_CIRCLE;
    ImGui::Spacing();

    ImGui::TextDisabled("Координаты");
    ImGui::Separator();

    float inputWidth = 200.0f; 

    if (currentAlgo == ALGO_BRESENHAM_CIRCLE) {
        ImGui::SetNextItemWidth(inputWidth);
        ImGui::InputInt2("Центр (X, Y)", &p1.x);
        
        ImGui::SetNextItemWidth(inputWidth / 2);
        ImGui::InputInt("Радиус (R)", &radius);
    } else {
        ImGui::SetNextItemWidth(inputWidth);
        ImGui::InputInt2("Начало (X1, Y1)", &p1.x);
        
        ImGui::SetNextItemWidth(inputWidth);
        ImGui::InputInt2("Конец (X2, Y2)", &p2.x);
    }
    ImGui::Spacing();

    ImGui::TextDisabled("Масштаб");
    ImGui::Separator();
    
    ImGui::SetNextItemWidth(inputWidth);
    ImGui::SliderInt("Масштаб", &scale, 2, 100);
    ImGui::Spacing();

    ImGui::Separator();
    
    ImGui::Text("Текущее время:   %.3f мкс", currentExecutionTime);
    ImGui::Text("Среднее время:   %.3f мкс", averageExecutionTime);
    ImGui::Text("Кол-во пикселей: %zu", pixels.size());
    
    ImGui::End();

    float canvasWidth = (float)WINDOW_WIDTH - PANEL_WIDTH;
    
    ImGui::SetNextWindowPos(ImVec2(PANEL_WIDTH, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(canvasWidth, (float)WINDOW_HEIGHT), ImGuiCond_Always);
    
    ImGui::Begin("Canvas", nullptr, window_flags | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar);
    
    DrawCanvas();

    ImGui::End();
}

void RasterController::DrawCanvas() {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    
    ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();
    ImVec2 canvas_sz = ImGui::GetContentRegionAvail();
    if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
    if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
    ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);
    ImGui::InvisibleButton("canvas_inv_btn", canvas_sz, ImGuiButtonFlags_MouseButtonLeft);
    bool isHovered = ImGui::IsItemHovered(); 
    bool isActive = ImGui::IsItemActive();

    if (isActive && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
        scrolling.x += ImGui::GetIO().MouseDelta.x;
        scrolling.y += ImGui::GetIO().MouseDelta.y;
    }

    if (isHovered) {
        float wheel = ImGui::GetIO().MouseWheel;
        if (wheel != 0.0f) {
            scale += (int)(wheel * 2);
            if (scale < 2) scale = 2;
            if (scale > 100) scale = 100;
        }
    }


    draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));
    draw_list->PushClipRect(canvas_p0, canvas_p1, true);

    ImVec2 origin = ImVec2(canvas_p0.x + canvas_sz.x * 0.5f + scrolling.x,
                           canvas_p0.y + canvas_sz.y * 0.5f + scrolling.y);

    auto ToScreen = [&](int x, int y) -> ImVec2 {
        return ImVec2(origin.x + x * scale, origin.y - y * scale);
    };

    if (scale >= 4) {
        const ImU32 GRID_COLOR = IM_COL32(220, 220, 220, 255);
        const ImU32 TEXT_COLOR = IM_COL32(0, 0, 0, 255);

        int x_min = (int)((canvas_p0.x - origin.x) / scale) - 1;
        int x_max = (int)((canvas_p1.x - origin.x) / scale) + 1;
        int y_min = (int)((origin.y - canvas_p1.y) / scale) - 1;
        int y_max = (int)((origin.y - canvas_p0.y) / scale) + 1;

        for (int x = x_min; x <= x_max; x++) {
            float xi = origin.x + x * scale;
            draw_list->AddLine(ImVec2(xi, canvas_p0.y), ImVec2(xi, canvas_p1.y), GRID_COLOR);
            if (scale > 15 || (x % 5 == 0)) 
                draw_list->AddText(ImVec2(xi + 2, origin.y + 2), TEXT_COLOR, std::to_string(x).c_str());
        }
        for (int y = y_min; y <= y_max; y++) {
            float yi = origin.y - y * scale;
            draw_list->AddLine(ImVec2(canvas_p0.x, yi), ImVec2(canvas_p1.x, yi), GRID_COLOR);
            if ((scale > 15 || (y % 5 == 0)) && y != 0)
                draw_list->AddText(ImVec2(origin.x + 2, yi - 14), TEXT_COLOR, std::to_string(y).c_str());
        }
    }

    {
        const ImU32 AXIS_COLOR = IM_COL32(255, 0, 0, 255);
        float th = 2.0f;
        draw_list->AddLine(ImVec2(origin.x, canvas_p0.y), ImVec2(origin.x, canvas_p1.y), AXIS_COLOR, th);
        draw_list->AddLine(ImVec2(origin.x, canvas_p0.y), ImVec2(origin.x - 5, canvas_p0.y + 10), AXIS_COLOR, th);
        draw_list->AddLine(ImVec2(origin.x, canvas_p0.y), ImVec2(origin.x + 5, canvas_p0.y + 10), AXIS_COLOR, th);

        draw_list->AddLine(ImVec2(canvas_p0.x, origin.y), ImVec2(canvas_p1.x, origin.y), AXIS_COLOR, th);
        draw_list->AddLine(ImVec2(canvas_p1.x, origin.y), ImVec2(canvas_p1.x - 10, origin.y - 5), AXIS_COLOR, th);
        draw_list->AddLine(ImVec2(canvas_p1.x, origin.y), ImVec2(canvas_p1.x - 10, origin.y + 5), AXIS_COLOR, th);
    }

    const ImU32 PIXEL_COLOR = IM_COL32(50, 50, 50, 200);
    const ImU32 BORDER_COLOR = IM_COL32(0, 0, 0, 255);

    for (const auto& p : pixels) {
        ImVec2 p_min = ToScreen(p.x, p.y + 1);
        ImVec2 p_max = ToScreen(p.x + 1, p.y);
        draw_list->AddRectFilled(p_min, p_max, PIXEL_COLOR);
        if (scale > 5) draw_list->AddRect(p_min, p_max, BORDER_COLOR);
    }

    const ImU32 IDEAL_COLOR = IM_COL32(0, 0, 255, 100);
    if (currentAlgo != ALGO_BRESENHAM_CIRCLE) {
        ImVec2 s_center = ImVec2(ToScreen(p1.x, p1.y).x + scale * 0.5f, ToScreen(p1.x, p1.y).y - scale * 0.5f);
        ImVec2 e_center = ImVec2(ToScreen(p2.x, p2.y).x + scale * 0.5f, ToScreen(p2.x, p2.y).y - scale * 0.5f);
        draw_list->AddLine(s_center, e_center, IDEAL_COLOR, 2.0f);
    } else {
        ImVec2 center = ToScreen(p1.x, p1.y);
        ImVec2 c_center = ImVec2(center.x + scale * 0.5f, center.y - scale * 0.5f);
        draw_list->AddCircle(c_center, radius * scale, IDEAL_COLOR, 64, 2.0f);
    }

    draw_list->PopClipRect();
}