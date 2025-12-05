#pragma once
#include "imgui.h"
#include <vector>
#include <string>

struct Point {
    int x, y;
};

enum RasterAlgorithm {
    ALGO_STEP_BY_STEP,
    ALGO_DDA,
    ALGO_BRESENHAM_LINE,
    ALGO_BRESENHAM_CIRCLE
};

class RasterController {
public:
    RasterController();
    void Render();

private:
    int scale;
    Point p1;
    Point p2;
    int radius;
    RasterAlgorithm currentAlgo;
    
    std::vector<Point> pixels;
    
    double currentExecutionTime; 
    double averageExecutionTime; 
    double totalTimeAccumulator; 
    long long frameCount;        
    
    Point lastP1, lastP2;
    int lastRadius;
    RasterAlgorithm lastAlgo;

    ImVec2 scrolling;

    void Calculate();
    void DrawCanvas();
    
    void AlgoStepByStep();
    void AlgoDDA();
    void AlgoBresenhamLine();
    void AlgoBresenhamCircle();
    
    void Plot(int x, int y);
};